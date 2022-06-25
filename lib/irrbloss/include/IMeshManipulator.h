// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MESH_MANIPULATOR_H_INCLUDED__
#define __I_MESH_MANIPULATOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "vector3d.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "IAnimatedMesh.h"
#include "IMeshBuffer.h"
#include "SVertexManipulator.h"

namespace irr {
namespace scene
{

	struct SMesh;

	//! An interface for easy manipulation of meshes.
	/** Scale, set alpha value, flip surfaces, and so on. This exists for
	fixing problems with wrong imported or exported meshes quickly after
	loading. It is not intended for doing mesh modifications and/or
	animations during runtime.
	*/
	class IMeshManipulator : public virtual IReferenceCounted
	{
	public:

		//! Flips the direction of surfaces.
		/** Changes backfacing triangles to frontfacing
		triangles and vice versa.
		\param mesh Mesh on which the operation is performed. */
		virtual void flipSurfaces(IMesh* mesh) const = 0;

		//! Sets the alpha vertex color value of the whole mesh to a new value.
		/** \param mesh Mesh on which the operation is performed.
		\param alpha New alpha value. Must be a value between 0 and 255. */
		void setVertexColorAlpha(IMesh* mesh, s32 alpha) const
		{
			apply(scene::SVertexColorSetAlphaManipulator(alpha), mesh);
		}

		//! Sets the alpha vertex color value of the whole mesh to a new value.
		/** \param buffer Meshbuffer on which the operation is performed.
		\param alpha New alpha value. Must be a value between 0 and 255. */
		void setVertexColorAlpha(IMeshBuffer* buffer, s32 alpha) const
		{
			apply(scene::SVertexColorSetAlphaManipulator(alpha), buffer);
		}

		//! Sets the colors of all vertices to one color
		/** \param mesh Mesh on which the operation is performed.
		\param color New color. */
		void setVertexColors(IMesh* mesh, video::SColor color) const
		{
			apply(scene::SVertexColorSetManipulator(color), mesh);
		}

		//! Sets the colors of all vertices to one color
		/** \param buffer Meshbuffer on which the operation is performed.
		\param color New color. */
		void setVertexColors(IMeshBuffer* buffer, video::SColor color) const
		{
			apply(scene::SVertexColorSetManipulator(color), buffer);
		}

		//! Recalculates all normals of the mesh.
		/** \param mesh: Mesh on which the operation is performed.
		\param smooth: If the normals shall be smoothed.
		\param angleWeighted: If the normals shall be smoothed in relation to their angles. More expensive, but also higher precision. */
		virtual void recalculateNormals(IMesh* mesh, bool smooth = false,
				bool angleWeighted = false) const=0;

		//! Recalculates all normals of the mesh buffer.
		/** \param buffer: Mesh buffer on which the operation is performed.
		\param smooth: If the normals shall be smoothed.
		\param angleWeighted: If the normals shall be smoothed in relation to their angles. More expensive, but also higher precision. */
		virtual void recalculateNormals(IMeshBuffer* buffer,
				bool smooth = false, bool angleWeighted = false) const=0;

		//! Recalculates tangents, requires a tangent mesh
		/** \param mesh Mesh on which the operation is performed.
		\param recalculateNormals If the normals shall be recalculated, otherwise original normals of the mesh are used unchanged.
		\param smooth If the normals shall be smoothed.
		\param angleWeighted If the normals shall be smoothed in relation to their angles. More expensive, but also higher precision.
		*/
		virtual void recalculateTangents(IMesh* mesh,
				bool recalculateNormals=false, bool smooth=false,
				bool angleWeighted=false) const=0;

		//! Recalculates tangents, requires a tangent mesh buffer
		/** \param buffer Meshbuffer on which the operation is performed.
		\param recalculateNormals If the normals shall be recalculated, otherwise original normals of the buffer are used unchanged.
		\param smooth If the normals shall be smoothed.
		\param angleWeighted If the normals shall be smoothed in relation to their angles. More expensive, but also higher precision.
		*/
		virtual void recalculateTangents(IMeshBuffer* buffer,
				bool recalculateNormals=false, bool smooth=false,
				bool angleWeighted=false) const=0;

		//! Scales the actual mesh, not a scene node.
		/** \param mesh Mesh on which the operation is performed.
		\param factor Scale factor for each axis. */
		void scale(IMesh* mesh, const core::vector3df& factor) const
		{
			apply(SVertexPositionScaleManipulator(factor), mesh, true);
		}

		//! Scales the actual meshbuffer, not a scene node.
		/** \param buffer Meshbuffer on which the operation is performed.
		\param factor Scale factor for each axis. */
		void scale(IMeshBuffer* buffer, const core::vector3df& factor) const
		{
			apply(SVertexPositionScaleManipulator(factor), buffer, true);
		}

