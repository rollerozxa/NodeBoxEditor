// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CAnimatedMeshSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "S3DVertex.h"
#include "os.h"
#ifdef _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_
#include "CShadowVolumeSceneNode.h"
#else
#include "IShadowVolumeSceneNode.h"
#endif // _IRR_COMPILE_WITH_SHADOW_VOLUME_SCENENODE_
#include "IMaterialRenderer.h"
#include "IMesh.h"
#include "IMeshCache.h"
#include "IAnimatedMesh.h"
#include "quaternion.h"
#include "IFileSystem.h"

namespace irr {
namespace scene {

//! constructor
CAnimatedMeshSceneNode::CAnimatedMeshSceneNode(IAnimatedMesh* mesh,
		ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position,
		const core::vector3df& rotation,
		const core::vector3df& scale)
: IAnimatedMeshSceneNode(parent, mgr, id, position, rotation, scale), Mesh(0),
	StartFrame(0), EndFrame(0), FramesPerSecond(0.025f),
	CurrentFrameNr(0.f), LastTimeMs(0),
	TransitionTime(0), Transiting(0.f), TransitingBlend(0.f),
	JointMode(EJUOR_NONE), JointsUsed(false),
	Looping(true), ReadOnlyMaterials(false), RenderFromIdentity(false),
	LoopCallBack(0), PassCount(0), Shadow(0) {
	#ifdef _DEBUG
	setDebugName("CAnimatedMeshSceneNode");
	#endif

	setMesh(mesh);
}

//! destructor
CAnimatedMeshSceneNode::~CAnimatedMeshSceneNode() {
	if (Mesh)
		Mesh->drop();

	if (Shadow)
		Shadow->drop();

	if (LoopCallBack)
		LoopCallBack->drop();
}

//! Returns the currently displayed frame number.
f32 CAnimatedMeshSceneNode::getFrameNr() const {
	return CurrentFrameNr;
}

//! Get CurrentFrameNr and update transiting settings
void CAnimatedMeshSceneNode::buildFrameNr(u32 timeMs) {
	if (Transiting!=0.f) {
		TransitingBlend += (f32)(timeMs) * Transiting;
		if (TransitingBlend > 1.f) {
			Transiting=0.f;
			TransitingBlend=0.f;
		}
	}

	if (StartFrame==EndFrame) {
		CurrentFrameNr = (f32)StartFrame; //Support for non animated meshes
	} else if (Looping) {
		// play animation looped
		CurrentFrameNr += timeMs * FramesPerSecond;

		// We have no interpolation between EndFrame and StartFrame,
		// the last frame must be identical to first one with our current solution.
		if (FramesPerSecond > 0.f) //forwards...
		{
			if (CurrentFrameNr > EndFrame)
				CurrentFrameNr = StartFrame + fmodf(CurrentFrameNr - StartFrame, (f32)(EndFrame-StartFrame));
		} else { //backwards...
			if (CurrentFrameNr < StartFrame)
				CurrentFrameNr = EndFrame - fmodf(EndFrame - CurrentFrameNr, (f32)(EndFrame-StartFrame));
		}
	} else {
		// play animation non looped

		CurrentFrameNr += timeMs * FramesPerSecond;
		if (FramesPerSecond > 0.f) { //forwards...
			if (CurrentFrameNr > (f32)EndFrame) {
				CurrentFrameNr = (f32)EndFrame;
				if (LoopCallBack)
					LoopCallBack->OnAnimationEnd(this);
			}
		}
		else //backwards...
		{
			if (CurrentFrameNr < (f32)StartFrame) {
				CurrentFrameNr = (f32)StartFrame;
				if (LoopCallBack)
					LoopCallBack->OnAnimationEnd(this);
			}
		}
	}
}

void CAnimatedMeshSceneNode::OnRegisterSceneNode() {
	if (IsVisible && Mesh) {
		// because this node supports rendering of mixed mode meshes consisting of
		// transparent and solid material at the same time, we need to go through all
		// materials, check of what type they are and register this node for the right
		// render pass according to that.

		video::IVideoDriver* driver = SceneManager->getVideoDriver();

		PassCount = 0;
		int transparentCount = 0;
		int solidCount = 0;

		// count transparent and solid materials in this scene node
		const u32 numMaterials = ReadOnlyMaterials ? Mesh->getMeshBufferCount() : Materials.size();
		for (u32 i=0; i<numMaterials; ++i) {
			const video::SMaterial& material = ReadOnlyMaterials ? Mesh->getMeshBuffer(i)->getMaterial() : Materials[i];

			if ( driver->needsTransparentRenderPass(material) )
				++transparentCount;
			else
				++solidCount;

			if (solidCount && transparentCount)
				break;
		}

		// register according to material types counted

		if (solidCount)
			SceneManager->registerNodeForRendering(this, scene::ESNRP_SOLID);

		if (transparentCount)
			SceneManager->registerNodeForRendering(this, scene::ESNRP_TRANSPARENT);

		ISceneNode::OnRegisterSceneNode();
	}
}

IMesh * CAnimatedMeshSceneNode::getMeshForCurrentFrame() {
	if(Mesh->getMeshType() != EAMT_SKINNED) {
		s32 frameNr = (s32) getFrameNr();
		s32 frameBlend = (s32) (core::fract ( getFrameNr() ) * 1000.f);
		return Mesh->getMesh(frameNr, frameBlend, StartFrame, EndFrame);
	} else {
		return 0;
	}
}

//! OnAnimate() is called just before rendering the whole scene.
void CAnimatedMeshSceneNode::OnAnimate(u32 timeMs) {
	if (LastTimeMs==0)	// first frame
	{
		LastTimeMs = timeMs;
	}

	// set CurrentFrameNr
	buildFrameNr(timeMs-LastTimeMs);

	// update bbox
	if (Mesh) {
		scene::IMesh * mesh = getMeshForCurrentFrame();

		if (mesh)
			Box = mesh->getBoundingBox();
	}
	LastTimeMs = timeMs;

	IAnimatedMeshSceneNode::OnAnimate(timeMs);
}

//! renders the node.
void CAnimatedMeshSceneNode::render() {
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!Mesh || !driver)
		return;

