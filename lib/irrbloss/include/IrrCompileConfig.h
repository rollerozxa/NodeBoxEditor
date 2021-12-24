// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_COMPILE_CONFIG_H_INCLUDED__
#define __IRR_COMPILE_CONFIG_H_INCLUDED__

//! Identifies the IrrlichtMt fork customized for the Minetest engine
#define IRRLICHT_VERSION_MT_REVISION 3
#define IRRLICHT_VERSION_MT "mt3"

//! Irrlicht SDK Version
#define IRRLICHT_VERSION_MAJOR 1
#define IRRLICHT_VERSION_MINOR 9
#define IRRLICHT_VERSION_REVISION 0
// This flag will be defined only in SVN, the official release code will have
// it undefined
#define IRRLICHT_VERSION_SVN alpha
#define IRRLICHT_SDK_VERSION "1.9.0" IRRLICHT_VERSION_MT

#include <stdio.h> // TODO: Although included elsewhere this is required at least for mingw

//! The defines for different operating system are:
//! _IRR_WINDOWS_ for all irrlicht supported Windows versions
//! _IRR_WINDOWS_API_ for Windows
//! _IRR_LINUX_PLATFORM_ for Linux (it is defined here if no other os is defined)
//! _IRR_SOLARIS_PLATFORM_ for Solaris
//! _IRR_OSX_PLATFORM_ for Apple systems running OSX
//! _IRR_POSIX_API_ for Posix compatible systems
//! Note: PLATFORM defines the OS specific layer, API can group several platforms

//! DEVICE is the windowing system used, several PLATFORMs support more than one DEVICE
//! Irrlicht can be compiled with more than one device
//! _IRR_COMPILE_WITH_WINDOWS_DEVICE_ for Windows API based device
//! _IRR_COMPILE_WITH_OSX_DEVICE_ for Cocoa native windowing on OSX
//! _IRR_COMPILE_WITH_X11_DEVICE_ for Linux X11 based device

//! Passing defines to the compiler which have NO in front of the _IRR definename is an alternative
//! way which can be used to disable defines (instead of outcommenting them in this header).
//! So defines can be controlled from Makefiles or Projectfiles which allows building
//! different library versions without having to change the sources.
//! Example: NO_IRR_COMPILE_WITH_X11_ would disable X11

//! WIN32 for Windows32
//! WIN64 for Windows64
// The windows platform and API support SDL and WINDOW device
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#define _IRR_WINDOWS_
#define _IRR_WINDOWS_API_
#define _IRR_COMPILE_WITH_WINDOWS_DEVICE_
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1500)
#  error "Only Microsoft Visual Studio 9.0 and later are supported."
#endif

#if defined(__SVR4) && defined(__sun)
#define _IRR_SOLARIS_PLATFORM_
#if defined(__sparc)
	#define __BIG_ENDIAN__
#endif
#endif

#if !defined(_IRR_WINDOWS_API_)
#ifndef _IRR_SOLARIS_PLATFORM_
#define _IRR_LINUX_PLATFORM_
#endif
#define _IRR_POSIX_API_
#define _IRR_COMPILE_WITH_X11_DEVICE_
#endif

//! Maximum number of texture an SMaterial can have, up to 8 are supported by Irrlicht.
#define _IRR_MATERIAL_MAX_TEXTURES_ 8

//! Define _IRR_COMPILE_WITH_OPENGL_ to compile the Irrlicht engine with OpenGL.
/** If you do not wish the engine to be compiled with OpenGL, comment this
define out. */
#define _IRR_COMPILE_WITH_OPENGL_
#ifdef NO_IRR_COMPILE_WITH_OPENGL_
#undef _IRR_COMPILE_WITH_OPENGL_
#endif

//! Define required options for OpenGL drivers.
#if defined(_IRR_COMPILE_WITH_OPENGL_)
	#if defined(_IRR_COMPILE_WITH_WINDOWS_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
		#define _IRR_COMPILE_WITH_WGL_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
		#define _IRR_COMPILE_WITH_GLX_MANAGER_
	#elif defined(_IRR_SOLARIS_PLATFORM_)
		#define _IRR_COMPILE_WITH_GLX_MANAGER_
	#elif defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
		#define _IRR_OPENGL_USE_EXTPOINTER_
	#endif
