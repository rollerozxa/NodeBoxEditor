// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_GEOMETRY_CREATOR_H_INCLUDED__
#define __C_GEOMETRY_CREATOR_H_INCLUDED__

#include "IGeometryCreator.h"
#include "SMeshBuffer.h"

namespace irr {
namespace scene {

//! class for creating geometry on the fly
class CGeometryCreator : public IGeometryCreator {
public:
	virtual IMesh* createCubeMesh(const core::vector3df& size, ECUBE_MESH_TYPE type) const _IRR_OVERRIDE_;
};

} // end namespace scene
} // end namespace irr

#endif
