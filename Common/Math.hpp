/** \file
\brief Mathematical module
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Math \n
Module components: \ref code_math
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_MATH_H_
#define COMMON_MATH_H_

#ifdef WIN32
	#include <windows.h> // bo bez tego w d3d9types.h s¹ b³êdy bo nie zna DWORD itp., poza tym potrzebny jest Point itd.
#endif

#ifdef USE_DIRECTX
	#include <d3dx9.h>
#endif

#include <functional>

namespace common
{

/** \addtogroup code_math Math Module
Dokumentacja: \ref Module_Math \n
Nag³ówek: Math.hpp */
//@{

/// Szablon bazowy do konwersji miêdzy typami matematycznymi
/** Ta wersja nie dzia³a, specjalizacje bêd¹ dzia³aæ. */
template <typename DestT, typename SrcT>
DestT & math_cast(const SrcT &x)
{
	assert(0 && "Unsupported type in math_cast.");
}

/// Ma³a sta³a
const float FLOAT_EPSILON = 1e-6f;


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_point Punkt 2D z liczb ca³kowitych */
//@{

/// Punkt 2D z liczb ca³kowitych
/**
Kompatybilny ze struktur¹ POINT z Win32API.
Nazywa siê tak dziwnie, bo POINT to w Windows API marko i robi problem z nazw¹.
Konwersja do i ze stringa: "x,y", np. "10,1".
*/
struct POINT_
{
	union
	{
		struct
		{
			int x, y;
		};
		int v[2];
	};

	static const POINT_ ZERO;

	POINT_() { }
	POINT_(int x, int y) : x(x), y(y) { }

	bool operator == (const POINT_ &v) const { return x == v.x && y == v.y; }
	bool operator != (const POINT_ &v) const { return x != v.x || y != v.y; }

	POINT_ operator - () const { return POINT_(-x, -y); }
	POINT_ operator + (const POINT_ &v) const { return POINT_(x+v.x, y+v.y); }
	POINT_ operator - (const POINT_ &v) const { return POINT_(x-v.x, y-v.y); }
	POINT_ & operator += (const POINT_ &v) { x += v.x; y += v.y; return *this; }
	POINT_ & operator -= (const POINT_ &v) { x -= v.x; y -= v.y; return *this; }
	POINT_ & operator *= (int v) { x *= v; y *= v; return *this; }
	POINT_ & operator /= (int v) { x /= v; y /= v; return *this; }

	int & operator [] (uint32 Index) { return v[Index]; }
	const int & operator [] (uint32 Index) const { return v[Index]; }

	int * GetArray() { return &x; }
	const int * GetArray() const { return &x; }
};

inline POINT_ operator * (const POINT_ &pt, int v) { return POINT_(pt.x*v, pt.y*v); }
inline POINT_ operator / (const POINT_ &pt, int v) { return POINT_(pt.x/v, pt.y/v); }
inline POINT_ operator * (int v, const POINT_ &pt) { return POINT_(pt.x*v, pt.y*v); }
inline POINT_ operator / (int v, const POINT_ &pt) { return POINT_(pt.x/v, pt.y/v); }

#ifdef WIN32
	/// Konwersja z common::POINT_ do POINT
	template <> inline ::POINT & math_cast<::POINT, POINT_>(const POINT_ &x) { return (::POINT&)(x); }
	/// Konwersja z POINT do common::POINT_
	template <> inline POINT_ & math_cast<POINT_, ::POINT>(const ::POINT &x) { return (POINT_&)(x); }
#endif

extern const common::POINT_ POINT_ZERO;
extern const common::POINT_ POINT_ONE;
extern const common::POINT_ POINT_NEGATIVE_X;
extern const common::POINT_ POINT_NEGATIVE_Y;
extern const common::POINT_ POINT_POSITIVE_X;
extern const common::POINT_ POINT_POSITIVE_Y;

inline void swap(POINT_ &v1, POINT_ &v2)
{
	std::swap(v1.x, v2.x);
	std::swap(v1.y, v2.y);
}

inline void Mul(POINT_ *out, const POINT_ &p1, const POINT_ &p2)
{
	out->x = p1.x * p2.x;
	out->y = p1.y * p2.y;
}
inline void Div(POINT_ *out, const POINT_ &p1, const POINT_ &p2)
{
	out->x = p1.x / p2.x;
	out->y = p1.y / p2.y;
}

inline void Mul(POINT_ *inout, const POINT_ &v)
{
	inout->x *= v.x;
	inout->y *= v.y;
}
inline void Div(POINT_ *inout, const POINT_ &v)
{
	inout->x /= v.x;
	inout->y /= v.y;
}

inline void Min(common::POINT_ *out, const common::POINT_ &a, const common::POINT_ &b)
{
	out->x = std::min(a.x, b.x);
	out->y = std::min(a.y, b.y);
}

inline void Max(common::POINT_ *out, const common::POINT_ &a, const common::POINT_ &b)
{
	out->x = std::max(a.x, b.x);
	out->y = std::max(a.y, b.y);
}

//@}
// math_point

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_vec Wektor floatów 2D, 3D, 4D */
//@{

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
	typedef D3DXVECTOR2 VEC2;
#else
	/// Wektor floatów 2D
	struct VEC2
	{
		float x, y;

		VEC2() { }
		VEC2(float x, float y) : x(x), y(y) { }
		VEC2(const float *Array) : x(Array[0]), y(Array[1]) { }

		operator float * () { return &x; }
		operator const float * () const { return &x; }

		bool operator == (const VEC2 &v) const { return x == v.x && y == v.y; }
		bool operator != (const VEC2 &v) const { return x != v.x || y != v.y; }

		VEC2 operator + () const { return *this; }
		VEC2 operator - () const { return VEC2(-x, -y); }
		VEC2 operator + (const VEC2 &v) const { return VEC2(x+v.x, y+v.y); }
		VEC2 operator - (const VEC2 &v) const { return VEC2(x-v.x, y-v.y); }
		VEC2 & operator += (const VEC2 &v) { x += v.x; y += v.y; return *this; }
		VEC2 & operator -= (const VEC2 &v) { x -= v.x; y -= v.y; return *this; }
		VEC2 & operator *= (float v) { x *= v; y *= v; return *this; }
		VEC2 & operator /= (float v) { float vInv = 1.0f/v; x *= vInv; y *= vInv; return *this; }
	};
	inline VEC2 operator * (const VEC2 &pt, float v) { return VEC2(pt.x*v, pt.y*v); }
	inline VEC2 operator / (const VEC2 &pt, float v) { float vInv = 1.0f/v; return VEC2(pt.x*vInv, pt.y*vInv); }
	inline VEC2 operator * (float v, const VEC2 &pt) { return VEC2(pt.x*v, pt.y*v); }
#endif

inline void swap(VEC2 &v1, VEC2 &v2)
{
	std::swap(v1.x, v2.x);
	std::swap(v1.y, v2.y);
}

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
	typedef D3DXVECTOR3 VEC3;
