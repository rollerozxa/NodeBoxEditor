// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_MATH_H_INCLUDED__
#define __IRR_MATH_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "irrTypes.h"
#include <math.h>
#include <float.h>
#include <stdlib.h> // for abs() etc.
#include <limits.h> // For INT_MAX / UINT_MAX

#if defined(__BORLANDC__) || defined (__BCPLUSPLUS__) || defined (_WIN32_WCE)
	#define sqrtf(X) (irr::f32)sqrt((irr::f64)(X))
	#define sinf(X) (irr::f32)sin((irr::f64)(X))
	#define cosf(X) (irr::f32)cos((irr::f64)(X))
	#define asinf(X) (irr::f32)asin((irr::f64)(X))
	#define acosf(X) (irr::f32)acos((irr::f64)(X))
	#define atan2f(X,Y) (irr::f32)atan2((irr::f64)(X),(irr::f64)(Y))
	#define ceilf(X) (irr::f32)ceil((irr::f64)(X))
	#define floorf(X) (irr::f32)floor((irr::f64)(X))
	#define powf(X,Y) (irr::f32)pow((irr::f64)(X),(irr::f64)(Y))
	#define fmodf(X,Y) (irr::f32)fmod((irr::f64)(X),(irr::f64)(Y))
	#define fabsf(X) (irr::f32)fabs((irr::f64)(X))
	#define logf(X) (irr::f32)log((irr::f64)(X))
#endif

#ifndef FLT_MAX
#define FLT_MAX 3.402823466E+38F
#endif

#ifndef FLT_MIN
#define FLT_MIN 1.17549435e-38F
#endif

namespace irr {
namespace core
{

	//! Rounding error constant often used when comparing f32 values.

	const s32 ROUNDING_ERROR_S32 = 0;

#ifdef __IRR_HAS_S64
	const s64 ROUNDING_ERROR_S64 = 0;
#endif
	const f32 ROUNDING_ERROR_f32 = 0.000001f;
	const f64 ROUNDING_ERROR_f64 = 0.00000001;

#ifdef PI // make sure we don't collide with a define
#undef PI
#endif
	//! Constant for PI.
	const f32 PI = 3.14159265359f;

	//! Constant for reciprocal of PI.
	const f32 RECIPROCAL_PI = 1.0f/PI;

	//! Constant for half of PI.
	const f32 HALF_PI = PI/2.0f;

#ifdef PI64 // make sure we don't collide with a define
#undef PI64
#endif
	//! Constant for 64bit PI.
	const f64 PI64 = 3.1415926535897932384626433832795028841971693993751;

	//! Constant for 64bit reciprocal of PI.
	const f64 RECIPROCAL_PI64 = 1.0/PI64;

	//! 32bit Constant for converting from degrees to radians
	const f32 DEGTORAD = PI / 180.0f;

	//! 32bit constant for converting from radians to degrees (formally known as GRAD_PI)
	const f32 RADTODEG   = 180.0f / PI;

	//! 64bit constant for converting from degrees to radians (formally known as GRAD_PI2)
	const f64 DEGTORAD64 = PI64 / 180.0;

	//! 64bit constant for converting from radians to degrees
	const f64 RADTODEG64 = 180.0 / PI64;

	//! Utility function to convert a radian value to degrees
	/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
	\param radians The radians value to convert to degrees.
	*/
	inline f32 radToDeg(f32 radians) {
		return RADTODEG * radians;
	}

	//! Utility function to convert a radian value to degrees
	/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
	\param radians The radians value to convert to degrees.
	*/
	inline f64 radToDeg(f64 radians) {
		return RADTODEG64 * radians;
	}

	//! Utility function to convert a degrees value to radians
	/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
	\param degrees The degrees value to convert to radians.
	*/
	inline f32 degToRad(f32 degrees) {
		return DEGTORAD * degrees;
	}

	//! Utility function to convert a degrees value to radians
	/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
	\param degrees The degrees value to convert to radians.
	*/
	inline f64 degToRad(f64 degrees) {
		return DEGTORAD64 * degrees;
	}

	//! returns minimum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& min_(const T& a, const T& b) {
		return a < b ? a : b;
	}

