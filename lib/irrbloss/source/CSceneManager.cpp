// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CSceneManager.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "SAnimatedMesh.h"
#include "CMeshCache.h"
#include "IGUIEnvironment.h"
#include "IMaterialRenderer.h"
#include "IReadFile.h"
#include "IWriteFile.h"

#include "os.h"

#include "COBJMeshFileLoader.h"

#include "CCubeSceneNode.h"
#include "CCameraSceneNode.h"
#include "CLightSceneNode.h"
#include "CMeshSceneNode.h"

#include "IShadowVolumeSceneNode.h"

#include "CEmptySceneNode.h"

#include "CSceneCollisionManager.h"
#include "CTriangleSelector.h"
#include "COctreeTriangleSelector.h"
#include "CMetaTriangleSelector.h"

#include "CGeometryCreator.h"

#include <locale.h>

namespace irr {
namespace scene {

//! constructor
CSceneManager::CSceneManager(video::IVideoDriver* driver, io::IFileSystem* fs,
		gui::ICursorControl* cursorControl, IMeshCache* cache,
		gui::IGUIEnvironment* gui)
: ISceneNode(0, 0), Driver(driver), FileSystem(fs), GUIEnvironment(gui),
	CursorControl(cursorControl), CollisionManager(0),
	ActiveCamera(0), ShadowColor(150,0,0,0), AmbientLight(0,0,0,0),
	MeshCache(cache), CurrentRenderPass(ESNRP_NONE), LightManager(0) {
	#ifdef _DEBUG
	ISceneManager::setDebugName("CSceneManager ISceneManager");
	ISceneNode::setDebugName("CSceneManager ISceneNode");
	#endif

	// root node's scene manager
	SceneManager = this;

	if (Driver)
		Driver->grab();

	if (FileSystem)
		FileSystem->grab();

	if (CursorControl)
		CursorControl->grab();

	if (GUIEnvironment)
		GUIEnvironment->grab();

	// create mesh cache if not there already
	if (!MeshCache)
		MeshCache = new CMeshCache();
	else
		MeshCache->grab();

	// create collision manager
	CollisionManager = new CSceneCollisionManager(this, Driver);

	// create geometry creator
	GeometryCreator = new CGeometryCreator();

	// add file format loaders. add the least commonly used ones first,
	// as these are checked last

	// TODO: now that we have multiple scene managers, these should be
	// shallow copies from the previous manager if there is one.

	MeshLoaderList.push_back(new COBJMeshFileLoader(this, FileSystem));
}

//! destructor
CSceneManager::~CSceneManager() {
	clearDeletionList();

	//! force to remove hardwareTextures from the driver
	//! because Scenes may hold internally data bounded to sceneNodes
	//! which may be destroyed twice
	if (Driver)
		Driver->removeAllHardwareBuffers();

	if (FileSystem)
		FileSystem->drop();

	if (CursorControl)
		CursorControl->drop();

	if (CollisionManager)
		CollisionManager->drop();

	if (GeometryCreator)
		GeometryCreator->drop();

	if (GUIEnvironment)
		GUIEnvironment->drop();

	u32 i;
	for (i=0; i<MeshLoaderList.size(); ++i)
		MeshLoaderList[i]->drop();

	if (ActiveCamera)
		ActiveCamera->drop();
	ActiveCamera = 0;

	if (MeshCache)
		MeshCache->drop();

	if (LightManager)
		LightManager->drop();

	// remove all nodes and animators before dropping the driver
	// as render targets may be destroyed twice

	removeAll();

	if (Driver)
		Driver->drop();
}

//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
IAnimatedMesh* CSceneManager::getMesh(const io::path& filename, const io::path& alternativeCacheName) {
	io::path cacheName = alternativeCacheName.empty() ? filename : alternativeCacheName;
	IAnimatedMesh* msh = MeshCache->getMeshByName(cacheName);
	if (msh)
		return msh;

	io::IReadFile* file = FileSystem->createAndOpenFile(filename);
	if (!file) {
		os::Printer::log("Could not load mesh, because file could not be opened: ", filename, ELL_ERROR);
		return 0;
	}

	msh = getUncachedMesh(file, filename, cacheName);

	file->drop();

	return msh;
}

//! gets an animateable mesh. loads it if needed. returned pointer must not be dropped.
IAnimatedMesh* CSceneManager::getMesh(io::IReadFile* file) {
	if (!file)
		return 0;

	io::path name = file->getFileName();
	IAnimatedMesh* msh = MeshCache->getMeshByName(name);
	if (msh)
		return msh;

	msh = getUncachedMesh(file, name, name);

	return msh;
}

// load and create a mesh which we know already isn't in the cache and put it in there
IAnimatedMesh* CSceneManager::getUncachedMesh(io::IReadFile* file, const io::path& filename, const io::path& cachename) {
	IAnimatedMesh* msh = 0;

	// iterate the list in reverse order so user-added loaders can override the built-in ones
	s32 count = MeshLoaderList.size();
	for (s32 i=count-1; i>=0; --i) {
		if (MeshLoaderList[i]->isALoadableFileExtension(filename)) {
			// reset file to avoid side effects of previous calls to createMesh
			file->seek(0);
			msh = MeshLoaderList[i]->createMesh(file);
			if (msh) {
				MeshCache->addMesh(cachename, msh);
				msh->drop();
				break;
			}
		}
	}

	if (!msh)
		os::Printer::log("Could not load mesh, file format seems to be unsupported", filename, ELL_ERROR);
	else
		os::Printer::log("Loaded mesh", filename, ELL_DEBUG);

	return msh;
}

//! returns the video driver
video::IVideoDriver* CSceneManager::getVideoDriver() {
	return Driver;
}

//! returns the GUI Environment
gui::IGUIEnvironment* CSceneManager::getGUIEnvironment() {
	return GUIEnvironment;
}

//! Get the active FileSystem
/** \return Pointer to the FileSystem
This pointer should not be dropped. See IReferenceCounted::drop() for more information. */
io::IFileSystem* CSceneManager::getFileSystem() {
	return FileSystem;
}

//! adds a test scene node for test purposes to the scene. It is a simple cube of (1,1,1) size.
//! the returned pointer must not be dropped.
IMeshSceneNode* CSceneManager::addCubeSceneNode(f32 size, ISceneNode* parent,
		s32 id, const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale) {
	if (!parent)
		parent = this;

	IMeshSceneNode* node = new CCubeSceneNode(size, parent, this, id, position, rotation, scale);
	node->drop();

	return node;
}

//! adds a scene node for rendering a static mesh
//! the returned pointer must not be dropped.
IMeshSceneNode* CSceneManager::addMeshSceneNode(IMesh* mesh, ISceneNode* parent, s32 id,
	const core::vector3df& position, const core::vector3df& rotation,
	const core::vector3df& scale, bool alsoAddIfMeshPointerZero) {
	if (!alsoAddIfMeshPointerZero && !mesh)
		return 0;

	if (!parent)
		parent = this;

	IMeshSceneNode* node = new CMeshSceneNode(mesh, parent, this, id, position, rotation, scale);
	node->drop();

	return node;
}

//! Adds a camera scene node to the tree and sets it as active camera.
//! \param position: Position of the space relative to its parent where the camera will be placed.
//! \param lookat: Position where the camera will look at. Also known as target.
//! \param parent: Parent scene node of the camera. Can be null. If the parent moves,
//! the camera will move too.
//! \return Returns pointer to interface to camera
ICameraSceneNode* CSceneManager::addCameraSceneNode(ISceneNode* parent,
	const core::vector3df& position, const core::vector3df& lookat, s32 id,
	bool makeActive) {
	if (!parent)
		parent = this;

	ICameraSceneNode* node = new CCameraSceneNode(parent, this, id, position, lookat);

	if (makeActive)
		setActiveCamera(node);
	node->drop();

	return node;
}

//! Adds a dynamic light scene node. The light will cast dynamic light on all
//! other scene nodes in the scene, which have the material flag video::MTF_LIGHTING
//! turned on. (This is the default setting in most scene nodes).
ILightSceneNode* CSceneManager::addLightSceneNode(ISceneNode* parent,
	const core::vector3df& position, video::SColorf color, f32 range, s32 id) {
	if (!parent)
		parent = this;

	ILightSceneNode* node = new CLightSceneNode(parent, this, id, position, color, range);
	node->drop();

	return node;
}

//! Adds an empty scene node.
ISceneNode* CSceneManager::addEmptySceneNode(ISceneNode* parent, s32 id) {
	if (!parent)
		parent = this;

	ISceneNode* node = new CEmptySceneNode(parent, this, id);
	node->drop();

	return node;
}

//! Returns the root scene node. This is the scene node which is parent
//! of all scene nodes. The root scene node is a special scene node which
//! only exists to manage all scene nodes. It is not rendered and cannot
//! be removed from the scene.
//! \return Returns a pointer to the root scene node.
ISceneNode* CSceneManager::getRootSceneNode() {
	return this;
}

//! Returns the current active camera.
//! \return The active camera is returned. Note that this can be NULL, if there
//! was no camera created yet.
ICameraSceneNode* CSceneManager::getActiveCamera() const {
	return ActiveCamera;
}

//! Sets the active camera. The previous active camera will be deactivated.
//! \param camera: The new camera which should be active.
void CSceneManager::setActiveCamera(ICameraSceneNode* camera) {
	if (camera)
		camera->grab();
	if (ActiveCamera)
		ActiveCamera->drop();

	ActiveCamera = camera;
}

//! renders the node.
void CSceneManager::render() {
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSceneManager::getBoundingBox() const {
	_IRR_DEBUG_BREAK_IF(true) // Bounding Box of Scene Manager should never be used.

	static const core::aabbox3d<f32> dummy;
	return dummy;
}

//! returns if node is culled
bool CSceneManager::isCulled(const ISceneNode* node) const {
	const ICameraSceneNode* cam = getActiveCamera();
	if (!cam) {
		return false;
	}
	bool result = false;

	// has occlusion query information
	if (node->getAutomaticCulling() & scene::EAC_OCC_QUERY) {
		result = (Driver->getOcclusionQueryResult(const_cast<ISceneNode*>(node))==0);
	}

	// can be seen by a bounding box ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_BOX)) {
		core::aabbox3d<f32> tbox = node->getBoundingBox();
		node->getAbsoluteTransformation().transformBoxEx(tbox);
		result = !(tbox.intersectsWithBox(cam->getViewFrustum()->getBoundingBox() ));
	}

	// can be seen by a bounding sphere
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_SPHERE)) {
		const core::aabbox3df nbox = node->getTransformedBoundingBox();
		const float rad = nbox.getRadius();
		const core::vector3df center = nbox.getCenter();

		const float camrad = cam->getViewFrustum()->getBoundingRadius();
		const core::vector3df camcenter = cam->getViewFrustum()->getBoundingCenter();

		const float dist = (center - camcenter).getLengthSQ();
		const float maxdist = (rad + camrad) * (rad + camrad);

		result = dist > maxdist;
	}