#else
	/// Wektor floatów 3D
	struct VEC3
	{
		float x, y, z;

		VEC3() { }
		VEC3(float x, float y, float z) : x(x), y(y), z(z) { }
		VEC3(const float *Array) : x(Array[0]), y(Array[1]), z(Array[2]) { }

		operator float * () { return &x; }
		operator const float * () const { return &x; }

		bool operator == (const VEC3 &v) const { return x == v.x && y == v.y && z == v.z; }
		bool operator != (const VEC3 &v) const { return x != v.x || y != v.y || z != v.z; }

		VEC3 operator + () const { return *this; }
		VEC3 operator - () const { return VEC3(-x, -y, -z); }
		VEC3 operator + (const VEC3 &v) const { return VEC3(x+v.x, y+v.y, z+v.z); }
		VEC3 operator - (const VEC3 &v) const { return VEC3(x-v.x, y-v.y, z-v.z); }
		VEC3 & operator += (const VEC3 &v) { x += v.x; y += v.y; z += v.z; return *this; }
		VEC3 & operator -= (const VEC3 &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		VEC3 & operator *= (float v) { x *= v; y *= v; z *= v; return *this; }
		VEC3 & operator /= (float v) { float vInv = 1.0f/v; x *= vInv; y *= vInv; z *= vInv; return *this; }
	};
	inline VEC3 operator * (const VEC3 &pt, float v) { return VEC3(pt.x*v, pt.y*v, pt.z*v); }
	inline VEC3 operator / (const VEC3 &pt, float v) { float vInv = 1.0f/v; return VEC3(pt.x*vInv, pt.y*vInv, pt.z*vInv); }
	inline VEC3 operator * (float v, const VEC3 &pt) { return VEC3(pt.x*v, pt.y*v, pt.z*v); }
#endif

inline void swap(VEC3 &v1, VEC3 &v2)
{
	std::swap(v1.x, v2.x);
	std::swap(v1.y, v2.y);
	std::swap(v1.z, v2.z);
}

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
	typedef D3DXVECTOR4 VEC4;
#else
	/// Wektor floatów 4D
	struct VEC4
	{
		float x, y, z, w;

		VEC4() { }
		VEC4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
		VEC4(const float *Array) : x(Array[0]), y(Array[1]), z(Array[2]), w(Array[3]) { }

		operator float * () { return &x; }
		operator const float * () const { return &x; }

		bool operator == (const VEC4 &v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
		bool operator != (const VEC4 &v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

		VEC4 operator + () const { return *this; }
		VEC4 operator - () const { return VEC4(-x, -y, -z, -w); }
		VEC4 operator + (const VEC4 &v) const { return VEC4(x+v.x, y+v.y, z+v.z, w+v.w); }
		VEC4 operator - (const VEC4 &v) const { return VEC4(x-v.x, y-v.y, z-v.z, w-v.w); }
		VEC4 & operator += (const VEC4 &v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		VEC4 & operator -= (const VEC4 &v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		VEC4 & operator *= (float v) { x *= v; y *= v; z *= v; w *= v; return *this; }
		VEC4 & operator /= (float v) { float vInv = 1.0f/v; x *= vInv; y *= vInv; z *= vInv; w *= vInv; return *this; }
	};
	inline VEC4 operator * (const VEC4 &pt, float v) { return VEC4(pt.x*v, pt.y*v, pt.z*v, pt.w*v); }
	inline VEC4 operator / (const VEC4 &pt, float v) { float vInv = 1.0f/v; return VEC4(pt.x*vInv, pt.y*vInv, pt.z*vInv, pt.w*vInv); }
	inline VEC4 operator * (float v, const VEC4 &pt) { return VEC4(pt.x*v, pt.y*v, pt.z*v, pt.w*v); }
#endif

inline void swap(VEC4 &v1, VEC4 &v2)
{
	std::swap(v1.x, v2.x);
	std::swap(v1.y, v2.y);
	std::swap(v1.z, v2.z);
	std::swap(v1.w, v2.w);
}

extern const VEC2 VEC2_ZERO;
extern const VEC2 VEC2_ONE;
extern const VEC2 VEC2_POSITIVE_X;
extern const VEC2 VEC2_POSITIVE_Y;
extern const VEC2 VEC2_NEGATIVE_X;
extern const VEC2 VEC2_NEGATIVE_Y;

extern const VEC3 VEC3_ZERO;
extern const VEC3 VEC3_ONE;
extern const VEC3 VEC3_POSITIVE_X;
extern const VEC3 VEC3_POSITIVE_Y;
extern const VEC3 VEC3_POSITIVE_Z;
extern const VEC3 VEC3_NEGATIVE_X;
extern const VEC3 VEC3_NEGATIVE_Y;
extern const VEC3 VEC3_NEGATIVE_Z;

extern const VEC4 VEC4_ZERO;
extern const VEC4 VEC4_ONE;
extern const VEC4 VEC4_POSITIVE_X;
extern const VEC4 VEC4_POSITIVE_Y;
extern const VEC4 VEC4_POSITIVE_Z;
extern const VEC4 VEC4_POSITIVE_W;
extern const VEC4 VEC4_NEGATIVE_X;
extern const VEC4 VEC4_NEGATIVE_Y;
extern const VEC4 VEC4_NEGATIVE_Z;
extern const VEC4 VEC4_NEGATIVE_W;

/// Zwraca kwadrat d³ugoœci wektora
inline float LengthSq(const VEC2 &v)
{
	return v.x*v.x + v.y*v.y;
}
/// \copydoc LengthSq(const VEC2&)
inline float LengthSq(const VEC3 &v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z;
}
/// \copydoc LengthSq(const VEC2&)
inline float LengthSq(const VEC4 &v)
{
	return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
}

/// Zwraca d³ugoœæ wektora
inline float Length(const VEC2 &v)
{
	return sqrtf(LengthSq(v));
}
/// \copydoc Length(const VEC2&)
inline float Length(const VEC3 &v)
{
	return sqrtf(LengthSq(v));
}
/// \copydoc Length(const VEC2&)
inline float Length(const VEC4 &v)
{
	return sqrtf(LengthSq(v));
}

inline float ManhattanLength(const VEC2 &v)
{
	return fabsf(v.x) + fabsf(v.y);
}
inline float ManhattanLength(const VEC3 &v)
{
	return fabsf(v.x) + fabsf(v.y) + fabsf(v.z);
}
inline float ManhattanLength(const VEC4 &v)
{
	return fabsf(v.x) + fabsf(v.y) + fabsf(v.z) + fabsf(v.z);
}

inline float MaxLength(const VEC2 &v)
{
	return std::max(fabsf(v.x), fabsf(v.y));
}
inline float MaxLength(const VEC3 &v)
{
	return max3(fabsf(v.x), fabsf(v.y), fabsf(v.z));
}
inline float MaxLength(const VEC4 &v)
{
	return std::max(
		std::max(fabsf(v.x), fabsf(v.y)),
		std::max(fabsf(v.z), fabsf(v.z)) );
}

/// Zwraca wektor z³o¿ony z mniejszych sk³adowych podanych wektorów
inline void Min(VEC2 *Out, const VEC2 &v1, const VEC2 &v2)
{
	Out->x = std::min(v1.x, v2.x);
	Out->y = std::min(v1.y, v2.y);
}
inline void Min(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = std::min(v1.x, v2.x);
	Out->y = std::min(v1.y, v2.y);
	Out->z = std::min(v1.z, v2.z);
}
inline void Min(VEC4 *Out, const VEC4 &v1, const VEC4 &v2)
{
	Out->x = std::min(v1.x, v2.x);
	Out->y = std::min(v1.y, v2.y);
	Out->z = std::min(v1.z, v2.z);
	Out->w = std::min(v1.w, v2.w);
}

/// Zwraca wektor z³o¿ony z wiêkszych sk³adowych podanych wektorów
inline void Max(VEC2 *Out, const VEC2 &v1, const VEC2 &v2)
{
	Out->x = std::max(v1.x, v2.x);
	Out->y = std::max(v1.y, v2.y);
}
inline void Max(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = std::max(v1.x, v2.x);
	Out->y = std::max(v1.y, v2.y);
	Out->z = std::max(v1.z, v2.z);
}
inline void Max(VEC4 *Out, const VEC4 &v1, const VEC4 &v2)
{
	Out->x = std::max(v1.x, v2.x);
	Out->y = std::max(v1.y, v2.y);
	Out->z = std::max(v1.z, v2.z);
	Out->w = std::max(v1.w, v2.w);
}

inline void saturate(VEC2 &v)
{
	v.x = saturate(v.x); v.y = saturate(v.y);
}
inline void saturate(VEC3 &v)
{
	v.x = saturate(v.x); v.y = saturate(v.y); v.z = saturate(v.z);
}
inline void saturate(VEC4 &v)
{
	v.x = saturate(v.x); v.y = saturate(v.y); v.z = saturate(v.z); v.w = saturate(v.w);
}

/** \addtogroup math_vec_componentop Operacje na poszczególnych sk³adowych wektorów */
//@{

inline void Mul(VEC2 *Out, const VEC2 &v, float f)
{
	Out->x = v.x * f;
	Out->y = v.y * f;
}
inline void Mul(VEC3 *Out, const VEC3 &v, float f)
{
	Out->x = v.x * f;
	Out->y = v.y * f;
	Out->z = v.z * f;
}
inline void Mul(VEC4 *Out, const VEC4 &v, float f)
{
	Out->x = v.x * f;
	Out->y = v.y * f;
	Out->z = v.z * f;
	Out->w = v.w * f;
}

inline void Add(VEC2 *Out, const VEC2 &v1, const VEC2 &v2)
{
	Out->x = v1.x + v2.x;
	Out->y = v1.y + v2.y;
}
inline void Add(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = v1.x + v2.x;
	Out->y = v1.y + v2.y;
	Out->z = v1.z + v2.z;
}
inline void Add(VEC4 *Out, const VEC4 &v1, const VEC4 &v2)
{
	Out->x = v1.x + v2.x;
	Out->y = v1.y + v2.y;
	Out->z = v1.z + v2.z;
	Out->w = v1.w + v2.w;
}

inline void Sub(VEC2 *Out, const VEC2 &v1, const VEC2 &v2)
{
	Out->x = v1.x - v2.x;
	Out->y = v1.y - v2.y;
}
inline void Sub(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = v1.x - v2.x;
	Out->y = v1.y - v2.y;
	Out->z = v1.z - v2.z;
}
inline void Sub(VEC4 *Out, const VEC4 &v1, const VEC4 &v2)
{
	Out->x = v1.x - v2.x;
	Out->y = v1.y - v2.y;
	Out->z = v1.z - v2.z;
	Out->w = v1.w - v2.w;
}

inline void Mul(VEC2 *Out, const VEC2 &v1, const VEC2 &v2)
{
	Out->x = v1.x * v2.x;
	Out->y = v1.y * v2.y;
}
inline void Mul(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = v1.x * v2.x;
	Out->y = v1.y * v2.y;
	Out->z = v1.z * v2.z;
}
inline void Mul(VEC4 *Out, const VEC4 &v1, const VEC4 &v2)
{
	Out->x = v1.x * v2.x;
	Out->y = v1.y * v2.y;
	Out->z = v1.z * v2.z;
	Out->w = v1.w * v2.w;
}

inline void Div(VEC2 *Out, const VEC2 &v1, const VEC2 &v2)
{
	Out->x = v1.x / v2.x;
	Out->y = v1.y / v2.y;
}
inline void Div(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = v1.x / v2.x;
	Out->y = v1.y / v2.y;
	Out->z = v1.z / v2.z;
}
inline void Div(VEC4 *Out, const VEC4 &v1, const VEC4 &v2)
{
	Out->x = v1.x / v2.x;
	Out->y = v1.y / v2.y;
	Out->z = v1.z / v2.z;
	Out->w = v1.w / v2.w;
}

inline void Mul(VEC2 *inout, const VEC2 &v)
{
	inout->x *= v.x;
	inout->y *= v.y;
}
inline void Mul(VEC3 *inout, const VEC3 &v)
{
	inout->x *= v.x;
	inout->y *= v.y;
	inout->z *= v.z;
}
inline void Mul(VEC4 *inout, const VEC4 &v)
{
	inout->x *= v.x;
	inout->y *= v.y;
	inout->z *= v.z;
	inout->w *= v.w;
}

inline void Div(VEC2 *inout, const VEC2 &v)
{
	inout->x /= v.x;
	inout->y /= v.y;
}
inline void Div(VEC3 *inout, const VEC3 &v)
{
	inout->x /= v.x;
	inout->y /= v.y;
	inout->z /= v.z;
}
inline void Div(VEC4 *inout, const VEC4 &v)
{
	inout->x /= v.x;
	inout->y /= v.y;
	inout->z /= v.z;
	inout->w /= v.w;
}

inline void Abs(VEC2 *v) { v->x = fabsf(v->x); v->y = fabsf(v->y); }
inline void Abs(VEC3 *v) { v->x = fabsf(v->x); v->y = fabsf(v->y); v->z = fabsf(v->z); }
inline void Abs(VEC4 *v) { v->x = fabsf(v->x); v->y = fabsf(v->y); v->z = fabsf(v->z); v->w = fabsf(v->w); }

//@}
// math_vec_componentop

/// Iloczyn skalarny wektorów
inline float Dot(const VEC2 &v1, const VEC2 &v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}
inline float Dot(const VEC3 &v1, const VEC3 &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}
inline float Dot(const VEC4 &v1, const VEC4 &v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

/// Odwraca znak sk³adowych wektora
inline void Minus(VEC2 *Out, VEC2 &v) { Out->x = -v.x; Out->y = -v.y; }
inline void Minus(VEC3 *Out, VEC3 &v) { Out->x = -v.x; Out->y = -v.y; Out->z = -v.z; }
inline void Minus(VEC4 *Out, VEC4 &v) { Out->x = -v.x; Out->y = -v.y; Out->z = -v.z; Out->w = -v.w; }
inline void Minus(VEC2 *InOut) { InOut->x = -InOut->x; InOut->y = -InOut->y; }
inline void Minus(VEC3 *InOut) { InOut->x = -InOut->x; InOut->y = -InOut->y; InOut->z = -InOut->z; }
inline void Minus(VEC4 *InOut) { InOut->x = -InOut->x; InOut->y = -InOut->y; InOut->z = -InOut->z; InOut->w = -InOut->w; }

/// Iloczyn wektorowy dwóch wektorów daj¹cy liczbê mówi¹c¹ czy drugi wskazuje na lewo, czy na prawo wzglêdem pierwszego
/** W uk³adzie gdzie X jest w prawo a Y do góry, > 0 znaczy na lewo \n
W uk³adzie gdzie X jest w prawo a Y w dó³, > 0 znaczy na prawo */
inline float Cross(const VEC2 &v1, const VEC2 &v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

/// Iloczyn wektorowy dwóch wektorów 3D
inline void Cross(VEC3 *Out, const VEC3 &v1, const VEC3 &v2)
{
	Out->x = v1.y * v2.z - v1.z * v2.y;
	Out->y = v1.z * v2.x - v1.x * v2.z;
	Out->z = v1.x * v2.y - v1.y * v2.x;
}

/// Kwadrat odleg³oœci miêdzy dwoma punktami
inline float DistanceSq(const VEC2 &p1, const VEC2 &p2)
{
	return LengthSq(p2-p1);
}
inline float DistanceSq(const VEC3 &p1, const VEC3 &p2)
{
	return LengthSq(p2-p1);
}
inline float DistanceSq(const VEC4 &p1, const VEC4 &p2)
{
	return LengthSq(p2-p1);
}

/// Odleg³oœæ miêdzy dwoma punktami
inline float Distance(const VEC2 &p1, const VEC2 &p2)
{
	return Length(p2-p1);
}
inline float Distance(const VEC3 &p1, const VEC3 &p2)
{
	return Length(p2-p1);
}
inline float Distance(const VEC4 &p1, const VEC4 &p2)
{
	return Length(p2-p1);
}

inline float ManhattanDistance(const VEC2 &p1, const VEC2 &p2)
{
	return fabsf(p2.x-p1.x) + fabsf(p2.y-p1.y);
}
inline float ManhattanDistance(const VEC3 &p1, const VEC3 &p2)
{
	return fabsf(p2.x-p1.x) + fabsf(p2.y-p1.y) + fabsf(p2.z-p1.z);
}
inline float ManhattanDistance(const VEC4 &p1, const VEC4 &p2)
{
	return fabsf(p2.x-p1.x) + fabsf(p2.y-p1.y) + fabsf(p2.z-p1.z) + fabsf(p2.w-p1.w);
}

inline float MaxDistance(const VEC2 &p1, const VEC2 &p2)
{
	return std::max( fabsf(p2.x-p1.x), fabsf(p2.y-p1.y) );
}
inline float MaxDistance(const VEC3 &p1, const VEC3 &p2)
{
	return max3( fabsf(p2.x-p1.x), fabsf(p2.y-p1.y), fabsf(p2.z-p1.z) );
}
inline float MaxDistance(const VEC4 &p1, const VEC4 &p2)
{
	return std::max(
		std::max( fabsf(p2.x-p1.x), fabsf(p2.y-p1.y) ),
		std::max( fabsf(p2.z-p1.z), fabsf(p2.w-p1.w) ) );
}

/// Liniowa interpolacja wektorów
inline void Lerp(VEC2 *Out, const VEC2 &v1, const VEC2 &v2, float t)
{
	Out->x = v1.x + t*(v2.x-v1.x);
	Out->y = v1.y + t*(v2.y-v1.y);
}
inline void Lerp(VEC3 *Out, const VEC3 &v1, const VEC3 &v2, float t)
{
	Out->x = v1.x + t*(v2.x-v1.x);
	Out->y = v1.y + t*(v2.y-v1.y);
	Out->z = v1.z + t*(v2.z-v1.z);
}
inline void Lerp(VEC4 *Out, const VEC4 &v1, const VEC4 &v2, float t)
{
	Out->x = v1.x + t*(v2.x-v1.x);
	Out->y = v1.y + t*(v2.y-v1.y);
	Out->z = v1.z + t*(v2.z-v1.z);
	Out->w = v1.w + t*(v2.w-v1.w);
}

/// Normalizuje wektor, czyli zmienia mu d³ugoœæ na 1
inline void Normalize(VEC2 *Out, const VEC2 &v)
{
	float l = 1.0f / Length(v);
	Out->x = v.x * l;
	Out->y = v.y * l;
}
inline void Normalize(VEC3 *Out, const VEC3 &v)
{
	float l = 1.0f / Length(v);
	Out->x = v.x * l;
	Out->y = v.y * l;
	Out->z = v.z * l;
}
inline void Normalize(VEC4 *Out, const VEC4 &v)
{
	float l = 1.0f / Length(v);
	Out->x = v.x * l;
	Out->y = v.y * l;
	Out->z = v.z * l;
	Out->w = v.w * l;
}

/// Normalizuje wektor w miejscu
inline void Normalize(VEC2 *v)
{
	float rcl = 1.0f / Length(*v);
	v->x *= rcl;
	v->y *= rcl;
}
inline void Normalize(VEC3 *v)
{
	float rcl = 1.0f / Length(*v);
	v->x *= rcl;
	v->y *= rcl;
	v->z *= rcl;
}
inline void Normalize(VEC4 *v)
{
	float rcl = 1.0f / Length(*v);
	v->x *= rcl;
	v->y *= rcl;
	v->z *= rcl;
	v->w *= rcl;
}

/// Bezpieczna normalizacja wektora w miejscu. W przypadku wektora zerowego zwraca false.
inline bool SafeNormalize(VEC2 *v)
{
	float lenSq = LengthSq(*v);
	if (lenSq < FLOAT_EPSILON) return false;
	float f = 1.f / sqrtf(lenSq);
	v->x *= f; v->y *= f;
	return true;
}
inline bool SafeNormalize(VEC3 *v)
{
	float lenSq = LengthSq(*v);
	if (lenSq < FLOAT_EPSILON) return false;
	float f = 1.f / sqrtf(lenSq);
	v->x *= f; v->y *= f; v->z *= f;
	return true;
}
inline bool SafeNormalize(VEC4 *v)
{
	float lenSq = LengthSq(*v);
	if (lenSq < FLOAT_EPSILON) return false;
	float f = 1.f / sqrtf(lenSq);
	v->x *= f; v->y *= f; v->z *= f; v->w *= f;
	return true;
}

/// Konwersja z wektora 3D na 4D dopisuj¹ca W=1
inline void Vec3ToVec4(VEC4 *Out, const VEC3 &v)
{
	Out->x = v.x;
	Out->y = v.y;
	Out->z = v.z;
	Out->w = 1.0f;
}

/// Konwersja z wektora 4D na 3D ze zignorowaniem sk³adowej W
inline void Vec4ToVec3_Ignore(VEC3 *Out, const VEC4 &v)
{
	Out->x = v.x;
	Out->y = v.y;
	Out->z = v.z;
}
/// Konwersja z wektora 4D na 3D ze podzieleniem sk³adowych przez W
inline void Vec4ToVec3_Div(VEC3 *Out, const VEC4 &v)
{
	float rcw = 1.0f / v.w;
	Out->x = v.x * rcw;
	Out->y = v.y * rcw;
	Out->z = v.z * rcw;
}

/// Zwraca punkt w po³owie drogi miêdzy podanymi punktami
inline void MidPoint(VEC2 *Out, const VEC2 &p1, const VEC2 &p2)
{
	Out->x = (p1.x+p2.x) * 0.5f;
	Out->y = (p1.y+p2.y) * 0.5f;
}
inline void MidPoint(VEC3 *Out, const VEC3 &p1, const VEC3 &p2)
{
	Out->x = (p1.x+p2.x) * 0.5f;
	Out->y = (p1.y+p2.y) * 0.5f;
	Out->z = (p1.z+p2.z) * 0.5f;
}
inline void MidPoint(VEC4 *Out, const VEC4 &p1, const VEC4 &p2)
{
	Out->x = (p1.x+p2.x) * 0.5f;
	Out->y = (p1.y+p2.y) * 0.5f;
	Out->z = (p1.z+p2.z) * 0.5f;
	Out->w = (p1.w+p2.w) * 0.5f;
}

/// Zwraca true, jeœli podane wektory/punkty s¹ sobie mniej wiêcej równe
inline bool VecEqual(const VEC2 &v1, const VEC2 &v2) {
	return float_equal(v1.x, v2.x) && float_equal(v1.y, v2.y);
}
inline bool VecEqual(const VEC3 &v1, const VEC3 &v2) {
	return float_equal(v1.x, v2.x) && float_equal(v1.y, v2.y) && float_equal(v1.z, v2.z);
}
inline bool VecEqual(const VEC4 &v1, const VEC4 &v2) {
	return float_equal(v1.x, v2.x) && float_equal(v1.y, v2.y) && float_equal(v1.z, v2.z) && float_equal(v1.w, v2.w);
}

inline bool around(const VEC2 &v1, const VEC2 &v2, float Epsilon) {
	return around(v1.x, v2.x, Epsilon) && around(v1.y, v2.y, Epsilon);
}
inline bool around(const VEC3 &v1, const VEC3 &v2, float Epsilon) {
	return around(v1.x, v2.x, Epsilon) && around(v1.y, v2.y, Epsilon) && around(v1.z, v2.z, Epsilon);
}
inline bool around(const VEC4 &v1, const VEC4 &v2, float Epsilon) {
	return around(v1.x, v2.x, Epsilon) && around(v1.y, v2.y, Epsilon) && around(v1.z, v2.z, Epsilon) && around(v1.w, v2.w, Epsilon);
}

/// Zwraca wektor prostopad³y do podanego wektora 2D
/** W uk³adzie w którym X jest w prawo a Y do góry wektor le¿y na lewo od danego.
W uk³adzie w którym X jest w prawo a Y w dó³, le¿y na prawo. */
inline void Perpedicular(VEC2 *Out, const VEC2 &v)
{
	Out->x = -v.y;
	Out->y = v.x;
}

/// Oblicza wektor kierunku odbicia od p³aszczyzny o podanej normalnej
/**
\param v wskazuje na kierunek *do* tej p³aszczyzny.
\param PlaneNormal musi byæ znormalizowany.
*/
inline void Reflect(VEC2 *Out, const VEC2 &v, const VEC2 &PlaneNormal)
{
	*Out = v - (2.0f * Dot(v, PlaneNormal)) * PlaneNormal;
}
inline void Reflect(VEC3 *Out, const VEC3 &v, const VEC3 &PlaneNormal)
{
	*Out = v - (2.0f * Dot(v, PlaneNormal)) * PlaneNormal;
}

/// For the incident vector I and surface normal N, and the ratio of indices of refraction eta, return the refraction vector.
/** The input parameters for the incident vector I and the surface normal N must
already be normalized to get the desired results.
*/
inline void Refract(VEC2 *out, const VEC2 &I, const VEC2 &N, float eta)
{
	float N_dot_I = Dot(N, I);
	float k = 1.f - eta * eta * (1.f - N_dot_I * N_dot_I);
	if (k < 0.f)
		*out = VEC2_ZERO;
	else
		*out = eta * I - (eta * N_dot_I + sqrtf(k)) * N;
}
/// \copydoc Refract(VEC2, const VEC2&, const VEC2&, float)
inline void Refract(VEC3 *out, const VEC3 &I, const VEC3 &N, float eta)
{
	float N_dot_I = Dot(N, I);
	float k = 1.f - eta * eta * (1.f - N_dot_I * N_dot_I);
	if (k < 0.f)
		*out = VEC3_ZERO;
	else
		*out = eta * I - (eta * N_dot_I + sqrtf(k)) * N;
}

/// Rzutowanie wektora V na wektor N
/** Jeœli chcemy roz³o¿yæ wektor V na wektor prostopad³y i równoleg³y do N,
to wektorem równoleg³ym jest Out, a prostopad³y obliczamy jako V - Out.
\param[out] Out Bêdzie równoleg³y do N. */
inline void Project(VEC2 *Out, const VEC2 &V, const VEC2 &N)
{
	// Oryginalnie:
	// *Out = N * ( Dot(V, N) / LengthSq(N) );
	// Wersja szybsza:
	float T = (V.x*N.x + V.y*N.y) / (N.x*N.x + N.y*N.y);
	Out->x = N.x * T;
	Out->y = N.y * T;
}
inline void Project(VEC3 *Out, const VEC3 &V, const VEC3 &N)
{
	float T = (V.x*N.x + V.y*N.y + V.z*N.z) / (N.x*N.x + N.y*N.y + N.z*N.z);
	Out->x = N.x * T;
	Out->y = N.y * T;
	Out->z = N.z * T;
}
/// Wersja common::Project, kiedy wektor N jest ju¿ na pewno znormalizowany.
inline void Project_N(VEC2 *Out, const VEC2 &V, const VEC2 &N)
{
	// Oryginalnie:
	// *Out = N * Dot(V, N);
	// Wersja szybsza:
	float T = V.x*N.x + V.y*N.y;
	Out->x = N.x * T;
	Out->y = N.y * T;
}
inline void Project_N(VEC3 *Out, const VEC3 &V, const VEC3 &N)
{
	float T = V.x*N.x + V.y*N.y + V.z*N.z;
	Out->x = N.x * T;
	Out->y = N.y * T;
	Out->z = N.z * T;
}

/// Oblicza: u cross v dot w
inline float ScalarTripleProduct(const VEC3 &u, const VEC3 &v, const VEC3 &w)
{
	return u.x * (v.y * w.z - v.z * w.y) - u.y * (v.x * w.z - v.z * w.x) + u.z * (v.x * w.y - v.y * w.x);
}

/** \addtogroup math_vec_orthogonalisation Ortogonalizacja trzech wektorów
- Po tym przekszta³ceniu bêd¹ do siebie prostopad³e.
- Wektory wejœciowe nie musz¹ byæ znormalizowane.
- Wektory wyjœciowe niekoniecznie bêd¹ znormalizowane (nawet jeœli wejœciowe by³y).
*/
//@{
/// Ortogonalizacja trzech wektorów - Metoda szybka
void Orthogonalize_Fast(VEC3 *OutR1, VEC3 *OutR2, VEC3 *OutR3, const VEC3 &R1, const VEC3 &R2, const VEC3 &R3);
/// Ortogonalizacja trzech wektorów - Metoda dok³adna
void Orthogonalize(VEC3 *OutR1, VEC3 *OutR2, VEC3 *OutR3, const VEC3 &R1, const VEC3 &R2, const VEC3 &R3);
//@}

/// Wyznacza dwa dowolne wektory prostopad³e do podanego
/** Zwrócone wektory nie s¹ znormalizowane.
\param v nie musi byæ znormalizowany. */
void PerpedicularVectors(VEC3 *Out1, VEC3 *Out2, const VEC3 &v);

//@}
// math_vec


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_triangle Trójk¹t */
//@{

/// Oblicza signed pole powierzchni trójk¹ta
/** Wychodzi dodanie, kiedy wierzcho³ki s¹ CW. */
float TriangleArea(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3);
float TriangleArea(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3);

/// Zwraca true, jeœli wierzcho³ki trójk¹ta s¹ CW.
bool TriangleIsCW(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3);

/// Znajduje obwód trójk¹ta
float TrianglePerimeter(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3);
float TrianglePerimeter(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3);

/// Oblicza punkt na powierzhchni trójk¹ta we wsp. barycentrycznych
/** f to waga punktu p2, g to waga punktu p3. \n
Wzór: *Out = p1 + f*(p2-p1) + g*(p3-p1) \n
Inny wzór: *Out = p1*(1-f-g) + p2*f + p3*g */
inline void Barycentric(VEC2 *Out, const VEC2 &p1, const VEC2 &p2, const VEC2 &p3, float f, float g)
{
	float e = 1.0f-f-g;
	Out->x = e*p1.x + f*p2.x + g*p3.x;
	Out->y = e*p1.y + f*p2.y + g*p3.y;
}
inline void Barycentric(VEC3 *Out, const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, float f, float g)
{
	// Stara wersja, na podstawie DX SDK, du¿o wolniejsza:
	//VEC3 Diff2 = p2-p1;
	//VEC3 Diff3 = p3-p1;
	//Out->x = p1.x + f*Diff2.x + g*Diff3.x;
	//Out->y = p1.y + f*Diff2.y + g*Diff3.y;
	//Out->z = p1.z + f*Diff2.z + g*Diff3.z;

	float e = 1.0f-f-g;
	Out->x = e*p1.x + f*p2.x + g*p3.x;
	Out->y = e*p1.y + f*p2.y + g*p3.y;
	Out->z = e*p1.z + f*p2.z + g*p3.z;
}
inline void Barycentric(VEC4 *Out, const VEC4 &p1, const VEC4 &p2, const VEC4 &p3, float f, float g)
{
	float e = 1.0f-f-g;
	Out->x = e*p1.x + f*p2.x + g*p3.x;
	Out->y = e*p1.y + f*p2.y + g*p3.y;
	Out->z = e*p1.z + f*p2.z + g*p3.z;
	Out->w = e*p1.w + f*p2.w + g*p3.w;
}

/// Wspó³rzêdne barycentryczne punktu p wzglêdem trójk¹ta p1-p2-p3.
/** Wspó³czynniki e, f, g to wagi wierzcho³ków p1, p2, p3. \n
\param out_e [out] Mo¿na podaæ NULL. */
void CalcBarycentric(const VEC2 &p, const VEC2 &p1, const VEC2 &p2, const VEC2 &p3, float *out_e, float *out_f, float *out_g);
/// Wspó³rzêdne barycentryczne punktu p wzglêdem trójk¹ta p1-p2-p3.
/** Wspó³czynniki e, f, g to wagi wierzcho³ków p1, p2, p3. \n
Punkt p musi le¿eæ na jednej p³aszczyŸnie z trójk¹tem p1-p2-p3. \n
Normal to wektor normalny trójk¹ta p1-p2-p3. Mo¿na go podaæ jeœli jest ju¿ wyliczony albo podaæ NULL. */
void CalcBarycentric(const VEC3 &p, const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, const VEC3 *Normal, float *out_e, float *out_f, float *out_g);

/// Znajduje okr¹g wpisany w trójk¹t
void TriangleInscribedCircle(VEC2 *OutCenter, float *OutRadius, const VEC2 &v1, const VEC2 &v2, const VEC2 &v3);
void TriangleInscribedCircle(VEC3 *OutCenter, float *OutRadius, const VEC3 &v1, const VEC3 &v2, const VEC3 &v3);
/// Znajduje okr¹g opisany na trójk¹cie
void TriangleCircumscribedCircle(VEC2 *OutCenter, float *OutRadius, const VEC2 &v1, const VEC2 &v2, const VEC2 &v3);
void TriangleCircumscribedCircle(VEC3 *OutCenter, float *OutRadius, const VEC3 &v1, const VEC3 &v2, const VEC3 &v3);

//@}
// math_triangle


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_ray Promieñ */
//@{

/// Zwraca punkt na promieniu
inline void PointOnRay(VEC2 *Out, const VEC2 &Origin, const VEC2 &Direction, float t)
{
	Out->x = Origin.x + t * Direction.x;
	Out->y = Origin.y + t * Direction.y;
}
inline void PointOnRay(VEC3 *Out, const VEC3 &Origin, const VEC3 &Direction, float t)
{
	Out->x = Origin.x + t * Direction.x;
	Out->y = Origin.y + t * Direction.y;
	Out->z = Origin.z + t * Direction.z;
}

//@}
// math_ray


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_rect Prostok¹t */
//@{

/// Prostok¹t z liczb ca³kowitych
/**
RECTI jest kompatybilny ze struktur¹ RECT z Win32API.
Konwersja do i ze stringa: "left,top,right,bottom", np. "1,2,3,4".
Niezale¿nie od uk³adu wspó³rzêdnych, prawid³owy prostok¹t jest wtedy, kiedy top
jest mniejsze co do wartoœci od bottom, a left od right.
*/
struct RECTI
{
	POINT_ Min, Max;

	RECTI() { }
	RECTI(const POINT_ &min_, const POINT_ &max_) : Min(min_), Max(max_) { }
	RECTI(int minX, int minY, int maxX, int maxY) : Min(minX, minY), Max(maxX, maxY) { }
	RECTI(const int *Arr) : Min(Arr[0], Arr[1]), Max(Arr[2], Arr[3]) { }

	operator int * () { return &Min.x; }
	operator const int * () const { return &Min.x; }

	bool operator == (const RECTI &r) const { return Min == r.Min && Max == r.Max; }
	bool operator != (const RECTI &r) const { return Min != r.Min || Max != r.Max; }

	RECTI operator + () const { return *this; }
	RECTI operator - () const { return RECTI(-Max, -Min); }
	RECTI operator + (const POINT_ &p) const { return RECTI(Min+p, Max+p); }
	RECTI operator - (const POINT_ &p) const { return RECTI(Min-p, Max-p); }
	RECTI operator * (int s) const { return RECTI(Min*s, Max*s); }
	RECTI & operator += (const POINT_ &p) { Min += p; Max += p; return *this; }
	RECTI & operator -= (const POINT_ &p) { Min -= p; Max -= p; return *this; }
	RECTI & operator *= (int s) { Min *= s; Max *= s; return *this; }

	int & left()   { return Min.x; }
	int & top()    { return Min.y; }
	int & right()  { return Max.x; }
	int & bottom() { return Max.y; }
	int left()   const { return Min.x; }
	int top()    const { return Min.y; }
	int right()  const { return Max.x; }
	int bottom() const { return Max.y; }

	bool IsValid() const { return Min.x < Max.x && Min.y < Max.y; }
	void Repair()
	{
		if (Max.x < Min.x) std::swap(Min.x, Max.x);
		if (Max.y < Min.y) std::swap(Min.y, Max.y);
	}

	/// Powiêksza prostok¹t z ka¿dej strony o podan¹ wartoœæ
	void Extend(int d) { Min.x -= d; Min.y -= d; Max.x += d; Max.y += d; }
	/// Oblicza i zwraca d³ugoœæ przek¹tnej
	float Diagonal() const { return sqrtf( (float)((Max.x-Min.x)*(Max.x-Min.x)) + (float)((Max.y-Min.y)*(Max.y-Min.y)) ); }
	/// Oblicza i zwraca pole powierzchni
	int Field() const { return (Max.x-Min.x)*(Max.y-Min.y); }

	void GetSize(POINT_ *out) const { *out = Max - Min; }
	int GetSizeDim(uint dim) const { return Max[dim] - Min[dim]; }

	void GetHalfSize(POINT_ *out) { *out = (Max - Min) / 2; }
	int GetHalfSizeDim(uint dim) const { return (Max[dim] - Min[dim]) / 2; }

	void SetSize(const POINT_ &newSize) { Max = Min + newSize; }
	void SetSizeDim(uint dim, int newSize) { Max[dim] = Min[dim] + newSize; }

	void GetCenter(POINT_ *Out) const { *Out = (Min+Max)/2; }
	int GetCenterDim(uint dim) const { return (Min[dim]+Max[dim])/2; }

	void GetCorner(POINT_ *out, uint i) const
	{
		out->x = (i & 1) ? Max.x : Min.x;
		out->y = (i & 2) ? Max.y : Min.y;
	}
	/// Returns begin point and end point of one of rectangle edges. index must be 0..3.
	void GetEdge(POINT_ *outP1, POINT_ *outP2, uint index) const;

	void AddInternalPoint(const POINT_ &p)
	{
		if (p.x < Min.x) Min.x = p.x;
		if (p.y < Min.y) Min.y = p.y;
		if (p.x > Max.x) Max.x = p.x;
		if (p.y > Max.y) Max.y = p.y;
	}

	void AddInternalRect(const RECTI &rect)
	{
		if (rect.Min.x < Min.x) Min.x = rect.Min.x;
		if (rect.Min.y < Min.y) Min.y = rect.Min.y;
		if (rect.Max.x > Max.x) Max.x = rect.Max.x;
		if (rect.Max.y > Max.y) Max.y = rect.Max.y;
	}
};

extern const RECTI RECTI_ZERO;
extern const RECTI RECTI_ZERO_ONE;
extern const RECTI RECTI_MINUS_ONE_ONE;
extern const RECTI RECTI_MAX;
extern const RECTI RECTI_INV_MAX;

/// Prostok¹t z 4 floatów.
struct RECTF
{
	VEC2 Min, Max;

	RECTF() { }
	RECTF(const VEC2 &min_, const VEC2 &max_) : Min(min_), Max(max_) { }
	RECTF(float minX, float minY, float maxX, float maxY) : Min(minX, minY), Max(maxX, maxY) { }
	RECTF(const float *Arr) : Min(Arr[0], Arr[1]), Max(Arr[2], Arr[3]) { }

	operator float * () { return &Min.x; }
	operator const float * () const { return &Min.x; }

	bool operator == (const RECTF &r) const { return Min == r.Min && Max == r.Max; }
	bool operator != (const RECTF &r) const { return Min != r.Min || Max != r.Max; }

	RECTF operator + () const { return *this; }
	RECTF operator - () const { return RECTF(-Max, -Min); }
	RECTF operator + (const VEC2 &p) const { return RECTF(Min+p, Max+p); }
	RECTF operator - (const VEC2 &p) const { return RECTF(Min-p, Max-p); }
	RECTF operator * (float s) const { return RECTF(Min*s, Max*s); }
	RECTF operator / (float s) const { float sInv = 1.f/s; return RECTF(Min*sInv, Max*sInv); }
	RECTF & operator += (const VEC2 &p) { Min += p; Max += p; return *this; }
	RECTF & operator -= (const VEC2 &p) { Min -= p; Max -= p; return *this; }
	RECTF & operator *= (float s) { Min *= s; Max *= s; return *this; }
	RECTF & operator /= (float s) { float sInv = 1.f/s; Min *= sInv; Max *= sInv; return *this; }

	float & left()   { return Min.x; }
	float & top()    { return Min.y; }
	float & right()  { return Max.x; }
	float & bottom() { return Max.y; }
	float left()   const { return Min.x; }
	float top()    const { return Min.y; }
	float right()  const { return Max.x; }
	float bottom() const { return Max.y; }

	bool IsValid() const { return Min.x < Max.x && Min.y < Max.y; }
	void Repair()
	{
		if (Max.x < Min.x) std::swap(Min.x, Max.x);
		if (Max.y < Min.y) std::swap(Min.y, Max.y);
	}

	/// Powiêksza prostok¹t z ka¿dej strony o podan¹ wartoœæ
	void Extend(float d) { Min.x -= d; Min.y -= d; Max.x += d; Max.y += d; }
	/// Oblicza i zwraca d³ugoœæ przek¹tnej
	float Diagonal() const { return sqrtf( sqr(Max.x-Min.x) + sqr(Max.y-Min.y) ); }
	/// Oblicza i zwraca pole powierzchni
	float Field() const { return (Max.x-Min.x)*(Max.y-Min.y); }
	
	void GetSize(VEC2 *out) const { *out = Max - Min; }
	float GetSizeDim(uint dim) const { return Max[dim] - Min[dim]; }
	
	void GetHalfSize(VEC2 *out) { *out = (Max - Min) * 0.5f; }
	float GetHalfSizeDim(uint dim) const { return (Max[dim] - Min[dim]) * 0.5f; }

	/// Zmienia szerokoœæ i wysokoœæ na podan¹
	void SetSize(const VEC2 &newSize) { Max = Min + newSize; }
	void SetSizeDim(uint dim, float newSize) { Max[dim] = Min[dim] + newSize; }
	/// Oblicza i zwraca pozycjê œrodka
	void GetCenter(VEC2 *Out) const { *Out = (Min+Max)*0.5f; }
	float GetCenterDim(uint dim) const { return (Min[dim]+Max[dim])*0.5f; }

	void GetCorner(VEC2 *out, uint i) const
	{
		out->x = (i & 1) ? Max.x : Min.x;
		out->y = (i & 2) ? Max.y : Min.y;
	}
	/// Returns begin point and end point of one of rectangle edges. index must be 0..3.
	void GetEdge(VEC2 *outP1, VEC2 *outP2, uint index) const;
	/// Returns one of 4 possible quarters of this rect.
	void GetQuad(RECTF *out, uint index) const;

	void AddInternalPoint(const VEC2 &p)
	{
		if (p.x < Min.x) Min.x = p.x;
		if (p.y < Min.y) Min.y = p.y;
		if (p.x > Max.x) Max.x = p.x;
		if (p.y > Max.y) Max.y = p.y;
	}

	void AddInternalRect(const RECTF &rect)
	{
		if (rect.Min.x < Min.x) Min.x = rect.Min.x;
		if (rect.Min.y < Min.y) Min.y = rect.Min.y;
		if (rect.Max.x > Max.x) Max.x = rect.Max.x;
		if (rect.Max.y > Max.y) Max.y = rect.Max.y;
	}
};

extern const RECTF RECTF_ZERO;
extern const RECTF RECTF_ZERO_ONE;
extern const RECTF RECTF_MINUS_ONE_ONE;
extern const RECTF RECTF_MAX; ///< Maximal possible rectangle: -FLT_MAX..FLT_MAX
extern const RECTF RECTF_INV_MAX; ///< Inverted maximal rectangle: FLT_MAX..-FLT_MAX. It's invalid. Use it to initialize rectangle for finding min-max.

#ifdef WIN32
	/// Konwersja z common::RECTI do RECT z WinAPI
	template <> inline ::RECT & math_cast<::RECT, RECTI>(const RECTI &x) { return (::RECT&)(x); }
	/// Konwersja z RECT z WinAPI do common::RECTI
	template <> inline RECTI & math_cast<RECTI, ::RECT>(const ::RECT &x) { return (RECTI&)(x); }
#endif

inline void swap(RECTI &r1, RECTI &r2)
{
	std::swap(r1.Min.x, r2.Min.x); std::swap(r1.Min.y, r2.Min.y);
	std::swap(r1.Max.x, r2.Max.x); std::swap(r1.Max.y, r2.Max.y);
}

inline void swap(RECTF &r1, RECTF &r2)
{
	std::swap(r1.Min, r2.Min);
	std::swap(r1.Max, r2.Max);
}

inline void Mul(RECTI *out, const RECTI &rect, const POINT_ &v)
{
	Mul(&out->Min, rect.Min, v);
	Mul(&out->Max, rect.Max, v);
}
inline void Mul(RECTF *out, const RECTF &rect, const VEC2 &v)
{
	Mul(&out->Min, rect.Min, v);
	Mul(&out->Max, rect.Max, v);
}
inline void Div(RECTI *out, const RECTI &rect, const POINT_ &v)
{
	Div(&out->Min, rect.Min, v);
	Div(&out->Max, rect.Max, v);
}
inline void Div(RECTF *out, const RECTF &rect, const VEC2 &v)
{
	Div(&out->Min, rect.Min, v);
	Div(&out->Max, rect.Max, v);
}

inline void Mul(RECTI *inout, const POINT_ &v)
{
	Mul(&inout->Min, v);
	Mul(&inout->Max, v);
}
inline void Mul(RECTF *inout, const VEC2 &v)
{
	Mul(&inout->Min, v);
	Mul(&inout->Max, v);
}
inline void Div(RECTI *inout, const POINT_ &v)
{
	Div(&inout->Min, v);
	Div(&inout->Max, v);
}
inline void Div(RECTF *inout, const VEC2 &v)
{
	Div(&inout->Min, v);
	Div(&inout->Max, v);
}

/// Zwraca true, jeœli punkt nale¿y do prostok¹ta
inline bool PointInRect(const POINT_ &p, const RECTI &r)
{
	return
		(p.x >= r.Min.x) &&
		(p.x <= r.Max.x) &&
		(p.y >= r.Min.y) &&
		(p.y <= r.Max.y);
}
inline bool PointInRect(const VEC2 &p, const RECTF &r)
{
	return
		(p.x >= r.Min.x) &&
		(p.x <= r.Max.x) &&
		(p.y >= r.Min.y) &&
		(p.y <= r.Max.y);
}

/// Zwraca true, jeœli prostok¹t sr zawiera siê w ca³oœci wewn¹trz r
inline bool RectInRect(const RECTI &sr, const RECTI &r)
{
	return
		(sr.Min.x >= r.Min.x) &&
		(sr.Max.x <= r.Max.x) &&
		(sr.Min.y >= r.Min.y) &&
		(sr.Max.y <= r.Max.y);
}
inline bool RectInRect(const RECTF &sr, const RECTF &r)
{
	return
		(sr.Min.x >= r.Min.x) &&
		(sr.Max.x <= r.Max.x) &&
		(sr.Min.y >= r.Min.y) &&
		(sr.Max.y <= r.Max.y);
}

/// Zwraca true, jeœli podane prostok¹ty zachodz¹ na siebie
inline bool RectToRect(const RECTI &r1, const RECTI &r2)
{
	return
		(r1.Min.x <= r2.Max.x) &&
		(r1.Max.x >= r2.Min.x) &&
		(r1.Min.y <= r2.Max.y) &&
		(r1.Max.y >= r2.Min.y);
}
inline bool RectToRect(const RECTF &r1, const RECTF &r2)
{
	return
		(r1.Min.x <= r2.Max.x) &&
		(r1.Max.x >= r2.Min.x) &&
		(r1.Min.y <= r2.Max.y) &&
		(r1.Max.y >= r2.Min.y);
}

/// Jeœli prostok¹ty posiadaj¹ czêœæ wspóln¹, zwraca j¹ przez Out i zwraca true
inline bool Intersection(RECTI *Out, const RECTI &r1, const RECTI &r2)
{
	*Out = RECTI(
		std::max(r1.Min.x, r2.Min.x),
		std::max(r1.Min.y, r2.Min.y),
		std::min(r1.Max.x, r2.Max.x),
		std::min(r1.Max.y, r2.Max.y));
	return Out->IsValid();
}
inline bool Intersection(RECTF *Out, const RECTF &r1, const RECTF &r2)
{
	*Out = RECTF(
		std::max(r1.Min.x, r2.Min.x),
		std::max(r1.Min.y, r2.Min.y),
		std::min(r1.Max.x, r2.Max.x),
		std::min(r1.Max.y, r2.Max.y));
	return Out->IsValid();
}

/// Zwraca prostok¹t opisany na podanych dwóch prostok¹tach
inline void Union(RECTI *Out, const RECTI &r1, const RECTI &r2)
{
	Out->Min.x = std::min(r1.Min.x, r2.Min.x);
	Out->Min.y = std::min(r1.Min.y, r2.Min.y);
	Out->Max.x = std::max(r1.Max.x, r2.Max.x);
	Out->Max.y = std::max(r1.Max.y, r2.Max.y);
}
inline void Union(RECTF *Out, const RECTF &r1, const RECTF &r2)
{
	Out->Min.x = std::min(r1.Min.x, r2.Min.x);
	Out->Min.y = std::min(r1.Min.y, r2.Min.y);
	Out->Max.x = std::max(r1.Max.x, r2.Max.x);
	Out->Max.y = std::max(r1.Max.y, r2.Max.y);
}

inline void Lerp(RECTF *out, const RECTF &v1, const RECTF &v2, float t)
{
	out->Min.x = Lerp(v1.Min.x, v2.Min.x, t);
	out->Min.y = Lerp(v1.Min.y, v2.Min.y, t);
	out->Max.x = Lerp(v1.Max.x, v2.Max.x, t);
	out->Max.y = Lerp(v1.Max.y, v2.Max.y, t);
}

/// Wyznacza najbli¿szy punkt wewn¹trz prostok¹ta wzglêdem punktu p.
/** Jeœli p le¿y wewn¹trz tego prostok¹ta, zwrócone zostaje dok³adnie p. */
void ClosestPointInRect(POINT_ *Out, const RECTI &Rect, const POINT_ &p);
/// Wyznacza najbli¿szy punkt wewn¹trz prostok¹ta wzglêdem punktu p.
/** Jeœli p le¿y wewn¹trz tego prostok¹ta, zwrócone zostaje dok³adnie p. */
void ClosestPointInRect(VEC2 *Out, const RECTF &Rect, const VEC2 &p);

//@}
// math_rect


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_aabb AABB
Prostopad³oœcian AABB - Axis Aligned Bounding Box */
//@{

/// AABB z 2 punktów 3D
/**
Sk³adnia do konwersji na i z ³añcuch: p1x,p1y,p1z;p2x,p2y,p2z, np.:
"1,2,3;4,5,6"
*/
struct BOX
{
	VEC3 Min, Max;

	BOX() { }
	BOX(const VEC3 &Min, const VEC3 &Max) : Min(Min), Max(Max) { }
	BOX(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z) : Min(p1x, p1y, p1z), Max(p2x, p2y, p2z) { }
	BOX(const float *Arr) : Min(&Arr[0]), Max(&Arr[3]) { }

	operator float * () { return &Min.x; }
	operator const float * () const { return &Min.x; }

	bool operator == (const BOX &p) const { return Min == p.Min && Max == p.Max; }
	bool operator != (const BOX &p) const { return Min != p.Min || Max != p.Max; }

	BOX operator + () const { return *this; }
	BOX operator - () const { return BOX(-Max, -Min); }
	BOX operator + (const VEC3 &v) const { return BOX(Min+v, Max+v); }
	BOX operator - (const VEC3 &v) const { return BOX(Min-v, Max-v); }
	BOX operator * (float f) const { return BOX(Min*f, Max*f); }
	BOX operator / (float f) const { float fInv = 1.f/f; return BOX(Min*fInv, Max*fInv); }
	BOX & operator += (const VEC3 &v) { Min += v; Max += v; return *this; }
	BOX & operator -= (const VEC3 &v) { Min -= v; Max -= v; return *this; }
	BOX & operator *= (float f) { Min *= f; Max *= f; return *this; }
	BOX & operator /= (float f) { float fInv = 1.f/f; Min *= fInv; Max *= fInv; return *this; }

	bool IsValid() { return Min.x < Max.x && Min.y < Max.y && Min.z < Max.z; }
	void Repair()
	{
		if (Max.x < Min.x) std::swap(Min.x, Max.x);
		if (Max.y < Min.y) std::swap(Min.y, Max.y);
		if (Max.z < Min.z) std::swap(Min.z, Max.z);
	}

	/// Powiêksza boks z ka¿dej strony o podan¹ wartoœæ
	void Extend(float d) { Min.x -= d; Min.y -= d; Min.z -= d; Max.x += d; Max.y += d; Max.z += d; }
	/// Oblicza i zwraca rozmiary boksa w trzech wymiarach
	void GetSize(VEC3 *Out) const { Out->x = Max.x-Min.x; Out->y = Max.y-Min.y; Out->z = Max.z-Min.z; }
	float GetSizeDim(uint dim) const { return Max[dim]-Min[dim]; }
	/// Zmienia wielkoœæ na podan¹
	void SetSize(const VEC3 &NewSize) { Max.x = Min.x + NewSize.x; Max.y = Min.y + NewSize.y; Max.z = Min.z + NewSize.z; }
	void SetSizeDim(uint dim, float newSize) { Max[dim] = Min[dim] + newSize; }
	/// Oblicza i zwraca d³ugoœæ przek¹tnej
	float Diagonal() const { return sqrtf( (Max.x-Min.x)*(Max.x-Min.x) + (Max.y-Min.y)*(Max.y-Min.y) + (Max.z-Min.z)*(Max.z-Min.z) ); }
	/// Oblicza i zwraca objêtoœæ
	float Volume() const { return (Max.x-Min.x)*(Max.y-Min.y)*(Max.z-Min.z); }
	/// Oblicza i zwraca pozycjê œrodka
	void GetCenter(VEC3 *Out) const { Out->x = (Min.x+Max.x)*0.5f; Out->y = (Min.y+Max.y)*0.5f; Out->z = (Min.z+Max.z)*0.5f; }
	float GetCenterDim(uint dim) const { return (Min[dim]+Max[dim])*0.5f; }
	/// Oblicza i zwraca po³owy rozmiarów
	void GetHalfSize(VEC3 *OutHalfSize) const {
		OutHalfSize->x = (Max.x-Min.x)*0.5f;
		OutHalfSize->y = (Max.y-Min.y)*0.5f;
		OutHalfSize->z = (Max.z-Min.z)*0.5f;
	}
	float GetHalfSizeDim(uint dim) { return (Max[dim]-Min[dim])*0.5f; }
	/// Zwraca wybrany wierzcho³ek
	void GetCorner(VEC3 *Out, uint i) const
	{
		Out->x = (i & 1) ? Max.x : Min.x;
		Out->y = (i & 2) ? Max.y : Min.y;
		Out->z = (i & 4) ? Max.z : Min.z;
	}
	/// Zwraca wszystkie 6 wierzcho³ków w kolejnoœci
	/**
	Trzeba podaæ wskaŸnik do tablicy. Kolejnoœæ:
	\verbatim
	p1x, p1y, p1z; p2x, p1y, p1z; p1x, p2y, p1z; p2x, p2y, p1z
	p1x, p1y, p2z; p2x, p1y, p2z; p1x, p2y, p2z; p2x, p2y, p2z
	\endverbatim
	*/
	void GetAllCorners(VEC3 *Out) const
	{
		Out[0] = VEC3(Min.x, Min.y, Min.z); Out[1] = VEC3(Max.x, Min.y, Min.z); Out[2] = VEC3(Min.x, Max.y, Min.z); Out[3] = VEC3(Max.x, Max.y, Min.z);
		Out[4] = VEC3(Min.x, Min.y, Max.z); Out[5] = VEC3(Max.x, Min.y, Max.z); Out[6] = VEC3(Min.x, Max.y, Max.z); Out[7] = VEC3(Max.x, Max.y, Max.z);
	}
	/// Returns begin point and end point of selected edge. index must be 0..11.
	void GetEdge(VEC3 *outP1, VEC3 *outP2, uint index) const;
	/// Returns one of 8 possible octants of this box.
	void GetOct(BOX *out, uint index) const;

	/// Powiêksza tak ¿eby na pewno zawiera³ podany punkt
	void AddInternalPoint(const VEC3 &p)
	{
		Min.x = std::min(Min.x, p.x); Min.y = std::min(Min.y, p.y); Min.z = std::min(Min.z, p.z);
		Max.x = std::max(Max.x, p.x); Max.y = std::max(Max.y, p.y); Max.z = std::max(Max.z, p.z);
	}
	/// Powiêksza tak ¿eby na pewno zawiera³ podany boks
	/** Podany boks musi byæ prawid³owy. */
	void AddInternalBox(const BOX &b)
	{
		Min.x = std::min(Min.x, b.Min.x); Min.y = std::min(Min.y, b.Min.y); Min.z = std::min(Min.z, b.Min.z);
		Max.x = std::max(Max.x, b.Max.x); Max.y = std::max(Max.y, b.Max.y); Max.z = std::max(Max.z, b.Max.z);
	}
};

extern const BOX BOX_ZERO;
extern const BOX BOX_ZERO_ONE;
extern const BOX BOX_MINUS_ONE_ONE;
extern const BOX BOX_MAX; ///< Maximal possible box: -FLT_MAX..FLT_MAX
extern const BOX BOX_INV_MAX; ///< Inverted maximal box: FLT_MAX..-FLT_MAX. It's invalid. Use it to initialize box for finding min-max.


inline void swap(BOX &b1, BOX &b2)
{
	std::swap(b1.Min, b2.Min);
	std::swap(b1.Max, b2.Max);
}

inline void Mul(BOX *Out, const BOX &b, const VEC3 &v)
{
	Mul(&Out->Max, b.Max, v);
	Mul(&Out->Max, b.Max, v);
}
inline void Div(BOX *Out, const BOX &b, const VEC3 &v)
{
	Div(&Out->Max, b.Max, v);
	Div(&Out->Max, b.Max, v);
}

inline void Mul(BOX *inout, const VEC3 &v)
{
	Mul(&inout->Max, v);
	Mul(&inout->Max, v);
}
inline void Div(BOX *inout, const VEC3 &v)
{
	Div(&inout->Max, v);
	Div(&inout->Max, v);
}

/// Zwraca true, jeœli punkt nale¿y do boksa
inline bool PointInBox(const VEC3 &p, const BOX &b)
{
	return
		(p.x >= b.Min.x) && (p.x <= b.Max.x) &&
		(p.y >= b.Min.y) && (p.y <= b.Max.y) &&
		(p.z >= b.Min.z) && (p.z <= b.Max.z);
}

/// Zwraca true, jeœli boks sb zawiera siê w ca³oœci wewn¹trz b
inline bool BoxInBox(const BOX &sb, const BOX &b)
{
	return
		(sb.Min.x >= b.Min.x) && (sb.Max.x <= b.Max.x) &&
		(sb.Min.y >= b.Min.y) && (sb.Max.y <= b.Max.y) &&
		(sb.Min.z >= b.Min.z) && (sb.Max.z <= b.Max.z);
}

/// Zwraca true, jeœli podane boksy zachodz¹ na siebie
inline bool BoxToBox(const BOX &b1, const BOX &b2)
{
	return
		(b1.Min.x <= b2.Max.x) && (b1.Max.x >= b2.Min.x) &&
		(b1.Min.y <= b2.Max.y) && (b1.Max.y >= b2.Min.y) &&
		(b1.Min.z <= b2.Max.z) && (b1.Max.z >= b2.Min.z);
}

/// Jeœli boksy posiadaj¹ czêœæ wspóln¹, zwraca j¹ przez Out i zwraca true
inline bool Intersection(BOX *Out, const BOX &b1, const BOX &b2)
{
	*Out = BOX(
		VEC3(
			std::max(b1.Min.x, b2.Min.x),
			std::max(b1.Min.y, b2.Min.y),
			std::max(b1.Min.z, b2.Min.z)),
		VEC3(
			std::min(b1.Max.x, b2.Max.x),
			std::min(b1.Max.y, b2.Max.y),
			std::min(b1.Max.z, b2.Max.z)));
	return Out->IsValid();
}

/// Zwraca boks opisany na podanych dwóch boksach
inline void Union(BOX *Out, const BOX &b1, const BOX &b2)
{
	*Out = BOX(
		VEC3(
			std::min(b1.Min.x, b2.Min.x),
			std::min(b1.Min.y, b2.Min.y),
			std::min(b1.Min.z, b2.Min.z)),
		VEC3(
			std::max(b1.Max.x, b2.Max.x),
			std::max(b1.Max.y, b2.Max.y),
			std::max(b1.Max.z, b2.Max.z)));
}
inline void Lerp(BOX *out, const BOX &v1, const BOX &v2, float t)
{
	Lerp(&out->Min, v1.Min, v2.Min, t);
	Lerp(&out->Max, v1.Max, v2.Max, t);
}

//@}
// math_aabb


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_aabb OBB
Prostopad³oœcian o dowolnej orientacji OBB - Oriented Bounding Box */
//@{

/// OBB - Oriented Bounding Box - prostopad³oœcian o dowolnej orientacji
struct OBB
{
	/// Pozycja œrodka
	VEC3 Center;
	/// Po³owa szerokoœci, wysokoœci, g³êbokoœci
	VEC3 HalfSize;
	/// Osie wskazuj¹ce lokalne kierunki X, Y, Z.
	/** Musz¹ byæ jednostkowe i wzajemnie prostopad³e. */
	VEC3 Axes[3];
};

/// Zwraca true, jeœli dwa prostopad³oœciany OBB nachodz¹ na siebie.
bool ObbToObb(const OBB &a, const OBB &b);

//@}
// math_obb


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_rect3d Rect 3D
Dowolnie zorientowany prostok¹t w przestrzeni 3D - tak jakby p³aski OBB. */
//@{

/// P³aski prostok¹t, dowolnie zorientowany w przestrzeni 3D.
struct RECT3D
{
	/// Pozycja œrodka
	VEC3 Center;
	/// Po³owa szerokoœci, wysokoœci
	VEC2 HalfSize;
	/// Osie wskazuj¹ce lokalne kierunki X, Y.
	/** Musz¹ byæ jednostkowe i wzajemnie prostopad³e. */
	VEC3 Axes[2];
};

//@}
// math_rect3d


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_colors Kolory
Sta³e z kolorami, operacje generuj¹ce kolory itp. wype³niaj¹ kana³ alfa jedynk¹.
*/
//@{

/// Kolor w formacie A8R8G8B8.
/**
Operacje dodawania i odejmowania kolorów, a tak¿e dodawania, odejmowania,
mno¿enia i dzielenia przez liczbê pilnuj¹ zakresu i w razie czego obcinaj¹,
nie zawijaj¹.
*/
struct COLOR
{
	union
	{
		/// Ca³y kolor razem
		uint32 ARGB;
		/// Osobno poszczególne sk³adowe
		PACKED struct
		{
			uint8 B, G, R, A;
		};
	};

	COLOR() { }
	COLOR(uint32 c) : ARGB(c) { }
	COLOR(unsigned long c) : ARGB((uint32)c) { }
	COLOR(uint8 R, uint8 G, uint8 B) : B(B), G(G), R(R), A(0xFF) { }
	COLOR(uint8 A, uint8 R, uint8 G, uint8 B) : B(B), G(G), R(R), A(A) { }

	bool operator == (COLOR c) const { return ARGB == c.ARGB; }
	bool operator != (COLOR c) const { return ARGB != c.ARGB; }

	// Operatory unarne - dany kolor i negatyw koloru
	COLOR operator + () const { return *this; }
	COLOR operator - () const { return COLOR(~ARGB); }
	// Dodawanie i odejmowanie kolorów
	COLOR operator + (const COLOR &c) const { return COLOR(safe_add(A, c.A), safe_add(R, c.R), safe_add(G, c.G), safe_add(B, c.B)); }
	COLOR operator - (const COLOR &c) const { return COLOR(safe_sub(A, c.A), safe_sub(R, c.R), safe_sub(G, c.G), safe_sub(B, c.B)); }
	COLOR & operator += (const COLOR &c) { A = safe_add(A, c.A); R = safe_add(R, c.R); G = safe_add(G, c.G); B = safe_add(B, c.B); return *this; }
	COLOR & operator -= (const COLOR &c) { A = safe_sub(A, c.A); R = safe_sub(R, c.R); G = safe_sub(G, c.G); B = safe_sub(B, c.B); return *this; }
	// Mno¿enie i dzielenie koloru przez liczbê
	// Autor: Tarlandil
	COLOR operator * (float s) const { return COLOR( (uint8)(minmax(0, (int)(A*s), 255)), (uint8)(minmax(0, (int)(R*s), 255)), (uint8)(minmax(0, (int)(G*s), 255)), (uint8)(minmax(0, (int)(B*s), 255)) ); }
	COLOR operator / (float s) const { float sInv = 1.0f/s; return COLOR( (uint8)(minmax(0, (int)(A*sInv), 255)), (uint8)(minmax(0, (int)(R*sInv), 255)), (uint8)(minmax(0, (int)(G*sInv), 255)), (uint8)(minmax(0, (int)(B*sInv), 255)) ); }
	COLOR & operator *= (float s) { A = (uint8)(minmax(0, (int)(A*s), 255)); R = (uint8)(minmax(0, (int)(R*s), 255)); G = (uint8)(minmax(0, (int)(G*s), 255)); B = (uint8)(minmax(0, (int)(B*s), 255)); return *this; }
	COLOR & operator /= (float s) { float sInv = 1.0f/s; A = (uint8)(minmax(0, (int)(A*sInv), 255)); R = (uint8)(minmax(0, (int)(R*sInv), 255)); G = (uint8)(minmax(0, (int)(G*sInv), 255)); B = (uint8)(minmax(0, (int)(B*sInv), 255));  return *this; }
	// Mno¿enie dwóch kolorów, czyli modulacja
	COLOR operator * (const COLOR &c) const { return COLOR( (uint8)(((int)A*(int)c.A)/255), (uint8)(((int)R*(int)c.R)/255), (uint8)(((int)G*(int)c.G)/255), (uint8)(((int)B*(int)c.B)/255) ); }
	COLOR & operator *= (const COLOR &c) { A = (uint8)(((int)A*(int)c.A)/255); R = (uint8)(((int)R*(int)c.R)/255); G = (uint8)(((int)G*(int)c.G)/255); B = (uint8)(((int)B*(int)c.B)/255); return *this; }
};

extern const COLOR COLOR_TRANSPARENT_;
extern const COLOR COLOR_BLACK;
extern const COLOR COLOR_GRAY;
extern const COLOR COLOR_SILVER;
extern const COLOR COLOR_WHITE;
extern const COLOR COLOR_FUCHSIA;
extern const COLOR COLOR_RED;
extern const COLOR COLOR_ORANGE;
extern const COLOR COLOR_YELLOW;
extern const COLOR COLOR_LIME;
extern const COLOR COLOR_AQUA;
extern const COLOR COLOR_BLUE;
extern const COLOR COLOR_PURPLE;
extern const COLOR COLOR_MAROON;
extern const COLOR COLOR_OLIVE;
extern const COLOR COLOR_GREEN;
extern const COLOR COLOR_TEAL;

/// Kolor z 4 liczb typu float - R, G, B, A.
/**
Operacje dodawania i odejmowania kolorów, a tak¿e dodawania, odejmowania,
mno¿enia i dzielenia przez liczbê nie pilnuj¹ zakresu i wartoœci sk³adowych
mog¹ urosn¹æ powy¿ej 1.0 albo zmaleæ poni¿ej 0.0.
*/
struct COLORF
{
	float R, G, B, A;

	COLORF() { }
	COLORF(float R, float G, float B) : R(R), G(G), B(B), A(1.0f) { }
	COLORF(float A, float R, float G, float B) : R(R), G(G), B(B), A(A) { }
	COLORF(const float *Arr) : R(Arr[0]), G(Arr[1]), B(Arr[2]), A(Arr[3]) { }

	operator float * () { return &R; }
	operator const float * () const { return &R; }

	bool operator == (const COLORF &c) const { return A == c.A && R == c.R && G == c.G && B == c.B; }
	bool operator != (const COLORF &c) const { return A != c.A || R != c.R || G != c.G || B != c.B; }

	// Operatory unarne - dany kolor i negatyw koloru
	COLORF operator + () const { return *this; }
	COLORF operator - () const { return COLORF(1.0f-A, 1.0f-R, 1.0f-G, 1.0f-B); }
	// Dodawanie i odejmowanie kolorów
	COLORF operator + (const COLORF &c) const { return COLORF(A+c.A, R+c.R, G+c.G, B+c.B); }
	COLORF operator - (const COLORF &c) const { return COLORF(A-c.A, R-c.R, G-c.G, B-c.B); }
	COLORF & operator += (const COLORF &c) { A += c.A; R += c.R; G += c.G; B += c.B; return *this; }
	COLORF & operator -= (const COLORF &c) { A -= c.A; R -= c.R; G -= c.G; B -= c.B; return *this; }
	// Mno¿enie i dzielenie koloru przez liczbê
	COLORF operator * (float s) const { return COLORF(A*s, R*s, G*s, B*s); }
	COLORF operator / (float s) const { float sInv = 1.0f/s; return COLORF(A*sInv, R*sInv, G*sInv, B*sInv); }
	COLORF & operator *= (float s) { A *= s; R *= s; G *= s; B *= s; return *this; }
	COLORF & operator /= (float s) { float sInv = 1.0f/s; A *= sInv; R *= sInv; G *= sInv; B *= sInv; return *this; }
	// Mno¿enie dwóch kolorów, czyli modulacja
	COLORF operator * (const COLORF &c) const { return COLORF(A*c.A, R*c.R, G*c.G, B*c.B); }
	COLORF & operator *= (const COLORF &c) { A *= c.A; R *= c.R; G *= c.G; B *= c.B; return *this; }

	/// Sprawdza, czy sk³adowe mieszcz¹ siê w zakresie 0.0 .. 1.0
	bool IsValid() const { return (A >= 0.0f && A <= 1.0f) && (R >= 0.0f && R <= 1.0f) && (G >= 0.0f && G <= 1.0f) && (B >= 0.0f && B <= 1.0f); }
	/// Sprowadza sk³adowe do zakresu 0.0 .. 1.0
	void Repair()
	{
		A = minmax(0.0f, A, 1.0f); R = minmax(0.0f, R, 1.0f);
		G = minmax(0.0f, G, 1.0f); B = minmax(0.0f, B, 1.0f);
	}
};

inline void swap(COLORF &c1, COLORF &c2)
{
	std::swap(c1.A, c2.A); std::swap(c1.R, c2.R);
	std::swap(c1.G, c2.G); std::swap(c1.B, c2.B);
}

extern const COLORF COLORF_TRANSPARENT_;
extern const COLORF COLORF_BLACK;
extern const COLORF COLORF_GRAY;
extern const COLORF COLORF_SILVER;
extern const COLORF COLORF_WHITE;
extern const COLORF COLORF_FUCHSIA;
extern const COLORF COLORF_RED;
extern const COLORF COLORF_ORANGE;
extern const COLORF COLORF_YELLOW;
extern const COLORF COLORF_LIME;
extern const COLORF COLORF_AQUA;
extern const COLORF COLORF_BLUE;
extern const COLORF COLORF_PURPLE;
extern const COLORF COLORF_MAROON;
extern const COLORF COLORF_OLIVE;
extern const COLORF COLORF_GREEN;
extern const COLORF COLORF_TEAL;

inline COLORF operator * (float s, const COLORF &c) { return COLORF(c.A*s, c.R*s, c.G*s, c.B*s); }
inline COLORF operator / (float s, const COLORF &c) { float sInv = 1.0f/s; return COLORF(c.A*sInv, c.R*sInv, c.G*sInv, c.B*sInv); }

inline COLOR operator * (float s, const COLOR &c) { return COLOR( (uint8)(minmax(0, (int)(c.A*s), 255)), (uint8)(minmax(0, (int)(c.R*s), 255)), (uint8)(minmax(0, (int)(c.G*s), 255)), (uint8)(minmax(0, (int)(c.B*s), 255)) ); }
inline COLOR operator / (float s, const COLOR &c) { float sInv = 1.0f/s; return COLOR( (uint8)(minmax(0, (int)(c.A*sInv), 255)), (uint8)(minmax(0, (int)(c.R*sInv), 255)), (uint8)(minmax(0, (int)(c.G*sInv), 255)), (uint8)(minmax(0, (int)(c.B*sInv), 255)) ); }

template <> inline uint32 & math_cast<uint32, COLOR>(const COLOR &x) { return (uint32&)(x); }
template <> inline unsigned long & math_cast<unsigned long, COLOR>(const COLOR &x) { return (unsigned long&)(x); }
template <> inline COLOR & math_cast<COLOR, uint32>(const uint32 &x) { return (COLOR&)(x); }
template <> inline COLOR & math_cast<COLOR, unsigned long>(const unsigned long &x) { return (COLOR&)(x); }

#ifdef USE_DIRECTX
	template <> inline D3DXCOLOR & math_cast<D3DXCOLOR, COLORF>(const COLORF &x) { return (D3DXCOLOR&)(x); }
	template <> inline COLORF & math_cast<COLORF, D3DXCOLOR>(const D3DXCOLOR &x) { return (COLORF&)(x); }
#endif

/// Konwersje miêdzy tymi dwoma reprezentacjami kolorów
inline void ColorToColorf(COLORF *Out, COLOR c)
{
	Out->R = c.R / 255.0f;
	Out->G = c.G / 255.0f;
	Out->B = c.B / 255.0f;
	Out->A = c.A / 255.0f;
}
/** Tu musi byæ valid, inaczej wyjdzie zawiniête. */
inline COLOR ColorfToColor(const COLORF &c)
{
	return COLOR(
		(uint8)(minmax(0.0f, c.A, 1.0f)*255.0f+0.5f),
		(uint8)(minmax(0.0f, c.R, 1.0f)*255.0f+0.5f),
		(uint8)(minmax(0.0f, c.G, 1.0f)*255.0f+0.5f),
		(uint8)(minmax(0.0f, c.B, 1.0f)*255.0f+0.5f));
}

/// Konwersja koloru na ³añcuch
/** Formatem mo¿e byæ:
- 'x' = RRGGBB szesnastkowo
- 'X' = AARRGGBB szesnastkowo
- 'f' = R,G,B floaty
- 'F' = A,R,G,B floaty
*/
void ColorToStr(tstring *Out, COLOR Color, char Format = 'X');
void ColorToStr(tstring *Out, const COLORF &Color, char Format = 'F');
/// Konwersja ³añcucha na kolor
/** Jeœli kana³ alfa nie podany, zostaje wype³niony wartoœci¹ maksymaln¹.
£añcuchem mo¿e byæ:
- jedna z 16 nazw standardowych kolorów (wielkoœæ liter dowolna)
- wartoœæ szesnastkowa RRGGBB
- wartoœæ szesnastkowa AARRGGBB
- liczby float R,G,B
- liczby float A,R,G,B
*/
bool StrToColor(COLOR *Out, const tstring &Str);
bool StrToColor(COLORF *Out, const tstring &Str);

inline void Add(COLORF *Out, const COLORF &c1, const COLORF &c2)
{
	Out->A = c1.A + c2.A; Out->R = c1.R + c2.R;
	Out->G = c1.G + c2.G; Out->B = c1.B + c2.B;
}
inline void Sub(COLORF *Out, const COLORF &c1, const COLORF &c2)
{
	Out->A = c1.A - c2.A; Out->R = c1.R - c2.R;
	Out->G = c1.G - c2.G; Out->B = c1.B - c2.B;
}
inline void Mul(COLORF *Out, const COLORF &c1, const COLORF &c2)
{
	Out->A = c1.A * c2.A; Out->R = c1.R * c2.R;
	Out->G = c1.G * c2.G; Out->B = c1.B * c2.B;
}
inline void Mul(COLORF *inout, const COLORF &c)
{
	inout->A *= c.A; inout->R *= c.R;
	inout->G *= c.G; inout->B *= c.B;
}
inline void Mul(COLORF *Out, const COLORF &c, float s)
{
	Out->A = c.A * s; Out->R = c.R * s;
	Out->G = c.G * s; Out->B = c.B * s;
}
inline void Div(COLORF *Out, const COLORF &c, float s)
{
	Out->A = c.A / s; Out->R = c.R / s;
	Out->G = c.G / s; Out->B = c.B / s;
}
inline void Negative(COLORF *Out, const COLORF &c)
{
	Out->A = 1.0f - c.A; Out->R = 1.0f - c.R;
	Out->G = 1.0f - c.G; Out->B = 1.0f - c.B;
}

/// Konwertuje kolor miêdzy systemami ARGB i ABGR
inline COLOR ARGB_ABGR(COLOR color)
{
	return COLOR(
		(color.ARGB & 0xFF00FF00) |
		((color.ARGB >> 16) & 0x000000FF) |
		((color.ARGB & 0x000000FF) << 16)
		);
}

/// Konwertuje zwyk³y kolor COLOR (A8R8G8B8) na R5G6B5
inline uint16 ColorToR5G6B5(COLOR color)
{
	return
		((unsigned short)(color.R >> 3) << 11) |
		((unsigned short)(color.G >> 2) <<  5) |
		((unsigned short)(color.B >> 3));
}

/// Konwertuje kolor R5G6B5 na zwyk³y kolor COLOR (A8R8G8B8)
inline COLOR R5G6B5ToColor(uint16 color)
{
	return COLOR(
		255,
		(uint8)(color >> 11 << 3),
		(uint8)(color >>  5 << 2),
		(uint8)(color <<  3));
}

inline void Lerp(COLORF *Out, const COLORF &c1, const COLORF &c2, float t)
{
	Out->A = Lerp(c1.A, c2.A, t);
	Out->R = Lerp(c1.R, c2.R, t);
	Out->G = Lerp(c1.G, c2.G, t);
	Out->B = Lerp(c1.B, c2.B, t);
}
inline COLOR Lerp(COLOR c1, COLOR c2, float t)
{
	return COLOR(
		(uint8)(minmax(0, (int)(c1.A + t*((float)c2.A-(float)c1.A)), 255)),
		(uint8)(minmax(0, (int)(c1.R + t*((float)c2.R-(float)c1.R)), 255)),
		(uint8)(minmax(0, (int)(c1.G + t*((float)c2.G-(float)c1.G)), 255)),
		(uint8)(minmax(0, (int)(c1.B + t*((float)c2.B-(float)c1.B)), 255)) );
}
inline void saturate(COLORF &v)
{
	v.R = saturate(v.R); v.G = saturate(v.G); v.B = saturate(v.B); v.A = saturate(v.A);
}

/// Konwertuje kolor miêdzy HSB a RGB
/**
- Hue, Saturation, Brightness = 0..1
- Kana³ alfa zwraca 1.
*/
void HsbToRgb(COLORF *Out, float hue, float saturation, float brightness);
/// Konwertuje kolor RGB na HSB
void RgbToHsb(float *OutHue, float *OutSaturation, float *OutBrightness, const COLORF &Color);

/// Zwraca kolor têczy od czerwonego (odpowiada mu t = 0) do niebieskiego (t = 1)
/** Kana³ alfa zwraca 0. */
inline void ColorRainbow(COLORF *Out, float t)
{
	HsbToRgb(Out, t*0.75f, 1.0f, 1.0f);
}

/// Zwraca jasnoœæ podanego koloru (odcieñ szaroœci) z zakresu 0..1
inline float ColorToGrayscale(const COLORF &c)
{
	return 0.299f * c.R + 0.587f * c.G + 0.114f * c.B;
}
inline float ColorToGrayscale(COLOR &c)
{
	return 0.299f * (c.R/255.0f) + 0.587f * (c.G/255.0f) + 0.114f * (c.B/255.0f);
}

/// Zwraca kolor szary o podanej jasnoœci t = 0..1
inline void GrayscaleToColor(COLORF *Out, float t)
{
	Out->A = 1.0f;
	Out->R = Out->G = Out->B = t;
}
inline void GrayscaleToColor(COLOR *Out, float t)
{
	Out->A = 255;
	Out->R = Out->G = Out->B = (uint8)(t*255.0f);
}

/// Zmienia kolorowi kontrast na miejscu
/** Dla t = 0 wychodzi idealnie po³ówkowy szary. \n
Dla t = 1 wychodzi kolor oryginalny. \n
t mo¿e te¿ byæ wiêksze ni¿ 1. \n
Alfa zostaje bez zmian. */
inline void AdjustContrast(COLORF *Color, float t)
{
	Color->R = 0.5f + t * (Color->R - 0.5f);
	Color->G = 0.5f + t * (Color->G - 0.5f);
	Color->B = 0.5f + t * (Color->B - 0.5f);
}

/// Zmienia kolorowi nasycenie na miejscu
/** Dla t = 0 wychodzi dany kolor skonwertowany do odcienia szaroœci. \n
Dla t = 1 wychodzi kolor oryginalny. \n
t < 0 zmniejsza nasycenie, t > 0 zwiêksza nasycenie \n
Alfa zostaje bez zmian. */
inline void AdjustSaturation(COLORF *Color, float t)
{
	float gray = ColorToGrayscale(*Color);
	Color->R = gray + t * (Color->R - gray);
	Color->G = gray + t * (Color->G - gray);
	Color->B = gray + t * (Color->B - gray);
}

/// Przetwarza znormalizowany wektor na kolor XRGB. A zwraca 0.
inline COLOR VectorToColor(const VEC3 &v)
{
	int R = minmax(0, (int)(127.0f * v.x + 128.0f), 255);
	int G = minmax(0, (int)(127.0f * v.y + 128.0f), 255);
	int B = minmax(0, (int)(127.0f * v.z + 128.0f), 255);
	return COLOR((uint32)((R << 16) | (G << 8) | B));
}

/// Przetwarza kolor na wektor
inline void ColorToVector(VEC3 *Out, COLOR color)
{
	const float o = 1.0f / 127.0f;
	Out->x = (((color.ARGB >> 16) & 0xFF) - 128.0f) * o;
	Out->y = (((color.ARGB >>  8) & 0xFF) - 128.0f) * o;
	Out->z = (((color.ARGB      ) & 0xFF) - 128.0f) * o;
}

/// Zwraca "odleg³oœæ" dwóch kolorów wg jakiejœtam byle jakiej metryki. Uwzglêdnia sk³adowe R, G, B.
inline uint ColorDistance(COLOR Color1, COLOR Color2)
{
	int rd = (int)Color1.R - (int)Color2.R;
	int gd = (int)Color1.G - (int)Color2.G;
	int bd = (int)Color1.B - (int)Color2.B;
	return (uint)(rd*rd + gd*gd + bd*bd);
}

/// Converts between COLORF and VEC4, where (x,y,z,w) = (R,G,B,A)
inline VEC4 ColorfToVec4(const COLORF &c) { return VEC4(c.R, c.G, c.B, c.A); }
/// \copydoc ColorfToVec4
inline COLORF Vec4ToColorf(const VEC4 &v) { return COLORF(v.w, v.x, v.y, v.z); }

//@}
// math_colors


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_plane P³aszczyzna */
//@{

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
	typedef D3DXPLANE PLANE;
#else
	/// P³aszczyzna
	/**
	Reprezentuje p³aszczyznê w przestrzeni 3D z jedn¹ stron¹ dodatni¹, a drug¹
	ujemn¹. Opisana jest równaniem A x + B y + C z + D = 0. Wektor [A,B,C] to wektor
	normalny wskazuj¹cy w kierunku dodatniej pó³przestrzeni. Jeœli p³aszczyzna jest
	znormalizowana, wektor normalny jest znormalizowany, a D to odleg³oœæ
	p³aszczyzny od œrodka uk³adu wspó³rzêdnych.
	Reprezentacja jako string to A,R,G,B np. "1,2,3,0.2".
	P³aszczyzna maj¹ca a=b=c=0 (czyli sta³a ZERO) jest niepoprawna, zdegenerowana.
	*/
	struct PLANE
	{
		float a, b, c, d;

		PLANE() { }
		PLANE(float a, float b, float c, float d) : a(a), b(b), c(c), d(d) { }
		PLANE(const float *Arr) : a(Arr[0]), b(Arr[1]), c(Arr[2]), d(Arr[3]) { }

		operator float * () { return &a; }
		operator const float * () const { return &a; }

		bool operator == (const PLANE &p) const { return a == p.a && b == p.b && c == p.c && d == p.d; }
		bool operator != (const PLANE &p) const { return a != p.a || b != p.b || c != p.c || d != p.d; }

		PLANE operator + () const { return *this; }
		/// Zwraca p³aszczyznê zwrócon¹ w przeciwn¹ stronê
		PLANE operator - () const { return PLANE(-a, -b, -c, -d); }

		PLANE operator * (float s) const { return PLANE(a*s, b*s, c*s, d*s); }
		PLANE operator / (float s) const { float sInv = 1.0f/s; return PLANE(a*sInv, b*sInv, c*sInv, d*sInv); }
		PLANE & operator *= (float s) { a *= s; b *= s; c *= s; d *= s; return *this; }
		PLANE & operator /= (float s) { float sInv = 1.0f/s; a *= sInv; b *= sInv; c *= sInv; d *= sInv; return *this; }
	};
	inline PLANE operator * (float s, const PLANE &p) { return PLANE(p.a*s, p.b*s, p.c*s, p.d*s); }
#endif

extern const PLANE PLANE_ZERO;
extern const PLANE PLANE_POSITIVE_X; ///< P³aszczyzna pionowa, równoleg³a do Y i Z, wskazuj¹ca na prawo na X
extern const PLANE PLANE_POSITIVE_Y; ///< P³aszczyzna pozioma, równoleg³a do X i Z, wskazuj¹ca do góry na Y
extern const PLANE PLANE_POSITIVE_Z; ///< P³aszczyzna pionowa, równoleg³a do X i Y, wskazuj¹ca w g³¹b na Z
extern const PLANE PLANE_NEGATIVE_X; ///< P³aszczyzna pionowa, równoleg³a do Y i Z, wskazuj¹ca na lewo na X
extern const PLANE PLANE_NEGATIVE_Y; ///< P³aszczyzna pozioma, równoleg³a do X i Z, wskazuj¹ca w dó³ na Y
extern const PLANE PLANE_NEGATIVE_Z; ///< P³aszczyzna pionowa, równoleg³a do X i Y, wskazuj¹ca do gracza na Z

inline void swap(PLANE &p1, PLANE &p2)
{
	std::swap(p1.a, p2.a); std::swap(p1.b, p2.b);
	std::swap(p1.c, p2.c); std::swap(p1.d, p2.d);
}

/// Daje dostêp do wektora normalnego podanej p³aszczyzny, czyli jej sk³adowych a, b, c zrzutowanych jako VEC3.
inline VEC3 & GetPlaneNormal(PLANE &Plane) { return (VEC3&)Plane.a; }
/// Zwraca wektor normalny p³aszczyzny, czyli jej sk³adowe a, b, c zrzutowane jako VEC3.
inline const VEC3 & GetPlaneNormal(const PLANE &Plane) { return (const VEC3&)Plane.a; }

/// Zwraca jakiœ punkt le¿¹cy na tej p³aszczyŸnie
/** \param Plane Musi byæ znormalizowana. */
inline void GetPlaneMemberPoint(VEC3 *Out, const PLANE &Plane)
{
	*Out = GetPlaneNormal(Plane) * -Plane.d;
}

/// Zmienia odleg³oœæ od pocz¹tku uk³adu wspó³rzêdnych.
/** P³aszczyzna pozostaj¹c tak samo zorientowana zosta³a przesuniêta tak, ¿eby
zawieraæ w sobie podany punkt.
\param[in,out] Plane Musi byæ znormalizowana.
*/
inline void RecalcPlaneD(PLANE *Plane, const VEC3 &p)
{
	Plane->d = -Dot(GetPlaneNormal(*Plane), p);
}

/// Zwraca true, jeœli podane p³aszczyzny s¹ sobie mniej wiêcej równe
inline bool PlaneEqual(const PLANE &p1, const PLANE &p2) {
	return float_equal(p1.a, p2.a) && float_equal(p1.b, p2.b) && float_equal(p1.c, p2.c) && float_equal(p1.d, p2.d);
}

/// Tworzy p³aszczyznê na podstawie trzech punktów
/** P³aszczyzna bêdzie zorientowana dodatnio w kierunku tej pó³przestrzeni,
z której wierzcho³ki trójk¹ta s¹ widoczne jako zorientowane zgodnie z ruchem
wskazówek zegara w uk³adzie LH. */
inline void PointsToPlane(PLANE *Out, const VEC3 &p1, const VEC3 &p2, const VEC3 &p3) {
	Cross(&GetPlaneNormal(*Out), p2-p1, p3-p1);
	Normalize(&GetPlaneNormal(*Out));
	Out->d = -Dot(p1, GetPlaneNormal(*Out));
}
/// Tworzy p³aszczyznê z wektora normalnego i punktu le¿¹cego na tej p³aszczyŸnie
/** Czy Normal musi byæ znormalizowany? Nie wiem. */
inline void PointNormalToPlane(PLANE *Out, const VEC3 &Point, const VEC3 &Normal) {
	Out->a = Normal.x; Out->b = Normal.y; Out->c = Normal.z;
	Out->d = -Dot(Normal, Point);
}

/// Skalowanie p³aszczyzny
inline void Mul(PLANE *Out, const PLANE &p, float s)
{
	Out->a = p.a * s; Out->b = p.b * s;
	Out->c = p.c * s; Out->d = p.d * s;
}
inline void Div(PLANE *Out, const PLANE &p, float s)
{
	Out->a = p.a / s; Out->b = p.b / s;
	Out->c = p.c / s; Out->d = p.d / s;
}

/// Iloczyn skalarny p³aszczyzny i punktu we wspó³rzêdnych jednorodnych
/** Mo¿na u¿yæ do wyznaczenia czy tak zapisany punkt le¿y na p³aszczyŸnie,
po której stronie i jak daleko. */
inline float Dot(const PLANE &p, const VEC4 &pt)
{
	return p.a*pt.x + p.b*pt.y + p.c*pt.z + p.d*pt.w;
}

/// Iloczyn skalarny p³aszczyzny z punktem 3D: a*x+b*y+c*z+d*1
/** Mo¿na u¿yæ do wyznaczenia czy punkt le¿y na p³aszczyŸnie, po której stronie
i jak daleko. */
inline float DotCoord(const PLANE &p, const VEC3 &pt)
{
	return p.a*pt.x + p.b*pt.y + p.c*pt.z + p.d;
}

/// Iloczyn skalarny p³aszczyzny z wektorem 3D: a*x+b*y+c*z+d*0
/** Przydatny do wyznaczania k¹ta miêdzy p³aszczyzn¹ (jej normaln¹) a wektorem. */
inline float DotNormal(const PLANE &p, const VEC3 &v)
{
	return p.a*v.x + p.b*v.y + p.c*v.z;
}

/// Normalizacja p³aszczyzny
/** Po tej operacji wektor normalny p³aszczyzny jest znormalizowany.
P³aszczyzna pozostaje po³o¿ona i zorientowana tam gdzie by³a. */
inline void Normalize(PLANE *Out, const PLANE &p)
{
	float nlsq = 1.0f / Length(GetPlaneNormal(p));
	Out->a = p.a * nlsq; Out->b = p.b * nlsq;
	Out->c = p.c * nlsq; Out->d = p.d * nlsq;
}
/// Normalizacja p³aszczyzny w miejscu
inline void Normalize(PLANE *p)
{
	float nlsq = 1.0f / Length(GetPlaneNormal(*p));
	p->a *= nlsq; p->b *= nlsq;
	p->c *= nlsq; p->d *= nlsq;
}

/// Returns plane of one of 6 possible box faces.
/** Face order is: -X, +X, -Y, +Y, -Z, +Z.
Returned plane is normalized and points to the outside.
*/
void GetBoxPlane(PLANE *out, const BOX &box, uint index);

//@}
// math_plane


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_line2d Prosta 2D */
//@{

/// Prosta 2D
/**
Reprezentuje prost¹ w przestrzeni 2D z jedn¹ stron¹ dodatni¹, a drug¹
ujemn¹. Opisana jest równaniem A x + B y + C = 0. Wektor [A,B] to wektor
normalny wskazuj¹cy w kierunku dodatniej pó³p³aszczyzny. Jeœli prosta jest
znormalizowana, wektor normalny jest znormalizowany, a C to odleg³oœæ prostej
od œrodka uk³adu wspó³rzêdnych.
Reprezentacja jako string to A,B,C, np. "2,0,2.1"
Prosta maj¹ca a = b = 0 (czyli np. sta³a ZERO) jest niepoprawna, zdegenerowana.
*/
struct LINE2D
{
	float a, b, c;

	LINE2D() { }
	LINE2D(float a, float b, float c) : a(a), b(b), c(c) { }
	LINE2D(const VEC2 &Normal, float c) : a(Normal.x), b(Normal.y), c(c) { }
	LINE2D(const float *Arr) : a(Arr[0]), b(Arr[1]), c(Arr[2]) { }

	operator float * () { return &a; }
	operator const float * () const { return &a; }

	VEC2 & GetNormal() { return (VEC2&)a; }
	const VEC2 & GetNormal() const { return (VEC2&)a; }

	bool operator == (const LINE2D &L) const { return a == L.a && b == L.b && c == L.c; }
	bool operator != (const LINE2D &L) const { return a != L.a || b != L.b || c != L.c; }

	LINE2D operator + () const { return *this; }
	/// Zwraca prost¹ zwrócon¹ w przeciwn¹ stronê
	LINE2D operator - () const { return LINE2D(-a, -b, -c); }

	// Skalowanie
	LINE2D operator * (float s) const { return LINE2D(a*s, b*s, c*s); }
	LINE2D operator / (float s) const { float sInv = 1.0f/s; return LINE2D(a*sInv, b*sInv, c*sInv); }
	LINE2D & operator *= (float s) { a *= s; b *= s; c *= s; return *this; }
	LINE2D & operator /= (float s) { float sInv = 1.0f/s; a *= sInv; b *= sInv; c *= sInv; return *this; }

	/// Zwraca wektor wskazuj¹cy w kierunku prostej
	void GetTangent(VEC2 *Out) { Out->x = b; Out->y = -a; }

	/// Zwraca jakiœ punkt le¿¹cy na tej prostej
	/** Musi byæ znormalizowana. */
	void GetMemberPoint(VEC2 *Out)
	{
		*Out = GetNormal() * -c;
	}

	/// Zmienia odleg³oœæ od pocz¹tku uk³adu wspó³rzêdnych
	/** Prosta pozostaj¹c tak samo
	zorientowana zostaje przesuniêta tak, ¿eby zawieraæ w sobie podany punkt.
	Musi byæ znormalizowana. */
	void RecalcD(const VEC2 &p)
	{
		c = -Dot(GetNormal(), p);
	}
};
inline LINE2D operator * (float s, const LINE2D &p) { return LINE2D(p.a*s, p.b*s, p.c*s); }
inline LINE2D operator / (float s, const LINE2D &p) { float sInv = 1.0f/s; return LINE2D(p.a*sInv, p.b*sInv, p.c*sInv); }

extern const LINE2D LINE2D_ZERO;
extern const LINE2D LINE2D_POSITIVE_X;
extern const LINE2D LINE2D_NEGATIVE_X;
extern const LINE2D LINE2D_POSITIVE_Y;
extern const LINE2D LINE2D_NEGATIVE_Y;

/// Daje dostêp do wektora normalnego podanej prostej, czyli jej sk³adowych a, b zrzutowanych na VEC2.
inline VEC2 & GetLineNormal(LINE2D &line) { return (VEC2&)line.a; }
/// Zwraca wektor normalny prostej, czyli jej sk³adowe a, b zrzutowane na VEC2.
inline const VEC2 & GetLineNormal(const LINE2D &line) { return (const VEC2&)line.a; }

/// Zwraca true, jeœli podane proste s¹ sobie mniej wiêcej równe
inline bool LineEqual(const LINE2D &p1, const LINE2D &p2) {
	return float_equal(p1.a, p2.a) && float_equal(p1.b, p2.b) && float_equal(p1.c, p2.c);
}

inline void swap(LINE2D &p1, LINE2D &p2)
{
	std::swap(p1.a, p2.a); std::swap(p1.b, p2.b); std::swap(p1.c, p2.c);
}

/// Tworzy prost¹ na podstwie dwóch punktów
/** Wynikowa prosta nie jest znormalizowana. */
inline void PointsToLine(LINE2D *Out, const VEC2 &p1, const VEC2 &p2) {
	Out->a = p1.y - p2.y;
	Out->b = p2.x - p1.x;
	Out->c = - Out->a * p1.x - Out->b * p1.y;
}

/// Tworzy prost¹ z wektora normalnego i punktu le¿¹cego na tej prostej
inline void PointNormalToLine(LINE2D *Out, const VEC2 &Point, const VEC2 &Normal) {
	Out->a = Normal.x; Out->b = Normal.y;
	Out->c = -Dot(Normal, Point);
}

/// Zwraca true, jeœli podane proste s¹ równoleg³e
inline bool LinesParallel(const LINE2D &L1, const LINE2D &L2) {
	return FLOAT_ALMOST_ZERO(L1.a*L2.b - L2.a*L1.b);
}

/// Zwraca true, jeœli podane proste s¹ prostopad³e
inline bool LinesPerpedicular(const LINE2D &L1, const LINE2D &L2) {
	return FLOAT_ALMOST_ZERO(L1.a*L2.a + L1.b*L2.b);
}

/// Zwraca dot z wektorów normalnych dwóch prostych
/** Jeœli te proste s¹ znormalizowane, wynikiem jest cos k¹ta miêdzy tymi prostymi. */
inline float Dot(const LINE2D &L1, const LINE2D &L2) {
	return L1.a*L2.a + L1.b*L2.b;
}

/// Zwraca signed odleg³oœæ dwóch prostych na p³aszczyŸnie
/** Proste musz¹ byæ znormalizowane. */
inline float LineDistance(const LINE2D &L1, const LINE2D &L2) {
	return L1.c - L2.c;
}

/// Oblicza punkt przeciecia dwóch prostych
/** Jeœli proste nie przecinaj¹ siê, tylko s¹ równoleg³e lub siê pokrywan¹, to nie oblicza tylko zwraca false. */
bool LinesIntersection(VEC2 *Out, const LINE2D &L1, const LINE2D &L2);

/// Zwraca true, jeœli podane trzy punkty le¿¹ na jednej prostej
bool PointsColinear(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3);

/// Skalowanie prostej
inline void Mul(LINE2D *Out, const LINE2D &p, float s) {
	Out->a = p.a * s; Out->b = p.b * s; Out->c = p.c * s;
}
inline void Div(LINE2D *Out, const LINE2D &p, float s) {
	Out->a = p.a / s; Out->b = p.b / s; Out->c = p.c / s;
}

/// Iloczyn skalarny p³aszczyzny i punktu we wspó³rzêdnych jednorodnych 2D
/** Mo¿na u¿yæ do wyznaczenia czy tak zapisany punkt le¿y na p³aszczyŸnie,
po której stronie i jak daleko. */
inline float Dot(const LINE2D &p, const VEC3 &pt) {
	return p.a*pt.x + p.b*pt.y + p.c*pt.z;
}

/// Iloczyn skalarny prostej 2D z punktem 2D: a*x + b*y + c*1
/** Mo¿na u¿yæ do wyznaczenia czy punkt le¿y na prostej 2D, po której stronie i jak daleko. */
inline float DotCoord(const LINE2D &p, const VEC2 &pt) {
	return p.a*pt.x + p.b*pt.y + p.c;
}

/// Iloczyn skalarny prostej 2D z wektorem 2D: a*x + b*y + c*z + d*0
/** Przydatny do wyznaczania k¹ta miêdzy prost¹ 2D (jej normaln¹) a wektorem. */
inline float DotNormal(const LINE2D &p, const VEC2 &v) {
	return p.a*v.x + p.b*v.y;
}

/// Normalizacja prostej 2D
/** Po tej operacji wektor normalny prostej jest znormalizowany.
Prosta pozostaje po³o¿ona i zorientowana tam gdzie by³a. */
inline void Normalize(LINE2D *Out, const LINE2D &p) {
	float nlsq = 1.f / Length(p.GetNormal());
	Out->a = p.a * nlsq; Out->b = p.b * nlsq; Out->c = p.c * nlsq;
}
/// Normalizacja prostej w miejscu
inline void Normalize(LINE2D *p) {
	float nlsq = 1.f / Length(p->GetNormal());
	p->a *= nlsq; p->b *= nlsq; p->c *= nlsq;
}

/// Returns line of one of 4 possible box edges.
/** Face order is: -X, +X, -Y, +Y.
Returned line is normalized and points to the outside.
*/
void GetRectLine(LINE2D *out, const RECTF &rect, uint index);

//@}
// math_line3d


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_matrix Macierz */
//@{

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
	typedef D3DXMATRIX MATRIX;
#else
	/// Macierz 4 x 4
	/**
	Elementy s¹ indeksowane [wiersz,kolumna].
	Reprezentacja tekstowa to:
		a11,a12,a13,a14;a21,a22,a23,a24;a31,a32,a33,a34;a41,a42,a43,a44
	*/
	struct MATRIX
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4]; // [wiersz][kolumna]
		};

		MATRIX() { }
		MATRIX(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) : _11(_11), _12(_12), _13(_13), _14(_14), _21(_21), _22(_22), _23(_23), _24(_24), _31(_31), _32(_32), _33(_33), _34(_34), _41(_41), _42(_42), _43(_43), _44(_44) { }
		MATRIX(const float *Array) : _11(Array[0]), _12(Array[1]), _13(Array[2]), _14(Array[3]), _21(Array[4]), _22(Array[5]), _23(Array[6]), _24(Array[7]), _31(Array[8]), _32(Array[9]), _33(Array[10]), _34(Array[11]), _41(Array[12]), _42(Array[13]), _43(Array[14]), _44(Array[15]) { }

		operator float * () { return &_11; }
		operator const float * () const { return &_11; }

		bool operator == (const MATRIX &m) const { return _11 == m._11 && _12 == m._12 && _13 == m._13 && _14 == m._14 && _21 == m._21 && _22 == m._22 && _23 == m._23 && _24 == m._24 && _31 == m._31 && _32 == m._32 && _33 == m._33 && _34 == m._34 && _41 == m._41 && _42 == m._42 && _43 == m._43 && _44 == m._44; }
		bool operator != (const MATRIX &m) const { return _11 != m._11 || _12 != m._12 || _13 != m._13 || _14 != m._14 || _21 != m._21 || _22 != m._22 || _23 != m._23 || _24 != m._24 || _31 != m._31 || _32 != m._32 || _33 != m._33 || _34 != m._34 || _41 != m._41 || _42 != m._42 || _43 != m._43 || _44 != m._44; }

		float & operator () (unsigned Row, unsigned Col) { return m[Row][Col]; }
		float operator () (unsigned Row, unsigned Col) const { return m[Row][Col]; }

		MATRIX operator + () const { return *this; }
		MATRIX operator - () const;
		MATRIX operator + (const MATRIX &m) const;
		MATRIX operator - (const MATRIX &m) const;
		MATRIX operator * (const MATRIX &m) const
		{
			return MATRIX(
				_11 * m._11 + _12 * m._21 + _13 * m._31 + _14 * m._41,
				_11 * m._12 + _12 * m._22 + _13 * m._32 + _14 * m._42,
				_11 * m._13 + _12 * m._23 + _13 * m._33 + _14 * m._43,
				_11 * m._14 + _12 * m._24 + _13 * m._34 + _14 * m._44,

				_21 * m._11 + _22 * m._21 + _23 * m._31 + _24 * m._41,
				_21 * m._12 + _22 * m._22 + _23 * m._32 + _24 * m._42,
				_21 * m._13 + _22 * m._23 + _23 * m._33 + _24 * m._43,
				_21 * m._14 + _22 * m._24 + _23 * m._34 + _24 * m._44,

				_31 * m._11 + _32 * m._21 + _33 * m._31 + _34 * m._41,
				_31 * m._12 + _32 * m._22 + _33 * m._32 + _34 * m._42,
				_31 * m._13 + _32 * m._23 + _33 * m._33 + _34 * m._43,
				_31 * m._14 + _32 * m._24 + _33 * m._34 + _34 * m._44,

				_41 * m._11 + _42 * m._21 + _43 * m._31 + _44 * m._41,
				_41 * m._12 + _42 * m._22 + _43 * m._32 + _44 * m._42,
				_41 * m._13 + _42 * m._23 + _43 * m._33 + _44 * m._43,
				_41 * m._14 + _42 * m._24 + _43 * m._34 + _44 * m._44);
		}
		MATRIX & operator += (const MATRIX &m);
		MATRIX & operator -= (const MATRIX &m);
		MATRIX & operator *= (const MATRIX &m);

		MATRIX operator * (float s) const;
		MATRIX operator / (float s) const;
		MATRIX & operator *= (float s);
		MATRIX & operator /= (float s);
	};
#endif

extern const MATRIX MATRIX_ZERO;
extern const MATRIX MATRIX_IDENTITY;

void MatrixToStr(tstring *Out, const MATRIX &m);
bool StrToMatrix(MATRIX *Out, const tstring &Str);

inline void swap(MATRIX &m1, MATRIX &m2)
{
	std::swap(m1._11, m2._11); std::swap(m1._12, m2._12); std::swap(m1._13, m2._13); std::swap(m1._14, m2._14);
	std::swap(m1._21, m2._21); std::swap(m1._22, m2._22); std::swap(m1._23, m2._23); std::swap(m1._24, m2._24);
	std::swap(m1._31, m2._31); std::swap(m1._32, m2._32); std::swap(m1._33, m2._33); std::swap(m1._34, m2._34);
	std::swap(m1._41, m2._41); std::swap(m1._42, m2._42); std::swap(m1._43, m2._43); std::swap(m1._44, m2._44);
}

/// Zwraca true, jeœli podane macierze s¹ sobie mniej wiêcej równe
bool MatrixEqual(const MATRIX &m1, const MATRIX &m2);
/// Negacja macierzy w miejscu
void Minus(MATRIX *m);
/// Dodawanie macierzy
void Add(MATRIX *Out, const MATRIX &m1, const MATRIX &m2);
/// Odejmowanie macierzy
void Sub(MATRIX *Out, const MATRIX &m1, const MATRIX &m2);
/// Mno¿enie macierzy
void Mul(MATRIX *Out, const MATRIX &m1, const MATRIX &m2);
/// Mno¿enie macierzy przez skalar w miejscu
void Mul(MATRIX *m, float s);
/// Dzielenie macierzy przez skalar w miejscu
void Div(MATRIX *m, float s);

/// Multiplication of 3x3 submatrices
/** m1 and m2 are assumed to be in form of linear transforms like this
(4th rows and columns are ignored):
\verbatim
_11 _12 _13  0
_21 _22 _23  0
_31 _32 _33  0
 0   0   0   1
\endverbatim
Output matrix has only 3x3 submatrix modified.
4th row and column stay untouched. \n
Use this method for better efficiency whenever you deal with transforms that have no translation or perspective projection.
*/
void Mul3x3(MATRIX &out, const MATRIX &m1, const MATRIX &m2);
/// Multiplication of 4x3 submatrices
/** m1 and m2 are assumed to be in form of affine transforms like this
(4th columns are ignored):
\verbatim
_11 _12 _13  0
_21 _22 _23  0
_31 _32 _33  0
_41 _42 _43  1
\endverbatim
Output matrix has only 4x3 submatrix modified.
4th column stays untouched.
Use this method for better efficiency whenever you deal with transforms that have no perspective projection.
*/
void Mul4x3(MATRIX &out, const MATRIX &m1, const MATRIX &m2);

/// Mno¿enie wektora 2D jako [x,y,0,1] przez macierz, wychodzi wektor 4D
void Transform(VEC4 *Out, const VEC2 &v, const MATRIX &m);
/// Mno¿enie wektora 2D jako [x,y,0,1] przez macierz z olaniem wyjœciowego W
void Transform(VEC2 *Out, const VEC2 &v, const MATRIX &m);
/// Mno¿enie wektora 2D jako [x,y,0,1] przez macierz i sprowadzenie z powrotem do 2D przez podzielenie przez W
void TransformCoord(VEC2 *Out, const VEC2 &v, const MATRIX &m);
/// Mno¿enie wektora 2D jako [x,y,0,0] przez macierz
void TransformNormal(VEC2 *Out, const VEC2 &v, const MATRIX &m);

/// Mno¿enie wektora 3D jako [X,Y,Z,1] przez macierz, wychodzi wektor 4D
void Transform(VEC4 *Out, const VEC3 &v, const MATRIX &m);
/// Mno¿enie wektora 3D jako [X,Y,Z,1] przez macierz z olaniem wyjœciowego W
/** Dobre do przekszta³cania wektorów 3D jeœli macierz reprezentuje przekszta³cenie liniowe. */
void Transform(VEC3 *Out, const VEC3 &v, const MATRIX &m);
/// Mno¿enie wektora 3D jako [X,Y,Z,1] przez macierz i sprowadzenie z powrotem do 3D przez podzielenie przez W
/** Dobre do niezawodnego przekszta³cania wektorów 3D przez dowoln¹ macierz np. rzutowania. */
void TransformCoord(VEC3 *Out, const VEC3 &v, const MATRIX &m);
/// Mno¿enie wektora 3D jako [X,Y,Z,0] przez macierz
/** Dobre do przekszta³cania wektorów (np. normalnych), nie punktów, bo nie robi translacji. */
void TransformNormal(VEC3 *Out, const VEC3 &v, const MATRIX &m);

/// Przekszta³ca na raz ca³¹ tablicê wektorów - w miejscu lub z tablicy wejœciowej do wyjœciowej
void TransformArray(VEC3 OutPoints[], const VEC3 InPoints[], size_t PointCount, const MATRIX &M);
void TransformArray(VEC3 InOutPoints[], size_t PointCount, const MATRIX &M);
void TransformNormalArray(VEC3 OutPoints[], const VEC3 InPoints[], size_t PointCount, const MATRIX &M);
void TransformNormalArray(VEC3 InOutPoints[], size_t PointCount, const MATRIX &M);
void TransformCoordArray(VEC3 OutPoints[], const VEC3 InPoints[], size_t PointCount, const MATRIX &M);
void TransformCoordArray(VEC3 InOutPoints[], size_t PointCount, const MATRIX &M);

/// Mno¿enie wektora 4D przez macierz
/** Czyli przekszta³cenie wektora we wspó³rzêdnych jednorodnych przez t¹ macierz. */
void Transform(VEC4 *Out, const VEC4 &v, const MATRIX &m);

/// Mno¿y wektor przez transpozycjê podanej macierzy m (jej podmacierzy 3x3)
void TransformNormalByTranspose(VEC3 *Out, const VEC3 &v, const MATRIX &m);
/// Mno¿y punkt przez odwrotnosæ podanej macierzy m (jej podmacierzy 4x3), ale bez odwracania.
/** Czyli jeœli macierz zawiera rotacjê i translacjê, to wykonuje na punkcie
przekszta³cenie odwrotne do tego które jest w tej macierzy. \n
Pomys³odawca: Krzysiek K. */
void Untransform(VEC3 *Out, const VEC3 &v, const MATRIX &m);
/// Mno¿y wektor przez odwrotnosæ podanej macierzy m (jej podmacierzy 3x3), ale bez odwracania.
/** Czyli jeœli macierz zawiera rotacjê i translacjê, to wykonuje na punkcie
przekszta³cenie odwrotne do rotacji, która jest w tej macierzy. */
void UntransformNormal(VEC3 *Out, const VEC3 &v, const MATRIX &m);

/// Przekszta³ca p³aszczyznê przez macierz
/** \param p P³aszczyzna ma byæ znormalizowana.
\param m Macierz ma byæ odwrotna i transponowana wzglêdem normalnej macierzy. */
void Transform(PLANE *Out, const PLANE &p, const MATRIX &m);
/// Przekszta³ca promieñ przez macierz
void TransformRay(VEC3 *OutOrigin, VEC3 *OutDir, const VEC3 &RayOrigin, const VEC3 &RayDir, const MATRIX &m);

/// Przekszta³ca AABB przez podan¹ macierz i wylicza AABB tego co wysz³o po przekszta³ceniu
void TransformBox(BOX *Out, const BOX &In, const MATRIX &M);
/// Przekszta³ca AABB przez podan¹ macierz i wylicza AABB tego co wysz³o po przekszta³ceniu
/** Wersja dziel¹ca przez W - mo¿na stosowaæ te¿ do macierzy projekcji. */
void TransformBoxCoord(BOX *Out, const BOX &In, const MATRIX &M);

/// Ustawia translacjê w macierzy
void MatrixSetTranslation(MATRIX *InOut, float TranslationX, float TranslationY, float TranslationZ);
/// Ustawia translacjê w macierzy
void MatrixSetTranslation(MATRIX *InOut, const VEC3 &Translation);
/// Pobiera translacjê z macierzy
void MatrixGetTranslation(float *OutTranslationX, float *OutTranslationY, float *OutTranslationZ, const MATRIX &m);
/// Pobiera translacjê z macierzy
void MatrixGetTranslation(VEC3 *OutTranslation, const MATRIX &m);

/// Tworzy macierz przekszta³caj¹c¹ do lokalnego uk³adu wspó³rzêdnych wyznaczonego przez podane 3 osie
/** Nast¹pi obrót, a jeœli nie s¹ znormalizowane to i skalowanie. */
void AxesToMatrix(MATRIX *Out, const VEC3 &AxisX, const VEC3 &AxisY, const VEC3 &AxisZ);
/// Tworzy macierz przekszta³caj¹c¹ do lokalnego uk³adu wspó³rzêdnych wyznaczonego przez podane 3 osie i pozycjê pocz¹tku tego uk³adu.
/** Nast¹pi obrót, a jeœli osie s¹ znormalizowane to i skalowanie. */
void AxesToMatrixTranslation(MATRIX *Out, const VEC3 &Origin, const VEC3 &AxisX, const VEC3 &AxisY, const VEC3 &AxisZ);

/// Macierz kamery
/** Eye to punkt, Forward i Up to wektory. */
void LookAtLH(MATRIX *Out, const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up);
void LookAtRH(MATRIX *Out, const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up);

/// Tworzy macierz identycznoœciow¹
void Identity(MATRIX *Out);
/// Tworzy macierz translacji (przesuwania)
void Translation(MATRIX *Out, float x, float y, float z);
void Translation(MATRIX *Out, const VEC3 &v);
/// Tworzy macierz skalowania (powiêkszania, pomniejszania, rozci¹gania)
void Scaling(MATRIX *Out, float sx, float sy, float sz);
void Scaling(MATRIX *Out, const VEC3 &sv);
void Scaling(MATRIX *Out, float s);
/// Tworzy macierz skalowania wzd³u¿ podanego kierunku
/** \param k   wspó³czynnik skalowania
\param n   wektor kierunku, musi byæ znormalizowany */
void ScalingAxis(MATRIX *Out, const VEC3 &n, float k);

/// Macierz obrotu 2D
void Rotation2D(MATRIX *Out, float Angle);
/// Macierz obrotu wokó³ osi X
void RotationX(MATRIX *Out, float Angle);
/// Macierz obrotu wokó³ osi Y
void RotationY(MATRIX *Out, float Angle);
/// Macierz obrotu wokó³ osi Z
void RotationZ(MATRIX *Out, float Angle);
/** Axis musi byæ znormalizowany. */
void RotationAxisLH(MATRIX *Out, const VEC3 &Axis, float Angle);
inline void RotationAxisRH(MATRIX *Out, const VEC3 &Axis, float Angle) { RotationAxisLH(Out, Axis, -Angle); }

/// Macierz obrotu wokó³ trzech osi w kolejnoœci Yaw-Pitch-Roll (ZXY) - tzw. k¹ty Eulera
/** W przypadku wektora sk³adowe odpowiadaj¹ obrotom wokó³ poszczególnych osi, czyli:
  X = Pitch, Y = Yaw, Z = Roll
Przekszta³ca ze wsp. obiektu do œwiata (object -> interial space).
Czyli tak naprawdê robi ZXY.
\param Yaw jest wokó³ osi Y.
\param Pitch jest wokó³ osi X.
\param Roll jest wokó³ osi Z. */
void RotationYawPitchRoll(MATRIX *Out, float Yaw, float Pitch, float Roll);
void RotationYawPitchRoll(MATRIX *Out, const VEC3 &EulerAngles);
/// Przekszta³ca ze wsp. œwiata do obiektu (interial -> object space).
/** Czyli tworzy macierz dok³adnie odwrotn¹ do tej z RotationYawPitchRoll. */
void RotationYawPitchRollInv(MATRIX *Out, float Yaw, float Pitch, float Roll);
void RotationYawPitchRollInv(MATRIX *Out, const VEC3 &EulerAngles);

/*/// Przetwarza macierz obrotu (to musi byæ czysta macierz obrotu) na k¹ty Eulera
void RotationMatrixToEulerAngles(const MATRIX &m, float *Yaw, float *Pitch, float *Roll);*/

/// Tworzy macierz rzutowania ortogonalnego
/** Dane s¹ w uk³adzie kamery. */
void OrthoLH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar);
void OrthoRH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar);
void OrthoOffCenterLH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar);
void OrthoOffCenterRH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar);
/// Tworzy macierz rzutowania perspektywicznego
/** Width i Height s¹ wymiarami na p³aszczyŸnie bliskiej (near).
Left, Top, Right, Bottom prawdopodobnie te¿.
Tworzone macierze s¹ "W-Friendly" - wektor przemno¿ony przez tak¹ macierz ma w sk³adowej W komponent Z.
*/
void PerspectiveLH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar);
void PerspectiveRH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar);
void PerspectiveFovLH(MATRIX *Out, float FovY, float Aspect, float ZNear, float ZFar);
void PerspectiveFovRH(MATRIX *Out, float FovY, float Aspect, float ZNear, float ZFar);
void PerspectiveOffCenterLH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar);
void PerspectiveOffCenterRH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar);
/// Wersja tworz¹ca macierz z Z-Far = INFINITY
void PerspectiveFovLH_Inf(MATRIX *Out, float FovY, float Aspect, float ZNear);
/// Tworzy macierz sp³aszczaj¹c¹ geometriê na podan¹ p³aszczyznê w podanym kierunku padania œwiat³a
/** P³aszczyzna musi byæ znormalizowana. */
void MatrixShadow(MATRIX *Out, const VEC4 &Light, const PLANE &Plane);
/// Tworzy macierz odbijaj¹c¹ geometriê wzglêdem podanej p³aszczyzny
/** P³aszczyzna musi byæ znormalizowana. */
void MatrixReflect(MATRIX *Out, const PLANE &p);

