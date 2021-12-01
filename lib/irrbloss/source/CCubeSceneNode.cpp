// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#include "CCubeSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "SMeshBuffer.h"
#include "os.h"
#include "IShadowVolumeSceneNode.h"

namespace irr {
namespace scene {

	/*
        011         111
          /6,8------/5        y
         /  |      / |        ^  z
        /   |     /  |        | /
    010 3,9-------2  |        |/
        |   7- - -10,4 101     *---->x
        |  /      |  /
        |/        | /
        0------11,1/
       000       100
	*/

//! constructor
CCubeSceneNode::CCubeSceneNode(f32 size, ISceneNode* parent, ISceneManager* mgr,
		s32 id, const core::vector3df& position,
		const core::vector3df& rotation, const core::vector3df& scale)
	: IMeshSceneNode(parent, mgr, id, position, rotation, scale),
	Mesh(0), Shadow(0), Size(size) {
	#ifdef _DEBUG
	setDebugName("CCubeSceneNode");
	#endif

	setSize();
}

CCubeSceneNode::~CCubeSceneNode() {
	if (Shadow)
		Shadow->drop();
	if (Mesh)
		Mesh->drop();
}

void CCubeSceneNode::setSize() {
	if (Mesh)
		Mesh->drop();
	Mesh = SceneManager->getGeometryCreator()->createCubeMesh(core::vector3df(Size));
}

//! renders the node.
void CCubeSceneNode::render() {
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	if (Shadow)
		Shadow->updateShadowVolumes();

	// for debug purposes only:
	video::SMaterial mat = Mesh->getMeshBuffer(0)->getMaterial();

	// overwrite half transparency
	if (DebugDataVisible & scene::EDS_HALF_TRANSPARENCY)
		mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
	driver->setMaterial(mat);
	driver->drawMeshBuffer(Mesh->getMeshBuffer(0));
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CCubeSceneNode::getBoundingBox() const {
	return Mesh->getMeshBuffer(0)->getBoundingBox();
}

//! Removes a child from this scene node.
//! Implemented here, to be able to remove the shadow properly, if there is one,
//! or to remove attached childs.
bool CCubeSceneNode::removeChild(ISceneNode* child) {
	if (child && Shadow == child) {
		Shadow->drop();
		Shadow = 0;
	}

	return ISceneNode::removeChild(child);
}

void CCubeSceneNode::OnRegisterSceneNode() {
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);
	ISceneNode::OnRegisterSceneNode();
}

//! returns the material based on the zero based index i.
video::SMaterial& CCubeSceneNode::getMaterial(u32 i) {
	return Mesh->getMeshBuffer(0)->getMaterial();
}

//! returns amount of materials used by this scene node.
u32 CCubeSceneNode::getMaterialCount() const {
	return 1;
}

//! Creates a clone of this scene node and its children.
ISceneNode* CCubeSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager) {
	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CCubeSceneNode* nb = new CCubeSceneNode(Size, newParent,
		newManager, ID, RelativeTranslation);

	nb->cloneMembers(this, newManager);
	nb->getMaterial(0) = getMaterial(0);
	nb->Shadow = Shadow;
	if ( nb->Shadow )
		nb->Shadow->grab();

	if ( newParent )
		nb->drop();
	return nb;
}

} // end namespace scene
} // end namespace irr