	const bool isTransparentPass =
		SceneManager->getSceneNodeRenderPass() == scene::ESNRP_TRANSPARENT;

	++PassCount;

	scene::IMesh* m = getMeshForCurrentFrame();

	if(m) {
		Box = m->getBoundingBox();
	} else {
		#ifdef _DEBUG
			os::Printer::log("Animated Mesh returned no mesh to render.", Mesh->getDebugName(), ELL_WARNING);
		#endif
	}

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);

	if (Shadow && PassCount==1)
		Shadow->updateShadowVolumes();

	// for debug purposes only:

	bool renderMeshes = true;
	video::SMaterial mat;
	if (DebugDataVisible && PassCount==1) {
		// overwrite half transparency
		if (DebugDataVisible & scene::EDS_HALF_TRANSPARENCY) {

			for (u32 i=0; i<m->getMeshBufferCount(); ++i) {
				scene::IMeshBuffer* mb = m->getMeshBuffer(i);
				mat = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];
				mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;
				if (RenderFromIdentity)
					driver->setTransform(video::ETS_WORLD, core::IdentityMatrix );
				//else if (Mesh->getMeshType() == EAMT_SKINNED)
				//	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation * ((SSkinMeshBuffer*)mb)->Transformation);

				driver->setMaterial(mat);
				driver->drawMeshBuffer(mb);
			}
			renderMeshes = false;
		}
	}

	// render original meshes
	if (renderMeshes) {
		for (u32 i=0; i<m->getMeshBufferCount(); ++i) {
			const bool transparent = driver->needsTransparentRenderPass(Materials[i]);

			// only render transparent buffer if this is the transparent render pass
			// and solid only in solid pass
			if (transparent == isTransparentPass) {
				scene::IMeshBuffer* mb = m->getMeshBuffer(i);
				const video::SMaterial& material = ReadOnlyMaterials ? mb->getMaterial() : Materials[i];
				if (RenderFromIdentity)
					driver->setTransform(video::ETS_WORLD, core::IdentityMatrix );
				//else if (Mesh->getMeshType() == EAMT_SKINNED)
				//	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation * ((SSkinMeshBuffer*)mb)->Transformation);

				driver->setMaterial(material);
				driver->drawMeshBuffer(mb);
			}
		}
	}

	driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CAnimatedMeshSceneNode::getBoundingBox() const {
	return Box;
}