/// Transpozycja macierzy na miejscu
void Transpose(MATRIX *m);
/// Transpozycja macierzy
void Transpose(MATRIX *Out, const MATRIX &m);
/// Interpolacja liniowa macierzy
void Lerp(MATRIX *Out, const MATRIX &m1, const MATRIX &m2, float t);
/// Wyznacznik macierzy
float Det(const MATRIX &m);
/// Odwrotnoœæ macierzy
bool Inverse(MATRIX *Out, const MATRIX &m);


/// Macierz 3 x 3
/**
Elementy s¹ indeksowane [wiersz,kolumna].
Reprezentacja tekstowa to:
	a11,a12,a13,a21,a22,a23,a31,a32,a33
*/
struct MATRIX33
{
	union
	{
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
		float m[3][3]; // [wiersz][kolumna]
	};

	MATRIX33() { }
	MATRIX33(float _11, float _12, float _13, float _21, float _22, float _23, float _31, float _32, float _33) : _11(_11), _12(_12), _13(_13), _21(_21), _22(_22), _23(_23), _31(_31), _32(_32), _33(_33) { }
	MATRIX33(const float *Array) : _11(Array[0]), _12(Array[1]), _13(Array[2]), _21(Array[4]), _22(Array[5]), _23(Array[6]), _31(Array[8]), _32(Array[9]), _33(Array[10]) { }