#endif


//! Define _IRR_COMPILE_WITH_X11_ to compile the Irrlicht engine with X11 support.
/** If you do not wish the engine to be compiled with X11, comment this
define out. */
// Only used in LinuxDevice.
#define _IRR_COMPILE_WITH_X11_
#ifdef NO_IRR_COMPILE_WITH_X11_
#undef _IRR_COMPILE_WITH_X11_
#endif

//! On some Linux systems the XF86 vidmode extension, X11 RandR, or XInput2 are missing.
//! Use these defines to add/remove support for those dependencies as needed.
//! XInput2 (library called Xi) is currently only used to support touch-input.
#if defined(_IRR_LINUX_PLATFORM_) && defined(_IRR_COMPILE_WITH_X11_)
#define _IRR_LINUX_X11_VIDMODE_
//#define _IRR_LINUX_X11_RANDR_
//#define _IRR_LINUX_X11_XINPUT2_
#ifdef NO_IRR_LINUX_X11_VIDMODE_
#undef _IRR_LINUX_X11_VIDMODE_
#endif
#ifdef NO_IRR_LINUX_X11_RANDR_
#undef _IRR_LINUX_X11_RANDR_
#endif
#ifdef NO_IRR_LINUX_X11_XINPUT2_
#undef _IRR_LINUX_X11_XINPUT2_
#endif

//! X11 has by default only monochrome cursors, but using the Xcursor library we can also get color cursor support.
//! If you have the need for custom color cursors on X11 then enable this and make sure you also link
//! to the Xcursor library in your Makefile/Projectfile.
//#define _IRR_LINUX_XCURSOR_
#ifdef NO_IRR_LINUX_XCURSOR_
#undef _IRR_LINUX_XCURSOR_
#endif

#endif

//! Define _IRR_COMPILE_WITH_GUI_ to compile the engine with the built-in GUI
/** Disable this if you are using an external library to draw the GUI. If you disable this then
you will not be able to use anything provided by the GUI Environment, including loading fonts. */
#define _IRR_COMPILE_WITH_GUI_
#ifdef NO_IRR_COMPILE_WITH_GUI_
#undef _IRR_COMPILE_WITH_GUI_
#endif

// Some cleanup and standard stuff

#ifdef _IRR_WINDOWS_API_

// To build Irrlicht as a static library, you must define _IRR_STATIC_LIB_ in both the
// Irrlicht build, *and* in the user application, before #including <irrlicht.h>
#ifndef _IRR_STATIC_LIB_
#ifdef IRRLICHT_EXPORTS
#define IRRLICHT_API __declspec(dllexport)
#else
#define IRRLICHT_API __declspec(dllimport)
#endif // IRRLICHT_EXPORT
#else
#define IRRLICHT_API
#endif // _IRR_STATIC_LIB_

// Declare the calling convention.
#if defined(_STDCALL_SUPPORTED)
#define IRRCALLCONV __stdcall
#else
#define IRRCALLCONV __cdecl
#endif // STDCALL_SUPPORTED

#else // _IRR_WINDOWS_API_

// Force symbol export in shared libraries built with gcc.
#if (__GNUC__ >= 4) && !defined(_IRR_STATIC_LIB_) && defined(IRRLICHT_EXPORTS)
#define IRRLICHT_API __attribute__ ((visibility("default")))
#else
#define IRRLICHT_API
#endif

#define IRRCALLCONV

#endif // _IRR_WINDOWS_API_

//! Define __IRR_HAS_S64 if the irr::s64 type should be enable (needs long long, available on most platforms, but not part of ISO C++ 98)
#define __IRR_HAS_S64
#ifdef NO__IRR_HAS_S64
#undef __IRR_HAS_S64
#endif

#if defined(__BORLANDC__)
	#include <tchar.h>

	// Borland 5.5.1
	#if __BORLANDC__ == 0x551
		#undef _tfinddata_t
		#undef _tfindfirst
		#undef _tfindnext

		#define _tfinddata_t __tfinddata_t
		#define _tfindfirst  __tfindfirst
		#define _tfindnext   __tfindnext
		typedef long intptr_t;
	#endif
#endif

#ifndef __has_feature
  #define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif

#endif // __IRR_COMPILE_CONFIG_H_INCLUDED__

