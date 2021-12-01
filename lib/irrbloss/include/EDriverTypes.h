// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_DRIVER_TYPES_H_INCLUDED__
#define __E_DRIVER_TYPES_H_INCLUDED__

#include "irrTypes.h"

namespace irr {
namespace video {

	//! An enum for all types of drivers the Irrlicht Engine supports.
	enum E_DRIVER_TYPE {
		//! Null driver, useful for applications to run the engine without visualization.
		EDT_NULL,

		//! OpenGL device, available on most platforms.
		EDT_OPENGL,

		//! No driver, just for counting the elements
		EDT_COUNT
	};

} // end namespace video
} // end namespace irr

#endif