	operator float * () { return &_11; }
	operator const float * () const { return &_11; }

	bool operator == (const MATRIX33 &m) const { return _11 == m._11 && _12 == m._12 && _13 == m._13 && _21 == m._21 && _22 == m._22 && _23 == m._23 && _31 == m._31 && _32 == m._32 && _33 == m._33; }
	bool operator != (const MATRIX33 &m) const { return _11 != m._11 || _12 != m._12 || _13 != m._13 || _21 != m._21 || _22 != m._22 || _23 != m._23 || _31 != m._31 || _32 != m._32 || _33 != m._33; }

	float & operator () (unsigned Row, unsigned Col) { return m[Row][Col]; }
	float operator () (unsigned Row, unsigned Col) const { return m[Row][Col]; }

	MATRIX33 operator + () const { return *this; }
	MATRIX33 operator - () const;
	MATRIX33 operator + (const MATRIX33 &m) const;
	MATRIX33 operator - (const MATRIX33 &m) const;
	MATRIX33 operator * (const MATRIX33 &m) const
	{
		return MATRIX33(
			_11 * m._11 + _12 * m._21 + _13 * m._31,
			_11 * m._12 + _12 * m._22 + _13 * m._32,
			_11 * m._13 + _12 * m._23 + _13 * m._33,

			_21 * m._11 + _22 * m._21 + _23 * m._31,
			_21 * m._12 + _22 * m._22 + _23 * m._32,
			_21 * m._13 + _22 * m._23 + _23 * m._33,

			_31 * m._11 + _32 * m._21 + _33 * m._31,
			_31 * m._12 + _32 * m._22 + _33 * m._32,
			_31 * m._13 + _32 * m._23 + _33 * m._33);
	}
	MATRIX33 & operator += (const MATRIX33 &m);
	MATRIX33 & operator -= (const MATRIX33 &m);
	MATRIX33 & operator *= (const MATRIX33 &m);