	// can be seen by cam pyramid planes ?
	if (!result && (node->getAutomaticCulling() & scene::EAC_FRUSTUM_BOX)) {
		SViewFrustum frust = *cam->getViewFrustum();

		//transform the frustum to the node's current absolute transformation
		core::matrix4 invTrans(node->getAbsoluteTransformation(), core::matrix4::EM4CONST_INVERSE);
		//invTrans.makeInverse();
		frust.transform(invTrans);

		core::vector3df edges[8];
		node->getBoundingBox().getEdges(edges);

		for (s32 i=0; i<scene::SViewFrustum::VF_PLANE_COUNT; ++i) {
			bool boxInFrustum=false;
			for (u32 j=0; j<8; ++j) {
				if (frust.planes[i].classifyPointRelation(edges[j]) != core::ISREL3D_FRONT) {
					boxInFrustum=true;
					break;
				}
			}

			if (!boxInFrustum) {
				result = true;
				break;
			}
		}
	}

	return result;
}

//! registers a node for rendering it at a specific time.
u32 CSceneManager::registerNodeForRendering(ISceneNode* node, E_SCENE_NODE_RENDER_PASS pass) {
	u32 taken = 0;

	switch(pass) {
		// take camera if it is not already registered
	case ESNRP_CAMERA: {
			taken = 1;
			for (u32 i = 0; i != CameraList.size(); ++i) {
				if (CameraList[i] == node) {
					taken = 0;
					break;
				}
			}
			if (taken) {
				CameraList.push_back(node);
			}
		}
		break;

	case ESNRP_LIGHT: {
			LightList.push_back(node);
			taken = 1;
		}
		break;

	case ESNRP_SOLID:
		if (!isCulled(node)) {
			SolidNodeList.push_back(node);
			taken = 1;
		}
		break;
	case ESNRP_TRANSPARENT:
		if (!isCulled(node)) {
			TransparentNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_TRANSPARENT_EFFECT:
		if (!isCulled(node)) {
			TransparentEffectNodeList.push_back(TransparentNodeEntry(node, camWorldPos));
			taken = 1;
		}
		break;
	case ESNRP_AUTOMATIC:
		if (!isCulled(node)) {
			const u32 count = node->getMaterialCount();

			taken = 0;
			for (u32 i=0; i<count; ++i) {
				if (Driver->needsTransparentRenderPass(node->getMaterial(i))) {
					// register as transparent node
					TransparentNodeEntry e(node, camWorldPos);
					TransparentNodeList.push_back(e);
					taken = 1;
					break;
				}
			}

			// not transparent, register as solid
			if (!taken) {
				SolidNodeList.push_back(node);
				taken = 1;
			}
		}
		break;
	case ESNRP_SHADOW:
		if (!isCulled(node)) {
			ShadowNodeList.push_back(node);
			taken = 1;
		}
		break;

	case ESNRP_GUI:
		if (!isCulled(node)) {
			GuiNodeList.push_back(node);
			taken = 1;
		}

	case ESNRP_NONE: // ignore this one
		break;
	}

	return taken;
}

void CSceneManager::clearAllRegisteredNodesForRendering() {
	CameraList.clear();
	LightList.clear();
	SkyBoxList.clear();
	SolidNodeList.clear();
	TransparentNodeList.clear();
	TransparentEffectNodeList.clear();
	ShadowNodeList.clear();
	GuiNodeList.clear();
}

//! This method is called just before the rendering process of the whole scene.
//! draws all scene nodes
void CSceneManager::drawAll() {
	if (!Driver)
		return;

	u32 i; // new ISO for scoping problem in some compilers

	// reset all transforms
	Driver->setMaterial(video::SMaterial());
	Driver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_VIEW, core::IdentityMatrix );
	Driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );
	for (i=video::ETS_COUNT-1; i>=video::ETS_TEXTURE_0; --i)
		Driver->setTransform ( (video::E_TRANSFORMATION_STATE)i, core::IdentityMatrix );

	// do animations and other stuff.
	OnAnimate(os::Timer::getTime());

	/*!
		First Scene Node for prerendering should be the active camera
		consistent Camera is needed for culling
	*/
	camWorldPos.set(0,0,0);
	if (ActiveCamera) {
		ActiveCamera->render();
		camWorldPos = ActiveCamera->getAbsolutePosition();
	}

	// let all nodes register themselves
	OnRegisterSceneNode();

	if (LightManager)
		LightManager->OnPreRender(LightList);

	//render camera scenes
	{
		CurrentRenderPass = ESNRP_CAMERA;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		if (LightManager)
			LightManager->OnRenderPassPreRender(CurrentRenderPass);

		for (i=0; i<CameraList.size(); ++i)
			CameraList[i]->render();

		CameraList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRenderPass);
	}

	//render lights scenes
	{
		CurrentRenderPass = ESNRP_LIGHT;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		if (LightManager) {
			LightManager->OnRenderPassPreRender(CurrentRenderPass);
		} else {
			// Sort the lights by distance from the camera
			core::vector3df camWorldPos(0, 0, 0);
			if (ActiveCamera)
				camWorldPos = ActiveCamera->getAbsolutePosition();

			core::array<DistanceNodeEntry> SortedLights;
			SortedLights.set_used(LightList.size());
			for (s32 light = (s32)LightList.size() - 1; light >= 0; --light)
				SortedLights[light].setNodeAndDistanceFromPosition(LightList[light], camWorldPos);

			SortedLights.set_sorted(false);
			SortedLights.sort();

			for(s32 light = (s32)LightList.size() - 1; light >= 0; --light)
				LightList[light] = SortedLights[light].Node;
		}

		Driver->setAmbientLight(AmbientLight);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRenderPass);
	}

	// render default objects
	{
		CurrentRenderPass = ESNRP_SOLID;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		SolidNodeList.sort(); // sort by textures

		if (LightManager) {
			LightManager->OnRenderPassPreRender(CurrentRenderPass);
			for (i=0; i<SolidNodeList.size(); ++i) {
				ISceneNode* node = SolidNodeList[i].Node;
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		} else {
			for (i=0; i<SolidNodeList.size(); ++i)
				SolidNodeList[i].Node->render();
		}

		SolidNodeList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRenderPass);
	}

	// render shadows
	{
		CurrentRenderPass = ESNRP_SHADOW;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		if (LightManager) {
			LightManager->OnRenderPassPreRender(CurrentRenderPass);
			for (i=0; i<ShadowNodeList.size(); ++i) {
				ISceneNode* node = ShadowNodeList[i];
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		} else {
			for (i=0; i<ShadowNodeList.size(); ++i)
				ShadowNodeList[i]->render();
		}

		if (!ShadowNodeList.empty())
			Driver->drawStencilShadow(true,ShadowColor, ShadowColor,
				ShadowColor, ShadowColor);

		ShadowNodeList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRenderPass);
	}

	// render transparent objects.
	{
		CurrentRenderPass = ESNRP_TRANSPARENT;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		TransparentNodeList.sort(); // sort by distance from camera
		if (LightManager) {
			LightManager->OnRenderPassPreRender(CurrentRenderPass);

			for (i=0; i<TransparentNodeList.size(); ++i) {
				ISceneNode* node = TransparentNodeList[i].Node;
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		} else {
			for (i=0; i<TransparentNodeList.size(); ++i)
				TransparentNodeList[i].Node->render();
		}

		TransparentNodeList.set_used(0);

		if (LightManager)
			LightManager->OnRenderPassPostRender(CurrentRenderPass);
	}

	// render transparent effect objects.
	{
		CurrentRenderPass = ESNRP_TRANSPARENT_EFFECT;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		TransparentEffectNodeList.sort(); // sort by distance from camera

		if (LightManager) {
			LightManager->OnRenderPassPreRender(CurrentRenderPass);

			for (i=0; i<TransparentEffectNodeList.size(); ++i) {
				ISceneNode* node = TransparentEffectNodeList[i].Node;
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		} else {
			for (i=0; i<TransparentEffectNodeList.size(); ++i)
				TransparentEffectNodeList[i].Node->render();
		}
		TransparentEffectNodeList.set_used(0);
	}

	// render custom gui nodes
	{
		CurrentRenderPass = ESNRP_GUI;
		Driver->getOverrideMaterial().Enabled = ((Driver->getOverrideMaterial().EnablePasses & CurrentRenderPass) != 0);

		if (LightManager) {
			LightManager->OnRenderPassPreRender(CurrentRenderPass);

			for (i=0; i<GuiNodeList.size(); ++i) {
				ISceneNode* node = GuiNodeList[i];
				LightManager->OnNodePreRender(node);
				node->render();
				LightManager->OnNodePostRender(node);
			}
		} else {
			for (i=0; i<GuiNodeList.size(); ++i)
				GuiNodeList[i]->render();
		}

		GuiNodeList.set_used(0);
	}

	if (LightManager)
		LightManager->OnPostRender();

	LightList.set_used(0);
	clearDeletionList();

	CurrentRenderPass = ESNRP_NONE;
}

void CSceneManager::setLightManager(ILightManager* lightManager) {
	if (lightManager)
		lightManager->grab();
	if (LightManager)
		LightManager->drop();

	LightManager = lightManager;
}

//! Sets the color of stencil buffers shadows drawn by the scene manager.
void CSceneManager::setShadowColor(video::SColor color) {
	ShadowColor = color;
}

//! Returns the current color of shadows.
video::SColor CSceneManager::getShadowColor() const {
	return ShadowColor;
}

//! Returns the number of mesh loaders supported by Irrlicht at this time
u32 CSceneManager::getMeshLoaderCount() const {
	return MeshLoaderList.size();
}

//! Retrieve the given mesh loader
IMeshLoader* CSceneManager::getMeshLoader(u32 index) const {
	if (index < MeshLoaderList.size())
		return MeshLoaderList[index];
	else
		return 0;
}

//! Returns a pointer to the scene collision manager.
ISceneCollisionManager* CSceneManager::getSceneCollisionManager() {
	return CollisionManager;
}

//! Returns a pointer to the mesh manipulator.
IMeshManipulator* CSceneManager::getMeshManipulator() {
	return Driver->getMeshManipulator();
}

//! Creates a simple ITriangleSelector, based on a mesh.
ITriangleSelector* CSceneManager::createOctreeTriangleSelector(IMesh* mesh,
							ISceneNode* node, s32 minimalPolysPerNode) {
	if (!mesh)
		return 0;

	return new COctreeTriangleSelector(mesh, node, minimalPolysPerNode);
}

ITriangleSelector* CSceneManager::createOctreeTriangleSelector(IMeshBuffer* meshBuffer, irr::u32 materialIndex,
			ISceneNode* node, s32 minimalPolysPerNode) {
	if ( !meshBuffer)
		return 0;

	return new COctreeTriangleSelector(meshBuffer, materialIndex, node, minimalPolysPerNode);
}

//! Adds a scene node to the deletion queue.
void CSceneManager::addToDeletionQueue(ISceneNode* node) {
	if (!node)
		return;

	node->grab();
	DeletionList.push_back(node);
}

//! clears the deletion list
void CSceneManager::clearDeletionList() {
	if (DeletionList.empty())
		return;

	for (u32 i=0; i<DeletionList.size(); ++i) {
		DeletionList[i]->remove();
		DeletionList[i]->drop();
	}

	DeletionList.clear();
}

//! Returns the first scene node with the specified name.
ISceneNode* CSceneManager::getSceneNodeFromName(const char* name, ISceneNode* start) {
	if (start == 0)
		start = getRootSceneNode();

	if (!strcmp(start->getName(),name))
		return start;

	ISceneNode* node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it) {
		node = getSceneNodeFromName(name, *it);
		if (node)
			return node;
	}

	return 0;
}

