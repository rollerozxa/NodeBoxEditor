// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SHADER_CONSTANT_SET_CALLBACT_H_INCLUDED__
#define __I_SHADER_CONSTANT_SET_CALLBACT_H_INCLUDED__

#include "IReferenceCounted.h"

namespace irr {
namespace video
{
	class IMaterialRendererServices;
	class SMaterial;

//! Interface making it possible to set constants for gpu programs every frame.
/** Implement this interface in an own class and pass a pointer to it to one of
the methods in IGPUProgrammingServices when creating a shader. The
OnSetConstants method will be called every frame now. */
class IShaderConstantSetCallBack : public virtual IReferenceCounted {
public:

	//! Called to let the callBack know the used material (optional method)
	virtual void OnSetMaterial(const SMaterial& material) { }

	//! Called by the engine when the vertex and/or pixel shader constants for an material renderer should be set.
	/**
	Implement the IShaderConstantSetCallBack in an own class and implement your own
	OnSetConstants method using the given IMaterialRendererServices interface.
	Pass a pointer to this class to one of the methods in IGPUProgrammingServices
	when creating a shader. The OnSetConstants method will now be called every time
	before geometry is being drawn using your shader material.
	\param services: Pointer to an interface providing methods to set the constants for the shader.
	\param userData: Userdata int which can be specified when creating the shader.
	*/
	virtual void OnSetConstants(IMaterialRendererServices* services, s32 userData) = 0;
};


} // end namespace video
} // end namespace irr

#endif