	MATRIX33 operator * (float s) const;
	MATRIX33 operator / (float s) const;
	MATRIX33 & operator *= (float s);
	MATRIX33 & operator /= (float s);
};

extern const MATRIX33 MATRIX33_ZERO;
extern const MATRIX33 MATRIX33_IDENTITY;

/// Konwertuje macierz 3x3 na macierz 4x4
void Matrix33ToMatrix(MATRIX *Out, const MATRIX33 &m);
/// Wyci¹ga podmacierz 3x3 z macierzy 4x4
void MatrixToMatrix33(MATRIX33 *Out, const MATRIX &m);

/// Tworzy macierz identycznoœciow¹
void Identity(MATRIX33 *Out);

/// Transpozycja macierzy na miejscu
void Transpose(MATRIX33 *m);
/// Transpozycja macierzy
void Transpose(MATRIX33 *Out, const MATRIX33 &m);

//@}
// math_matrix


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_quaternion Kwaternion */
//@{

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
	typedef D3DXQUATERNION QUATERNION;
#else
	/// Kwaternion
	/**
	Reprezentacja jako ³añcuch to: "x,y,z,w", np.: "1,2,3.5,-10".
	Kwaterniony s¹ mno¿one od drugiej strony ni¿ matematycznie, dziêki czemu mo¿na
	³¹czyæ obroty które reprezentuj¹ w porz¹dku naturalnym, tak jak macierze.
	*/
	struct QUATERNION
	{
		float x, y, z, w;

		QUATERNION() { }
		QUATERNION(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
		QUATERNION(const float *Array) : x(Array[0]), y(Array[1]), z(Array[2]), w(Array[3]) { }

		operator float * () { return &x; }
		operator const float * () const { return &x; }

		bool operator == (const QUATERNION &v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
		bool operator != (const QUATERNION &v) const { return x != v.x || y != v.y || z != v.z || w != v.w; }

		QUATERNION operator + () const { return *this; }
		QUATERNION operator - () const { return QUATERNION(-x, -y, -z, -w); }
		QUATERNION operator + (const QUATERNION &q) const { return QUATERNION(x+q.x, y+q.y, z+q.z, w+q.w); }
		QUATERNION operator - (const QUATERNION &q) const { return QUATERNION(x-q.x, y-q.y, z-q.z, w-q.w); }
		QUATERNION operator * (const QUATERNION &q) const { return QUATERNION(w*q.x + x*q.w + y*q.z - z*q.y, w*q.y + y*q.w + z*q.x - x*q.z, w*q.z + z*q.w + x*q.y - y*q.x, w*q.w - x*q.x - y*q.y - z*q.z); }
		QUATERNION & operator += (const QUATERNION &q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
		QUATERNION & operator -= (const QUATERNION &q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
		QUATERNION & operator *= (const QUATERNION &q) { QUATERNION R; R.x = w*q.x + x*q.w + y*q.z - z*q.y; R.y = w*q.y + y*q.w + z*q.x - x*q.z; R.z = w*q.z + z*q.w + x*q.y - y*q.x; R.w = w*q.w - x*q.x - y*q.y - z*q.z; *this = R; return *this; }
		QUATERNION & operator *= (float v) { x *= v; y *= v; z *= v; w *= v; return *this; }
		QUATERNION & operator /= (float v) { float vInv = 1.0f/v; x *= vInv; y *= vInv; z *= vInv; w *= vInv; return *this; }
	};
	inline QUATERNION operator * (const QUATERNION &pt, float v) { return QUATERNION(pt.x*v, pt.y*v, pt.z*v, pt.w*v); }
	inline QUATERNION operator / (const QUATERNION &pt, float v) { float vInv = 1.0f/v; return QUATERNION(pt.x*vInv, pt.y*vInv, pt.z*vInv, pt.w*vInv); }
	inline QUATERNION operator * (float v, const QUATERNION &pt) { return QUATERNION(pt.x*v, pt.y*v, pt.z*v, pt.w*v); }
	inline QUATERNION operator / (float v, const QUATERNION &pt) { float vInv = 1.0f/v; return QUATERNION(pt.x*vInv, pt.y*vInv, pt.z*vInv, pt.w*vInv); }
#endif

extern const QUATERNION QUATERNION_ZERO;
extern const QUATERNION QUATERNION_IDENTITY;

inline void swap(QUATERNION &v1, QUATERNION &v2)
{
	std::swap(v1.x, v2.x);
	std::swap(v1.y, v2.y);
	std::swap(v1.z, v2.z);
	std::swap(v1.w, v2.w);
}

/// Zwraca k¹t obrotu jaki reprezentuje ten kwaternion
inline float CalcQuaternionAngle(const QUATERNION &Q) { return safe_acos(Q.w) * 2.0f; }
/// Zwraca oœ obrotu jak¹ reprezentuje ten kwaternion
void CalcQuaternionAxis(VEC3 *Out, const QUATERNION &Q);

/// Zwraca true, jeœli podane kwaterniony s¹ sobie mniej wiêcej równe
inline bool QuaternionEqual(const QUATERNION &q1, const QUATERNION &q2) {
	return float_equal(q1.x, q2.x) && float_equal(q1.y, q2.y) && float_equal(q1.z, q2.z) && float_equal(q1.w, q2.w);
}

/// Ujemny kwaternion
void Minus(QUATERNION *q);
inline void Minus(QUATERNION *Out, const QUATERNION &q) { *Out = q; Minus(Out); }
/// Dodawanie, odejmowanie, mno¿enie kwaternionów
void Add(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2);
void Sub(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2);
void Mul(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2);
/// Mno¿enie i dzielenie kwaterniona przez skalar
void Mul(QUATERNION *q, float v);
void Div(QUATERNION *q, float v);
inline void Mul(QUATERNION *Out, const QUATERNION &q, float v) { *Out = q; Mul(Out, v); }
inline void Div(QUATERNION *Out, const QUATERNION &q, float v) { *Out = q; Div(Out, v); }

/// Tworzy kwaternion na podstawie macierzy obrotu
void RotationMatrixToQuaternion(QUATERNION *Out, const MATRIX &RotationMatrix);
/// Przekszta³ca kwaternion w macierz obrotu
void QuaternionToRotationMatrix(MATRIX *Out, const QUATERNION &q);
/// Przekszta³ca punkt/wektor przez kwaternion obracaj¹c go o ten kwaternion
/**
- To nie jest wcale super szybkie i nie przydaje siê a¿ tak czêsto.
  Lepiej sobie przekszta³ciæ kwaternion na macierz.
- Out = q * p * q^-1
*/
void QuaternionTransform(VEC3 *Out, const VEC3 &p, const QUATERNION &q);
/// Konwersja k¹tów Eulera na kwaternion (który obraca ze wsp. obiektu do œwiata, czyli object -> interial space) [a mo¿e odwrotnie?]
/** Ta funkcja dzia³a identycznie z D3DXQuaternionRotationYawPitchRoll. */
void EulerAnglesToQuaternionO2I(QUATERNION *Out, float Yaw, float Pitch, float Roll);
/// Konwersja k¹tów Eulera na kwaternion (który obraca ze wsp. œwiata do obiektu, czyli interial -> object space) [a mo¿e odwrotnie?]
void EulerAnglesToQuaternionI2O(QUATERNION *Out, float Yaw, float Pitch, float Roll);
/// Konwersja kwaterniona który obraca ze wsp. obiektu do œwiata, czyli object -> interial space, na k¹ty Eulera
void QuaternionO2IToEulerAngles(float *Yaw, float *Pitch, float *Roll, const QUATERNION &q);
/// Konwersja kwaterniona który obraca ze wsp. œwiata do obiektu, czyli interial -> object space, na k¹ty Eulera
void QuaternionI2OToEulerAngles(float *Yaw, float *Pitch, float *Roll, const QUATERNION &q);

/// Tworzy kwaternion obracaj¹cy wokó³ osi X
void QuaternionRotationX(QUATERNION *Out, float a);
/// Tworzy kwaternion obracaj¹cy wokó³ osi Y
void QuaternionRotationY(QUATERNION *Out, float a);
/// Tworzy kwaternion obracaj¹cy wokó³ osi Z
void QuaternionRotationZ(QUATERNION *Out, float a);
/// Tworzy kwaternion na podstawie osi obrotu i k¹ta
void AxisToQuaternion(QUATERNION *Out, const VEC3 &Axis, float Angle);

/// Oblica "ró¿nicê" dwóch kwaternionów reprezentuj¹c¹ taki obrót ¿e a*Out = b
/** To jest obliczane tak: Out = a^-1 * b, wiêc jeœli masz ju¿ a^-1 to nie u¿ywaj. */
void QuaternionDiff(QUATERNION *Out, const QUATERNION &a, const QUATERNION &b);
/// Kwaternion sprzê¿ony do danego
void Conjugate(QUATERNION *q);
inline void Conjugate(QUATERNION *Out, const QUATERNION &q) { *Out = q; Conjugate(Out); }
/// Odwrotnoœæ kwaterniona
void Inverse(QUATERNION *Out, const QUATERNION &q);
/// Normalizacja kwaternion
void Normalize(QUATERNION *Out, const QUATERNION &q);
void Normalize(QUATERNION *InOut);
/// Iloczyn skalarny dwóch kwaternionów
float Dot(const QUATERNION &q1, const QUATERNION &q2);
/// Kwadrat d³ugoœci kwaterniona
float LengthSq(const QUATERNION &q);
/// D³ugoœæ kwaterniona
float Length(const QUATERNION &q);
void Log(QUATERNION *Out, const QUATERNION &q);
void Exp(QUATERNION *Out, const QUATERNION &q);
/// Podnieœ kwaternion do potêgi.
/** Wyk³adnik 0..1 daje interpolacje kwaterniona od identycznoœciowego do q. */
void Pow(QUATERNION *InOut, float t);
inline void Pow(QUATERNION *Out, const QUATERNION &q, float t) { *Out = q; Pow(Out, t); }
/// Interpolacja normalna kwaternionów
void Lerp(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2, float t);
/// Interpolacja sferyczna kwaternionów
/** Sprawdzona, daje wyniki dok³adnie tekie same jak D3DXQuaternionSlerp. */
void Slerp(QUATERNION *Out, const QUATERNION &q0, const QUATERNION &q1, float t);
/** NIE TESTOWANA */
void Squad(QUATERNION *Out, const QUATERNION &Q1, const QUATERNION &A, const QUATERNION &B, const QUATERNION &C, float t);
/** NIE TESTOWANA */
void SquadSetup(QUATERNION *OutA, QUATERNION *OutB, QUATERNION *OutC, const QUATERNION &Q0, const QUATERNION &Q1, const QUATERNION &Q2, const QUATERNION &Q3);

//@}
// math_quaternion


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_affine2d Affine 2D transform */
//@{

/// Affine transform of 2D point/vector
/**
\verbatim
(xOut, yOut) = | a b |  | x | + | c |
               | d e |  | y |   | f |
\endverbatim
It means that:
\verbatim
(xOut, yOut) = (a*x + b*y + c, d*x + e*y + f)
\endverbatim
Multiplication order is OpenGL-like, opposite to what MATRIX represents. \n
Elements are indexed as [row, column]. \n
Text representation is: a,b,c;d,e,f
*/
struct AFFINE2D
{
	union {
		struct {
			float a, b, c, d, e, f;
		};
		float m[2][3];
	};

	AFFINE2D() { }
	AFFINE2D(float a, float b, float c, float d, float e, float f) : a(a), b(b), c(c), d(d), e(e), f(f) { }
	AFFINE2D(const float *arr) : a(arr[0]), b(arr[1]), c(arr[2]), d(arr[3]), e(arr[4]), f(arr[5]) { }

	operator float * () { return &a; }
	operator const float * () const { return &a; }

	float & operator [] (size_t index) { return (&a)[index]; }
	float operator [] (size_t index) const { return (&a)[index]; }

	bool operator == (const AFFINE2D &rhs) const { return a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d && e == rhs.e && f == rhs.f; }
	bool operator != (const AFFINE2D &rhs) const { return a != rhs.a || b != rhs.b || c != rhs.c || d != rhs.d || e != rhs.e || f != rhs.f; }

	float & operator () (uint row, uint col) { return m[row][col]; }
	float operator () (uint row, uint col) const { return m[row][col]; }

	AFFINE2D operator + () const { return *this; }
	AFFINE2D operator - () const { return AFFINE2D(-a, -b, -c, -d, -e, -f); }

	AFFINE2D operator + (const AFFINE2D &rhs) const { return AFFINE2D(a + rhs.a, b + rhs.b, c + rhs.c, d + rhs.d, e + rhs.e, f + rhs.f); }
	AFFINE2D operator - (const AFFINE2D &rhs) const { return AFFINE2D(a - rhs.a, b - rhs.b, c - rhs.c, d - rhs.d, e - rhs.e, f - rhs.f); }
	AFFINE2D operator * (const AFFINE2D &rhs) const
	{
		return AFFINE2D(
			a*rhs.a + b*rhs.d, a*rhs.b + b*rhs.e, a*rhs.c + b*rhs.f + c,
			d*rhs.a + e*rhs.d, d*rhs.b + e*rhs.e, d*rhs.c + e*rhs.f + f);
	}

	AFFINE2D & operator += (const AFFINE2D &rhs) { a += rhs.a; b += rhs.b; c += rhs.c; d += rhs.d; e += rhs.e; f += rhs.f; return *this; }
	AFFINE2D & operator -= (const AFFINE2D &rhs) { a -= rhs.a; b -= rhs.b; c -= rhs.c; d -= rhs.d; e -= rhs.e; f -= rhs.f; return *this; }
	AFFINE2D & operator *= (const AFFINE2D &rhs) { return *this = *this * rhs; }

	AFFINE2D operator * (float s) const { return AFFINE2D(a*s, b*s, c*s, d*s, e*s, f*s); }
	AFFINE2D operator / (float s) const { s = 1.f/s; return AFFINE2D(a*s, b*s, c*s, d*s, e*s, f*s); }
	AFFINE2D & operator *= (float s) { a*=s; b*=s; c*=s; d*=s; e*=s; f*=s; return *this; }
	AFFINE2D & operator /= (float s) { s = 1.f/s; a*=s; b*=s; c*=s; d*=s; e*=s; f*=s; return *this; }
};

extern const AFFINE2D AFFINE2D_ZERO;
extern const AFFINE2D AFFINE2D_IDENTITY;

void Affine2dToStr(tstring *Out, const AFFINE2D &aff);
bool StrToAffine2d(AFFINE2D *Out, const tstring &Str);

inline void swap(AFFINE2D &aff1, AFFINE2D &aff2)
{
	std::swap(aff1.a, aff2.a); std::swap(aff1.b, aff2.b); std::swap(aff1.c, aff2.c);
	std::swap(aff1.d, aff2.d); std::swap(aff1.e, aff2.e); std::swap(aff1.f, aff2.f);
}

/// Per component multiplication.
inline void Mul(AFFINE2D &out, const AFFINE2D &lhs, const AFFINE2D &rhs)
{
	out.a = lhs.a*rhs.a; out.b = lhs.b*rhs.b; out.c = lhs.c*rhs.c;
	out.d = lhs.d*rhs.d; out.e = lhs.e*rhs.e; out.f = lhs.f*rhs.f;
}
/// Per component division.
inline void Div(AFFINE2D &out, const AFFINE2D &lhs, const AFFINE2D &rhs)
{
	out.a = lhs.a/rhs.a; out.b = lhs.b/rhs.b; out.c = lhs.c/rhs.c;
	out.d = lhs.d/rhs.d; out.e = lhs.e/rhs.e; out.f = lhs.f/rhs.f;
}

/// 2D point transformation, including translation.
inline void Transform(VEC2 &out, const VEC2 &v, const AFFINE2D &aff)
{
	out.x = aff.a*v.x + aff.b*v.y + aff.c;
	out.y = aff.d*v.x + aff.e*v.y + aff.f;
}
/// 2D vector transformation by 2x2 submatrix, with no translation.
inline void TransformNormal(VEC2 &out, const VEC2 &v, const AFFINE2D &aff)
{
	out.x = aff.a*v.x + aff.b*v.y;
	out.y = aff.d*v.x + aff.e*v.y;
}

inline void Translation(AFFINE2D &out, const VEC2 &v)
{
	out = AFFINE2D(
		1.f, 0.f, v.x,
		0.f, 1.f, v.y);
}
inline void Translation(AFFINE2D &out, float x, float y)
{
	out = AFFINE2D(
		1.f, 0.f, x,
		0.f, 1.f, y);
}

inline void Scaling(AFFINE2D &out, float s)
{
	out = AFFINE2D(
		s,   0.f, 0.f,
		0.f, s,   0.f);
}
inline void Scaling(AFFINE2D &out, const VEC2 &v)
{
	out = AFFINE2D(
		v.x, 0.f, 0.f,
		0.f, v.y, 0.f);
}
inline void Scaling(AFFINE2D &out, float x, float y)
{
	out = AFFINE2D(
		x,   0.f, 0.f,
		0.f, y,   0.f);
}

inline void Rotation(AFFINE2D &out, float angle)
{
	float angleSin, angleCos;
	sincos(angle, &angleSin, &angleCos);
	out = AFFINE2D(
		angleCos, -angleSin, 0.f,
		angleSin,  angleCos, 0.f);
}

/// Transposes the 2x2 submatrix.
inline void Transpose(AFFINE2D &inout)
{
	std::swap(inout.b, inout.d);
}

void Lerp(AFFINE2D &out, const AFFINE2D &lhs, const AFFINE2D &rhs, float t);
void Inverse(AFFINE2D &out, const AFFINE2D &aff);

/** Transforms between AFFINE2D and MATRIX, where MATRIX looks like this:
\verbatim
| a d 0 0 |
| b e 0 0 |
| 0 0 1 0 |
| c f 0 1 |
\endverbatim
*/
void Affine2dToMatrix(MATRIX &out, const AFFINE2D &aff);
/// \copydoc common::Affine2dToMatrix
void MatrixToAffine2d(AFFINE2D &out, const MATRIX &m);

//@}
// math_affine2d


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_frustum Frustum */
//@{

struct FRUSTUM_PLANES;
struct FRUSTUM_POINTS;

/// Frustum opisany 6 p³aszczyznami z wektorami normalnymi zwróconymi do wewn¹trz.
/**
Podczas tworzenia:
- Jeœli WorldViewProj jest macierz¹ Proj, otrzymane p³aszczyzny s¹ we wsp. widoku.
- Jeœli WorldViewProj jest macierz¹ View*Proj, otrzymane p³aszczyzny s¹ we wsp. œwiata.
- Jeœli WorldViewProj jest macierz¹ World*View*Proj, otrzymane p³aszczyzny s¹ we wsp. lokalnych obiektu.
*/
struct FRUSTUM_PLANES
{
	/** \name Indeksy do tablicy p³aszczyzn
	Przedrostek PLANE_ jest, bo NEAR zosta³o gdzieœ zdefiniowane jako makro - grrrrr!
	*/
	//@{
	static const size_t PLANE_LEFT   = 0;
	static const size_t PLANE_RIGHT  = 1;
	static const size_t PLANE_TOP    = 2;
	static const size_t PLANE_BOTTOM = 3;
	static const size_t PLANE_NEAR   = 4;
	static const size_t PLANE_FAR    = 5;
	//@}

	PLANE Planes[6];

	/// Tworzy niezainicjalizowany
	FRUSTUM_PLANES() { }
	/// Inicjalizuje na podstawie macierzy
	FRUSTUM_PLANES(const MATRIX &WorldViewProj) { Set(WorldViewProj); }
	/// Inicjalizuje na podstawie reprezentacji punktowej
	FRUSTUM_PLANES(const FRUSTUM_POINTS &FrustumPoints) { Set(FrustumPoints); }
	/// Wype³nia
	void Set(const MATRIX &WorldViewProj);
	void Set(const FRUSTUM_POINTS &FrustumPoints);
	/// Normalizuje p³aszczyzny
	void Normalize();

	PLANE & operator [] (size_t Index) { return Planes[Index]; }
	const PLANE & operator [] (size_t Index) const { return Planes[Index]; }
};

/// Frustum zapisany jako 8 wierzcho³ków
/**
Uzyskiwanie przez konwersjê macierzy ViewProj do FRUSTUM_PLANES i z niego
do FRUSTUM_POINTS jest du¿o szybsze ni¿ budowanie FRUSTUM_POINTS z macierzy
ViewProjInv - 2 (Release) - 8 (Debug) razy szybsze!
*/
struct FRUSTUM_POINTS
{
	/** \name indeksy do tablicy punktów
	Kolejno na przeciêciu p³aszczyzn. */
	//@{
	static const size_t NEAR_LEFT_BOTTOM  = 0;
	static const size_t NEAR_RIGHT_BOTTOM = 1;
	static const size_t NEAR_LEFT_TOP     = 2;
	static const size_t NEAR_RIGHT_TOP    = 3;
	static const size_t FAR_LEFT_BOTTOM   = 4;
	static const size_t FAR_RIGHT_BOTTOM  = 5;
	static const size_t FAR_LEFT_TOP      = 6;
	static const size_t FAR_RIGHT_TOP     = 7;
	//@}

	VEC3 Points[8];

	/// Tworzy niezainicjalizowany
	FRUSTUM_POINTS() { }
	/// Inicjalizuje na podstawie p³aszczyzn
	FRUSTUM_POINTS(const FRUSTUM_PLANES &FrustumPlanes) { Set(FrustumPlanes); }
	/// Inicjalizuje na podstawie ODWROTNOŒCI macierzy View*Projection
	FRUSTUM_POINTS(const MATRIX &WorldViewProjInv) { Set(WorldViewProjInv); }
	/// Wype³nia
	void Set(const FRUSTUM_PLANES &FrustumPlanes);
	void Set(const MATRIX &WorldViewProjInv);

	VEC3 & operator [] (size_t Index) { return Points[Index]; }
	const VEC3 & operator [] (size_t Index) const { return Points[Index]; }

	/// Wylicza i zwraca œrodek frustuma
	void CalcCenter(VEC3 *Out) const;
	/// Oblicza najmniejszy AABB otaczaj¹cy frustum dany tymi punktami
	void CalcBoundingBox(BOX *Box) const;
	/// Oblicza sferê otaczaj¹c¹ frustum
	/** Mo¿e nie najlepsz¹, ale jak¹œtam... (a mo¿e to jest najlepsza? nie wiem :P) */
	void CalcBoundingSphere(VEC3 *SphereCenter, float *SphereRadius) const;
};

/// Frustum - reprezentacja radarowa.
/**
- Bardzo szybki jest test kolizji z punktem i ze sfer¹ (common::PointInFrustum,
  common::SphereToFrustum).
- UWAGA! Jako wektor Up podawaæ prawdziwy wektor kamery do góry, tzw. RealUp,
  który jest naprawdê wzajemnie prostopad³y do Forward i Right, a nie ten który
  zawsze wynosi (0,1,0) - to by³ b³¹d którego siê naszuka³em.
*/
class FRUSTUM_RADAR
{
private:
	VEC3 m_Eye;
	VEC3 m_Forward;
	VEC3 m_Up;
	VEC3 m_Right;
	float m_RFactor;
	float m_UFactor;
	float m_RSphereFactor;
	float m_USphereFactor;
	float m_ZNear;
	float m_ZFar;

public:
	/// Tworzy niezainicjalizowany
	FRUSTUM_RADAR() { }
	/// Tworzy w pe³ni zainicjalizowany
	FRUSTUM_RADAR(const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up, const VEC3 &Right, float FovY, float Aspect, float ZNear, float ZFar) { Set(Eye, Forward, Up, Right, FovY, Aspect, ZNear, ZFar); }

	/** \name Pamiêtane pola
	Zwraca poszczególne pamiêtane pola (FovY i Aspect nie pamiêta bezpoœrednio)
	*/
	//@{
	const VEC3 & GetEye() const     { return m_Eye; }
	const VEC3 & GetForward() const { return m_Forward; }
	const VEC3 & GetUp() const      { return m_Up; }
	const VEC3 & GetRight() const   { return m_Right; }
	float GetZNear() const   { return m_ZNear; }
	float GetZFar() const    { return m_ZFar; }
	//@}

	/** \name Dane pomocnicze
	Zwraca dane pomocnicze
	*/
	//@{
	float GetRFactor() const { return m_RFactor; }
	float GetUFactor() const { return m_UFactor; }
	float GetRSphereFactor() const { return m_RSphereFactor; }
	float GetUSphereFactor() const { return m_USphereFactor; }
	//@}

	/** \name Ustawianie poszczególnych pól */
	//@{
	void SetEye    (const VEC3 &Eye)     { m_Eye = Eye; }
	void SetForward(const VEC3 &Forward) { m_Forward = Forward; }
	void SetUp     (const VEC3 &Up)      { m_Up = Up; }
	void SetRight  (const VEC3 &Right)   { m_Right = Right; }
	void SetZNear(float ZNear) { m_ZNear = ZNear; }
	void SetZFor (float ZFar)  { m_ZFar = ZFar; }
	void SetFovAndAspect(float FovY, float Aspect);
	//@}
	
	/// Kompletnie wype³nia
	void Set(const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up, const VEC3 &Right, float FovY, float Aspect, float ZNear, float ZFar) { SetProjection(FovY, Aspect, ZNear, ZFar); SetView(Eye, Forward, Up, Right); }
	/// Wype³nia jedn¹ po³ówkê danych
	void SetProjection(float FovY, float Aspect, float ZNear, float ZFar);
	/// Wype³nia drug¹ po³ówkê danych
	void SetView(const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up, const VEC3 &Right);
};

//@}
// math_frustum


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup mathcapsule Kapsu³a */
//@{

/// Objêtoœæ kapsu³y
float CapsuleVolume(const VEC3 &p1, const VEC3 &p2, float R);
/// Losuje punkt z wnêtrza kapsu³y. Rozk³ad równomierny.
void RandomPointInCapsule(VEC3 *Out, const VEC3 &p1, const VEC3 &p2, float R, RandomGenerator &Rand);
void RandomPointInCapsule(VEC3 *Out, const VEC3 &p1, const VEC3 &p2, float R);

//@}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// INNE

/** \addtogroup math_smoothcd Wyg³adzanie zmiennej
Funkcja wyg³adzaj¹ca zmianê zmiennej w czasie - SmoothCD.
Nak³adki na t¹ funkcjê - szablon specjalizowany dla: float, common::VEC2, common::VEC3.
Klasa szablonowa enkapsuluj¹ca pamiêtanie parametrów wyg³adzania. */
//@{

template <typename T>
inline void SmoothCD_T(T *InOutPos, const T &Dest, T *InOutVel, float SmoothTime, float TimeDelta)
{
	assert(0 && "SmoothCD_T for invalid template type.");
}

template <>
inline void SmoothCD_T<float>(float *InOutPos, const float &Dest, float *InOutVel, float SmoothTime, float TimeDelta)
{
	SmoothCD(InOutPos, Dest, InOutVel, SmoothTime, TimeDelta);
}

template <>
inline void SmoothCD_T<VEC2>(VEC2 *InOutPos, const VEC2 &Dest, VEC2 *InOutVel, float SmoothTime, float TimeDelta)
{
	SmoothCD(&InOutPos->x, Dest.x, &InOutVel->x, SmoothTime, TimeDelta);
	SmoothCD(&InOutPos->y, Dest.y, &InOutVel->y, SmoothTime, TimeDelta);
}

template <>
inline void SmoothCD_T<VEC3>(VEC3 *InOutPos, const VEC3 &Dest, VEC3 *InOutVel, float SmoothTime, float TimeDelta)
{
	SmoothCD(&InOutPos->x, Dest.x, &InOutVel->x, SmoothTime, TimeDelta);
	SmoothCD(&InOutPos->y, Dest.y, &InOutVel->y, SmoothTime, TimeDelta);
	SmoothCD(&InOutPos->z, Dest.z, &InOutVel->z, SmoothTime, TimeDelta);
}

template <typename T>
class SmoothCD_obj
{
public:
	T Pos;
	T Dest; ///< Zapamiêtana ostatnia wartoœæ docelowa - mo¿e siê przydaæ.
	T Vel;
	float SmoothTime;

	/// Tworzy niezainicjalizowany
	SmoothCD_obj() { }
	/// Tworzy zainicjalizowany
	SmoothCD_obj(const T &Pos, float SmoothTime, const T &Vel) : Pos(Pos), Dest(Pos), Vel(Vel), SmoothTime(SmoothTime) { }
	/// Ustawia parametry
	void Set(const T &Pos, const T &Vel) { this->Pos = Pos; this->Dest = Pos; this->Vel = Vel; }

	/// Wykonuje krok czasowy
	void Update(const T &Dest, float TimeDelta) {
		this->Dest = Dest;
		SmoothCD_T<T>(&Pos, Dest, &Vel, SmoothTime, TimeDelta);
	}
};

//@}
// math_smoothcd


/** \addtogroup math_spherical Wspó³rzêdne sferyczne
- Uk³ad kartezjañski: DirectX: X w prawo, Y w górê, Z w g³¹b
- Uk³ad sferyczny: Yaw to d³ugoœæ geograficzna, Pitch to szerokoœæ geograficzna, R to promieñ wodz¹cy
*/
//@{

/// Tu mo¿na podawaæ co siê chce.
void SphericalToCartesian(VEC3 *Out, float Yaw, float Pitch, float R);

/**
\param[out] Yaw
  Wychodzi -PI..PI.
  Mo¿na podaæ NULL, jeœli akurat nas nie interesuje - wtedy liczy szybciej.
\param[out] Pitch
  Wychodzi -PI/2..PI/2.
  Mo¿na podaæ NULL, jeœli akurat nas nie interesuje - wtedy liczy szybciej.
\param[out] R
  Wychodzi > 0.
  Mo¿na podaæ NULL, jeœli akurat nas nie interesuje - wtedy liczy szybciej.
\param Pos
  Nie podawaæ wektora zerowego bo wyjd¹ buraki.
*/
void CartesianToSpherical(float *OutYaw, float *OutPitch, float *OutR, const VEC3 &Pos);

//@}
// math_spherical

/// Zwraca wypuk³¹ otoczkê zbioru punktów 2D.
/**
- Na wejœciu musi byæ co najmniej jeden punkt.
- Punkty bêd¹ u³o¿one zgodnie z ruchem wskazówek zegara, jeœli uk³ad jest X w prawo a Y w dó³.
- Punkty powtarzaj¹ce siê s¹ chyba usuwane, tzn. wynik jest bez powtórzeñ.
- Punkty le¿¹ce na prostej krawêdzi raz pojawiaj¹ siê w wyniku a raz nie - niezdefiniowane.
*/
void ConvexHull2D(std::vector<VEC2> *OutPoints, const VEC2 InPoints[], size_t InPointCount);

/**
Macierz rotacji przekszta³caj¹ca wektory z przestrzeni Half-Life 2 do
przestrzeni stycznej Tangent Space i jej transpozycja przekszta³caj¹ca
z powrotem. To s¹ macierz ortogonalne.
\verbatim
b0 = ( -1/sqrt(6), -1/sqrt(2), 1/sqrt(3) )
b1 = ( -1/sqrt(6),  1/sqrt(2), 1/sqrt(3) )
b2 = ( sqrt(2/3),   0,         1/sqrt(3) )
\endverbatim
*/
extern const MATRIX HL2_TO_TANGENT_MATRIX;
extern const MATRIX TANGENT_TO_HL2_MATRIX;
/// Wektory bazowe HL2 Space wyra¿one w Tangent Space
extern const VEC3 HL2_BASIS_VECTORS[3];


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_curves Curves */
//@{

/// Calculates value of Catmull-Rom curve at given point t = 0..1 between points b and c.
/** Params a, b, c, d are four control points. \n
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void EvalCatmullRomCurve(T *out, const T &a, const T &b, const T &c, const T &d, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;
	*out = 0.5f * ( (2.f *b) + (c-a)*t + (2*a - 5*b + 4*c - d)*t2 + (3*b - 3*c + d - a)*t3 );
}

/// Evaluates tangent (not normalized) of Catmull-Rom curve at given point t = 0..1 between points b and c.
/** Params a, b, c, d are four control points. \n
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void EvalCatmullRomCurveDiff(T *out, const T &a, const T &b, const T &c, const T &d, float t)
{
	float t2 = t * t;
	*out = 0.5f * ( (c - a) + 2.f*(a*2.f - b*5.f + c* 4.f - d)*t + 3.f*(a*-1.f + b*3.f -c*3.f + d)*t2 );
}

/// Calculates value of quadratic Bezier curve at given point t = 0..1 between points a and c.
/** Param b is a control points of the curve. \n
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void EvalQuadraticBezierCurve(T *out, const T &a, const T &b, const T &c, float t)
{
	float t_inv = 1.f - t;
	out = t_inv*t_inv*a + 2.f*t*t_inv*b + t*t*c;
}

/// Calculates value of cubic Bezier curve at given point t = 0..1 between points a and d.
/** Params b, c are control points of the curve. \n
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void EvalCubicBezierCurve(T *out, const T &a, const T &b, const T &c, const T &d, float t)
{
	float t2=t*t;
	float t3=t2*t;
	float v=1-t;
	float v2=v*v;
	float v3=v2*v;
	*out = a*v3 + b*3.f*v2*t + c*3.f*v*t2 + d*t3; 
}

/// Evaluates tangent (not normalized) of cubic Bezier curve at given point t = 0..1 between points a and d.
/** Params b, c are control points of the curve. \n
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void EvalCubicBezierCurveDiff(T *out, const T &a, const T &b, const T &c, const T &d, float t)
{
	float tinv = 1.f - t;
	float tinvpow2 = tinv * tinv;
	float upow2 = t * t;
	float ta = 3.f * tinvpow2;
	float tb = 6.f * tinv * t;
	float tc = 3.f * upow2;
	/*float c0 = -ta;
	float c1 = ta - tb;
	float c2 = tb - tc;
	float c3 = tc;*/ // WTF?! These values never used. Wai it supposed to be this way?
	*out = b*(ta-tb) + c*(tb-tc) + d*tc - a*ta;
}

/// Splits cubic Bezier curve in the middle (at t=0.5) into two cubic Bezier curves.
/** a, b, c, d are points of the input curve. \n
out1[0], out1[1], out1[2], out1[3] will be points of the first output curve. \n
out2[0], out2[1], out2[2], out2[3] will be points of the second output curve.
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void SplitCubicBezierCurve(T out1[4], T out2[4], const T &a, const T &b, const T &c, const T &d)
{
	out1[0] = a;
	out1[1] = a*0.5f   + b*0.5f;
	out1[2] = a*0.25f  + b*0.5f   + c*0.25f;
	out1[3] = a*0.125f + b*0.375f + c*0.375f + d*0.125f;
	out2[3] = d;
	out2[2] = d*0.5f   + c*0.5f;
	out2[1] = d*0.25f  + c*0.5f   + b*0.25f;
	out2[0] = d*0.125f + c*0.375f + b*0.375f + a*0.125f;
}

/// Splits cubic Bezier curve in the given point (at parameter t) into two cubic Bezier curves.
/** a, b, c, d are points of the input curve. \n
out1[0], out1[1], out1[2], out1[3] will be points of the first output curve. \n
out2[0], out2[1], out2[2], out2[3] will be points of the second output curve.
Use types like float, VEC2, VEC3, VEC4 as T. */
template <typename T>
inline void SplitCubicBezierCurve(T out1[4], T out2[4], const T &a, const T &b, const T &c, const T &d, float t)
{
	T p01 = a + (b - a) * t;
	T p12 = b + (c - b) * t;
	T p23 = c + (d - c) * t;
	T pa = p01 + (p12 - p01) * t;
	T pb = p12 + (p23 - p12) * t;
	T pc = pa + (pb - pa) * t;
	out1[0] = a;  out1[1] = p01; out1[2] = pa;  out1[3] = pc;
	out2[0] = pc; out2[1] = pb;  out2[2] = p23; out2[3] = d;
}

template <typename T>
class CubicBezierCurveIntegral
{
public:
	void Setup(const T &a, const T &b, const T &c, const T &d, const T &zero)
	{
		m_Data[0] = zero;
		m_Data[1] = a * 0.25f;
		m_Data[2] = (a + b) * 0.25f;
		m_Data[3] = (a + b + c) * 0.25f;
		m_Data[4] = (a + b + c + d) * 0.25f;
	}

	void Calc(T *out, float t)
	{
		float t2 = t * t;
		float t3 = t2 * t;
		float t4 = t3 * t;

		float inv_t = 1.0f - t;
		float inv_t2 = inv_t * inv_t;
		float inv_t3 = inv_t2 * inv_t;
		float inv_t4 = inv_t3 * inv_t;

		*out = m_Data[0]*inv_t4 + m_Data[1]*4*inv_t3*t + m_Data[2]*6*inv_t2*t2 + m_Data[3]*4*inv_t*t3 + m_Data[4]*t4;
	}

private:
	T m_Data[5];
};

float CalcCubicBezierCurveLength(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d, float t1, float t2);
float GetCubicBezierCurveArcLengthParam(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d, float len, float epsilon);
/// Calculates Y value and T parameter of cubic Bezier curve intersected with axis-aligned line x.
bool IntersectCubicBezierCurve2DWithX(const VEC2 curve[4], float x, float eps, float *outY, float *outT, float t0 = 0.f, float t1 = 1.f);

//@}
// math_curves


/** \addtogroup math_comp Comparisons
*/
//@{

template <typename Comp> inline bool PointAllCmp(const POINT_ &lhs, const POINT_ &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) && compObj(lhs.y, rhs.y); }
template <typename Comp> inline bool Vec2AllCmp(const VEC2 &lhs, const VEC2 &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) && compObj(lhs.y, rhs.y); }
template <typename Comp> inline bool Vec3AllCmp(const VEC3 &lhs, const VEC3 &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) && compObj(lhs.y, rhs.y) && compObj(lhs.z, rhs.z); }
template <typename Comp> inline bool Vec4AllCmp(const VEC4 &lhs, const VEC4 &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) && compObj(lhs.y, rhs.y) && compObj(lhs.z, rhs.z) && compObj(lhs.w, rhs.w); }
template <typename Comp> inline bool ColorfAllCmp(const COLORF &lhs, const COLORF &rhs) { Comp compObj; return compObj(lhs.R, rhs.R) && compObj(lhs.G, rhs.G) && compObj(lhs.B, rhs.B) && compObj(lhs.A, rhs.A); }
template <typename Comp> inline bool ColorAllCmp(COLOR lhs, COLOR rhs) { Comp compObj; return compObj(lhs.R, rhs.R) && compObj(lhs.G, rhs.G) && compObj(lhs.B, rhs.B) && compObj(lhs.A, rhs.A); }
template <typename Comp> inline bool MatrixAllCmp(const MATRIX &lhs, const MATRIX &rhs)
{
	Comp compObj;
	return compObj(lhs._11, rhs._11) && compObj(lhs._12, rhs._12) && compObj(lhs._13, rhs._13) && compObj(lhs._14, rhs._14)
		&& compObj(lhs._21, rhs._21) && compObj(lhs._22, rhs._22) && compObj(lhs._23, rhs._23) && compObj(lhs._24, rhs._24)
		&& compObj(lhs._31, rhs._31) && compObj(lhs._32, rhs._32) && compObj(lhs._33, rhs._33) && compObj(lhs._34, rhs._34)
		&& compObj(lhs._41, rhs._41) && compObj(lhs._42, rhs._42) && compObj(lhs._43, rhs._43) && compObj(lhs._44, rhs._44);
}