//! Returns the first scene node with the specified id.
ISceneNode* CSceneManager::getSceneNodeFromId(s32 id, ISceneNode* start) {
	if (start == 0)
		start = getRootSceneNode();

	if (start->getID() == id)
		return start;

	ISceneNode* node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it) {
		node = getSceneNodeFromId(id, *it);
		if (node)
			return node;
	}

	return 0;
}

//! Returns the first scene node with the specified type.
ISceneNode* CSceneManager::getSceneNodeFromType(scene::ESCENE_NODE_TYPE type, ISceneNode* start) {
	if (start == 0)
		start = getRootSceneNode();

	if (start->getType() == type || ESNT_ANY == type)
		return start;

	ISceneNode* node = 0;

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it) {
		node = getSceneNodeFromType(type, *it);
		if (node)
			return node;
	}

	return 0;
}

//! returns scene nodes by type.
void CSceneManager::getSceneNodesFromType(ESCENE_NODE_TYPE type, core::array<scene::ISceneNode*>& outNodes, ISceneNode* start) {
	if (start == 0)
		start = getRootSceneNode();

	if (start->getType() == type || ESNT_ANY == type)
		outNodes.push_back(start);

	const ISceneNodeList& list = start->getChildren();
	ISceneNodeList::ConstIterator it = list.begin();

	for (; it!=list.end(); ++it) {
		getSceneNodesFromType(type, outNodes, *it);
	}
}