	//! returns minimum of three values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& min_(const T& a, const T& b, const T& c) {
		return a < b ? min_(a, c) : min_(b, c);
	}

	//! returns maximum of two values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& max_(const T& a, const T& b) {
		return a < b ? b : a;
	}

	//! returns maximum of three values. Own implementation to get rid of the STL (VS6 problems)
	template<class T>
	inline const T& max_(const T& a, const T& b, const T& c) {
		return a < b ? max_(b, c) : max_(a, c);
	}

	//! returns abs of two values. Own implementation to get rid of STL (VS6 problems)
	template<class T>
	inline T abs_(const T& a) {
		return a < (T)0 ? -a : a;
	}

	//! returns linear interpolation of a and b with ratio t
	//! \return: a if t==0, b if t==1, and the linear interpolation else
	template<class T>
	inline T lerp(const T& a, const T& b, const f32 t) {
		return (T)(a*(1.f-t)) + (b*t);
	}

	//! clamps a value between low and high
	template <class T>
	inline const T clamp (const T& value, const T& low, const T& high) {
		return min_ (max_(value,low), high);
	}

	//! swaps the content of the passed parameters
	// Note: We use the same trick as boost and use two template arguments to
	// avoid ambiguity when swapping objects of an Irrlicht type that has not
	// it's own swap overload. Otherwise we get conflicts with some compilers
	// in combination with stl.
	template <class T1, class T2>
	inline void swap(T1& a, T2& b) {
		T1 c(a);
		a = b;
		b = c;
	}

	template <class T>
	inline T roundingError();

	template <>
	inline f32 roundingError() {
		return ROUNDING_ERROR_f32;
	}

	template <>
	inline f64 roundingError() {
		return ROUNDING_ERROR_f64;
	}

	template <>
	inline s32 roundingError() {
		return ROUNDING_ERROR_S32;
	}

	template <>
	inline u32 roundingError() {
		return ROUNDING_ERROR_S32;
	}

#ifdef __IRR_HAS_S64
	template <>
	inline s64 roundingError() {
		return ROUNDING_ERROR_S64;
	}

	template <>
	inline u64 roundingError() {
		return ROUNDING_ERROR_S64;
	}
#endif

	template <class T>
	inline T relativeErrorFactor() {
		return 1;
	}

	template <>
	inline f32 relativeErrorFactor() {
		return 4;
	}

	template <>
	inline f64 relativeErrorFactor() {
		return 8;
	}

	//! returns if a equals b, taking possible rounding errors into account
	template <class T>
	inline bool equals(const T a, const T b, const T tolerance = roundingError<T>()) {
		return (a + tolerance >= b) && (a - tolerance <= b);
	}


	//! returns if a equals b, taking relative error in form of factor
	//! this particular function does not involve any division.
	template <class T>
	inline bool equalsRelative( const T a, const T b, const T factor = relativeErrorFactor<T>()) {
		//https://eagergames.wordpress.com/2017/04/01/fast-parallel-lines-and-vectors-test/

		const T maxi = max_( a, b);
		const T mini = min_( a, b);
		const T maxMagnitude = max_( maxi, -mini);

		return	(maxMagnitude*factor + maxi) == (maxMagnitude*factor + mini); // MAD Wise
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const f64 a, const f64 tolerance = ROUNDING_ERROR_f64) {
		return fabs(a) <= tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const f32 a, const f32 tolerance = ROUNDING_ERROR_f32) {
		return fabsf(a) <= tolerance;
	}

	//! returns if a equals not zero, taking rounding errors into account
	inline bool isnotzero(const f32 a, const f32 tolerance = ROUNDING_ERROR_f32) {
		return fabsf(a) > tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const s32 a, const s32 tolerance = 0) {
		return ( a & 0x7ffffff ) <= tolerance;
	}

	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const u32 a, const u32 tolerance = 0) {
		return a <= tolerance;
	}

#ifdef __IRR_HAS_S64
	//! returns if a equals zero, taking rounding errors into account
	inline bool iszero(const s64 a, const s64 tolerance = 0) {
		return abs_(a) <= tolerance;
	}