template <typename Comp> inline bool PointAnyCmp(const POINT_ &lhs, const POINT_ &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) || compObj(lhs.y, rhs.y); }
template <typename Comp> inline bool Vec2AnyCmp(const VEC2 &lhs, const VEC2 &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) || compObj(lhs.y, rhs.y); }
template <typename Comp> inline bool Vec3AnyCmp(const VEC3 &lhs, const VEC3 &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) || compObj(lhs.y, rhs.y) || compObj(lhs.z, rhs.z); }
template <typename Comp> inline bool Vec4AnyCmp(const VEC4 &lhs, const VEC4 &rhs) { Comp compObj; return compObj(lhs.x, rhs.x) || compObj(lhs.y, rhs.y) || compObj(lhs.z, rhs.z) || compObj(lhs.w, rhs.w); }
template <typename Comp> inline bool ColorfAnyCmp(const COLORF &lhs, const COLORF &rhs) { Comp compObj; return compObj(lhs.R, rhs.R) || compObj(lhs.G, rhs.G) || compObj(lhs.B, rhs.B) || compObj(lhs.A, rhs.A); }
template <typename Comp> inline bool ColorAnyCmp(COLOR lhs, COLOR rhs) { Comp compObj; return compObj(lhs.R, rhs.R) || compObj(lhs.G, rhs.G) || compObj(lhs.B, rhs.B) || compObj(lhs.A, rhs.A); }
template <typename Comp> inline bool MatrixAnyCmp(const MATRIX &lhs, const MATRIX &rhs)
{
	Comp compObj;
	return compObj(lhs._11, rhs._11) || compObj(lhs._12, rhs._12) || compObj(lhs._13, rhs._13) || compObj(lhs._14, rhs._14)
		|| compObj(lhs._21, rhs._21) || compObj(lhs._22, rhs._22) || compObj(lhs._23, rhs._23) || compObj(lhs._24, rhs._24)
		|| compObj(lhs._31, rhs._31) || compObj(lhs._32, rhs._32) || compObj(lhs._33, rhs._33) || compObj(lhs._34, rhs._34)
		|| compObj(lhs._41, rhs._41) || compObj(lhs._42, rhs._42) || compObj(lhs._43, rhs._43) || compObj(lhs._44, rhs._44);
}

inline bool AllGreater(const POINT_ &lhs, const POINT_ &rhs) { return PointAllCmp< std::greater<int> >(lhs, rhs); }
inline bool AllGreater(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AllCmp< std::greater<float> >(lhs, rhs); }
inline bool AllGreater(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AllCmp< std::greater<float> >(lhs, rhs); }
inline bool AllGreater(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AllCmp< std::greater<float> >(lhs, rhs); }
inline bool AllGreater(const COLORF &lhs, const COLORF &rhs) { return ColorfAllCmp< std::greater<float> >(lhs, rhs); }
inline bool AllGreater(COLOR lhs, COLOR rhs) { return ColorAllCmp< std::greater<uint8> >(lhs, rhs); }
inline bool AllGreater(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAllCmp< std::greater<float> >(lhs, rhs); }

inline bool AllGreaterEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAllCmp< std::greater_equal<int> >(lhs, rhs); }
inline bool AllGreaterEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AllCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AllGreaterEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AllCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AllGreaterEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AllCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AllGreaterEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAllCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AllGreaterEqual(COLOR lhs, COLOR rhs) { return ColorAllCmp< std::greater_equal<uint8> >(lhs, rhs); }
inline bool AllGreaterEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAllCmp< std::greater_equal<float> >(lhs, rhs); }

inline bool AllLess(const POINT_ &lhs, const POINT_ &rhs) { return PointAllCmp< std::less<int> >(lhs, rhs); }
inline bool AllLess(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AllCmp< std::less<float> >(lhs, rhs); }
inline bool AllLess(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AllCmp< std::less<float> >(lhs, rhs); }
inline bool AllLess(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AllCmp< std::less<float> >(lhs, rhs); }
inline bool AllLess(const COLORF &lhs, const COLORF &rhs) { return ColorfAllCmp< std::less<float> >(lhs, rhs); }
inline bool AllLess(COLOR lhs, COLOR rhs) { return ColorAllCmp< std::less<uint8> >(lhs, rhs); }
inline bool AllLess(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAllCmp< std::less<float> >(lhs, rhs); }

inline bool AllLessEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAllCmp< std::less_equal<int> >(lhs, rhs); }
inline bool AllLessEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AllCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AllLessEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AllCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AllLessEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AllCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AllLessEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAllCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AllLessEqual(COLOR lhs, COLOR rhs) { return ColorAllCmp< std::less_equal<uint8> >(lhs, rhs); }
inline bool AllLessEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAllCmp< std::less_equal<float> >(lhs, rhs); }

inline bool AllEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAllCmp< std::equal_to<int> >(lhs, rhs); }
inline bool AllEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AllCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AllEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AllCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AllEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AllCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AllEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAllCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AllEqual(COLOR lhs, COLOR rhs) { return ColorAllCmp< std::equal_to<uint8> >(lhs, rhs); }
inline bool AllEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAllCmp< std::equal_to<float> >(lhs, rhs); }

inline bool AllNotEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAllCmp< std::not_equal_to<int> >(lhs, rhs); }
inline bool AllNotEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AllCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AllNotEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AllCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AllNotEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AllCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AllNotEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAllCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AllNotEqual(COLOR lhs, COLOR rhs) { return ColorAllCmp< std::not_equal_to<uint8> >(lhs, rhs); }
inline bool AllNotEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAllCmp< std::not_equal_to<float> >(lhs, rhs); }

inline bool AnyGreater(const POINT_ &lhs, const POINT_ &rhs) { return PointAnyCmp< std::greater<int> >(lhs, rhs); }
inline bool AnyGreater(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AnyCmp< std::greater<float> >(lhs, rhs); }
inline bool AnyGreater(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AnyCmp< std::greater<float> >(lhs, rhs); }
inline bool AnyGreater(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AnyCmp< std::greater<float> >(lhs, rhs); }
inline bool AnyGreater(const COLORF &lhs, const COLORF &rhs) { return ColorfAnyCmp< std::greater<float> >(lhs, rhs); }
inline bool AnyGreater(COLOR lhs, COLOR rhs) { return ColorAnyCmp< std::greater<uint8> >(lhs, rhs); }
inline bool AnyGreater(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAnyCmp< std::greater<float> >(lhs, rhs); }

inline bool AnyGreaterEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAnyCmp< std::greater_equal<int> >(lhs, rhs); }
inline bool AnyGreaterEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AnyCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AnyGreaterEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AnyCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AnyGreaterEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AnyCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AnyGreaterEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAnyCmp< std::greater_equal<float> >(lhs, rhs); }
inline bool AnyGreaterEqual(COLOR lhs, COLOR rhs) { return ColorAnyCmp< std::greater_equal<uint8> >(lhs, rhs); }
inline bool AnyGreaterEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAnyCmp< std::greater_equal<float> >(lhs, rhs); }