//! returns the material based on the zero based index i.
video::SMaterial& CAnimatedMeshSceneNode::getMaterial(u32 i) {
	if (i >= Materials.size())
		return ISceneNode::getMaterial(i);

	return Materials[i];
}


//! returns amount of materials used by this scene node.
u32 CAnimatedMeshSceneNode::getMaterialCount() const {
	return Materials.size();
}

//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
void CAnimatedMeshSceneNode::setReadOnlyMaterials(bool readonly) {
	ReadOnlyMaterials = readonly;
}

//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
bool CAnimatedMeshSceneNode::isReadOnlyMaterials() const {
	return ReadOnlyMaterials;
}

//! Sets a new mesh
void CAnimatedMeshSceneNode::setMesh(IAnimatedMesh* mesh) {
	if (!mesh)
		return; // won't set null mesh

	if (Mesh != mesh) {
		if (Mesh)
			Mesh->drop();

		Mesh = mesh;

		// grab the mesh (it's non-null!)
		Mesh->grab();
	}

	// get materials and bounding box
	Box = Mesh->getBoundingBox();

	IMesh* m = Mesh->getMesh(0,0);
	if (m) {
		Materials.clear();
		Materials.reallocate(m->getMeshBufferCount());

		for (u32 i=0; i<m->getMeshBufferCount(); ++i) {
			IMeshBuffer* mb = m->getMeshBuffer(i);
			if (mb)
				Materials.push_back(mb->getMaterial());
			else
				Materials.push_back(video::SMaterial());
		}
	}

	// clean up joint nodes
	if (JointsUsed) {
		JointsUsed=false;
	}
}

//! updates the absolute position based on the relative and the parents position
void CAnimatedMeshSceneNode::updateAbsolutePosition() {
	IAnimatedMeshSceneNode::updateAbsolutePosition();
}

/*!
*/
ISceneNode* CAnimatedMeshSceneNode::clone(ISceneNode* newParent, ISceneManager* newManager) {
	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CAnimatedMeshSceneNode* newNode =
		new CAnimatedMeshSceneNode(Mesh, NULL, newManager, ID, RelativeTranslation,
						 RelativeRotation, RelativeScale);

	if (newParent) {
		newNode->setParent(newParent); 	// not in constructor because virtual overload for updateAbsolutePosition won't be called
		newNode->drop();
	}

	newNode->cloneMembers(this, newManager);

	newNode->Materials = Materials;
	newNode->Box = Box;
	newNode->Mesh = Mesh;
	newNode->StartFrame = StartFrame;
	newNode->EndFrame = EndFrame;
	newNode->FramesPerSecond = FramesPerSecond;
	newNode->CurrentFrameNr = CurrentFrameNr;
	newNode->JointMode = JointMode;
	newNode->JointsUsed = JointsUsed;
	newNode->TransitionTime = TransitionTime;
	newNode->Transiting = Transiting;
	newNode->TransitingBlend = TransitingBlend;
	newNode->Looping = Looping;
	newNode->ReadOnlyMaterials = ReadOnlyMaterials;
	newNode->LoopCallBack = LoopCallBack;
	if (newNode->LoopCallBack)
		newNode->LoopCallBack->grab();
	newNode->PassCount = PassCount;
	newNode->Shadow = Shadow;
	if (newNode->Shadow)
		newNode->Shadow->grab();
	newNode->PretransitingSave = PretransitingSave;
	newNode->RenderFromIdentity = RenderFromIdentity;

	return newNode;
}

} // end namespace scene
} // end namespace irr