//! Posts an input event to the environment. Usually you do not have to
//! use this method, it is used by the internal engine.
bool CSceneManager::postEventFromUser(const SEvent& event) {
	bool ret = false;
	ICameraSceneNode* cam = getActiveCamera();
	if (cam)
		ret = cam->OnEvent(event);

	return ret;
}

//! Removes all children of this scene node
void CSceneManager::removeAll() {
	ISceneNode::removeAll();
	setActiveCamera(0);
	// Make sure the driver is reset, might need a more complex method at some point
	if (Driver)
		Driver->setMaterial(video::SMaterial());
}

//! Clears the whole scene. All scene nodes are removed.
void CSceneManager::clear() {
	removeAll();
}

//! Returns current render pass.
E_SCENE_NODE_RENDER_PASS CSceneManager::getSceneNodeRenderPass() const {
	return CurrentRenderPass;
}

//! Returns an interface to the mesh cache which is shared between all existing scene managers.
IMeshCache* CSceneManager::getMeshCache() {
	return MeshCache;
}

//! Creates a new scene manager.
ISceneManager* CSceneManager::createNewSceneManager(bool cloneContent) {
	CSceneManager* manager = new CSceneManager(Driver, FileSystem, CursorControl, MeshCache, GUIEnvironment);

	if (cloneContent)
		manager->cloneMembers(this, manager);

	return manager;
}

//! Sets ambient color of the scene
void CSceneManager::setAmbientLight(const video::SColorf &ambientColor) {
	AmbientLight = ambientColor;
}

//! Returns ambient color of the scene
const video::SColorf& CSceneManager::getAmbientLight() const {
	return AmbientLight;
}

// creates a scenemanager
ISceneManager* createSceneManager(video::IVideoDriver* driver,
		io::IFileSystem* fs, gui::ICursorControl* cursorcontrol,
		gui::IGUIEnvironment *guiEnvironment) {
	return new CSceneManager(driver, fs, cursorcontrol, 0, guiEnvironment );
}

} // end namespace scene
} // end namespace irr