#endif

	inline s32 s32_min(s32 a, s32 b) {
		const s32 mask = (a - b) >> 31;
		return (a & mask) | (b & ~mask);
	}

	inline s32 s32_max(s32 a, s32 b) {
		const s32 mask = (a - b) >> 31;
		return (b & mask) | (a & ~mask);
	}

	inline s32 s32_clamp (s32 value, s32 low, s32 high) {
		return s32_min(s32_max(value,low), high);
	}

	/*
		float IEEE-754 bit representation

		0      0x00000000
		1.0    0x3f800000
		0.5    0x3f000000
		3      0x40400000
		+inf   0x7f800000
		-inf   0xff800000
		+NaN   0x7fc00000 or 0x7ff00000
		in general: number = (sign ? -1:1) * 2^(exponent) * 1.(mantissa bits)
	*/

	typedef union { u32 u; s32 s; f32 f; } inttofloat;

	#define F32_AS_S32(f)		(*((s32 *) &(f)))
	#define F32_AS_U32(f)		(*((u32 *) &(f)))
	#define F32_AS_U32_POINTER(f)	( ((u32 *) &(f)))

	#define F32_VALUE_0		0x00000000
	#define F32_VALUE_1		0x3f800000
	#define F32_SIGN_BIT		0x80000000U
	#define F32_EXPON_MANTISSA	0x7FFFFFFFU

	//! code is taken from IceFPU
	//! Integer representation of a floating-point value.
	inline u32 IR(f32 x) {inttofloat tmp; tmp.f=x; return tmp.u;}

	//! Absolute integer representation of a floating-point value
	#define AIR(x)			(IR(x)&0x7fffffff)

	//! Floating-point representation of an integer value.
	inline f32 FR(u32 x) {inttofloat tmp; tmp.u=x; return tmp.f;}
	inline f32 FR(s32 x) {inttofloat tmp; tmp.s=x; return tmp.f;}

	//! integer representation of 1.0
	#define IEEE_1_0		0x3f800000
	//! integer representation of 255.0
	#define IEEE_255_0		0x437f0000

	#define	F32_LOWER_0(n)		((n) <  0.0f)
	#define	F32_LOWER_EQUAL_0(n)	((n) <= 0.0f)
	#define	F32_GREATER_0(n)	((n) >  0.0f)
	#define	F32_GREATER_EQUAL_0(n)	((n) >= 0.0f)
	#define	F32_EQUAL_1(n)		((n) == 1.0f)
	#define	F32_EQUAL_0(n)		((n) == 0.0f)
	#define	F32_A_GREATER_B(a,b)	((a) > (b))

#ifndef REALINLINE
	#ifdef _MSC_VER
		#define REALINLINE __forceinline
	#else
		#define REALINLINE inline
	#endif
#endif

	// NOTE: This is not as exact as the c99/c++11 round function, especially at high numbers starting with 8388609
	//       (only low number which seems to go wrong is 0.49999997 which is rounded to 1)
	//      Also negative 0.5 is rounded up not down unlike with the standard function (p.E. input -0.5 will be 0 and not -1)
	inline f32 round_( f32 x ) {
		return floorf( x + 0.5f );
	}

	// calculate: sqrt ( x )
	REALINLINE f32 squareroot(const f32 f) {
		return sqrtf(f);
	}

	// calculate: sqrt ( x )
	REALINLINE f64 squareroot(const f64 f) {
		return sqrt(f);
	}

	// calculate: sqrt ( x )
	REALINLINE s32 squareroot(const s32 f) {
		return static_cast<s32>(squareroot(static_cast<f32>(f)));
	}

#ifdef __IRR_HAS_S64
	// calculate: sqrt ( x )
	REALINLINE s64 squareroot(const s64 f) {
		return static_cast<s64>(squareroot(static_cast<f64>(f)));
	}
#endif

	// calculate: 1 / sqrt ( x )
	REALINLINE f64 reciprocal_squareroot(const f64 x) {
		return 1.0 / sqrt(x);
	}

	// calculate: 1 / sqrtf ( x )
	REALINLINE f32 reciprocal_squareroot(const f32 f) {
		return 1.f / sqrtf(f);
	}

	// calculate: 1 / sqrtf( x )
	REALINLINE s32 reciprocal_squareroot(const s32 x) {
		return static_cast<s32>(reciprocal_squareroot(static_cast<f32>(x)));
	}

	// calculate: 1 / x
	REALINLINE f32 reciprocal( const f32 f ) {
		return 1.f / f;
	}

	// calculate: 1 / x
	REALINLINE f64 reciprocal ( const f64 f ) {
		return 1.0 / f;
	}

	REALINLINE s32 floor32(f32 x) {
		return (s32) floorf ( x );
	}

	REALINLINE s32 ceil32 ( f32 x ) {
		return (s32) ceilf ( x );
	}

	// NOTE: Please check round_ documentation about some inaccuracies in this compared to standard library round function.
	REALINLINE s32 round32(f32 x) {
		return (s32) round_(x);
	}

	inline f32 fract ( f32 x ) {
		return x - floorf ( x );
	}

} // end namespace core
} // end namespace irr

	using irr::core::IR;
	using irr::core::FR;

#endif