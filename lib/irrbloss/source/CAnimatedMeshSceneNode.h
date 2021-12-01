// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__
#define __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__

#include "IAnimatedMeshSceneNode.h"
#include "IAnimatedMesh.h"

#include "matrix4.h"

namespace irr {
namespace scene {
	class CAnimatedMeshSceneNode : public IAnimatedMeshSceneNode
	{
	public:

		//! constructor
		CAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, ISceneManager* mgr,	s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! destructor
		virtual ~CAnimatedMeshSceneNode();

		//! frame
		virtual void OnRegisterSceneNode() _IRR_OVERRIDE_;

		//! OnAnimate() is called just before rendering the whole scene.
		virtual void OnAnimate(u32 timeMs) _IRR_OVERRIDE_;

		//! renders the node.
		virtual void render() _IRR_OVERRIDE_;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const _IRR_OVERRIDE_;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hierarchy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i) _IRR_OVERRIDE_;

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const _IRR_OVERRIDE_;

		//! Returns the current displayed frame number.
		virtual f32 getFrameNr() const _IRR_OVERRIDE_;

		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		/* In this way it is possible to change the materials a mesh causing all mesh scene nodes
		referencing this mesh to change too. */
		virtual void setReadOnlyMaterials(bool readonly) _IRR_OVERRIDE_;

		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		virtual bool isReadOnlyMaterials() const _IRR_OVERRIDE_;

		//! Sets a new mesh
		virtual void setMesh(IAnimatedMesh* mesh) _IRR_OVERRIDE_;

		//! Returns the current mesh
		virtual IAnimatedMesh* getMesh(void) _IRR_OVERRIDE_ { return Mesh; }

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const _IRR_OVERRIDE_ { return ESNT_ANIMATED_MESH; }

		//! updates the absolute position based on the relative and the parents position
		virtual void updateAbsolutePosition() _IRR_OVERRIDE_;

		//! Creates a clone of this scene node and its children.
		/** \param newParent An optional new parent.
		\param newManager An optional new scene manager.
		\return The newly created clone of this node. */
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) _IRR_OVERRIDE_;

	private:

		//! Get a static mesh for the current frame of this animated mesh
		IMesh* getMeshForCurrentFrame();

		void buildFrameNr(u32 timeMs);

		core::array<video::SMaterial> Materials;
		core::aabbox3d<f32> Box;
		IAnimatedMesh* Mesh;

		s32 StartFrame;
		s32 EndFrame;
		f32 FramesPerSecond;
		f32 CurrentFrameNr;

		u32 LastTimeMs;
		u32 TransitionTime; //Transition time in millisecs
		f32 Transiting; //is mesh transiting (plus cache of TransitionTime)
		f32 TransitingBlend; //0-1, calculated on buildFrameNr

		//0-unused, 1-get joints only, 2-set joints only, 3-move and set
		E_JOINT_UPDATE_ON_RENDER JointMode;
		bool JointsUsed;

		bool Looping;
		bool ReadOnlyMaterials;
		bool RenderFromIdentity;

		IAnimationEndCallBack* LoopCallBack;
		s32 PassCount;

		IShadowVolumeSceneNode* Shadow;

		core::array<core::matrix4> PretransitingSave;
	};

} // end namespace scene
} // end namespace irr

#endif

