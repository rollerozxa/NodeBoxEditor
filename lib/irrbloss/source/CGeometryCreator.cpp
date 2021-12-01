// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CGeometryCreator.h"
#include "SAnimatedMesh.h"
#include "SMeshBuffer.h"
#include "SMesh.h"
#include "IMesh.h"
#include "IVideoDriver.h"
#include "os.h"

namespace irr {
namespace scene {

IMesh* CGeometryCreator::createCubeMesh(const core::vector3df& size, ECUBE_MESH_TYPE type) const {
	SMesh* mesh = new SMesh;

	const video::SColor clr(255,255,255,255);

	if ( type == ECMT_1BUF_12VTX_NA ) {
		SMeshBuffer* buffer = new SMeshBuffer();

		// Create indices (pos, neg describes normal direction of front-face)
		const u16 u[36] = {	0,2,1,   0,3,2,		// NEG_Z
							1,5,4,   1,2,5,		// POS_X
							4,6,7,   4,5,6,		// POS_Z
							7,3,0,   7,6,3,		// NEG_X
							9,5,2,   9,8,5,		// POS_Y
							0,11,10, 0,10,7};	// NEG_Y

		buffer->Indices.set_used(36);

		for (u32 i=0; i<36; ++i)
			buffer->Indices[i] = u[i];

		// Create vertices
		buffer->Vertices.reallocate(12);

		buffer->Vertices.push_back(video::S3DVertex(0,0,0, -1,-1,-1, clr, 0, 1));	// 0
		buffer->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 1, 1));	// 1
		buffer->Vertices.push_back(video::S3DVertex(1,1,0,  1, 1,-1, clr, 1, 0));	// 2
		buffer->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 0, 0));	// 3
		buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 0, 1));	// 4
		buffer->Vertices.push_back(video::S3DVertex(1,1,1,  1, 1, 1, clr, 0, 0));	// 5
		buffer->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 1, 0));	// 6
		buffer->Vertices.push_back(video::S3DVertex(0,0,1, -1,-1, 1, clr, 1, 1));	// 7
		buffer->Vertices.push_back(video::S3DVertex(0,1,1, -1, 1, 1, clr, 0, 1));	// 8
		buffer->Vertices.push_back(video::S3DVertex(0,1,0, -1, 1,-1, clr, 1, 1));	// 9
		buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1,-1, 1, clr, 1, 0));	// 10
		buffer->Vertices.push_back(video::S3DVertex(1,0,0,  1,-1,-1, clr, 0, 0));	// 11

		// Recalculate bounding box and set cube size
		buffer->BoundingBox.reset(0,0,0);

		for (u32 i=0; i<12; ++i) {
			buffer->Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
			buffer->Vertices[i].Pos *= size;
			buffer->Vertices[i].Normal.normalize();
			buffer->BoundingBox.addInternalPoint(buffer->Vertices[i].Pos);
		}

		mesh->addMeshBuffer(buffer);
		buffer->drop();
	} else if (type == ECMT_6BUF_4VTX_NP) {
		for (int b = 0; b < 6; ++b) {
			SMeshBuffer* buffer = new SMeshBuffer();

			// Create indices
			const u16 u[6] = { 0,2,1,0,3,2 };

			buffer->Indices.set_used(6);

			for (int i = 0; i < 6; ++i)
				buffer->Indices[i] = u[i];

			// Create vertices
			buffer->Vertices.reallocate(4);

			switch (b) {
				case 0:
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,	0, 0,-1, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,  0, 0,-1, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  0, 0,-1, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,  0, 0,-1, clr, 0, 0));
				break;
				case 1:
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,	1, 0, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,  1, 0, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  1, 0, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  1, 0, 0, clr, 0, 0));
				break;
				case 2:
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,	0, 0, 1, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,  0, 0, 1, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  0, 0, 1, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  0, 0, 1, clr, 0, 0));
				break;
				case 3:
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,	-1, 0, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,  -1, 0, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,  -1, 0, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  -1, 0, 0, clr, 0, 0));
				break;
				case 4:
					buffer->Vertices.push_back(video::S3DVertex(0,1,0,	0, 1, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,0,  0, 1, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,1,1,  0, 1, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,1,1,  0, 1, 0, clr, 0, 0));
				break;
				case 5:
					buffer->Vertices.push_back(video::S3DVertex(0,0,1,	0, -1, 0, clr, 0, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,1,  0, -1, 0, clr, 1, 1));
					buffer->Vertices.push_back(video::S3DVertex(1,0,0,  0, -1, 0, clr, 1, 0));
					buffer->Vertices.push_back(video::S3DVertex(0,0,0,  0, -1, 0, clr, 0, 0));
				break;
			}

			// Recalculate bounding box and set cube size
			for (u32 i=0; i<4; ++i) {
				buffer->Vertices[i].Pos -= core::vector3df(0.5f, 0.5f, 0.5f);
				buffer->Vertices[i].Pos *= size;
				if (i == 0)
					buffer->BoundingBox.reset(buffer->Vertices[i].Pos);
				else
					buffer->BoundingBox.addInternalPoint(buffer->Vertices[i].Pos);
			}

			mesh->addMeshBuffer(buffer);
			buffer->drop();
		}
	}

	mesh->recalculateBoundingBox();
	return mesh;
}

} // end namespace scene
} // end namespace irr
