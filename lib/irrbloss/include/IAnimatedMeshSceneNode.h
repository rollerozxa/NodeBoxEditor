// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__
#define __I_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IAnimatedMesh.h"

namespace irr {
namespace scene
{
	class IShadowVolumeSceneNode;

	enum E_JOINT_UPDATE_ON_RENDER
	{
		//! do nothing
		EJUOR_NONE = 0,

		//! get joints positions from the mesh (for attached nodes, etc)
		EJUOR_READ,

		//! control joint positions in the mesh (eg. ragdolls, or set the animation from animateJoints() )
		EJUOR_CONTROL
	};


	class IAnimatedMeshSceneNode;

	//! Callback interface for catching events of ended animations.
	/** Implement this interface and use
	IAnimatedMeshSceneNode::setAnimationEndCallback to be able to
	be notified if an animation playback has ended.
	**/
	class IAnimationEndCallBack : public virtual IReferenceCounted
	{
	public:

		//! Will be called when the animation playback has ended.
		/** See IAnimatedMeshSceneNode::setAnimationEndCallback for
		more information.
		\param node: Node of which the animation has ended. */
		virtual void OnAnimationEnd(IAnimatedMeshSceneNode* node) = 0;
	};

	//! Scene node capable of displaying an animated mesh.
	class IAnimatedMeshSceneNode : public ISceneNode
	{
	public:

		//! Constructor
		IAnimatedMeshSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f))
			: ISceneNode(parent, mgr, id, position, rotation, scale) {}

		//! Destructor
		virtual ~IAnimatedMeshSceneNode() {}

		//! Returns the currently displayed frame number.
		virtual f32 getFrameNr() const = 0;

		//! Sets if the scene node should not copy the materials of the mesh but use them in a read only style.
		/** In this way it is possible to change the materials a mesh
		causing all mesh scene nodes referencing this mesh to change
		too. */
		virtual void setReadOnlyMaterials(bool readonly) = 0;

		//! Returns if the scene node should not copy the materials of the mesh but use them in a read only style
		virtual bool isReadOnlyMaterials() const = 0;

		//! Sets a new mesh
		virtual void setMesh(IAnimatedMesh* mesh) = 0;

		//! Returns the current mesh
		virtual IAnimatedMesh* getMesh(void) = 0;

		//! Creates a clone of this scene node and its children.
		/** \param newParent An optional new parent.
		\param newManager An optional new scene manager.
		\return The newly created clone of this node. */
		virtual ISceneNode* clone(ISceneNode* newParent=0, ISceneManager* newManager=0) = 0;

	};

} // end namespace scene
} // end namespace irr

#endif