		//! Applies a transformation to a mesh
		/** \param mesh Mesh on which the operation is performed.
		\param m transformation matrix.
		\param normalsUpdate When 0 - don't update normals.
							 When 1 - update normals with inverse transposed of the transformation matrix
		*/
		void transform(IMesh* mesh, const core::matrix4& m, u32 normalsUpdate = 0) const
		{
			apply(SVertexPositionTransformManipulator(m), mesh, true);

			if ( normalsUpdate == 1 ) {
				core::matrix4 invT;
				if ( m.getInverse(invT) ) {
					invT = invT.getTransposed();
					apply(SVertexNormalTransformManipulator(invT), mesh, false);
				}
			}
		}

		//! Applies a transformation to a meshbuffer
		/** \param buffer Meshbuffer on which the operation is performed.
		\param m transformation matrix.
		\param normalsUpdate When 0 - don't update normals.
							 When 1 - update normals with inverse transposed of the transformation matrix
		*/
		void transform(IMeshBuffer* buffer, const core::matrix4& m, u32 normalsUpdate = 0) const
		{
			apply(SVertexPositionTransformManipulator(m), buffer, true);

			if ( normalsUpdate == 1 ) {
				core::matrix4 invT;
				if ( m.getInverse(invT) ) {
					invT = invT.getTransposed();
					apply(SVertexNormalTransformManipulator(invT), buffer, false);
				}
			}
		}

		//! Apply a manipulator on the Meshbuffer
		/** \param func A functor defining the mesh manipulation.
		\param buffer The Meshbuffer to apply the manipulator to.
		\param boundingBoxUpdate Specifies if the bounding box should be updated during manipulation.
		\return True if the functor was successfully applied, else false. */
		template <typename Functor>
		bool apply(const Functor& func, IMeshBuffer* buffer, bool boundingBoxUpdate=false) const
		{
			return apply_(func, buffer, boundingBoxUpdate, func);
		}


		//! Apply a manipulator on the Mesh
		/** \param func A functor defining the mesh manipulation.
		\param mesh The Mesh to apply the manipulator to.
		\param boundingBoxUpdate Specifies if the bounding box should be updated during manipulation.
		\return True if the functor was successfully applied, else false. */
		template <typename Functor>
		bool apply(const Functor& func, IMesh* mesh, bool boundingBoxUpdate=false) const
		{
			if (!mesh)
				return true;
			bool result = true;
			core::aabbox3df bufferbox;
			for (u32 i=0; i<mesh->getMeshBufferCount(); ++i) {
				result &= apply(func, mesh->getMeshBuffer(i), boundingBoxUpdate);
				if (boundingBoxUpdate) {
					if (0==i)
						bufferbox.reset(mesh->getMeshBuffer(i)->getBoundingBox());
					else
						bufferbox.addInternalBox(mesh->getMeshBuffer(i)->getBoundingBox());
				}
			}
			if (boundingBoxUpdate)
				mesh->setBoundingBox(bufferbox);
			return result;
		}

protected:
		//! Apply a manipulator based on the type of the functor
		/** \param func A functor defining the mesh manipulation.
		\param buffer The Meshbuffer to apply the manipulator to.
		\param boundingBoxUpdate Specifies if the bounding box should be updated during manipulation.
		\param typeTest Unused parameter, which handles the proper call selection based on the type of the Functor which is passed in two times.
		\return True if the functor was successfully applied, else false. */
		template <typename Functor>
		bool apply_(const Functor& func, IMeshBuffer* buffer, bool boundingBoxUpdate, const IVertexManipulator& typeTest) const
		{
			if (!buffer)
				return true;

			core::aabbox3df bufferbox;
			for (u32 i=0; i<buffer->getVertexCount(); ++i) {
				switch (buffer->getVertexType()) {
				case video::EVT_STANDARD:
					{
						video::S3DVertex* verts = (video::S3DVertex*)buffer->getVertices();
						func(verts[i]);
					}
					break;
				case video::EVT_2TCOORDS:
					{
						video::S3DVertex2TCoords* verts = (video::S3DVertex2TCoords*)buffer->getVertices();
						func(verts[i]);
					}
					break;
				case video::EVT_TANGENTS:
					{
						video::S3DVertexTangents* verts = (video::S3DVertexTangents*)buffer->getVertices();
						func(verts[i]);
					}
					break;
				}
				if (boundingBoxUpdate) {
					if (0==i)
						bufferbox.reset(buffer->getPosition(0));
					else
						bufferbox.addInternalPoint(buffer->getPosition(i));
				}
			}
			if (boundingBoxUpdate)
				buffer->setBoundingBox(bufferbox);
			return true;
		}
};

} // end namespace scene
} // end namespace irr


#endif