inline bool AnyLess(const POINT_ &lhs, const POINT_ &rhs) { return PointAnyCmp< std::less<int> >(lhs, rhs); }
inline bool AnyLess(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AnyCmp< std::less<float> >(lhs, rhs); }
inline bool AnyLess(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AnyCmp< std::less<float> >(lhs, rhs); }
inline bool AnyLess(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AnyCmp< std::less<float> >(lhs, rhs); }
inline bool AnyLess(const COLORF &lhs, const COLORF &rhs) { return ColorfAnyCmp< std::less<float> >(lhs, rhs); }
inline bool AnyLess(COLOR lhs, COLOR rhs) { return ColorAnyCmp< std::less<uint8> >(lhs, rhs); }
inline bool AnyLess(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAnyCmp< std::less<float> >(lhs, rhs); }

inline bool AnyLessEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAnyCmp< std::less_equal<int> >(lhs, rhs); }
inline bool AnyLessEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AnyCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AnyLessEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AnyCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AnyLessEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AnyCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AnyLessEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAnyCmp< std::less_equal<float> >(lhs, rhs); }
inline bool AnyLessEqual(COLOR lhs, COLOR rhs) { return ColorAnyCmp< std::less_equal<uint8> >(lhs, rhs); }
inline bool AnyLessEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAnyCmp< std::less_equal<float> >(lhs, rhs); }

inline bool AnyEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAnyCmp< std::equal_to<int> >(lhs, rhs); }
inline bool AnyEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AnyCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AnyEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AnyCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AnyEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AnyCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AnyEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAnyCmp< std::equal_to<float> >(lhs, rhs); }
inline bool AnyEqual(COLOR lhs, COLOR rhs) { return ColorAnyCmp< std::equal_to<uint8> >(lhs, rhs); }
inline bool AnyEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAnyCmp< std::equal_to<float> >(lhs, rhs); }

inline bool AnyNotEqual(const POINT_ &lhs, const POINT_ &rhs) { return PointAnyCmp< std::not_equal_to<int> >(lhs, rhs); }
inline bool AnyNotEqual(const VEC2 &lhs, const VEC2 &rhs) { return Vec2AnyCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AnyNotEqual(const VEC3 &lhs, const VEC3 &rhs) { return Vec3AnyCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AnyNotEqual(const VEC4 &lhs, const VEC4 &rhs) { return Vec4AnyCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AnyNotEqual(const COLORF &lhs, const COLORF &rhs) { return ColorfAnyCmp< std::not_equal_to<float> >(lhs, rhs); }
inline bool AnyNotEqual(COLOR lhs, COLOR rhs) { return ColorAnyCmp< std::not_equal_to<uint8> >(lhs, rhs); }
inline bool AnyNotEqual(const MATRIX &lhs, const MATRIX &rhs) { return MatrixAnyCmp< std::not_equal_to<float> >(lhs, rhs); }

//@}
// math_vec_comp


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_collisions Kolizje */
//@{

/// Funkcja wielofunkcyjna
/**
- Wynik > 0 - punkt c le¿y na lewo od skierowanego odcinka a-b.
  Trójk¹t abc jest skierowany przeciwnie do ruchu wskazówek zegara.
- Wynik < 0 - punkt c le¿y na prawo od skierowanego odcinka a-b.
  Trójk¹t abc jest skierowany zgodnie z ruchem wskazówek zegara.
- Wynik = 0 - podane trzy punkty le¿¹ w jednej linii.
- abs(Wynik) - podwojona powierzchnia trój¹ta abc.
*/
float Orient2D(const VEC2 &a, const VEC2 &b, const VEC2 &c);
/// Funkcja wielofunkcyjna
/**
- Wynik < 0 - punkt d le¿y nad p³aszczyzn¹ definiowan¹ przez trójk¹t abc,
  widzian¹ tak ¿e ten trójk¹t jest zorientowany przeciwnie do ruchu wskazówek
  zegara.
- Wynik > 0 - punkt d le¿y pod p³aszczyzn¹ definiowan¹ przez trójk¹t abc,
  widzian¹ tak ¿e ten trójk¹t jest zorientowany przeciwnie do ruchu wskazówek
  zegara.
- Wynik = 0 - wszystkie cztery punkty le¿¹ w jednej p³aszczyŸnie.
- abs(Wynik) - szeœciokrotna objêtoœæ czworoœcianu tworzonego przez punkty abcd.
*/
float Orient3D(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d);

inline bool PointInCircle(const VEC2 &pt, const VEC2 &circleCenter, float circleRadius)
{
	return DistanceSq(pt, circleCenter) <= sqr(circleRadius);
}
/// Zwraca true, jeœli punkt p nale¿y do trójk¹ta abc
bool PointInTriangle(const VEC2 &p, const VEC2 &a, const VEC2 &b, const VEC2 &c);
/// Zwraca true, jeœli podany czworok¹t jest wypuk³y.
/** Nie testowana, mo¿e Ÿle dzia³aæ dla zdegenerowanych - TODO. */
bool IsQuadConvex(const VEC2 &a, const VEC2 &b, const VEC2 &c, const VEC2 &d);
/// Zwraca true, jeœli podany czworok¹t jest wypuk³y (nie zdegenerowany, nie wklês³y, nie samoprzecinaj¹cy siê).
/** Wszystkie cztery wierzcho³ki musz¹ le¿eæ w jednej p³aszczyŸnie. */
bool IsQuadConvex(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d);
/// Dany punkt p i promieñ (RayOrig, RayDir), oblicza parametr t dla punktu na tym promieniu bêd¹cego rzutem tego punktu na ten promieñ.
/** W ten sposób mo¿na obliczyæ rzut punktu na promieñ (punkt z promienia najbli¿szy
danemu punktowi) jako RayOrig + RayDir*t. Mo¿na te¿ policzyæ najbli¿szy punkt
na pó³prostej lub odcinku limituj¹c najpierw t od 0 do 1 czy od 0 do dlugoœci odcinka.
UWAGA! Jeœli RayDir nie jest jednostkowy, wynik trzeba podzieliæ przez LengthSq(RayDir). */
float ClosestPointOnLine(const VEC3 &p, const VEC3 &RayOrig, const VEC3 &RayDir);
/// Oblicza najbli¿sze sobie punkty na dwóch odcinkach
/** Dane s¹ dwa odcinki: Seg1_Beg..Seg1_End i Seg2_Beg..Seg2_End.
Funkcja oblicza najbli¿sze sobie punkty na tych odcinkach.
Parametry do tych odcinków zwraca przez OutSeg1_t, OutSeg2_t.
Te punkty zwraca przez OutSeg1_pt, OutSeg2_pt.
Ca³a funkcja zwraca kwadrat odleg³oœci miêdzy tymi punktami.
*/
float ClosestPointSegmentSegment(
	const VEC3 &Seg1_Beg, const VEC3 &Seg1_End, const VEC3 &Seg2_Beg, const VEC3 &Seg2_End,
	float &OutSeg1_t, float &OutSeg2_t, VEC3 &OutSeg1_pt, VEC3 &OutSeg2_pt);
/// Zwraca kwadrat odleg³oœci punktu od prostej
/** \param LineDir   musi byæ znormalizowane! */
float PointToLineDistanceSq(const VEC2 &P, const VEC2 &LineOrig, const VEC2 &LineDir);
float PointToLineDistanceSq(const VEC3 &P, const VEC3 &LineOrig, const VEC3 &LineDir);
/// Zwraca kwadrat odleg³oœci punktu od odcinka
float PointToSegmentDistanceSq(const VEC2 &p, const VEC2 &segment_p1, const VEC2 &segment_p2);
/// Zwraca kwadrat odleg³oœci punktu od odcinka
float PointToSegmentDistanceSq(const VEC3 &p, const VEC3 &segment_p1, const VEC3 &segment_p2);
/// Zwraca true, jeœli punkt le¿y na prostej
/** \param LineDir   musi byæ znormalizowane! */
bool PointOnLine(const VEC3 &P, const VEC3 &LineOrig, const VEC3 &LineDir);
/// Zwraca parametr T punktu le¿¹cego na prostej
/** \param P   Punkt musi naprawdê le¿eæ na tej prostej! */
float PointToLine(const VEC3 &P, const VEC3 &LineOrig, const VEC3 &LineDir);
/// Zwraca true, jeœli punkt le¿y na odcinku.
/** \param P   Punkt musi le¿eæ na prostej przechodz¹cej przez ten odcinek! */
bool PointInLineSegment(const VEC3 &P, const VEC3 &SegmentBeg, const VEC3 &SegmentEnd);
/// Wyznacza najbli¿szy punkt le¿¹cy na odcinku p1-p2 wzglêdem punktu p
/** \param Out [out] Mo¿na podaæ NULL.
\return Zwraca parametr t dla odcinka, wyra¿ony w d³ugoœciach odcinka. */
float ClosestPointOnSegment(VEC2 *Out, const VEC2 &p, const VEC2 &p1, const VEC2 &p2);
float ClosestPointOnSegment(VEC3 *Out, const VEC3 &p, const VEC3 &p1, const VEC3 &p2);
/// Wyznacza najbli¿szy punkt le¿¹cy na pó³prostej wzglêdem punktu p
/** \param Out [out] Mo¿na podaæ NULL.
\return Zwraca parametr t dla pó³prostej, wyra¿ony w d³ugoœciach odcinka. */
float ClosestPointOnRay(VEC2 *Out, const VEC2 &p, const VEC2 &RayOrig, const VEC2 &RayDir);
float ClosestPointOnRay(VEC3 *Out, const VEC3 &p, const VEC3 &RayOrig, const VEC3 &RayDir);
/// Wyznacza najbli¿szy punkt wewn¹trz prostopad³oœcianu wzglêdem punktu p.
/** Jeœli p le¿y wewn¹trz tego prostopad³oœcianu, zwrócone zostaje dok³adnie p. */
void ClosestPointInBox(VEC3 *Out, const BOX &Box, const VEC3 &p);
/// Znajduje punkt OutPoint nale¿¹cy do podanego OBB, najbli¿szy do punktu Point.
void ClosestPointInObb(VEC3 *OutPoint, const VEC3 &Point, const OBB &Obb);
/// Znajduje punkt OutPoint nale¿¹cy do podanego prostok¹ta 3D, najbli¿szy do punktu Point.
void ClosestPointInRect3d(VEC3 *OutPoint, const VEC3 &Point, const RECT3D &Rect3d);
/// Znajduje punkt OutPoint nale¿¹cy do podanego prostok¹ta 3D wyznaczonego przez trzy wierzcho³ki: a, b, c, najbli¿szy do punktu Point.
void ClosestPointInRect3d(VEC3 *OutPoint, const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c);
/// Znajduje punkt OutPoint nale¿¹cy do trójk¹ta (a, b, c) najbli¿szy do punktu p
void ClosestPointInTriangle(VEC3 *OutPoint, const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c);
/// \copydoc ClosestPointInRect3d(VEC3*, const VEC3&, const VEC3&, const VEC3&, const VEC3&)
void ClosestPointInTriangle(VEC2 *out, const VEC2 &pt, const VEC2 &triA, const VEC2 &triB, const VEC2 &triC);
/// Znajduje punkt OutPoint nale¿¹cy do podanego czworoœcianu wyznaczonego przez wierzcho³ki: a, b, c, d, najbli¿szy do punktu p.
void ClosestPointInTetrahedron(VEC3 *OutPoint, const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d);
/// Returns squared distance from point to rectangle. Returns 0 if point inside rectangle.
float PointToRectDistanceSq(const VEC2 &p, const RECTF &rect);
float PointToRectManhattanDistance(const VEC2 &p, const RECTF &rect);
float PointToRectMaxDistance(const VEC2 &p, const RECTF &rect);
/// Kwadrat odleg³oœci punktu od prostopad³oœcianu
/** Jeœli p le¿y wewn¹trz tego prostopad³oœcianu, zwraca 0. */
float PointToBoxDistanceSq(const VEC3 &p, const BOX &box);
float PointToBoxManhattanDistance(const VEC3 &p, const BOX &box);
float PointToBoxMaxDistance(const VEC3 &p, const BOX &box);
/// Returns squared distance between two rectangles. Returns 0 if given rectangles overlap.
float RectToRectDistanceSq(const RECTF &rect1, const RECTF &rect2);
float RectToRectManhattanDistance(const RECTF &rect1, const RECTF &rect2);
float RectToRectMaxDistance(const RECTF &rect1, const RECTF &rect2);
/// Returns squared distance between two axis-aligned boxes. Returns 0 if given boxes overlap.
float BoxToBoxDistanceSq(const BOX &box1, const BOX &box2);
float BoxToBoxManhattanDistance(const BOX &box1, const BOX &box2);
float BoxToBoxMaxDistance(const BOX &box1, const BOX &box2);
/// Kwadrat odleg³oœci punktu od OBB.
float PointToObbDistanceSq(const VEC3 &Point, const OBB &Obb);
bool CircleToRect(const VEC2 &circleCenter, float circleRadius, const RECTF &rect);
/// Zwraca true, jeœli podany punkt nale¿y do sfery
bool PointInSphere(const VEC3 &p, const VEC3 &SphereCenter, float SphereRadius);
/// Zwraca odleg³oœæ punktu od powierzchni sfery
/** Jeœli punkt le¿y wewn¹trz sfery, zwróci odleg³oœæ ujemn¹. */
float PointToSphereDistance(const VEC3 &p, const VEC3 &SphereCenter, float SphereRadius);
/// Wyznacza najbli¿szy punkt na powierzchni sfery wzglêdem punktu p.
/** Punkt mo¿e byæ wewn¹trz sfery - te¿ jest OK, zwróci z powierzchni sfery.
Punkt nie mo¿e byæ dok³adnie na œrodku sfery - wtedy jest dzielenie przez zero. */
void ClosestPointOnSphere(VEC3 *Out, const VEC3 &SphereCenter, float SphereRadius, const VEC3 &p);
/// Zwraca true, jeœli podany punkt nale¿y do p³aszczyzny
/** P³aszczyzna nie musi byæ znormalizowana. */
bool PointOnPlane(const VEC3 &p, const PLANE &plane);
/// Wyznacza najbli¿szy punkt na p³aszczyŸnie wzglêdem punktu p.
/** P³aszczyzna musi byæ znormalizowana. */
void ClosestPointOnPlane(VEC3 *Out, const PLANE &Plane, const VEC3 &p);
/// Zwraca true, jeœli punkt nale¿y do trójk¹ta
bool PointInTriangle(const VEC3 &point, const VEC3 &pa, const VEC3 &pb, const VEC3 &pc);
/// Zwraca true, jeœli punkt nale¿y do wnêtrza podanego frustuma
bool PointInFrustum(const VEC3 &p, const FRUSTUM_PLANES &Frustum);
bool PointInFrustum(const VEC3 &p, const FRUSTUM_RADAR &Frustum);
/// Funkcja wyznacza parametry t1 i t2 dla punktów na dwóch prostych, w których te proste s¹ najbli¿ej siebie.
/** Dane s¹ dwie proste (RayOrig1, RayDir1) i (RayOrig2, RayDir2).
Funkcja wyznacza parametry t1 i t2 dla punktów na tych prostych, w których te proste s¹ najbli¿ej siebie.
Jeœli nie da siê takich znaleŸæ (proste s¹ równoleg³e), funkcja zwraca false.
Te punkty mo¿na policzyæ ze wzorów p1=RayOrig1+t1*RayDir1 i p2=RayOrig2+t2*RayDir2.
Proste faktycznie przecinaj¹ siê jeœli odleg³oœæ miêdzy tymi punktami wynosi ok. 0. */
bool ClosestPointsOnLines(float *OutT1, float *OutT2, const VEC3 &RayOrig1, const VEC3 &RayDir1, const VEC3 &RayOrig2, const VEC3 &RayDir2);
/// Kolizja promienia z prostopad³oœcianem
/** Jeœli promieñ nie przecina prostopad³oœcianu, zwraca false.
Jeœli promieñ przecina prostopad³oœcian, zwraca true i przez OutT zwraca odleg³oœæ w wielokrotnoœciach d³ugoœci RayDir.
Jeœli promieñ przecina prostopad³oœcian od ty³u, funkcja te¿ zwraca true i zwraca OutT ujemne.
Jeœli RayOrig jest wewn¹trz prostopad³oœcianu, funkcja zwraca true i OutT = 0. */
bool RayToBox(float *OutT, const VEC3 &RayOrig, const VEC3 &RayDir, const BOX &Box);
/// Wylicza kolizjê promienia ze sfer¹
/** Jeœli nie ma kolizji, zwraca false.
Jeœli jest kolizja, zwraca true i parametr T.
Jeœli kolizja jest z ty³u promienia - wczeœniej ni¿ jego pocz¹tek - zwraca true i T ujemne.
Jeœli pocz¹tek promienia jest wewn¹trz sfery, zwraca true i T=0 */
bool RayToSphere(const VEC3 &RayOrig, const VEC3 &RayDir, const VEC3 &SphereCenter, float SphereRadius, float *OutT);
/// Przeciêcie promienia z p³aszczyzn¹
/** \param[out] OutT   Zwrócony parametr dla promienia (NIE mo¿na podaæ 0)
\param[out] OutVD   Zwrócona liczba mówi¹ca o zwrocie p³aszczyzny (NIE mo¿na podaæ 0)
  OutVD > 0 oznacza, ¿e p³aszczyzna jest ty³em do promienia
\return   false jeœli promieñ by³ równoleg³y do p³aszczyzny i nie uda³o siê nic policzyæ. */
bool RayToPlane(const VEC3 &RayOrig, const VEC3 &RayDir, const PLANE &Plane, float *OutT, float *OutVD);
/// Kolizja promienia z trójk¹tem
/** \param[out] OutT   Mo¿na podaæ NULL jeœli nie interesuje nas odleg³oœæ kolizji.
\param BackfaceCulling   Jeœli true, funkcja zawsze uwzglêdnia wy³¹cznie kolizjê
z trójk¹tem zwróconym w stronê CW (jak w DirectX). */
bool RayToTriangle(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 &p0, const VEC3 &p1, const VEC3 &p2,
	bool BackfaceCulling,
	float *OutT);
/// Kolizja promienia z czworok¹tem w 3D
/** Czworok¹t musi byæ p³aski, wypuk³y i zorientowany odpowiednio.
Kolizja dzia³a tylko w jedn¹ stronê.
Dla T ujemnego równie¿ zwraca false. Niestety nie zwraca T.
\param OutPoint Parametr opcjonalny. */
bool RayToQuad(const VEC3 &RayOrig, const VEC3 &RayDir, const VEC3 QuadPoints[4], VEC3 *OutPoint = NULL);
/// Kolizja promienia z frustumem
bool RayToFrustum(const VEC3 &RayOrig, const VEC3 &RayDir, const FRUSTUM_PLANES &Frustum, float *t_near, float *t_far);
/// Kolizja promienia z dowolnie zorientowanym cylindrem
/** Jeœli kolizja jest z ty³u, te¿ zwraca true i OutT wychodzi ujemne.
Zwraca zawsze ten z dwóch punktów przeciêcia który ma mniejsze OutT. */
bool RayToCylinder(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 &CylinderBeg, const VEC3 &CylinderEnd, float CylinderRadius,
	float *OutT);
/// Czy punkt le¿y wewn¹trz wieloœciana wypuk³ego.
/** P³aszczyzny wieloœcianu maj¹ mieæ wektory normalne zwrócone do wewn¹trz. */
bool PointInConvexPolyhedron(const VEC3 &Point, const PLANE PolyhedronPlanes[], size_t PolyhedronPlaneCount);
bool PointInConvexPolyhedron(const VEC3 &Point, const void *PolyhedronPlanes, size_t PolyhedronPlaneCount, int PolyhedronPlaneStride);
/// Kolizja promienia z wieloœcianem wypuk³ym.
/** P³aszczyzny wieloœcianu maj¹ mieæ wektory normalne zwrócone do wewn¹trz.
Ta funkcja testuje tylko kolizjê pó³prostej.
Jeœli wieloœcian jest z ty³u promienia, funkcja zwraca false.
T zawsze wychodz¹ nieujemne. */
bool RayToConvexPolyhedron(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const PLANE PolyhedronPlanes[], size_t PolyhedronPlaneCount,
	float *OutBegT, float *OutEndT);
bool RayToConvexPolyhedron(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const void *PolyhedronPlanes, size_t PolyhedronPlaneCount, int PolyhedronPlaneStride,
	float *OutBegT, float *OutEndT);
/// Zwraca true jeœli kula koliduje z prostopad³oœcianem (tak¿e jeœli jest w jego œrodku)
bool SphereToBox(const VEC3 &SphereCenter, float SphereRadius, const BOX &Box);
/// Zwraca true, jeœli sfera koliduje z prostopad³oœcianem zorientowanym
bool SphereToObb(const VEC3 &SphereCenter, float SphereRadius, const OBB &Obb);
/// Zwraca true, jeœli sfera koliduje z trójk¹tem
bool SphereToTriangle(const VEC3 &SphereCenter, float SphereRadius, const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC);
/// Zwraca true, jeœli sfera zawiera siê w ca³oœci w prostopad³oœcianie
bool SphereInBox(const VEC3 &SphereCenter, float SphereRadius, const BOX &Box);
/// Zwraca true, jeœli prostopad³oœcian zawiera siê w ca³oœci w sferze
bool BoxInSphere(const BOX &Box, const VEC3 &SphereCenter, float SphereRadius);
/// Klasyfikuje po³o¿enie prostopad³oœcianu wzglêdem p³aszczyzny
/**
- -1 = prostopad³oœcian w ca³oœci po ujemnej stronie p³aszczyzny
-  0 = prostopad³oœcian przecina p³aszczyznê
- +1 = prostopad³oœcian w ca³oœci po dodatniej stronie p³aszczyzny
\param plane P³aszczyzna musi byæ znormalizowana */
int ClassifyBoxToPlane(const PLANE &plane, const BOX &box);
/// Klasyfikuje po³o¿enie zorientowanego prostopad³oœcianu OBB wzglêdem p³aszczyzny. Zwraca -1, 0, 1.
int ClassifyObbToPlane(const PLANE &Plane, const OBB &Obb);
/// Kolizja p³aszczyzny z prostopad³oœcianem
/** \param plane P³aszczyzna musi byæ znormalizowana */
inline bool PlaneToBox(const PLANE &plane, const BOX &box) { return ClassifyBoxToPlane(plane, box) == 0; }
/// Zwraca true, jeœli trójk¹t zawiera siê w prostopad³oœcianie
bool TriangleInBox(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, const BOX &Box);
/// Zwraca true, jeœli trójk¹t przecina prostopad³oœcian lub siê w nim zawiera
/** \param[in] OptTrianglePlane   Mo¿na podaæ jeœli siê ju¿ ma wyliczone,
¿eby przyspieszyæ. Nie musi byæ znormalizowane. */
bool TriangleToBox(const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC, const BOX &Box);
/// Zwraca true, jeœli podany prostopad³oœcian jest widoczny choæ trochê w bryle widzenia
/** Uwaga! W rzadkich przypadkach mo¿e stwierdziæ kolizjê chocia¿ jej nie ma. */
bool BoxToFrustum_Fast(const BOX &Box, const FRUSTUM_PLANES &Frustum);
/// Zwraca true, jeœli podany prostopad³oœcian jest widoczny choæ trochê w bryle widzenia
/**
Test dok³adny, ale wolniejszy i wymaga te¿ reprezentacji punktowej frustuma.
\param[in] OptFrustumPoints to parametr opcjonalny - podaj jeœli masz ju¿ wyliczony. */
bool BoxToFrustum(const BOX &Box, const FRUSTUM_PLANES &FrustumPlanes, const FRUSTUM_POINTS *OptFrustumPoints = NULL);
bool BoxToFrustum(const BOX &Box, const FRUSTUM_RADAR &Frustum);
/// Zwraca true, jeœli AABB jest w ca³oœci wewn¹trz frustuma
bool BoxInFrustum(const BOX &Box, const FRUSTUM_PLANES &Frustum);
/// Zwraca true jeœli podane dwie sfery koliduj¹ ze sob¹
bool SphereToSphere(const VEC3 &Center1, float Radius1, const VEC3 &Center2, float Radius2);
/// Zwraca true, jeœli sfera koliduje z p³aszczyzn¹
/** P³aszczyzna musi byæ znormalizowana. */
bool SphereToPlane(const VEC3 &SphereCenter, float SphereRadius, const PLANE &Plane);
/// Klasyfikuje po³o¿enie sfery wzglêdem p³aszczyzny
/**
- -1 = sfera w ca³oœci po ujemnej stronie p³aszczyzny
-  0 = sfera przecina p³aszczyznê
- +1 = sfera w ca³oœci po dodatniej stronie p³aszczyzny
*/
int ClassifySphereToPlane(const VEC3 &SphereCenter, float SphereRadius, const PLANE &Plane);
/// Kolizja kapsu³y (lub sfery poruszaj¹cej siê od CapsuleBeg do CapsuleEnd) z p³aszczyzn¹
/** \param Plane P³aszczyzna musi byæ znormalizowana. */
bool CapsuleToPlane(const VEC3 &CapsuleBeg, const VEC3 &CapsuleEnd, float CapsuleRadius, const PLANE &Plane);
/// Zwraca true, jeœli sfera koliduje z frustumem
/** Uwaga! W rzadkich przypadkach mo¿e stwierdziæ kolizjê chocia¿ jej nie ma. */
bool SphereToFrustum_Fast(const VEC3 &SphereCenter, float SphereRadius, const FRUSTUM_PLANES &Frustum);
/// Zwraca true, jeœli sfera koliduje z frustumem
/** Test dok³adny, ale wolniejszy i wymaga te¿ reprezentacji punktowej frustuma.
\param[in] OptFrustumPoints   Parametr opcjonalny - podaj jeœli masz ju¿ wyliczony. */
bool SphereToFrustum(
	const VEC3 &SphereCenter, float SphereRadius,
	const FRUSTUM_PLANES &FrustumPlanes, const FRUSTUM_POINTS *OptFrustumPoints = NULL);
/// Testuje przeciêcie sfery z frustumem.
/** Uwaga! Wygl¹da na to, ¿e z nienznaych przyczyn ta funkcja nie zwraca true
tak¿e dla sfery która nie jest œciœle koliduj¹ca z frustumem, ale z pewnym
niewielkim marginesem b³êdu w kierunku +/- Right.
*/
bool SphereToFrustum(const VEC3 &SphereCenter, float SphereRadius, const FRUSTUM_RADAR &Frustum);
/// Zwraca true, jeœli sfera zawiera siê w ca³oœci wewn¹trz frustuma
bool SphereInFrustum(const VEC3 &SphereCenter, float SphereRadius, const FRUSTUM_PLANES &Frustum);
/// Wylicza kolizjê dwóch p³aszczyzn zwracaj¹c parametry prostej tworz¹cej ich przeciêcie
/** Jesli nie ma kolizji bo p³aszczyzny s¹ równoleg³e, zwraca false.
P³aszczyzny musz¹ byæ znormalizowane.
\param OutLineDir [out] nie jest znormalizowany. */
bool Intersect2Planes(const PLANE &Plane1, const PLANE &Plane2, VEC3 *OutLineOrig, VEC3 *OutLineDir);
/// Zwraca punkt przeciêcia trzech p³aszczyzn lub false, jeœli taki punkt nie istnieje
/** P³aszczyzny musz¹ byæ znormalizowane. */
bool Intersect3Planes(const PLANE &P1, const PLANE &P2, const PLANE &P3, VEC3 *OutP);
/// Kolizja p³aszczyzny z trójk¹tem
bool PlaneToTriangle(const PLANE &Plane, const VEC3 &p0, const VEC3 &p1, const VEC3 &p2);
/// Klasyfikuje po³o¿enie trójk¹ta wzglêdem p³aszczyzny
/**
- -1 = trójk¹t w ca³oœci po ujemnej stronie p³aszczyzny
-  0 = trójk¹t przecina p³aszczyznê
- +1 = trójk¹t w ca³oœci po dodatniej stronie p³aszczyzny
*/
int ClassifyPlaneToTriangle(const PLANE &Plane, const VEC3 &p0, const VEC3 &p1, const VEC3 &p2);
/// Kolizja p³aszczyzny z frustumem (klasyfikacja)
int ClassifyFrustumToPlane(const FRUSTUM_POINTS &Frustum, const PLANE &Plane);
/// Zwraca true, jeœli dwa odcinki 2D zachodz¹ na siebie
/** \param OutSeg1_t Parametr opcjonalny. Zwraca parametr 0..1 miejsca przeciêcia na odcinku Seg1. */
bool SegmentToSegment(const VEC2 &Seg1_Beg, const VEC2 &Seg1_End, const VEC2 &Seg2_Beg, const VEC2 &Seg2_End, float *OutSeg1_t = NULL);
/// Testuje kolizjê dwóch trójk¹tów 3D
/** Nie wiem co siê dzieje jeœli trójk¹ty s¹ zdegenerowane. */
bool TriangleToTriangle(
	const VEC3 &V0,const VEC3 &V1,const VEC3 &V2,
	const VEC3 &U0,const VEC3 &U1,const VEC3 &U2);
/// Testuje, czy trójk¹t le¿y w ca³oœci wewn¹trz frustuma
bool TriangleInFrustum(const VEC3 &t1, const VEC3 &t2, const VEC3 &t3, const FRUSTUM_PLANES &Frustum);
/// Testuje kolizjê trójk¹ta z frustumem
/** \param[in] OptTrianglePlane   Podaj jeœli masz ju¿ wyliczone, aby przyspieszyæ.
\param[in] OptFrustumPoints   Podaj jeœli masz ju¿ wyliczone, aby przyspieszyæ. */
bool TriangleToFrustum(
	const VEC3 &t1, const VEC3 &t2, const VEC3 &t3, const PLANE *OptTrianglePlane,
	const FRUSTUM_PLANES &FrustumPlanes, const FRUSTUM_POINTS *OptFrustumPoints);
/// Testuje kolizjê miêdzy dwoma frustumami
bool FrustumToFrustum(
	const FRUSTUM_PLANES &Frustum1_Planes, const FRUSTUM_POINTS &Frustum1_Points,
	const FRUSTUM_PLANES &Frustum2_Planes, const FRUSTUM_POINTS &Frustum2_Points);
/// Kolizja sfery z kapsu³¹
bool SphereToCapsule(
	const VEC3 &SphereCenter, float SphereRadius,
	const VEC3 &Capsule_Beg, const VEC3 &Capsule_End, float Capsule_Radius);
/// Kolizja kapsu³y z kapsu³¹
bool CapsuleToCapsule(
	const VEC3 &Capsule1_Beg, const VEC3 &Capsule1_End, float Capsule1_Radius,
	const VEC3 &Capsule2_Beg, const VEC3 &Capsule2_End, float Capsule2_Radius);
/// Sprawdza, czy punkt le¿y wewn¹trz wypuk³ego wielok¹ta 2D.
/** Wielok¹t musi mieæ wierzcho³ki zorientowane CW. */
bool PointInConvexPolygon(const VEC2 &Point, const VEC2 PolygonPoints[], size_t PolygonPointCount);
bool PointInConvexPolygon(const VEC2 &Point, const void *PolygonPoints, size_t PolygonPointCount, int PolygonPointStride);
/// Zwraca true, jeœli podany punkt le¿y wewn¹trz podanego wielok¹ta (2D).
/** Wielok¹t mo¿e mieæ dowolny kszta³t, tak¿e niewypuk³y, a nawet sam siê przecinaæ.
Jego wierzcho³ki mog¹ biec w dowolnym kierunku. */
bool PointInPolygon(const VEC2 &Point, const VEC2 PolygonPoints[], size_t PolygonPointCount);
bool PointInPolygon(const VEC2 &Point, const void *PolygonPoints, size_t PolygonPointCount, int PolygonPointStride);
/// Zwraca true, jeœli promieñ przecina wielok¹t wypuk³y 3D i wtedy przez OutT zwraca parametr dla promienia.
/** Wierzcho³ki wielok¹ta musz¹ le¿eæ w jednej p³aszczyŸnie.
Jeœli kolizja jest od ty³u pocz¹tku promienia, zwraca true i OutT ujemne.
Uwaga! Stosuje rzutowanie na jedn¹ z g³ównych p³aszczyzn, testowana tylko dla jednej z trzech mo¿liwych. BackfaceCulling mo¿e dzia³aæ Ÿle.
\param BackfaceCulling true oznacza, ¿e promieñ trafia tylko jeœli wchodzi z jednej strony,
z tej z której wierzcho³ki wielok¹ta s¹ widoczne jako zorientowane w kierunku CW.
\param PolygonPlane Opcjonalny. Podaj, jeœli masz ju¿ wyliczony. */
bool RayToConvexPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 PolygonPoints[], size_t PolygonPointCount,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane = NULL);
bool RayToConvexPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const void *PolygonPoints, size_t PolygonPointCount, int PolygonPointStride,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane = NULL);
/// Zwraca true, jeœli promieñ przecina wielok¹t 3D i wtedy przez OutT zwraca parametr dla promienia.
/** Wierzcho³ki wielok¹ta musz¹ le¿eæ w jednej p³aszczyŸnie.
Wielok¹t mo¿e byæ dowolny, nie musi byæ wypuk³y.
Uwaga! Stosuje rzutowanie na jedn¹ z g³ównych p³aszczyzn, testowana tylko dla jednej z trzech mo¿liwych. BackfaceCulling mo¿e dzia³aæ Ÿle.
\param BackfaceCulling true oznacza, ¿e promieñ trafia tylko jeœli wchodzi z jednej strony,
z tej z której wierzcho³ki wielok¹ta s¹ widoczne jako zorientowane w kierunku CW.
\param PolygonPlane Opcjonalny. Podaj, jeœli masz ju¿ wyliczony. */
bool RayToPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 PolygonPoints[], size_t PolygonPointCount,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane = NULL);
bool RayToPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const void *PolygonPoints, size_t PolygonPointCount, int PolygonPointStride,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane = NULL);

