// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_ANIMATED_MESH_H_INCLUDED__
#define __I_ANIMATED_MESH_H_INCLUDED__

#include "aabbox3d.h"
#include "IMesh.h"

namespace irr {
namespace scene
{
	//! Interface for an animated mesh.
	class IAnimatedMesh : public IMesh
	{
	public:

		//! Gets the frame count of the animated mesh.
		virtual u32 getFrameCount() const = 0;

		//! Returns the IMesh interface for a frame.
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1) = 0;

		//! Returns the type of the animated mesh.
		virtual E_ANIMATED_MESH_TYPE getMeshType() const _IRR_OVERRIDE_
		{
			return EAMT_UNKNOWN;
		}
	};

} // end namespace scene
} // end namespace irr

#endif

