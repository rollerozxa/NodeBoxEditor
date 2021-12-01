// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E_DEVICE_TYPES_H_INCLUDED__
#define __E_DEVICE_TYPES_H_INCLUDED__

namespace irr {
	//! An enum for the different device types supported by the Irrlicht Engine.
	enum E_DEVICE_TYPE
	{
		//! A device native to Microsoft Windows
		EIDT_WIN32,

		//! A device native to Unix style operating systems.
		EIDT_X11,

		//! A device native to Mac OSX
		EIDT_OSX,

		//! This selection allows Irrlicht to choose the best device from the ones available.
		EIDT_BEST,
	};

} // end namespace irr

#endif // __E_DEVICE_TYPES_H_INCLUDED__
