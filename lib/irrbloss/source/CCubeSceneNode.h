// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_CUBE_SCENE_NODE_H_INCLUDED__
#define __C_CUBE_SCENE_NODE_H_INCLUDED__

#include "IMeshSceneNode.h"
#include "SMesh.h"

namespace irr {
namespace scene {
	class CCubeSceneNode : public IMeshSceneNode
	{
	public:

		//! constructor
		CCubeSceneNode(f32 size, ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		virtual ~CCubeSceneNode();

		virtual void OnRegisterSceneNode() _IRR_OVERRIDE_;

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

		//! Returns type of the scene node
		virtual ESCENE_NODE_TYPE getType() const _IRR_OVERRIDE_ { return ESNT_CUBE; }

		//! Creates a clone of this scene node and its children.
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) _IRR_OVERRIDE_;

		//! Sets a new mesh to display
		virtual void setMesh(IMesh* mesh) _IRR_OVERRIDE_ {}

		//! Returns the current mesh
		virtual IMesh* getMesh(void) _IRR_OVERRIDE_ { return Mesh; }

		//! Removes a child from this scene node.
		//! Implemented here, to be able to remove the shadow properly, if there is one,
		//! or to remove attached child.
		virtual bool removeChild(ISceneNode* child) _IRR_OVERRIDE_;

	private:
		void setSize();

		IMesh* Mesh;
		IShadowVolumeSceneNode* Shadow;
		f32 Size;
	};

} // end namespace scene
} // end namespace irr

#endif

