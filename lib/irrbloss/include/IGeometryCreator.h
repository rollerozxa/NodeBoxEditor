// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GEOMETRY_CREATOR_H_INCLUDED__
#define __I_GEOMETRY_CREATOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "IMesh.h"
#include "IImage.h"

namespace irr {
namespace video
{
	class IVideoDriver;
	class SMaterial;
}

namespace scene {

	enum ECUBE_MESH_TYPE
	{
		//! Single buffer with 12 different vertices, normals are average of adjacent planes
		//! Order for outgoing (front-face) normals of planes would be: NEG_Z, POS_X, POS_Z, NEG_X, POS_Y, NEG_Y
		ECMT_1BUF_12VTX_NA,

		//! One buffer per side, each with 4 vertices, normals are perpendicular to sides
		//! Note: You probably will have to scale down your texture uv's to avoid white lines at borders
		//        as this mesh sets them to 0,1 values. We can't do that when creating the mesh as it
		//        depends on texture resolution which we don't know at that point.
		ECMT_6BUF_4VTX_NP
	};

//! Helper class for creating geometry on the fly.
/** You can get an instance of this class through ISceneManager::getGeometryCreator() */
class IGeometryCreator : public IReferenceCounted {
public:

	//! Creates a simple cube mesh.
	/**
	\param size Dimensions of the cube.
	\param type One of ECUBE_MESH_TYPE. So you can chose between cubes with single material or independent materials per side.
	\return Generated mesh.
	*/
	virtual IMesh* createCubeMesh(const core::vector3df& size=core::vector3df(5.f,5.f,5.f), ECUBE_MESH_TYPE type = ECMT_1BUF_12VTX_NA) const =0;
};


} // end namespace scene
} // end namespace irr

#endif // __I_GEOMETRY_CREATOR_H_INCLUDED__