/// Liczy kolizjê poruszaj¹cej siê sfery z p³aszczyzn¹
/**
- P³aszczyzna musi byæ znormalizowana.
- Jako OutT0 i OutT1 mo¿na podawaæ NULL, jeœli nas akurat nie interesuje.
- Jeœli sfera porusza siê w drug¹ stronê, te¿ wychodzi true i T wychodz¹ ujemne.
- Jeœli sfera koliduje z p³aszczyzn¹ i porusza siê w kierunku do niej równoleg³ym, jest T0=0, T1=MAXFLOAT i return true.
*/
bool SweptSphereToPlane(const VEC3 &SphereCenter, float SphereRadius, const VEC3 &SphereSweepDir, const PLANE &Plane, float *OutT0, float *OutT1);
/// Oblicza kolizjê poruszaj¹cej siê sfery z frustumem
/**
- Frustum musi byæ znormalizowany.
- Jeœli sfera le¿y w œrodku, te¿ oczywiœcie zwraca true.
*/
bool SweptSphereToFrustum(const VEC3 &SphereCenter, float SphereRadius, const VEC3 &SphereSweepDir, const FRUSTUM_PLANES &Frustum);
/// Analityczna kolizja dwóch poruszaj¹cych siê sfer.
/**
- Jeœli nie ma kolizji, zwraca false. T1 i T2 niezdefiniowane.
- Jeœli jest kolizja, zwraca true. Wówczas przez T1 i T2 zwraca czas pocz¹tku i koñca kolizji wyra¿ony we wielokrotnoœciach d³ugoœci SphereSweepDir1 (T1 <= T2).
- Jeœli kolizja nastêpuje z ty³u, te¿ zwraca true, a T1 i/lub T2 bêd¹ ujemne.

\param SphereSweepDir1    Kierunek ruchu sfery 1 w stosunku do sfery 2. Nie musi byæ znormalizowany.
*/
bool SweptSphereToSphere(
	const VEC3 &SphereCenter1, float SphereRadius1,
	const VEC3 &SphereCenter2, float SphereRadius2,
	const VEC3 &SphereSweepDir1,
	float *OutT1, float *OutT2);
/// Kolizja poruszaj¹cego siê AABB z drugim AABB.
/**
- Jeœli nie zachodzi, zwraca false.
- Jeœli zachodzi, zwraca true i T jako czas pocz¹tkowy kontaktu.
- Jeœli zachodzi z ty³u, zwraca true i T ujemne.
- Jeœli zachodzi w chwili pocz¹tkowej, zwraca true i T=0.

\param SweepDirBox1    Nie musi byæ znormalizowany. T bêdzie wyra¿one we wielokrotnoœciach jego d³ugoœci.
*/
bool SweptBoxToBox(const BOX &Box1, const BOX &Box2, const VEC3 &SweepDirBox1, float *OutT1, float *OutT2);

/** \param rayDir Must be normalized.
\param coneDirVec Must be normalized.
\param coneHalfAngle Must be less than PI_2.
\return Number of intersections. */
int RayToCone(const VEC3 &rayOrig, const VEC3 & rayDir, const VEC3 &coneVertex, const VEC3 &coneDirVec, float coneHalfAngle, VEC3 intersections[2]);
bool PointInCone(const VEC3 &pt, const VEC3 &coneVertex, const VEC3 &coneDirVec, float coneHalfAngle);
bool SegmentIntersectsOrIsContainedInCone(const VEC3 & segmentPoint1, const VEC3 & segmentPoint2, const VEC3 & coneVertex, const VEC3 & coneDirVec, const float coneHalfAngle, VEC3 intersections[2], int &numIntersectionsFound);
bool SegmentIntersectsOrIsContainedInCone(const VEC3 & segmentPoint1, const VEC3 & segmentPoint2, const VEC3 & coneVertex, const VEC3 & coneDirVec, const float coneHalfAngle);

//@}
// math_collisions

/// Z podanej tablicy punktów typu VEC2 znajduje punkt najbli¿szy wzglêdem p, zwraca jego indeks.
/** \param OutDistance jest opcjonalny, mo¿na podaæ NULL Jeœli odleg³oœæ od niego nas nie interesuje. */
size_t ClosestPoint(const VEC2 &p, const VEC2 Points[], size_t PointCount, float *OutDistance = NULL);
size_t ClosestPoint(const VEC2 &p, const void *Data, size_t PointCount, int Stride, float *OutDistance = NULL);
/// Z podanej tablicy punktów typu VEC3 znajduje punkt najbli¿szy wzglêdem p, zwraca jego indeks.
/** \param OutDistance jest opcjonalny, mo¿na podaæ NULL Jeœli odleg³oœæ od niego nas nie interesuje. */
size_t ClosestPoint(const VEC3 &p, const VEC3 Points[], size_t PointCount, float *OutDistance = NULL);
size_t ClosestPoint(const VEC3 &p, const void *Data, size_t PointCount, int Stride, float *OutDistance = NULL);
/// Z podanej tablicy punktów typu VEC2 znajduje punkt najdalszy wzglêdem p, zwraca jego indeks.
/** \param OutDistance jest opcjonalny, mo¿na podaæ NULL Jeœli odleg³oœæ od niego nas nie interesuje. */
size_t FurthestPoint(const VEC2 &p, const VEC2 Points[], size_t PointCount, float *OutDistance = NULL);
size_t FurthestPoint(const VEC2 &p, const void *Data, size_t PointCount, int Stride, float *OutDistance = NULL);
/// Z podanej tablicy punktów typu VEC3 znajduje punkt najdalszy wzglêdem p, zwraca jego indeks.
/** \param OutDistance jest opcjonalny, mo¿na podaæ NULL Jeœli odleg³oœæ od niego nas nie interesuje. */
size_t FurthestPoint(const VEC3 &p, const VEC3 Points[], size_t PointCount, float *OutDistance = NULL);
size_t FurthestPoint(const VEC3 &p, const void *Data, size_t PointCount, int Stride, float *OutDistance = NULL);
/// Z podanej tablicy punktów typu VEC2 znajduje najbardziej wysuniêty w kierunku okreœlonym przez Dir. Zwraca jego indeks.
size_t FurthestPointForDir(const VEC2 &Dir, const VEC2 Points[], size_t PointCount);
size_t FurthestPointForDir(const VEC2 &Dir, const void *Data, size_t PointCount, int Stride);
/// Z podanej tablicy punktów typu VEC3 znajduje najbardziej wysuniêty w kierunku okreœlonym przez Dir. Zwraca jego indeks.
size_t FurthestPointForDir(const VEC3 &Dir, const VEC3 Points[], size_t PointCount);
size_t FurthestPointForDir(const VEC3 &Dir, const void *Data, size_t PointCount, int Stride);

/// Tworzy najmniejszy prostok¹t otaczaj¹cy podany zbiór punktów typu VEC2.
void RectBoundingPoints(RECTF *OutRect, const VEC2 Points[], size_t PointCount);
void RectBoundingPoints(RECTF *OutRect, const void *Data, size_t PointCount, int Stride);
/// Tworzy najmniejszy boks otaczaj¹cy podany zbiór punktów typu VEC3.
void BoxBoundingPoints(BOX *box, const VEC3 points[], size_t PointCount);
void BoxBoundingPoints(BOX *box, const void *Data, size_t PointCount, int Stride);
/// Liczy sferê otaczaj¹c¹ dwie sfery
void SphereBoundingSpheres(VEC3 *OutCenter, float *OutRadius, const VEC3 &Center1, float Radius1, const VEC3 &Center2, float Radius2);

/// Znajduje okr¹g otaczaj¹cy podany zbiór punktów typu VEC2.
/** Nie jest to mo¿e najmniejszy okr¹g, ale przynajmniej ten algorytm jest prosty i szybki. */
void CircleBoundingPoints(VEC2 *OutSphereCenter, float *OutSphereRadius, const VEC2 Points[], size_t PointCount);
void CircleBoundingPoints(VEC2 *OutSphereCenter, float *OutSphereRadius, const void *PointData, size_t PointCount, int PointStride);
/// Znajduje sferê otaczaj¹c¹ podany zbiór punktów typu VEC3.
/** Nie jest to mo¿e najmniejsza sfera, ale przynajmniej ten algorytm jest prosty i szybki. */
void SphereBoundingPoints(VEC3 *OutSphereCenter, float *OutSphereRadius, const VEC3 Points[], size_t PointCount);
void SphereBoundingPoints(VEC3 *OutSphereCenter, float *OutSphereRadius, const void *PointData, size_t PointCount, int PointStride);

/// Znajduje promieñ najmniejszego okrêgu o œrodku (0,0) otaczaj¹cego podany zbiór punktów 2D.
void OriginCircleBoundingPoints(float *OutRadius, const VEC2 Points[], size_t PointCount);
void OriginCircleBoundingPoints(float *OutRadius, const void *PointData, size_t PointCount, int PointStride);
/// Znajduje promieñ najmniejszej sfery o œrodku (0,0) otaczaj¹cej podany zbiór punktów 3D.
void OriginSphereBoundingPoints(float *OutRadius, const VEC3 Points[], size_t PointCount);
void OriginSphereBoundingPoints(float *OutRadius, const void *PointData, size_t PointCount, int PointStride);

/// Dla podanego zbioru punktów typu VEC2 oblicza ich centroid, czyli po prostu œredni¹.
void CalcCentroid(VEC2 *OutCentroid, const VEC2 Points[], size_t PointCount);
void CalcCentroid(VEC2 *OutCentroid, const void *PointData, size_t PointCount, int PointStride);
/// Dla podanego zbioru punktów typu VEC3 oblicza ich centroid, czyli po prostu œredni¹.
void CalcCentroid(VEC3 *OutCentroid, const VEC3 Points[], size_t PointCount);
void CalcCentroid(VEC3 *OutCentroid, const void *PointData, size_t PointCount, int PointStride);

/// Oblicza macierz kowariancji dla podanego zbioru punktów typu VEC3.
void CalcCovarianceMatrix(MATRIX33 *OutCov, const VEC3 Points[], size_t PointCount);
void CalcCovarianceMatrix(MATRIX33 *OutCov, const void *PointData, size_t PointCount, int PointStride);
/// Oblicza wartoœci w³asne i wektory w³asne dla podanej macierzy symetrycznej (np. macierz kowariancji).
/** Uwaga! Du¿o obliczeñ, procedura iteracyjna. Mo¿e dzia³aæ wolno.
\param InCovOutEigenvalues [in,out] Jako wejœcie - dana macierz. Jako wyjœcie - na przek¹tnej ma wartoœci w³asne.
\param OutEigenvectors [out] Wektory w³asne zapisane w kolumnach. */
void CalcEigenvaluesAndEigenvectors(MATRIX33 *InCovOutEigenvalues, MATRIX33 *OutEigenvectors);

/// Oblicza sferê otaczaj¹c¹ box
void BoxBoundingSphere(VEC3 *SphereCenter, float *SphereRadius, const BOX &Box);
/// Oblicza box otaczaj¹cy sferê
void SphereBoundingBox(BOX *Out, const VEC3 &SphereCenter, float SphereRadius);
/// Wylicza AABB otaczaj¹cy podany zbiór sfer
/** \param SphereCount   Musi byæ wiêksze od 0. */
void BoxBoundingSpheres(BOX *OutBox, const VEC3 SpheresCenter[], const float SpheresRadius[], size_t SphereCount);

/// Losuje punkt wewn¹trz kuli o œrodku w (0,0,0) i promieniu 1. Rozk³ad równomierny.
void RandomPointInUnitSphere(VEC3 *Out, RandomGenerator &Rand);
void RandomPointInUnitSphere(VEC3 *Out);
/// Losuje punkt wewn¹trz ko³a o œrodku w (0,0) i promieniu 1. Rozk³ad równomierny.
void RandomPointInUnitCircle(VEC2 *Out, RandomGenerator &Rand);
void RandomPointInUnitCircle(VEC2 *Out);

/// Calculates orientation of a 3D billboard in according to its parameters and camera parameters.
/** OutRight, OutUp will be normalized vectors with X, Y directions for the billboard. \n
DegreesOfFreedom and UseRealDir control billboard orientation mode.
DegreesOfFreedom may be 0, 1, 2. \n
0 = billboard not rotating at all (DefinedRight and DefinedUp required). \n
1 = billboard rotating only around its Y axis (DefinedUp required). \n
2 = billboard rotating around its Y and X axes (neither DefinedRight nor DefinedUp required). \n
DefinedRight and DefinedUp are original direction vectors of a billboard. \n
CamRightDir must be orthogonal to CamReadUpDir, so CamRealUpDir cannot be constant like VEC3_POSITIVE_Y. \n
CameraToObjectDir must be normalize(BillboardCenterPos - EyePos). */
void CalcBillboardOrientation(
	VEC3 *OutRight, VEC3 *OutUp,
	unsigned DegreesOfFreedom, bool UseRealDir,
	const VEC3 &DefinedRight, const VEC3 &DefinedUp,
	const VEC3 &CamRightDir, const VEC3 &CamRealUpDir, const VEC3 &CameraToObjectDir);

/// Klasa, która dziêki pamiêtaniu wyników poœrednich przyspiesza testowanie kolizji jednego trójk¹ta z wieloma promieniami.
/** Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson */
class RayToTriangleCalc
{
public:
	/// Tworzy NIEZAINICJALIZOWANY
	RayToTriangleCalc() { }
	/// Tworzy i inicjalizuje
	RayToTriangleCalc(const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC) { Init(TriA, TriB, TriC); }
	/// Inicjalizuje
	void Init(const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC);

	/// Liczy kolizjê podanego promienia z wpisanym do obiektu trójk¹tem.
	/** Jeœli kolizja jest od ty³u promienia, równie¿ zwraca true i OutT ujemne.
	\param BackfaceCulling Jeœli true, uwzglêdnia kolizjê tylko od strony z której wierzcho³ki trójk¹ta s¹ CW.
	\param OutT [out] Parametr do promienia.
	\param OutU [out] Wspó³rzêdna barycentryczna.
	\param OutV [out] Wspó³rzêdna barycentryczna.
	\param OutW [out] Wspó³rzêdna barycentryczna.
	\param OutPoint [out] Punkt kolizji. */
	bool RayToTriangle(
		const VEC3 &RayOrig, const VEC3 &RayDir, bool BackfaceCulling,
		float *OutT, float *OutU, float *OutV, float *OutW, VEC3 *OutPoint) const;

private:
	PLANE m_Plane;
	PLANE m_EdgePlaneBC;
	PLANE m_EdgePlaneCA;
};


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
/** \addtogroup math_poisson_disc Poisson Disc */
//@{

extern const float POISSON_DISC_1D[];
const uint32 POISSON_DISC_1D_COUNT = 100;

extern const VEC2 POISSON_DISC_2D[];
const uint32 POISSON_DISC_2D_COUNT = 100;

extern const VEC3 POISSON_DISC_3D[];
const uint32 POISSON_DISC_3D_COUNT = 100;

//@}
// math_poisson_disc

//@}
// code_math

} // namespace common


/** \addtogroup code_math */
//@{
/** \addtogroup math_swizzle Vector swizzling
Here are macros for convinient building vectors from arbitrary components of
other vector, just like in HLSL and other shader languages.

For example, in HLSL you can extend float2 to float4 with repeating (x,y)
components two times with code like this:

\verbatim
float2 v2 = float2(1, 2);
float4 v4 = v2.xyxy;
\endverbatim

In C++ with CommonLib, you can do it like this:

\code
common::VEC2 v2 = common::VEC2(1.f, 2.f);
common::VEC4 v4 = common::VEC4(v2.x, v2.y, v2.x, v2.y);
\endcode

But the more convenient way is to use VEC4_SWIZZLE macro:

\code
common::VEC2 v2 = common::VEC2(1.f, 2.f);
common::VEC4 v4 = VEC4_SWIZZLE(v2,x,y,x,y);
\endcode

Source vector can be of any size (VEC2, VEC3, VEC4), as long as you reference
only existing components (x, y, z, w).

Type of destination vector is determined by using particular macro (e.g.
VEC3_SWIZZLE returns VEC3).

You can use expression of any type as source value, as long as it can be indexed
with dot operator. E.g.:

\code
MATRIX m = ...;
VEC4 mDiagonal = VEC4_SWIZZLE(m,_11,_22,_33,_44);
BOX box = ...;
VEC2 boxRangeX = VEC2_SWIZZLE(box,Min.x,Max.x);
\endcode
*/
//@{

#define VEC2_SWIZZLE(v, c0, c1)         (common::VEC2((v).c0, (v).c1))
#define VEC3_SWIZZLE(v, c0, c1, c2)     (common::VEC3((v).c0, (v).c1, (v).c2))
#define VEC4_SWIZZLE(v, c0, c1, c2, c3) (common::VEC4((v).c0, (v).c1, (v).c2, (v).c3))

//@}
// math_swizzle
//@}
// code_math


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Konwersje

/** \addtogroup code_sthtostr */
//@{

template <>
struct SthToStr_obj<common::POINT_>
{
	void operator () (tstring *Str, const common::POINT_ &Sth)
	{
		*Str = common::Format(_T("#,#")) % Sth.x % Sth.y;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::POINT_>
{
	bool operator () (common::POINT_ *Sth, const tstring &Str)
	{
		size_t CommaPos = Str.find(_T(','));
		if (CommaPos == tstring::npos) return false;
		if (!StrToSth<int>(&Sth->x, Str.substr(0, CommaPos))) return false;
		if (!StrToSth<int>(&Sth->y, Str.substr(CommaPos+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::VEC2>
{
	void operator () (tstring *Str, const common::VEC2 &Sth)
	{
		*Str = common::Format(_T("#,#")) % Sth.x % Sth.y;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::VEC2>
{
	bool operator () (common::VEC2 *Sth, const tstring &Str)
	{
		size_t CommaPos = Str.find(_T(','));
		if (CommaPos == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->x, Str.substr(0, CommaPos))) return false;
		if (!StrToSth<float>(&Sth->y, Str.substr(CommaPos+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::VEC3>
{
	void operator () (tstring *Str, const common::VEC3 &Sth)
	{
		*Str = common::Format(_T("#,#,#")) % Sth.x % Sth.y % Sth.z;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::VEC3>
{
	bool operator () (common::VEC3 *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->x, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Sth->y, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Sth->z, Str.substr(CommaPos2+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::VEC4>
{
	void operator () (tstring *Str, const common::VEC4 &Sth)
	{
		*Str = common::Format(_T("#,#,#,#")) % Sth.x % Sth.y % Sth.z % Sth.w;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::VEC4>
{
	bool operator () (common::VEC4 *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		size_t CommaPos3 = Str.find(_T(','), CommaPos2+1);
		if (CommaPos3 == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->x, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Sth->y, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Sth->z, Str.substr(CommaPos2+1, CommaPos3-(CommaPos2+1)))) return false;
		if (!StrToSth<float>(&Sth->w, Str.substr(CommaPos3+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::RECTI>
{
	void operator () (tstring *Str, const common::RECTI &Sth)
	{
		*Str = common::Format(_T("#,#,#,#")) % Sth.Min.x % Sth.Min.y % Sth.Max.x % Sth.Max.y;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::RECTI>
{
	bool operator () (common::RECTI *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		size_t CommaPos3 = Str.find(_T(','), CommaPos2+1);
		if (CommaPos3 == tstring::npos) return false;
		if (!StrToSth<int>(&Sth->Min.x, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<int>(&Sth->Min.y, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<int>(&Sth->Max.x, Str.substr(CommaPos2+1, CommaPos3-(CommaPos2+1)))) return false;
		if (!StrToSth<int>(&Sth->Max.y, Str.substr(CommaPos3+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::RECTF>
{
	void operator () (tstring *Str, const common::RECTF &Sth)
	{
		*Str = common::Format(_T("#,#,#,#")) % Sth.Min.x % Sth.Min.y % Sth.Max.x % Sth.Max.y;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::RECTF>
{
	bool operator () (common::RECTF *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		size_t CommaPos3 = Str.find(_T(','), CommaPos2+1);
		if (CommaPos3 == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->Min.x, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Sth->Min.y, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Sth->Max.x, Str.substr(CommaPos2+1, CommaPos3-(CommaPos2+1)))) return false;
		if (!StrToSth<float>(&Sth->Max.y, Str.substr(CommaPos3+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::BOX>
{
	void operator () (tstring *Str, const common::BOX &Sth)
	{
		*Str = common::Format(_T("#;#")) % Sth.Min % Sth.Max;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::BOX>
{
	bool operator () (common::BOX *Sth, const tstring &Str)
	{
		size_t SemicolonPos = Str.find(_T(';'));
		if (SemicolonPos == tstring::npos) return false;
		if (!StrToSth<common::VEC3>(&Sth->Min, Str.substr(0, SemicolonPos))) return false;
		if (!StrToSth<common::VEC3>(&Sth->Max, Str.substr(SemicolonPos+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <> struct SthToStr_obj<common::COLOR> {
	void operator () (tstring *Str, const common::COLOR &Sth) { common::ColorToStr(Str, Sth); }
	static inline bool IsSupported() { return true; }
};
template <> struct StrToSth_obj<common::COLOR> {
	bool operator () (common::COLOR *Sth, const tstring &Str) { return common::StrToColor(Sth, Str); }
	static inline bool IsSupported() { return true; }
};

template <> struct SthToStr_obj<common::COLORF> {
	void operator () (tstring *Str, const common::COLORF &Sth) { common::ColorToStr(Str, Sth); }
	static inline bool IsSupported() { return true; }
};
template <> struct StrToSth_obj<common::COLORF> {
	bool operator () (common::COLORF *Sth, const tstring &Str) { return common::StrToColor(Sth, Str); }
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::PLANE>
{
	void operator () (tstring *Str, const common::PLANE &Sth)
	{
		*Str = common::Format(_T("#,#,#,#")) % Sth.a % Sth.b % Sth.c % Sth.d;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::PLANE>
{
	bool operator () (common::PLANE *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		size_t CommaPos3 = Str.find(_T(','), CommaPos2+1);
		if (CommaPos3 == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->a, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Sth->b, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Sth->c, Str.substr(CommaPos2+1, CommaPos3-(CommaPos2+1)))) return false;
		if (!StrToSth<float>(&Sth->d, Str.substr(CommaPos3+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::LINE2D>
{
	void operator () (tstring *Str, const common::LINE2D &Sth)
	{
		*Str = common::Format(_T("#,#,#")) % Sth.a % Sth.b % Sth.c;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::LINE2D>
{
	bool operator () (common::LINE2D *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->a, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Sth->b, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Sth->c, Str.substr(CommaPos2+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <> struct SthToStr_obj<common::MATRIX> {
	void operator () (tstring *Str, const common::MATRIX &Sth) { common::MatrixToStr(Str, Sth); }
	static inline bool IsSupported() { return true; }
};
template <> struct StrToSth_obj<common::MATRIX> {
	bool operator () (common::MATRIX *Sth, const tstring &Str) { return common::StrToMatrix(Sth, Str); }
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::QUATERNION>
{
	void operator () (tstring *Str, const common::QUATERNION &Sth)
	{
		*Str = common::Format(_T("#,#,#,#")) % Sth.x % Sth.y % Sth.z % Sth.w;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::QUATERNION>
{
	bool operator () (common::QUATERNION *Sth, const tstring &Str)
	{
		size_t CommaPos1 = Str.find(_T(','));
		if (CommaPos1 == tstring::npos) return false;
		size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
		if (CommaPos2 == tstring::npos) return false;
		size_t CommaPos3 = Str.find(_T(','), CommaPos2+1);
		if (CommaPos3 == tstring::npos) return false;
		if (!StrToSth<float>(&Sth->x, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Sth->y, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Sth->z, Str.substr(CommaPos2+1, CommaPos3-(CommaPos2+1)))) return false;
		if (!StrToSth<float>(&Sth->w, Str.substr(CommaPos3+1))) return false;
		return true;
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct SthToStr_obj<common::AFFINE2D>
{
	void operator () (tstring *Str, const common::AFFINE2D &Sth)
	{
		common::Affine2dToStr(Str, Sth);
	}
	static inline bool IsSupported() { return true; }
};

template <>
struct StrToSth_obj<common::AFFINE2D>
{
	bool operator () (common::AFFINE2D *Sth, const tstring &Str)
	{
		return common::StrToAffine2d(Sth, Str);
	}
	static inline bool IsSupported() { return true; }
};

//@}
// code_sthtostr
#endif
