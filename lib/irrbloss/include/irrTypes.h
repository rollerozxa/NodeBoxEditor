// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_TYPES_H_INCLUDED__
#define __IRR_TYPES_H_INCLUDED__

#include "IrrCompileConfig.h"
#include <stdint.h>

namespace irr {

//! 8 bit unsigned variable.
typedef uint8_t			u8;

//! 8 bit signed variable.
typedef int8_t			s8;

//! 8 bit character variable.
/** This is a typedef for char, it ensures portability of the engine. */
typedef char			c8;



//! 16 bit unsigned variable.
typedef uint16_t		u16;

//! 16 bit signed variable.
typedef int16_t			s16;



//! 32 bit unsigned variable.
typedef uint32_t 		u32;

//! 32 bit signed variable.
typedef int32_t			s32;


#ifdef __IRR_HAS_S64
//! 64 bit unsigned variable.
typedef uint64_t		u64;

//! 64 bit signed variable.
typedef int64_t			s64;
#endif	// __IRR_HAS_S64



//! 32 bit floating point variable.
/** This is a typedef for float, it ensures portability of the engine. */
typedef float				f32;

//! 64 bit floating point variable.
/** This is a typedef for double, it ensures portability of the engine. */
typedef double				f64;


} // end namespace irr


#include <wchar.h>
#ifdef _IRR_WINDOWS_API_
//! Defines for s{w,n}printf_irr because s{w,n}printf methods do not match the ISO C
//! standard on Windows platforms.
//! We want int snprintf_irr(char *str, size_t size, const char *format, ...);
//! and int swprintf_irr(wchar_t *wcs, size_t maxlen, const wchar_t *format, ...);
#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
#define swprintf_irr swprintf_s
#define snprintf_irr sprintf_s
#elif !defined(__CYGWIN__)
#define swprintf_irr _snwprintf
#define snprintf_irr _snprintf
#endif

// define the wchar_t type if not already built in.
#ifdef _MSC_VER
#ifndef _WCHAR_T_DEFINED
//! A 16 bit wide character type.
/**
	Defines the wchar_t-type.
	In VS6, its not possible to tell
	the standard compiler to treat wchar_t as a built-in type, and
	sometimes we just don't want to include the huge stdlib.h or wchar.h,
	so we'll use this.
*/
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif // wchar is not defined
#endif // microsoft compiler
#else
#define swprintf_irr swprintf
#define snprintf_irr snprintf
#endif // _IRR_WINDOWS_API_

namespace irr {

//! Type name for character type used by the file system.
/** Should the wide character version of the FileSystem be used it is a
16 bit character variable. Used for Unicode Filesystem and Unicode strings.
Else it is a 8 bit character variable. Used for ansi Filesystem and non-unicode
strings
*/
typedef char fschar_t;
#define _IRR_TEXT(X) X

} // end namespace irr

//! define a break macro for debugging.
#if defined(_DEBUG)
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && !defined (_WIN32_WCE)
#if defined(WIN64) || defined(_WIN64) // using portable common solution for x64 configuration
	#include <crtdbg.h>
	#define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
#else
	#define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_asm int 3}
#endif
#else
	#include "assert.h"
	#define _IRR_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#endif
#else
	#define _IRR_DEBUG_BREAK_IF( _CONDITION_ )
#endif

//! Defines a deprecated macro which generates a warning at compile time
/** The usage is simple
For typedef:		typedef _IRR_DEPRECATED_ int test1;
For classes/structs:	class _IRR_DEPRECATED_ test2 { ... };
For methods:		class test3 { _IRR_DEPRECATED_ virtual void foo() {} };
For functions:		template<class T> _IRR_DEPRECATED_ void test4(void) {}
**/
#if defined(IGNORE_DEPRECATED_WARNING)
#define _IRR_DEPRECATED_
#elif _MSC_VER >= 1310 //vs 2003 or higher
#define _IRR_DEPRECATED_ __declspec(deprecated)
#elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)) // all versions above 3.0 should support this feature
#define _IRR_DEPRECATED_  __attribute__ ((deprecated))
#else
#define _IRR_DEPRECATED_
#endif

//! Defines an override macro, to protect virtual functions from typos and other mismatches
/** Usage in a derived class:
virtual void somefunc() _IRR_OVERRIDE_;
*/
#if ( ((__GNUC__ > 4 ) || ((__GNUC__ == 4 ) && (__GNUC_MINOR__ >= 7))) && (defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L) )
#define _IRR_OVERRIDE_ override
#elif (_MSC_VER >= 1600 ) /* supported since MSVC 2010 */
#define _IRR_OVERRIDE_ override
#elif (__clang_major__ >= 3 && __has_feature(cxx_override_control))
#define _IRR_OVERRIDE_ override
#else
#define _IRR_OVERRIDE_
#endif

// memory debugging
#if defined(_DEBUG) && defined(IRRLICHT_EXPORTS) && defined(_MSC_VER) && \
	(_MSC_VER > 1299) && !defined(_IRR_DONT_DO_MEMORY_DEBUGGING_HERE) && !defined(_WIN32_WCE)

	#define CRTDBG_MAP_ALLOC
	#define _CRTDBG_MAP_ALLOC
	#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
	#include <stdlib.h>
	#include <crtdbg.h>
	#define new DEBUG_CLIENTBLOCK
#endif

//! ignore VC8 warning deprecated
/** The Microsoft compiler */
#if defined(_IRR_WINDOWS_API_) && defined(_MSC_VER) && (_MSC_VER >= 1400)
	//#pragma warning( disable: 4996)
	//#define _CRT_SECURE_NO_DEPRECATE 1
	//#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif


//! creates four CC codes used in Irrlicht for simple ids
/** some compilers can create those by directly writing the
code like 'code', but some generate warnings so we use this macro here */
#define MAKE_IRR_ID(c0, c1, c2, c3) \
		((irr::u32)(irr::u8)(c0) | ((irr::u32)(irr::u8)(c1) << 8) | \
		((irr::u32)(irr::u8)(c2) << 16) | ((irr::u32)(irr::u8)(c3) << 24 ))

#endif // __IRR_TYPES_H_INCLUDED__
