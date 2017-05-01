/** \file
\brief Mathematical module
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Math \n
Module components: \ref code_math
*/
#include "Base.hpp"
#include <climits>
#include <cfloat>
#include "Math.hpp"


namespace common
{

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// struct POINT_

const POINT_ POINT_ZERO       = POINT_( 0,  0);
const POINT_ POINT_ONE        = POINT_( 1,  1);
const POINT_ POINT_NEGATIVE_X = POINT_(-1,  0);
const POINT_ POINT_NEGATIVE_Y = POINT_( 0, -1);
const POINT_ POINT_POSITIVE_X = POINT_( 1,  0);
const POINT_ POINT_POSITIVE_Y = POINT_( 0,  1);


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// WEKTOR 2D, 3D i 4D Z FLOATÓW

const VEC2 VEC2_ZERO = VEC2(0.0f, 0.0f);
const VEC2 VEC2_ONE  = VEC2(1.0f, 1.0f);
const VEC2 VEC2_POSITIVE_X = VEC2(+1.0f, 0.0f);
const VEC2 VEC2_POSITIVE_Y = VEC2(0.0f, +1.0f);
const VEC2 VEC2_NEGATIVE_X = VEC2(-1.0f, 0.0f);
const VEC2 VEC2_NEGATIVE_Y = VEC2(0.0f, -1.0f);

const VEC3 VEC3_ZERO = VEC3(0.0f, 0.0f, 0.0f);
const VEC3 VEC3_ONE  = VEC3(1.0f, 1.0f, 1.0f);
const VEC3 VEC3_POSITIVE_X = VEC3(+1.0f, 0.0f, 0.0f);
const VEC3 VEC3_POSITIVE_Y = VEC3(0.0f, +1.0f, 0.0f);
const VEC3 VEC3_POSITIVE_Z = VEC3(0.0f, 0.0f, +1.0f);
const VEC3 VEC3_NEGATIVE_X = VEC3(-1.0f, 0.0f, 0.0f);
const VEC3 VEC3_NEGATIVE_Y = VEC3(0.0f, -1.0f, 0.0f);
const VEC3 VEC3_NEGATIVE_Z = VEC3(0.0f, 0.0f, -1.0f);

const VEC4 VEC4_ZERO = VEC4(0.0f, 0.0f, 0.0f, 0.0f);
const VEC4 VEC4_ONE  = VEC4(1.0f, 1.0f, 1.0f, 1.0f);
const VEC4 VEC4_POSITIVE_X = VEC4(+1.0f, 0.0f, 0.0f, 0.0f);
const VEC4 VEC4_POSITIVE_Y = VEC4(0.0f, +1.0f, 0.0f, 0.0f);
const VEC4 VEC4_POSITIVE_Z = VEC4(0.0f, 0.0f, +1.0f, 0.0f);
const VEC4 VEC4_POSITIVE_W = VEC4(0.0f, 0.0f, 0.0f, +1.0f);
const VEC4 VEC4_NEGATIVE_X = VEC4(-1.0f, 0.0f, 0.0f, 0.0f);
const VEC4 VEC4_NEGATIVE_Y = VEC4(0.0f, -1.0f, 0.0f, 0.0f);
const VEC4 VEC4_NEGATIVE_Z = VEC4(0.0f, 0.0f, -1.0f, 0.0f);
const VEC4 VEC4_NEGATIVE_W = VEC4(0.0f, 0.0f, 0.0f, -1.0f);


/*
Algorytmy ortogonalizacji wektorów to wzór Gram-Schmidt na podstwie ksi¹¿ki "3D
Math Primer for Graphics and Game Development", str. 134.
*/

void Orthogonalize_Fast(VEC3 *OutR1, VEC3 *OutR2, VEC3 *OutR3, const VEC3 &R1, const VEC3 &R2, const VEC3 &R3)
{
	*OutR1 = R1;
	*OutR2 = R2 - ( Dot(R2, *OutR1) / Dot(*OutR1, *OutR1) ) * (*OutR1);
	*OutR3 = R3 - ( Dot(R3, *OutR1) / Dot(*OutR1, *OutR1) ) * (*OutR1) - ( Dot(R3, *OutR2) / Dot(*OutR2, *OutR2) ) * (*OutR2);
}

void Orthogonalize(VEC3 *OutR1, VEC3 *OutR2, VEC3 *OutR3, const VEC3 &R1, const VEC3 &R2, const VEC3 &R3)
{
	const float K = 0.25f;
	const uint C = 10;

	VEC3 src1 = R1, src2 = R2, src3 = R3;
	VEC3 dst1, dst2, dst3;
	VEC3 *psrc1 = &src1, *psrc2 = &src2, *psrc3 = &src3;
	VEC3 *pdst1 = &dst1, *pdst2 = &dst2, *pdst3 = &dst3;

	for (uint i = 0; i < C; i++)
	{
		*pdst1 = (*psrc1) - K * ( Dot(*psrc1, *psrc2) / Dot(*psrc2, *psrc2) ) * (*psrc2) - K * ( Dot(*psrc1, *psrc3) / Dot(*psrc3, *psrc3) ) * (*psrc3);
		*pdst2 = (*psrc2) - K * ( Dot(*psrc2, *psrc1) / Dot(*psrc1, *psrc1) ) * (*psrc1) - K * ( Dot(*psrc2, *psrc3) / Dot(*psrc3, *psrc3) ) * (*psrc3);
		*pdst3 = (*psrc3) - K * ( Dot(*psrc3, *psrc1) / Dot(*psrc1, *psrc1) ) * (*psrc1) - K * ( Dot(*psrc3, *psrc2) / Dot(*psrc2, *psrc2) ) * (*psrc2);

		std::swap(psrc1, pdst1);
		std::swap(psrc2, pdst2);
		std::swap(psrc3, pdst3);
	}

	// Na koniec ¿eby siê upewniæ ¿e s¹ doskonale prostopad³e
	Orthogonalize_Fast(OutR1, OutR2, OutR3, *psrc1, *psrc2, *psrc3);
}

void PerpedicularVectors(VEC3 *Out1, VEC3 *Out2, const VEC3 &v)
{
	if (FLOAT_ALMOST_ZERO(v.x) && FLOAT_ALMOST_ZERO(v.y))
		Cross(Out1, v, VEC3_POSITIVE_Y);
	else
		Cross(Out1, v, VEC3_POSITIVE_Z);
	Cross(Out2, *Out1, v);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// TRÓJK¥T

float TriangleArea(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3)
{
	return 0.5f * ( (p1.y-p3.y)*(p2.x-p3.x) + (p2.y-p3.y)*(p3.x-p1.x) );
}

float TriangleArea(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3)
{
	VEC3 cr; Cross(&cr, p3-p2, p1-p3);
	return 0.5f * Length(cr);
}

bool TriangleIsCW(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3)
{
	return (p1.y-p3.y)*(p2.x-p3.x) + (p2.y-p3.y)*(p3.x-p1.x) >= 0.0f;
}

float TrianglePerimeter(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3)
{
	return Distance(p1, p2) + Distance(p2, p3) + Distance(p3, p1);
}

float TrianglePerimeter(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3)
{
	return Distance(p1, p2) + Distance(p2, p3) + Distance(p3, p1);
}

void CalcBarycentric(const VEC2 &p, const VEC2 &p1, const VEC2 &p2, const VEC2 &p3, float *out_e, float *out_f, float *out_g)
{
	// Na podstawie ksi¹¿ki:
	// Real-Time Collision Detection, Christer Ericson
	VEC2 v0 = p2 - p1;
	VEC2 v1 = p3 - p1;
	VEC2 v2 = p  - p1;
	float d00 = Dot(v0, v0);
	float d01 = Dot(v0, v1);
	float d11 = Dot(v1, v1);
	float d20 = Dot(v2, v0);
	float d21 = Dot(v2, v1);
	float denom_rcp = 1.0f / (d00 * d11 - d01 * d01);
	*out_f = (d11 * d20 - d01 * d21) * denom_rcp;
	*out_g = (d00 * d21 - d01 * d20) * denom_rcp;
	if (out_e) *out_e = 1.0f - *out_f - *out_g;
}

/*
Ta wersja jest mniej optymalna, ni¿ ta poniej z rzutowaniem.

void CalcBarycentric(const VEC3 &p, const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, float *out_e, float *out_f, float *out_g)
{
	// Na podstawie ksi¹¿ki:
	// Real-Time Collision Detection, Christer Ericson
	VEC3 v0 = p2 - p1;
	VEC3 v1 = p3 - p1;
	VEC3 v2 = p  - p1;
	float d00 = Dot(v0, v0);
	float d01 = Dot(v0, v1);
	float d11 = Dot(v1, v1);
	float d20 = Dot(v2, v0);
	float d21 = Dot(v2, v1);
	float denom_rcp = 1.0f / (d00 * d11 - d01 * d01);
	*out_f = (d11 * d20 - d01 * d21) * denom_rcp;
	*out_g = (d00 * d21 - d01 * d20) * denom_rcp;
	if (out_e) *out_e = 1.0f - *out_f - *out_g;
}
*/

void CalcBarycentric(const VEC3 &p, const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, const VEC3 *Normal, float *out_e, float *out_f, float *out_g)
{
	// Na podstawie ksia¿ki "3D Math Primer for Graphics and Game Development", str. 263.

	// First, compute two clockwise edge vectors
	VEC3 d1 = p2-p1;
	VEC3 d2 = p3-p2;

	// Compute surface normal using cross product. In many cases
	// this step could be skipped, since we would have the surface
	// normal precomputed. We do not need to normalize it, although
	// if a precomputed normal was normalized, it would be OK.
	VEC3 n;
	if (Normal)
		n = *Normal;
	else
		Cross(&n, d1, d2);

	// Locate dominant axis of normal, and select plane of projection
	float u1, u2, u3, u4;
	float v1, v2, v3, v4;
	float abs_nx = fabsf(n.x), abs_ny = fabsf(n.y), abs_nz = fabsf(n.z);
	if ((abs_nx >= abs_ny) && (abs_nx >= abs_nz))
	{
		// Discard x, project onto yz plane
		u1 = p1.y-p3.y;
		u2 = p2.y-p3.y;
		u3 = p.y-p1.y;
		u4 = p.y-p3.y;

		v1 = p1.z-p3.z;
		v2 = p2.z-p3.z;
		v3 = p.z-p1.z;
		v4 = p.z-p3.z;
	}
	else if (abs_ny >= abs_nz)
	{
		// Discard y, project onto xz plane
		u1 = p1.z-p3.z;
		u2 = p2.z-p3.z;
		u3 = p.z-p1.z;
		u4 = p.z-p3.z;

		v1 = p1.x-p3.x;
		v2 = p2.x-p3.x;
		v3 = p.x-p1.x;
		v4 = p.x-p3.x;
	}
	else
	{
		u1 = p1.x-p3.x;
		u2 = p2.x-p3.x;
		u3 = p.x-p1.x;
		u4 = p.x-p3.x;

		v1 = p1.y-p3.y;
		v2 = p2.y-p3.y;
		v3 = p.y-p1.y;
		v4 = p.y-p3.y;
	}

	// Compute denominator
	float OneOverDenom = 1.0f / (v1*u2 - v2*u1);
	// Compute barycentric coordinates
	*out_e = (v4*u2 - v2*u4) * OneOverDenom;
	*out_f = (v1*u3 - v3*u1) * OneOverDenom;
	*out_g = 1.0f - *out_e - *out_f;
}

/*
Funkcje TriangleInscribedCircle i TriangleCircumscribedCircle s¹ napisane na
podstawie nVidia SDK, modu³ nv_algebra.
*/

void TriangleInscribedCircle(VEC2 *OutCenter, float *OutRadius, const VEC2 &v1, const VEC2 &v2, const VEC2 &v3)
{
	float area = fabsf(TriangleArea(v1, v2, v3));
	if (FLOAT_ALMOST_ZERO(area))
	{
		*OutCenter = v1;
		*OutRadius = 0.f;
		return;
	}

	float oo_perim = 1.f / TrianglePerimeter(v1, v2, v3);

	VEC2 diff;

	diff = v2 - v3;
	*OutCenter = v1 * Length(diff);

	diff = v3 - v1;
	*OutCenter += v2 * Length(diff);

	diff = v1 - v2;
	*OutCenter += v3 * Length(diff);

	*OutCenter *= oo_perim;
	*OutRadius = 2.f * area * oo_perim;
}

void TriangleInscribedCircle(VEC3 *OutCenter, float *OutRadius, const VEC3 &v1, const VEC3 &v2, const VEC3 &v3)
{
	float area = fabsf(TriangleArea(v1, v2, v3));
	if (FLOAT_ALMOST_ZERO(area))
	{
		*OutCenter = v1;
		*OutRadius = 0.f;
		return;
	}

	float oo_perim = 1.f / TrianglePerimeter(v1, v2, v3);

	VEC3 diff;

	diff = v2 - v3;
	*OutCenter = v1 * Length(diff);

	diff = v3 - v1;
	*OutCenter += v2 * Length(diff);

	diff = v1 - v2;
	*OutCenter += v3 * Length(diff);

	*OutCenter *= oo_perim;
	*OutRadius = 2.f * area * oo_perim;
}

void TriangleCircumscribedCircle(VEC2 *OutCenter, float *OutRadius, const VEC2 &v1, const VEC2 &v2, const VEC2 &v3)
{
	VEC2 e0, e1;
	float d1, d2, d3;
	float c1, c2, c3, oo_c;

	e0 = v3 - v1;
	e1 = v2 - v1;
	d1 = Dot(e0, e1);

	e0 = v3 - v2;
	e1 = v1 - v2;
	d2 = Dot(e0, e1);

	e0 = v1 - v3;
	e1 = v2 - v3;
	d3 = Dot(e0, e1);

	c1 = d2 * d3;
	c2 = d3 * d1;
	c3 = d1 * d2;
	oo_c = 1.f / (c1 + c2 + c3);

	*OutCenter  = v1 * (c2 + c3);
	*OutCenter += v2 * (c3 + c1);
	*OutCenter += v3 * (c1 + c2);
	*OutCenter *= oo_c * 0.5f;

	*OutRadius = 0.5f * sqrtf( (d1 + d2) * (d2 + d3) * (d3 + d1) * oo_c);
}

void TriangleCircumscribedCircle(VEC3 *OutCenter, float *OutRadius, const VEC3 &v1, const VEC3 &v2, const VEC3 &v3)
{
	VEC3 e0, e1;
	float d1, d2, d3;
	float c1, c2, c3, oo_c;

	e0 = v3 - v1;
	e1 = v2 - v1;
	d1 = Dot(e0, e1);

	e0 = v3 - v2;
	e1 = v1 - v2;
	d2 = Dot(e0, e1);

	e0 = v1 - v3;
	e1 = v2 - v3;
	d3 = Dot(e0, e1);

	c1 = d2 * d3;
	c2 = d3 * d1;
	c3 = d1 * d2;
	oo_c = 1.f / (c1 + c2 + c3);

	*OutCenter  = v1 * (c2 + c3);
	*OutCenter += v2 * (c3 + c1);
	*OutCenter += v3 * (c1 + c2);
	*OutCenter *= oo_c * 0.5f;

	*OutRadius = 0.5f * sqrtf( (d1 + d2) * (d2 + d3) * (d3 + d1) * oo_c);
}

/*void ProjectPointOnSegment(VEC2 *out, const VEC2 &pt, const VEC2 &lineA, const VEC2 &lineB)
{
	float u, l = LengthSq(lineB-lineA);
	
	if (fabs(l) > FLT_EPSILON)
	{
		u = ((pt.x-lineA.x)*(lineB.x-lineA.x) + (pt.y-lineA.y)*(lineB.y-lineA.y)) / l;

		if (u<=0)
		{
			out->x = lineA.x;
			out->y = lineA.y;
		}
		else if (u>=1)
		{
			out->x = lineB.x;
			out->y = lineB.y;
		}
		else
		{
			out->x = lineA.x + (lineB.x-lineA.x) * u;
			out->y = lineA.y + (lineB.y-lineA.y) * u;
		}
	}	
}*/


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// PROSTOK¥T

const RECTI RECTI_ZERO = RECTI(0, 0, 0, 0);
const RECTI RECTI_ZERO_ONE = RECTI(0, 0, 1, 1);
const RECTI RECTI_MINUS_ONE_ONE = RECTI(-1, -1, 1, 1);
const RECTI RECTI_MAX = RECTI(MININT32, MININT32, MAXINT32, MAXINT32);
const RECTI RECTI_INV_MAX = RECTI(MAXINT32, MAXINT32, MININT32, MININT32);

const RECTF RECTF_ZERO = RECTF(0.0f, 0.0f, 0.0f, 0.0f);
const RECTF RECTF_ZERO_ONE = RECTF(0.0f, 0.0f, 1.0f, 1.0f);
const RECTF RECTF_MINUS_ONE_ONE = RECTF(-1.0f, -1.0f, 1.0f, 1.0f);
const RECTF RECTF_MAX = RECTF(-FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX);
const RECTF RECTF_INV_MAX = RECTF(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);


static const uint RECT_EDGE_CORNER1[] = { 0, 2, 0, 1 };
static const uint RECT_EDGE_CORNER2[] = { 1, 3, 2, 3 };

void RECTI::GetEdge(POINT_ *outP1, POINT_ *outP2, uint index) const
{
	GetCorner(outP1, RECT_EDGE_CORNER1[index]);
	GetCorner(outP2, RECT_EDGE_CORNER2[index]);
}
void RECTF::GetEdge(VEC2 *outP1, VEC2 *outP2, uint index) const
{
	GetCorner(outP1, RECT_EDGE_CORNER1[index]);
	GetCorner(outP2, RECT_EDGE_CORNER2[index]);
}

void RECTF::GetQuad(RECTF *out, uint index) const
{
	VEC2 center;
	GetCenter(&center);

	switch (index)
	{
	case 0:
		*out = RECTF(Min.x, Min.y, center.x, center.y);
		return;
	case 1:
		*out = RECTF(center.x, Min.y, Max.x, center.y);
		return;
	case 2:
		*out = RECTF(Min.x, center.y, center.x, Max.y);
		return;
	case 3:
		*out = RECTF(center.x, center.y, Max.x, Max.y);
		return;
	default:
		assert(false);
	}
}

void ClosestPointInRect(POINT_ *Out, const RECTI &Rect, const POINT_ &p)
{
	Out->x = minmax(Rect.Min.x, p.x, Rect.Max.x);
	Out->y = minmax(Rect.Min.y, p.y, Rect.Max.y);
}

void ClosestPointInRect(VEC2 *Out, const RECTF &Rect, const VEC2 &p)
{
	Out->x = minmax(Rect.Min.x, p.x, Rect.Max.x);
	Out->y = minmax(Rect.Min.y, p.y, Rect.Max.y);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// AXIS ALIGNED BOUNDING BOX

const BOX BOX_ZERO = BOX(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
const BOX BOX_ZERO_ONE = BOX(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
const BOX BOX_MINUS_ONE_ONE = BOX(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f);
const BOX BOX_MAX = BOX(-FLT_MAX, -FLT_MAX, -FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
const BOX BOX_INV_MAX = BOX(FLT_MAX, FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

static const uint BOX_EDGE_VERTEX1[] = {
	0, 2, 4, 6,
	0, 1, 4, 5,
	0, 1, 2, 3,
};
static const uint BOX_EDGE_VERTEX2[] = {
	1, 3, 5, 7,
	2, 3, 6, 7,
	4, 5, 6, 7,
};

void BOX::GetEdge(VEC3 *outP1, VEC3 *outP2, uint index) const
{
	GetCorner(outP1, BOX_EDGE_VERTEX1[index]);
	GetCorner(outP2, BOX_EDGE_VERTEX2[index]);
}

void BOX::GetOct(BOX *out, uint index) const
{
	VEC3 center;
	GetCenter(&center);

	switch (index)
	{
	case 0:
		*out = BOX(Min.x, Min.y, Min.z, center.x, center.y, center.z);
		return;
	case 1:
		*out = BOX(center.x, Min.y, Min.z, Max.x, center.y, center.z);
		return;
	case 2:
		*out = BOX(Min.x, center.y, Min.z, center.x, Max.y, center.z);
		return;
	case 3:
		*out = BOX(center.x, center.y, Min.z, Max.x, Max.y, center.z);
		return;
	case 4:
		*out = BOX(Min.x, Min.y, center.z, center.x, center.y, Max.z);
		return;
	case 5:
		*out = BOX(center.x, Min.y, center.z, Max.x, center.y, Max.z);
		return;
	case 6:
		*out = BOX(Min.x, center.y, center.z, center.x, Max.y, Max.z);
		return;
	case 7:
		*out = BOX(center.x, center.y, center.z, Max.x, Max.y, Max.z);
		return;

	default:
		assert(false);
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// ORIENTED BOUNDING BOX

bool ObbToObb(const OBB &a, const OBB &b)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	float ra, rb;
	float R[3][3], AbsR[3][3];

	// Compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = Dot(a.Axes[i], b.Axes[j]);

	// Compute translation vector t
	VEC3 t = b.Center - a.Center;
	// Bring translation into a's coordinate frame
	t = VEC3(Dot(t, a.Axes[0]), Dot(t, a.Axes[1]), Dot(t, a.Axes[2]));

	// Compute common subexpressions. Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and
	// their cross product is (near) null (see text for details)
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			AbsR[i][j] = fabs(R[i][j]) + FLOAT_EPSILON;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++) {
		ra = a.HalfSize[i];
		rb = b.HalfSize[0] * AbsR[i][0] + b.HalfSize[1] * AbsR[i][1] + b.HalfSize[2] * AbsR[i][2];
		if (fabs(t[i]) > ra + rb) return false;
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++) {
		ra = a.HalfSize[0] * AbsR[0][i] + a.HalfSize[1] * AbsR[1][i] + a.HalfSize[2] * AbsR[2][i];
		rb = b.HalfSize[i];
		if (fabs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return false;
	}

	// Test axis L = A0 x B0
	ra = a.HalfSize[1] * AbsR[2][0] + a.HalfSize[2] * AbsR[1][0];
	rb = b.HalfSize[1] * AbsR[0][2] + b.HalfSize[2] * AbsR[0][1];
	if (fabs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;

	// Test axis L = A0 x B1
	ra = a.HalfSize[1] * AbsR[2][1] + a.HalfSize[2] * AbsR[1][1];
	rb = b.HalfSize[0] * AbsR[0][2] + b.HalfSize[2] * AbsR[0][0];
	if (fabs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;

	// Test axis L = A0 x B2
	ra = a.HalfSize[1] * AbsR[2][2] + a.HalfSize[2] * AbsR[1][2];
	rb = b.HalfSize[0] * AbsR[0][1] + b.HalfSize[1] * AbsR[0][0];
	if (fabs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;

	// Test axis L = A1 x B0
	ra = a.HalfSize[0] * AbsR[2][0] + a.HalfSize[2] * AbsR[0][0];
	rb = b.HalfSize[1] * AbsR[1][2] + b.HalfSize[2] * AbsR[1][1];
	if (fabs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;

	// Test axis L = A1 x B1
	ra = a.HalfSize[0] * AbsR[2][1] + a.HalfSize[2] * AbsR[0][1];
	rb = b.HalfSize[0] * AbsR[1][2] + b.HalfSize[2] * AbsR[1][0];
	if (fabs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;

	// Test axis L = A1 x B2
	ra = a.HalfSize[0] * AbsR[2][2] + a.HalfSize[2] * AbsR[0][2];
	rb = b.HalfSize[0] * AbsR[1][1] + b.HalfSize[1] * AbsR[1][0];
	if (fabs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;

	// Test axis L = A2 x B0
	ra = a.HalfSize[0] * AbsR[1][0] + a.HalfSize[1] * AbsR[0][0];
	rb = b.HalfSize[1] * AbsR[2][2] + b.HalfSize[2] * AbsR[2][1];
	if (fabs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;

	// Test axis L = A2 x B1
	ra = a.HalfSize[0] * AbsR[1][1] + a.HalfSize[1] * AbsR[0][1];
	rb = b.HalfSize[0] * AbsR[2][2] + b.HalfSize[2] * AbsR[2][0];
	if (fabs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;

	// Test axis L = A2 x B2
	ra = a.HalfSize[0] * AbsR[1][2] + a.HalfSize[1] * AbsR[0][2];
	rb = b.HalfSize[0] * AbsR[2][1] + b.HalfSize[1] * AbsR[2][0];
	if (fabs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;

	// Since no separating axis found, the OBBs must be intersecting
	return true;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// KOLORY

const COLOR COLOR_TRANSPARENT_ = COLOR(0x00000000u);
const COLOR COLOR_BLACK   = COLOR(0xFF000000);
const COLOR COLOR_GRAY    = COLOR(0xFF808080);
const COLOR COLOR_SILVER  = COLOR(0xFFC0C0C0);
const COLOR COLOR_WHITE   = COLOR(0xFFFFFFFF);
const COLOR COLOR_FUCHSIA = COLOR(0xFFFF00FF);
const COLOR COLOR_RED     = COLOR(0xFFFF0000);
const COLOR COLOR_ORANGE  = COLOR(0xFFFFA500);
const COLOR COLOR_YELLOW  = COLOR(0xFFFFFF00);
const COLOR COLOR_LIME    = COLOR(0xFF00FF00);
const COLOR COLOR_AQUA    = COLOR(0xFF00FFFF);
const COLOR COLOR_BLUE    = COLOR(0xFF0000FF);
const COLOR COLOR_PURPLE  = COLOR(0xFF800080);
const COLOR COLOR_MAROON  = COLOR(0xFF800000);
const COLOR COLOR_OLIVE   = COLOR(0xFF808000);
const COLOR COLOR_GREEN   = COLOR(0xFF008000);
const COLOR COLOR_TEAL    = COLOR(0xFF008080);

const COLORF COLORF_TRANSPARENT_ = COLORF(0.f, 0.f, 0.f, 0.f);
const COLORF COLORF_BLACK   = COLORF(0.0f, 0.0f, 0.0f);
const COLORF COLORF_GRAY    = COLORF(0.5f, 0.5f, 0.5f);
const COLORF COLORF_SILVER  = COLORF(0.75f, 0.75f, 0.75f);
const COLORF COLORF_WHITE   = COLORF(1.0f, 1.0f, 1.0f);
const COLORF COLORF_FUCHSIA = COLORF(1.0f, 0.0f, 1.0f);
const COLORF COLORF_RED     = COLORF(1.0f, 0.0f, 0.0f);
const COLORF COLORF_ORANGE  = COLORF(1.0f, 0.64453125f, 0.0f);
const COLORF COLORF_YELLOW  = COLORF(1.0f, 1.0f, 0.0f);
const COLORF COLORF_LIME    = COLORF(0.0f, 1.0f, 0.0f);
const COLORF COLORF_AQUA    = COLORF(0.0f, 1.0f, 1.0f);
const COLORF COLORF_BLUE    = COLORF(0.0f, 0.0f, 1.0f);
const COLORF COLORF_PURPLE  = COLORF(0.5f, 0.0f, 0.5f);
const COLORF COLORF_MAROON  = COLORF(0.5f, 0.0f, 0.0f);
const COLORF COLORF_OLIVE   = COLORF(0.5f, 0.5f, 0.0f);
const COLORF COLORF_GREEN   = COLORF(0.0f, 0.5f, 0.0f);
const COLORF COLORF_TEAL    = COLORF(0.0f, 0.5f, 0.5f);



void ColorToStr(tstring *Out, COLOR Color, char Format)
{
	if (Format == 'x')
		UintToStr2(Out, Color.ARGB & 0x00FFFFFF, 6, 16);
	else if (Format == 'X')
		UintToStr2(Out, Color.ARGB, 8, 16);
	else if (Format == 'f' || Format == 'F')
	{
		COLORF Colorf;
		ColorToColorf(&Colorf, Color);
		ColorToStr(Out, Colorf, Format);
	}
	else
		assert(0 && "Invalid format char in ColorToStr for COLOR.");
}

void ColorToStr(tstring *Out, const COLORF &Color, char Format)
{
	if (Format == 'f')
	{
		tstring sr, sg, sb;
		FloatToStr(&sr, Color.R);
		FloatToStr(&sg, Color.G);
		FloatToStr(&sb, Color.B);
		*Out = sr; *Out += _T(','); *Out += sg; *Out += _T(','); *Out += sb;
	}
	else if (Format == 'F')
	{
		tstring sa, sr, sg, sb;
		FloatToStr(&sa, Color.A);
		FloatToStr(&sr, Color.R);
		FloatToStr(&sg, Color.G);
		FloatToStr(&sb, Color.B);
		*Out = sa; *Out += _T(','); *Out += sr; *Out += _T(','); *Out += sg; *Out += _T(','); *Out += sb;
	}
	else if (Format == 'x' || Format == 'X')
	{
		COLOR c = ColorfToColor(Color);
		ColorToStr(Out, c, Format);
	}
	else
		assert(0 && "Invalid format char in ColorToStr for COLORF.");
}

// [Funkcja wewnêtrzna]
// Str jest w formacie floatów R,G,B lub A,R,G,B.
bool StrFloatsToColorf(COLORF *Out, const tstring &Str, size_t CommaPos1)
{
	// Pozycja pierwszego przecinka jest w parametrze
	// ZnajdŸ nastêpne przecinki
	size_t CommaPos2 = Str.find(_T(','), CommaPos1+1);
	if (CommaPos2 == tstring::npos) return false;
	size_t CommaPos3 = Str.find(_T(','), CommaPos2+1);
	// Nie ma trzeciego przecinka - R,G,B
	if (CommaPos3 == tstring::npos)
	{
		Out->A = 1.0f;
		if (!StrToSth<float>(&Out->R, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Out->G, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Out->B, Str.substr(CommaPos2+1))) return false;
	}
	// Jest trzeci przecinek - A,R,G,B
	else
	{
		if (!StrToSth<float>(&Out->A, Str.substr(0, CommaPos1))) return false;
		if (!StrToSth<float>(&Out->R, Str.substr(CommaPos1+1, CommaPos2-(CommaPos1+1)))) return false;
		if (!StrToSth<float>(&Out->G, Str.substr(CommaPos2+1, CommaPos3-(CommaPos2+1)))) return false;
		if (!StrToSth<float>(&Out->B, Str.substr(CommaPos3+1))) return false;
	}
	return true;
}

bool StrToColor(COLOR *Out, const tstring &Str)
{
	size_t CommaPos1 = Str.find(_T(','));
	// Jest przecinek - s¹ floaty
	if (CommaPos1 != tstring::npos)
	{
		COLORF Colorf;
		if (!StrFloatsToColorf(&Colorf, Str, CommaPos1)) return false;
		*Out = ColorfToColor(Colorf);
		return true;
	}
	// Nie ma przecinków
	tstring LStr;
	LowerCase(&LStr, Str);
	// Standardowe nazwy
	if (LStr == _T("black"))        *Out = COLOR_BLACK;
	else if (LStr == _T("gray"))    *Out = COLOR_GRAY;
	else if (LStr == _T("silver"))  *Out = COLOR_SILVER;
	else if (LStr == _T("white"))   *Out = COLOR_WHITE;
	else if (LStr == _T("fuchsia")) *Out = COLOR_FUCHSIA;
	else if (LStr == _T("red"))     *Out = COLOR_RED;
	else if (LStr == _T("orange"))  *Out = COLOR_ORANGE;
	else if (LStr == _T("yellow"))  *Out = COLOR_YELLOW;
	else if (LStr == _T("lime"))    *Out = COLOR_LIME;
	else if (LStr == _T("aqua"))    *Out = COLOR_AQUA;
	else if (LStr == _T("blue"))    *Out = COLOR_BLUE;
	else if (LStr == _T("purple"))  *Out = COLOR_PURPLE;
	else if (LStr == _T("maroon"))  *Out = COLOR_MAROON;
	else if (LStr == _T("olive"))   *Out = COLOR_OLIVE;
	else if (LStr == _T("green"))   *Out = COLOR_GREEN;
	else if (LStr == _T("teal"))    *Out = COLOR_TEAL;
	// RRGGBB lub AARRGGBB szesnastkowo
	else
	{
		// RRGGBB
		if (Str.size() == 6)
		{
			if (StrToUint(&Out->ARGB, Str, 16) != 0) return false;
			Out->ARGB |= 0xFF000000;
			return true;
		}
		// AARRGGBB
		else if (Str.size() == 8)
			return StrToUint(&Out->ARGB, Str, 16) == 0;
		// Z³a d³ugoœæ
		else return false;
	}
	// To odnoœnie standardowych nazw
	return true;
}

bool StrToColor(COLORF *Out, const tstring &Str)
{
	size_t CommaPos1 = Str.find(_T(','));
	// Jest przecinek - s¹ floaty
	if (CommaPos1 != tstring::npos)
		return StrFloatsToColorf(Out, Str, CommaPos1);
	// Nie ma przecinków
	tstring LStr;
	LowerCase(&LStr, Str);
	// Standardowe nazwy
	if (LStr == _T("black"))        *Out = COLORF_BLACK;
	else if (LStr == _T("gray"))    *Out = COLORF_GRAY;
	else if (LStr == _T("silver"))  *Out = COLORF_SILVER;
	else if (LStr == _T("white"))   *Out = COLORF_WHITE;
	else if (LStr == _T("fuchsia")) *Out = COLORF_FUCHSIA;
	else if (LStr == _T("red"))     *Out = COLORF_RED;
	else if (LStr == _T("orange"))  *Out = COLORF_ORANGE;
	else if (LStr == _T("yellow"))  *Out = COLORF_YELLOW;
	else if (LStr == _T("lime"))    *Out = COLORF_LIME;
	else if (LStr == _T("aqua"))    *Out = COLORF_AQUA;
	else if (LStr == _T("blue"))    *Out = COLORF_BLUE;
	else if (LStr == _T("purple"))  *Out = COLORF_PURPLE;
	else if (LStr == _T("maroon"))  *Out = COLORF_MAROON;
	else if (LStr == _T("olive"))   *Out = COLORF_OLIVE;
	else if (LStr == _T("green"))   *Out = COLORF_GREEN;
	else if (LStr == _T("teal"))    *Out = COLORF_TEAL;
	// RRGGBB lub AARRGGBB szesnastkowo
	else
	{
		// RRGGBB
		if (Str.size() == 6)
		{
			COLOR c;
			if (StrToUint(&c.ARGB, Str, 16) != 0) return false;
			c.ARGB |= 0xFF000000;
			ColorToColorf(Out, c);
			return true;
		}
		// AARRGGBB
		else if (Str.size() == 8)
		{
			COLOR c;
			if (StrToUint(&c.ARGB, Str, 16) != 0) return false;
			ColorToColorf(Out, c);
			return true;
		}
		// Z³a d³ugoœæ
		else return false;
	}
	// To do standardowych kolorów
	Out->A = 1.0f;
	return true;
}

void HsbToRgb(COLORF *Out, float hue, float saturation, float brightness)
{
	Out->A = 1.0f;

	float h = frac(hue) * 6.0f;
	float f = frac(h);
	float p = brightness * (1.0f - saturation);
	float q = brightness * (1.0f - saturation * f);
	float t = brightness * (1.0f - (saturation * (1.0f - f)));

	switch ((int)h)
	{
	case 0:
		Out->R = brightness;
		Out->G = t;
		Out->B = p;
		return;
	case 1:
		Out->R = q;
		Out->G = brightness;
		Out->B = p;
		return;
	case 2:
		Out->R = p;
		Out->G = brightness;
		Out->B = t;
		return;
	case 3:
		Out->R = p;
		Out->G = q;
		Out->B = brightness;
		return;
	case 4:
		Out->R = t;
		Out->G = p;
		Out->B = brightness;
		return;
	default:
		Out->R = brightness;
		Out->G = p;
		Out->B = q;
		return;
	}
}

void RgbToHsb(float *OutHue, float *OutSaturation, float *OutBrightness, const COLORF &Color)
{
	float Min = min3(Color.R, Color.G, Color.B);
	float Max = max3(Color.R, Color.G, Color.B);

	*OutBrightness = Max;

	if (FLOAT_ALMOST_ZERO(Max))
		*OutSaturation = 0.f;
	else
		*OutSaturation = 1.f - (Min / Max);

	if (Max == Min)
		*OutHue = 0.f; // Undefined
	else if (Max == Color.R && Color.G >= Color.B)
		*OutHue = (60.f/360.f) * (Color.G-Color.B)/(Max-Min);
	else if (Max == Color.R) // && Color.G < Color.B
		*OutHue = (60.f/360.f) * (Color.G-Color.B)/(Max-Min) + 1.f;
	else if (Max == Color.G)
		*OutHue = (60.f/360.f) * (Color.B-Color.R)/(Max-Min) + (120.f/360.f);
	else // Max == Color.B
		*OutHue = (60.f/360.f) * (Color.R-Color.G)/(Max-Min) + (240.f/360.f);
}

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// P£ASZCZYZNA

const PLANE PLANE_ZERO = PLANE(0.0f, 0.0f, 0.0f, 0.0f);
const PLANE PLANE_POSITIVE_X = PLANE(1.0f, 0.0f, 0.0f, 0.0f);
const PLANE PLANE_POSITIVE_Y = PLANE(0.0f, 1.0f, 0.0f, 0.0f);
const PLANE PLANE_POSITIVE_Z = PLANE(0.0f, 0.0f, 1.0f, 0.0f);
const PLANE PLANE_NEGATIVE_X = PLANE(-1.0f, 0.0f, 0.0f, 0.0f);
const PLANE PLANE_NEGATIVE_Y = PLANE(0.0f, -1.0f, 0.0f, 0.0f);
const PLANE PLANE_NEGATIVE_Z = PLANE(0.0f, 0.0f, -1.0f, 0.0f);

void GetBoxPlane(PLANE *out, const BOX &box, uint index)
{
	GetPlaneNormal(*out) = VEC3_ZERO;
	uint axis = index>>1;

	if (index&1)
	{
		out->d  = -box.Max[axis];
		GetPlaneNormal(*out)[axis] = 1.f;
	}
	else
	{
		out->d = box.Min[axis];
		GetPlaneNormal(*out)[axis] = -1.f;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// PROSTA 2D

const LINE2D LINE2D_ZERO = LINE2D(0.f, 0.f, 0.f);
const LINE2D LINE2D_POSITIVE_X = LINE2D( 1.f, 0.f, 0.f);
const LINE2D LINE2D_NEGATIVE_X = LINE2D(-1.f, 0.f, 0.f);
const LINE2D LINE2D_POSITIVE_Y = LINE2D(0.f,  1.f, 0.f);
const LINE2D LINE2D_NEGATIVE_Y = LINE2D(0.f, -1.f, 0.f);

bool LinesIntersection(VEC2 *Out, const LINE2D &L1, const LINE2D &L2)
{
	float DET1 = L1.a*L2.b - L1.b*L2.a;
	if (FLOAT_ALMOST_ZERO(DET1)) return false;
	Out->x = (L1.b*L2.c - L1.c*L2.b) / DET1;
	Out->y = (L1.c*L2.a - L1.a*L2.c) / DET1;
	return true;
}

bool PointsCollinear(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3)
{
	return FLOAT_ALMOST_ZERO(
		p1.x*p2.y + p1.y*p3.x + p2.x*p3.y -
		p3.x*p2.y - p2.x*p1.y - p3.y*p1.x);
}

void GetRectLine(LINE2D *out, const RECTF &rect, uint index)
{
	GetLineNormal(*out) = VEC2_ZERO;
	uint axis = index>>1;

	if (index&1)
	{
		out->c = -rect.Max[axis];
		GetLineNormal(*out)[axis] = 1.f;
	}
	else
	{
		out->c = rect.Min[axis];
		GetLineNormal(*out)[axis] = -1.f;
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// MACIERZ

const MATRIX MATRIX_ZERO = MATRIX(
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f);
const MATRIX MATRIX_IDENTITY = MATRIX(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

const MATRIX33 MATRIX33_ZERO = MATRIX33(
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f);
const MATRIX33 MATRIX33_IDENTITY = MATRIX33(
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f);

#if defined(USE_DIRECTX) && !defined(DOXYGEN)
// Nic
#else
	MATRIX MATRIX::operator - () const
	{
		return MATRIX(
			-_11, -_12, -_13, -_14,
			-_21, -_22, -_23, -_24,
			-_31, -_32, -_33, -_34,
			-_41, -_42, -_43, -_44);
	}

	MATRIX MATRIX::operator + (const MATRIX &m) const
	{
		return MATRIX(
			_11 + m._11, _12 + m._12, _13 + m._13, _14 + m._14,
			_21 + m._21, _22 + m._22, _23 + m._23, _24 + m._24,
			_31 + m._31, _32 + m._32, _33 + m._33, _34 + m._34,
			_41 + m._41, _42 + m._42, _43 + m._43, _44 + m._44);
	}

	MATRIX MATRIX::operator - (const MATRIX &m) const
	{
		return MATRIX(
			_11 - m._11, _12 - m._12, _13 - m._13, _14 - m._14,
			_21 - m._21, _22 - m._22, _23 - m._23, _24 - m._24,
			_31 - m._31, _32 - m._32, _33 - m._33, _34 - m._34,
			_41 - m._41, _42 - m._42, _43 - m._43, _44 - m._44);
	}

	MATRIX & MATRIX::operator += (const MATRIX &m)
	{
		_11 += m._11; _12 += m._12; _13 += m._13; _14 += m._14;
		_21 += m._21; _22 += m._22; _23 += m._23; _24 += m._24;
		_31 += m._31; _32 += m._32; _33 += m._33; _34 += m._34;
		_41 += m._41; _42 += m._42; _43 += m._43; _44 += m._44;
		return *this;
	}

	MATRIX & MATRIX::operator -= (const MATRIX &m)
	{
		_11 -= m._11; _12 -= m._12; _13 -= m._13; _14 -= m._14;
		_21 -= m._21; _22 -= m._22; _23 -= m._23; _24 -= m._24;
		_31 -= m._31; _32 -= m._32; _33 -= m._33; _34 -= m._34;
		_41 -= m._41; _42 -= m._42; _43 -= m._43; _44 -= m._44;
		return *this;
	}

	MATRIX MATRIX::operator * (float s) const
	{
		return MATRIX(
			_11 * s, _12 * s, _13 * s, _14 * s,
			_21 * s, _22 * s, _23 * s, _24 * s,
			_31 * s, _32 * s, _33 * s, _34 * s,
			_41 * s, _42 * s, _43 * s, _44 * s);
	}

	MATRIX MATRIX::operator / (float s) const
	{
		s = 1.0f / s;
		return MATRIX(
			_11 * s, _12 * s, _13 * s, _14 * s,
			_21 * s, _22 * s, _23 * s, _24 * s,
			_31 * s, _32 * s, _33 * s, _34 * s,
			_41 * s, _42 * s, _43 * s, _44 * s);
	}

	MATRIX & MATRIX::operator *= (float s)
	{
		_11 *= s; _12 *= s; _13 *= s; _14 *= s;
		_21 *= s; _22 *= s; _23 *= s; _24 *= s;
		_31 *= s; _32 *= s; _33 *= s; _34 *= s;
		_41 *= s; _42 *= s; _43 *= s; _44 *= s;
		return *this;
	}

	MATRIX & MATRIX::operator /= (float s)
	{
		s = 1.0f / s;
		_11 *= s; _12 *= s; _13 *= s; _14 *= s;
		_21 *= s; _22 *= s; _23 *= s; _24 *= s;
		_31 *= s; _32 *= s; _33 *= s; _34 *= s;
		_41 *= s; _42 *= s; _43 *= s; _44 *= s;
		return *this;
	}

	MATRIX & MATRIX::operator *= (const MATRIX &m)
	{
		MATRIX Tmp = MATRIX(
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
		*this = Tmp;

		return *this;
	}
#endif

MATRIX33 MATRIX33::operator - () const
{
	return MATRIX33(
		-_11, -_12, -_13,
		-_21, -_22, -_23,
		-_31, -_32, -_33);
}

MATRIX33 MATRIX33::operator + (const MATRIX33 &m) const
{
	return MATRIX33(
		_11 + m._11, _12 + m._12, _13 + m._13,
		_21 + m._21, _22 + m._22, _23 + m._23,
		_31 + m._31, _32 + m._32, _33 + m._33);
}

MATRIX33 MATRIX33::operator - (const MATRIX33 &m) const
{
	return MATRIX33(
		_11 - m._11, _12 - m._12, _13 - m._13,
		_21 - m._21, _22 - m._22, _23 - m._23,
		_31 - m._31, _32 - m._32, _33 - m._33);
}

MATRIX33 & MATRIX33::operator += (const MATRIX33 &m)
{
	_11 += m._11; _12 += m._12; _13 += m._13;
	_21 += m._21; _22 += m._22; _23 += m._23;
	_31 += m._31; _32 += m._32; _33 += m._33;
	return *this;
}

MATRIX33 & MATRIX33::operator -= (const MATRIX33 &m)
{
	_11 -= m._11; _12 -= m._12; _13 -= m._13;
	_21 -= m._21; _22 -= m._22; _23 -= m._23;
	_31 -= m._31; _32 -= m._32; _33 -= m._33;
	return *this;
}

MATRIX33 MATRIX33::operator * (float s) const
{
	return MATRIX33(
		_11 * s, _12 * s, _13 * s,
		_21 * s, _22 * s, _23 * s,
		_31 * s, _32 * s, _33 * s);
}

MATRIX33 MATRIX33::operator / (float s) const
{
	s = 1.0f / s;
	return MATRIX33(
		_11 * s, _12 * s, _13 * s,
		_21 * s, _22 * s, _23 * s,
		_31 * s, _32 * s, _33 * s);
}

MATRIX33 & MATRIX33::operator *= (float s)
{
	_11 *= s; _12 *= s; _13 *= s;
	_21 *= s; _22 *= s; _23 *= s;
	_31 *= s; _32 *= s; _33 *= s;
	return *this;
}

MATRIX33 & MATRIX33::operator /= (float s)
{
	s = 1.0f / s;
	_11 *= s; _12 *= s; _13 *= s;
	_21 *= s; _22 *= s; _23 *= s;
	_31 *= s; _32 *= s; _33 *= s;
	return *this;
}

MATRIX33 & MATRIX33::operator *= (const MATRIX33 &m)
{
	MATRIX33 Tmp = MATRIX33(
		_11 * m._11 + _12 * m._21 + _13 * m._31,
		_11 * m._12 + _12 * m._22 + _13 * m._32,
		_11 * m._13 + _12 * m._23 + _13 * m._33,

		_21 * m._11 + _22 * m._21 + _23 * m._31,
		_21 * m._12 + _22 * m._22 + _23 * m._32,
		_21 * m._13 + _22 * m._23 + _23 * m._33,

		_31 * m._11 + _32 * m._21 + _33 * m._31,
		_31 * m._12 + _32 * m._22 + _33 * m._32,
		_31 * m._13 + _32 * m._23 + _33 * m._33);
	*this = Tmp;

	return *this;
}

bool MatrixEqual(const MATRIX &m1, const MATRIX &m2)
{
	return
		float_equal(m1._11, m2._11) && float_equal(m1._12, m2._12) && float_equal(m1._13, m2._13) && float_equal(m1._14, m2._14) &&
		float_equal(m1._21, m2._21) && float_equal(m1._22, m2._22) && float_equal(m1._23, m2._23) && float_equal(m1._24, m2._24) &&
		float_equal(m1._31, m2._31) && float_equal(m1._32, m2._32) && float_equal(m1._33, m2._33) && float_equal(m1._34, m2._34) &&
		float_equal(m1._41, m2._41) && float_equal(m1._42, m2._42) && float_equal(m1._43, m2._43) && float_equal(m1._44, m2._44);
}

void Minus(MATRIX *m)
{
	m->_11 = -m->_11; m->_12 = -m->_12; m->_13 = -m->_13; m->_14 = -m->_14;
	m->_21 = -m->_21; m->_22 = -m->_22; m->_23 = -m->_23; m->_24 = -m->_24;
	m->_31 = -m->_31; m->_32 = -m->_32; m->_33 = -m->_33; m->_34 = -m->_34;
	m->_41 = -m->_41; m->_42 = -m->_42; m->_43 = -m->_43; m->_44 = -m->_44;
}

void Add(MATRIX *Out, const MATRIX &m1, const MATRIX &m2)
{
	Out->_11 = m1._11 + m2._11; Out->_12 = m1._12 + m2._12; Out->_13 = m1._13 + m2._13; Out->_14 = m1._14 + m2._14;
	Out->_21 = m1._21 + m2._21; Out->_22 = m1._22 + m2._22; Out->_23 = m1._23 + m2._23; Out->_24 = m1._24 + m2._24;
	Out->_31 = m1._31 + m2._31; Out->_32 = m1._32 + m2._32; Out->_33 = m1._33 + m2._33; Out->_34 = m1._34 + m2._34;
	Out->_41 = m1._41 + m2._41; Out->_42 = m1._42 + m2._42; Out->_43 = m1._43 + m2._43; Out->_44 = m1._44 + m2._44;
}

void Sub(MATRIX *Out, const MATRIX &m1, const MATRIX &m2)
{
	Out->_11 = m1._11 - m2._11; Out->_12 = m1._12 - m2._12; Out->_13 = m1._13 - m2._13; Out->_14 = m1._14 - m2._14;
	Out->_21 = m1._21 - m2._21; Out->_22 = m1._22 - m2._22; Out->_23 = m1._23 - m2._23; Out->_24 = m1._24 - m2._24;
	Out->_31 = m1._31 - m2._31; Out->_32 = m1._32 - m2._32; Out->_33 = m1._33 - m2._33; Out->_34 = m1._34 - m2._34;
	Out->_41 = m1._41 - m2._41; Out->_42 = m1._42 - m2._42; Out->_43 = m1._43 - m2._43; Out->_44 = m1._44 - m2._44;
}

void Mul(MATRIX *Out, const MATRIX &m1, const MATRIX &m2)
{
	Out->_11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31 + m1._14 * m2._41;
	Out->_12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32 + m1._14 * m2._42;
	Out->_13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33 + m1._14 * m2._43;
	Out->_14 = m1._11 * m2._14 + m1._12 * m2._24 + m1._13 * m2._34 + m1._14 * m2._44;

	Out->_21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31 + m1._24 * m2._41;
	Out->_22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32 + m1._24 * m2._42;
	Out->_23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33 + m1._24 * m2._43;
	Out->_24 = m1._21 * m2._14 + m1._22 * m2._24 + m1._23 * m2._34 + m1._24 * m2._44;

	Out->_31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31 + m1._34 * m2._41;
	Out->_32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32 + m1._34 * m2._42;
	Out->_33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33 + m1._34 * m2._43;
	Out->_34 = m1._31 * m2._14 + m1._32 * m2._24 + m1._33 * m2._34 + m1._34 * m2._44;

	Out->_41 = m1._41 * m2._11 + m1._42 * m2._21 + m1._43 * m2._31 + m1._44 * m2._41;
	Out->_42 = m1._41 * m2._12 + m1._42 * m2._22 + m1._43 * m2._32 + m1._44 * m2._42;
	Out->_43 = m1._41 * m2._13 + m1._42 * m2._23 + m1._43 * m2._33 + m1._44 * m2._43;
	Out->_44 = m1._41 * m2._14 + m1._42 * m2._24 + m1._43 * m2._34 + m1._44 * m2._44;
}

void Mul3x3(MATRIX &out, const MATRIX &m1, const MATRIX &m2)
{
	out._11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31;
	out._12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32;
	out._13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33;

	out._21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31;
	out._22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32;
	out._23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33;

	out._31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31;
	out._32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32;
	out._33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33;
}

void Mul4x3(MATRIX &out, const MATRIX &m1, const MATRIX &m2)
{
	out._11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31;
	out._12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32;
	out._13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33;

	out._21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31;
	out._22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32;
	out._23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33;

	out._31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31;
	out._32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32;
	out._33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33;

	out._41 = m1._41 * m2._11 + m1._42 * m2._21 + m1._43 * m2._31 + m2._41;
	out._42 = m1._41 * m2._12 + m1._42 * m2._22 + m1._43 * m2._32 + m2._42;
	out._43 = m1._41 * m2._13 + m1._42 * m2._23 + m1._43 * m2._33 + m2._43;
}

void Mul(MATRIX *m, float s)
{
	m->_11 *= s; m->_12 *= s; m->_13 *= s; m->_14 *= s;
	m->_21 *= s; m->_22 *= s; m->_23 *= s; m->_24 *= s;
	m->_31 *= s; m->_32 *= s; m->_33 *= s; m->_34 *= s;
	m->_41 *= s; m->_42 *= s; m->_43 *= s; m->_44 *= s;
}

void Div(MATRIX *m, float s)
{
	s = 1.0f / s;
	m->_11 *= s; m->_12 *= s; m->_13 *= s; m->_14 *= s;
	m->_21 *= s; m->_22 *= s; m->_23 *= s; m->_24 *= s;
	m->_31 *= s; m->_32 *= s; m->_33 *= s; m->_34 *= s;
	m->_41 *= s; m->_42 *= s; m->_43 *= s; m->_44 *= s;
}

void MatrixToStr(tstring *Out, const MATRIX &m)
{
	tstring s;

	FloatToStr(&s, m._11); *Out  = s; *Out += _T(',');
	FloatToStr(&s, m._12); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._13); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._14); *Out += s; *Out += _T(';');

	FloatToStr(&s, m._21); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._22); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._23); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._24); *Out += s; *Out += _T(';');

	FloatToStr(&s, m._31); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._32); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._33); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._34); *Out += s; *Out += _T(';');

	FloatToStr(&s, m._41); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._42); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._43); *Out += s; *Out += _T(',');
	FloatToStr(&s, m._44); *Out += s;
}

bool StrToMatrix(MATRIX *Out, const tstring &Str)
{
	tstring Parts[4], Numbers[4], Dupa;
	size_t Index = 0;
	tstring Delimiter1 = _T(";"), Delimiter2 = _T(",");
	if (!Split(Str, Delimiter1, &Parts[0], &Index)) return false;
	if (!Split(Str, Delimiter1, &Parts[1], &Index)) return false;
	if (!Split(Str, Delimiter1, &Parts[2], &Index)) return false;
	if (!Split(Str, Delimiter1, &Parts[3], &Index)) return false;
	if (Split(Str, Delimiter1, &Dupa, &Index)) return false;

	for (uint PartI = 0; PartI < 4; PartI++)
	{
		Index = 0;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[0], &Index)) return false;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[1], &Index)) return false;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[2], &Index)) return false;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[3], &Index)) return false;
		if (Split(Parts[PartI], Delimiter2, &Dupa, &Index)) return false;

		if (StrToFloat(&Out->operator ()(PartI, 0), Numbers[0]) != 0) return false;
		if (StrToFloat(&Out->operator ()(PartI, 1), Numbers[1]) != 0) return false;
		if (StrToFloat(&Out->operator ()(PartI, 2), Numbers[2]) != 0) return false;
		if (StrToFloat(&Out->operator ()(PartI, 3), Numbers[3]) != 0) return false;
	}

	return true;
}

/* Wzór mno¿enia wektora przez macierz:
	Out->x = v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41;
	Out->y = v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42;
	Out->z = v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43;
	Out->w = v.x * m._14 + v.y * m._24 + v.z * m._34 + v.w * m._44;
*/

void Transform(VEC4 *Out, const VEC2 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21 + m._41;
	Out->y = v.x * m._12 + v.y * m._22 + m._42;
	Out->z = v.x * m._13 + v.y * m._23 + m._43;
	Out->w = v.x * m._14 + v.y * m._24 + m._44;
}

void Transform(VEC2 *Out, const VEC2 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21 + m._41;
	Out->y = v.x * m._12 + v.y * m._22 + m._42;
}

void TransformCoord(VEC2 *Out, const VEC2 &v, const MATRIX &m)
{
	VEC3 Out1; // To tak naprawdê [X,Y,0,W]
	Out1.x = v.x * m._11 + v.y * m._21 + m._41;
	Out1.y = v.x * m._12 + v.y * m._22 + m._42;
	Out1.z = v.x * m._14 + v.y * m._24 + m._44;
	float wrc = 1.0f / Out1.z;
	Out->x = Out1.x * wrc;
	Out->y = Out1.y * wrc;
}

void TransformNormal(VEC2 *Out, const VEC2 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21;
	Out->y = v.x * m._12 + v.y * m._22;
}

void Transform(VEC4 *Out, const VEC3 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
	Out->y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
	Out->z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;
	Out->w = v.x * m._14 + v.y * m._24 + v.z * m._34 + m._44;
}

void Transform(VEC3 *Out, const VEC3 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41;
	Out->y = v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42;
	Out->z = v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43;
}

void TransformCoord(VEC3 *Out, const VEC3 &v, const MATRIX &m)
{
	VEC4 Out1, vv = VEC4(v.x, v.y, v.z, 1.0f);
	Out1.x = vv.x * m._11 + vv.y * m._21 + vv.z * m._31 + vv.w * m._41;
	Out1.y = vv.x * m._12 + vv.y * m._22 + vv.z * m._32 + vv.w * m._42;
	Out1.z = vv.x * m._13 + vv.y * m._23 + vv.z * m._33 + vv.w * m._43;
	Out1.w = vv.x * m._14 + vv.y * m._24 + vv.z * m._34 + vv.w * m._44;
	float wrc = 1.0f / Out1.w;
	Out->x = Out1.x * wrc;
	Out->y = Out1.y * wrc;
	Out->z = Out1.z * wrc;
}

void TransformNormal(VEC3 *Out, const VEC3 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	Out->y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	Out->z = v.x * m._13 + v.y * m._23 + v.z * m._33;
}

void Transform(VEC4 *Out, const VEC4 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41;
	Out->y = v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42;
	Out->z = v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43;
	Out->w = v.x * m._14 + v.y * m._24 + v.z * m._34 + v.w * m._44;
}

void TransformNormalByTranspose(VEC3 *Out, const VEC3 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._12 + v.z * m._13;
	Out->y = v.x * m._21 + v.y * m._22 + v.z * m._23;
	Out->z = v.x * m._31 + v.y * m._32 + v.z * m._33;
}

void Untransform(VEC3 *Out, const VEC3 &v, const MATRIX &m)
{
	VEC3 t = VEC3(
		v.x - m._41,
		v.y - m._42,
		v.z - m._43);

	Out->x = t.x * m._11 + t.y * m._12 + t.z * m._13;
	Out->y = t.x * m._21 + t.y * m._22 + t.z * m._23;
	Out->z = t.x * m._31 + t.y * m._32 + t.z * m._33;
}

void UntransformNormal(VEC3 *Out, const VEC3 &v, const MATRIX &m)
{
	Out->x = v.x * m._11 + v.y * m._12 + v.z * m._13;
	Out->y = v.x * m._21 + v.y * m._22 + v.z * m._23;
	Out->z = v.x * m._31 + v.y * m._32 + v.z * m._33;
}

void Transform(PLANE *Out, const PLANE &p, const MATRIX &m)
{
	Out->a = p.a * m._11 + p.b * m._21 + p.c * m._31 + p.d * m._41;
	Out->b = p.a * m._12 + p.b * m._22 + p.c * m._32 + p.d * m._42;
	Out->c = p.a * m._13 + p.b * m._23 + p.c * m._33 + p.d * m._43;
	Out->d = p.a * m._14 + p.b * m._24 + p.c * m._34 + p.d * m._44;
}

void TransformArray(VEC3 OutPoints[], const VEC3 InPoints[], size_t PointCount, const MATRIX &M)
{
	for (size_t i = 0; i < PointCount; i++)
		Transform(&OutPoints[i], InPoints[i], M);
}

void TransformArray(VEC3 InOutPoints[], size_t PointCount, const MATRIX &M)
{
	VEC3 v;
	for (size_t i = 0; i < PointCount; i++)
	{
		Transform(&v, InOutPoints[i], M);
		InOutPoints[i] = v;
	}
}

void TransformNormalArray(VEC3 OutPoints[], const VEC3 InPoints[], size_t PointCount, const MATRIX &M)
{
	for (size_t i = 0; i < PointCount; i++)
		TransformNormal(&OutPoints[i], InPoints[i], M);
}

void TransformNormalArray(VEC3 InOutPoints[], size_t PointCount, const MATRIX &M)
{
	VEC3 v;
	for (size_t i = 0; i < PointCount; i++)
	{
		TransformNormal(&v, InOutPoints[i], M);
		InOutPoints[i] = v;
	}
}

void TransformCoordArray(VEC3 OutPoints[], const VEC3 InPoints[], size_t PointCount, const MATRIX &M)
{
	for (size_t i = 0; i < PointCount; i++)
		TransformCoord(&OutPoints[i], InPoints[i], M);
}

void TransformCoordArray(VEC3 InOutPoints[], size_t PointCount, const MATRIX &M)
{
	VEC3 v;
	for (size_t i = 0; i < PointCount; i++)
	{
		TransformCoord(&v, InOutPoints[i], M);
		InOutPoints[i] = v;
	}
}

void TransformRay(VEC3 *OutOrigin, VEC3 *OutDir, const VEC3 &RayOrigin, const VEC3 &RayDir, const MATRIX &m)
{
	Transform(OutOrigin, RayOrigin, m);
	TransformNormal(OutDir, RayDir, m);
}

/* Stary, naiwny algorytm.
void TransformBox(BOX *Out, const BOX &In, const MATRIX &M)
{
	VEC3 Min, Max;

	In.GetCorner(&Min, 0);
	Transform(&Max, Min, M);
	Out->Min = Out->Max = Max;

	for (size_t i = 1; i < 8; i++)
	{
		In.GetCorner(&Min, i);
		Transform(&Max, Min, M);
		Out->AddInternalPoint(Max);
	}
}*/

void TransformBox(BOX *Out, const BOX &In, const MATRIX &M)
{
	/* Nowy algorytm, na podstawie ksi¹¿ki:
	Real-Time Collision Detection, Christer Ericson
	*/

	// For all three axes
	for (uint i = 0; i < 3; i++)
	{
		// Start by adding in translation
		Out->Min[i] = Out->Max[i] = ( (VEC3&)M._41 )[i];
		// Form extent by summing smaller and larger terms respectively
		for (uint j = 0; j < 3; j++)
		{
			float e = M(j, i) * In.Min[j];
			float f = M(j, i) * In.Max[j];
			if (e < f)
			{
				Out->Min[i] += e;
				Out->Max[i] += f;
			}
			else
			{
				Out->Min[i] += f;
				Out->Max[i] += e;
			}
		}
	}
}

void TransformBoxCoord(BOX *Out, const BOX &In, const MATRIX &M)
{
	VEC3 Min, Max;

	In.GetCorner(&Min, 0);
	TransformCoord(&Max, Min, M);
	Out->Min = Out->Max = Max;

	for (uint i = 1; i < 8; i++)
	{
		In.GetCorner(&Min, i);
		TransformCoord(&Max, Min, M);
		Out->AddInternalPoint(Max);
	}
}

/*
Wzór wype³niania macierzy (ta jest jednostkowa):
	Out->_11 = 1.0f; Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = 1.0f; Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f; Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
*/

void MatrixSetTranslation(MATRIX *InOut, float TranslationX, float TranslationY, float TranslationZ)
{
	InOut->_41 = TranslationX;
	InOut->_42 = TranslationY;
	InOut->_43 = TranslationZ;
}

void MatrixSetTranslation(MATRIX *InOut, const VEC3 &Translation)
{
	InOut->_41 = Translation.x;
	InOut->_42 = Translation.y;
	InOut->_43 = Translation.z;
}

void MatrixGetTranslation(float *OutTranslationX, float *OutTranslationY, float *OutTranslationZ, const MATRIX &m)
{
	*OutTranslationX = m._41;
	*OutTranslationY = m._42;
	*OutTranslationZ = m._43;
}

void MatrixGetTranslation(VEC3 *OutTranslation, const MATRIX &m)
{
	OutTranslation->x = m._41;
	OutTranslation->y = m._42;
	OutTranslation->z = m._43;
}

void AxesToMatrix(MATRIX *Out, const VEC3 &AxisX, const VEC3 &AxisY, const VEC3 &AxisZ)
{
	// To by³o Ÿle! Nie wiem sk¹d siê to wziê³o, ale zgodnie z ksi¹¿k¹
	// "3D Math Primer for Graphics and Game Developers" oraz z moimi obliczeniami
	// kiedy wektory s¹ wierszowe i mno¿one prez macierz w kolejnoœci wektor*macierz
	// (czyli tak jak w DirectX) to do tej macierzy wpisujemy poszczególne wektory osi
	// w wierszach, a nie w kolumnach!
	//Out->_11 = AxisX.x; Out->_12 = AxisY.x; Out->_13 = AxisZ.x; Out->_14 = 0.0f;
	//Out->_21 = AxisX.y; Out->_22 = AxisY.y; Out->_23 = AxisZ.y; Out->_24 = 0.0f;
	//Out->_31 = AxisX.z; Out->_32 = AxisY.z; Out->_33 = AxisZ.z; Out->_34 = 0.0f;
	//Out->_41 = 0.0f;    Out->_42 = 0.0f;    Out->_43 = 0.0f;    Out->_44 = 1.0f;

	// Tak jest dobrze:
	Out->_11 = AxisX.x; Out->_12 = AxisX.y; Out->_13 = AxisX.z; Out->_14 = 0.0f;
	Out->_21 = AxisY.x; Out->_22 = AxisY.y; Out->_23 = AxisY.z; Out->_24 = 0.0f;
	Out->_31 = AxisZ.x; Out->_32 = AxisZ.y; Out->_33 = AxisZ.z; Out->_34 = 0.0f;
	Out->_41 = 0.0f;    Out->_42 = 0.0f;    Out->_43 = 0.0f;    Out->_44 = 1.0f;
}

void AxesToMatrixTranslation(MATRIX *Out, const VEC3 &Origin, const VEC3 &AxisX, const VEC3 &AxisY, const VEC3 &AxisZ)
{
	// Na podstawie powy¿szej (w wersji ju¿ poprawnionej):
	Out->_11 = AxisX.x;  Out->_12 = AxisX.y;  Out->_13 = AxisX.z;  Out->_14 = 0.0f;
	Out->_21 = AxisY.x;  Out->_22 = AxisY.y;  Out->_23 = AxisY.z;  Out->_24 = 0.0f;
	Out->_31 = AxisZ.x;  Out->_32 = AxisZ.y;  Out->_33 = AxisZ.z;  Out->_34 = 0.0f;
	Out->_41 = Origin.x; Out->_42 = Origin.y; Out->_43 = Origin.z; Out->_44 = 1.0f;
}

void LookAtLH(MATRIX *Out, const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up)
{
	VEC3 zaxis = Forward;
	Normalize(&zaxis);
	VEC3 xaxis; Cross(&xaxis, Up, zaxis);
	Normalize(&xaxis);
	VEC3 yaxis; Cross(&yaxis, zaxis, xaxis);

	Out->_11 = xaxis.x; Out->_12 = yaxis.x; Out->_13 = zaxis.x; Out->_14 = 0.0f;
	Out->_21 = xaxis.y; Out->_22 = yaxis.y; Out->_23 = zaxis.y; Out->_24 = 0.0f;
	Out->_31 = xaxis.z; Out->_32 = yaxis.z; Out->_33 = zaxis.z; Out->_34 = 0.0f;
	Out->_41 = -Dot(xaxis, Eye); Out->_42 = -Dot(yaxis, Eye); Out->_43 = -Dot(zaxis, Eye); Out->_44 = 1.0f;
}

void LookAtRH(MATRIX *Out, const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up)
{
	VEC3 zaxis = - Forward;
	Normalize(&zaxis);
	VEC3 xaxis; Cross(&xaxis, Up, zaxis);
	Normalize(&xaxis);
	VEC3 yaxis; Cross(&yaxis, zaxis, xaxis);

	Out->_11 = xaxis.x; Out->_12 = yaxis.x; Out->_13 = zaxis.x; Out->_14 = 0.0f;
	Out->_21 = xaxis.y; Out->_22 = yaxis.y; Out->_23 = zaxis.y; Out->_24 = 0.0f;
	Out->_31 = xaxis.z; Out->_32 = yaxis.z; Out->_33 = zaxis.z; Out->_34 = 0.0f;
	Out->_41 = -Dot(xaxis, Eye); Out->_42 = -Dot(yaxis, Eye); Out->_43 = -Dot(zaxis, Eye); Out->_44 = 1.0f;
}

void Identity(MATRIX *Out)
{
	Out->_11 = 1.0f; Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = 1.0f; Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f; Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void Translation(MATRIX *Out, float x, float y, float z)
{
	Out->_11 = 1.0f; Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = 1.0f; Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f; Out->_34 = 0.0f;
	Out->_41 = x;    Out->_42 = y;    Out->_43 = z;    Out->_44 = 1.0f;
}

void Translation(MATRIX *Out, const VEC3 &v)
{
	Out->_11 = 1.0f; Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = 1.0f; Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f; Out->_34 = 0.0f;
	Out->_41 = v.x;  Out->_42 = v.y;  Out->_43 = v.z;  Out->_44 = 1.0f;
}

void Scaling(MATRIX *Out, float sx, float sy, float sz)
{
	Out->_11 = sx;   Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = sy;   Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = sz;   Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void Scaling(MATRIX *Out, const VEC3 &sv)
{
	Out->_11 = sv.x; Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = sv.y; Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = sv.z; Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void Scaling(MATRIX *Out, float s)
{
	Out->_11 = s;    Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = s;    Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = s;    Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void ScalingAxis(MATRIX *Out, const VEC3 &n, float k)
{
	float k1 = k - 1.0f;
	Out->_11 = 1.0f+k1*n.x*n.x; Out->_12 =      k1*n.x*n.y; Out->_13 =      k1*n.x*n.z; Out->_14 = 0.0f;
	Out->_21 =      k1*n.x*n.y; Out->_22 = 1.0f+k1*n.y*n.y; Out->_23 =      k1*n.y*n.z; Out->_24 = 0.0f;
	Out->_31 =      k1*n.x*n.z; Out->_32 =      k1*n.y*n.z; Out->_33 = 1.0f+k1*n.z*n.z; Out->_34 = 0.0f;
	Out->_41 = 0.0f;            Out->_42 = 0.0f;            Out->_43 = 0.0f;            Out->_44 = 1.0f;
}

void Rotation2D(MATRIX *Out, float Angle)
{
	float s = sinf(Angle);
	float c = cosf(Angle);
	Out->_11 = c;    Out->_12 = - s;  Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = s;    Out->_22 =   c;  Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f; Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void RotationX(MATRIX *Out, float Angle)
{
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);
	Out->_11 = 1.0f; Out->_12 = 0.0f; Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = Cos;  Out->_23 = Sin;  Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = -Sin; Out->_33 = Cos;  Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void RotationY(MATRIX *Out, float Angle)
{
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);
	Out->_11 = Cos;  Out->_12 = 0.0f; Out->_13 = -Sin; Out->_14 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = 1.0f; Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = Sin;  Out->_32 = 0.0f; Out->_33 = Cos;  Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void RotationZ(MATRIX *Out, float Angle)
{
	float Sin = sinf(Angle);
	float Cos = cosf(Angle);
	Out->_11 = Cos;  Out->_12 = Sin;  Out->_13 = 0.0f; Out->_14 = 0.0f;
	Out->_21 = -Sin; Out->_22 = Cos;  Out->_23 = 0.0f; Out->_24 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f; Out->_34 = 0.0f;
	Out->_41 = 0.0f; Out->_42 = 0.0f; Out->_43 = 0.0f; Out->_44 = 1.0f;
}

void RotationAxisLH(MATRIX *Out, const VEC3 &Axis, float Angle)
{
	float c = cosf(Angle), s = sinf(Angle), t = 1.0f - c;
	Out->_11 = t*Axis.x*Axis.x+c;        Out->_12 = t*Axis.x*Axis.y+s*Axis.z; Out->_13 = t*Axis.x*Axis.z-s*Axis.y; Out->_14 = 0.0f;
	Out->_21 = t*Axis.x*Axis.y-s*Axis.z; Out->_22 = t*Axis.y*Axis.y+c;        Out->_23 = t*Axis.y*Axis.z+s*Axis.x; Out->_24 = 0.0f;
	Out->_31 = t*Axis.x*Axis.z+s*Axis.y; Out->_32 = t*Axis.y*Axis.z-s*Axis.x; Out->_33 = t*Axis.z*Axis.z+c;        Out->_34 = 0.0f;
	Out->_41 = 0.0f;                     Out->_42 = 0.0f;                     Out->_43 = 0.0f;                     Out->_44 = 1.0f;
}

/* Tu by³y funkcje takie jak ta, wszystkie 6 kombinacji, ale one s¹ 1. powolne 2. niepotrzebne
void RotationYZX(MATRIX *Out, float Yaw, float Pitch, float Roll)
{
	MATRIX my, mx, mz, Out1;
	RotationY(&my, Yaw);
	RotationX(&mx, Pitch);
	RotationZ(&mz, Roll);
	Mul(&Out1, my, mz);
	Mul(Out, Out1, mx);
}*/

void RotationYawPitchRoll(MATRIX *Out, float Yaw, float Pitch, float Roll)
{
	// Wersja oryginalna:
	//MATRIX my, mx, mz, Out1;
	//RotationY(&my, Yaw);
	//RotationX(&mx, Pitch);
	//RotationZ(&mz, Roll);
	//Mul(&Out1, mz, mx);
	//Mul(Out, Out1, my);

	// Wersja rozpisana, du¿o szybsza:
	float sy = sinf(Yaw), cy = cosf(Yaw);
	float sp = sinf(Pitch), cp = cosf(Pitch);
	float sr = sinf(Roll), cr = cosf(Roll);
	Out->_11 = cy*cr+sy*sp*sr;  Out->_12 = sr*cp; Out->_13 = -sy*cr+cy*sp*sr; Out->_14 = 0.0f;
	Out->_21 = -cy*sr+sy*sp*cr; Out->_22 = cr*cp; Out->_23 = sr*sy+cy*sp*cr;  Out->_24 = 0.0f;
	Out->_31 = sy*cp;           Out->_32 = -sp;   Out->_33 = cy*cp;           Out->_34 = 0.0f;
	Out->_41 = 0.0f;            Out->_42 = 0.0f;  Out->_43 = 0.0f;            Out->_44 = 1.0f;
}

void RotationYawPitchRoll(MATRIX *Out, const VEC3 &EulerAngles)
{
	float sy = sinf(EulerAngles.y), cy = cosf(EulerAngles.y);
	float sp = sinf(EulerAngles.x), cp = cosf(EulerAngles.x);
	float sr = sinf(EulerAngles.z), cr = cosf(EulerAngles.z);
	Out->_11 = cy*cr+sy*sp*sr;  Out->_12 = sr*cp; Out->_13 = -sy*cr+cy*sp*sr; Out->_14 = 0.0f;
	Out->_21 = -cy*sr+sy*sp*cr; Out->_22 = cr*cp; Out->_23 = sr*sy+cy*sp*cr;  Out->_24 = 0.0f;
	Out->_31 = sy*cp;           Out->_32 = -sp;   Out->_33 = cy*cp;           Out->_34 = 0.0f;
	Out->_41 = 0.0f;            Out->_42 = 0.0f;  Out->_43 = 0.0f;            Out->_44 = 1.0f;
}

void RotationYawPitchRollInv(MATRIX *Out, float Yaw, float Pitch, float Roll)
{
	// Wersja rozpisana, du¿o szybsza:
	float sy = sinf(Yaw), cy = cosf(Yaw);
	float sp = sinf(Pitch), cp = cosf(Pitch);
	float sr = sinf(Roll), cr = cosf(Roll);
	Out->_11 = cy*cr+sy*sp*sr;  Out->_12 = -cy*sr+sy*sp*cr; Out->_13 = sy*cp; Out->_14 = 0.0f;
	Out->_21 = sr*cp;           Out->_22 = cr*cp;           Out->_23 = -sp;   Out->_24 = 0.0f;
	Out->_31 = -sy*cr+cy*sp*sr; Out->_32 = sr*sy+cy*sp*cr;  Out->_33 = cy*cp; Out->_34 = 0.0f;
	Out->_41 = 0.0f;            Out->_42 = 0.0f;            Out->_43 = 0.0f;  Out->_44 = 1.0f;
}

void RotationYawPitchRollInv(MATRIX *Out, const VEC3 &EulerAngles)
{
	float sy = sinf(EulerAngles.y), cy = cosf(EulerAngles.y);
	float sp = sinf(EulerAngles.x), cp = cosf(EulerAngles.x);
	float sr = sinf(EulerAngles.z), cr = cosf(EulerAngles.z);
	Out->_11 = cy*cr+sy*sp*sr;  Out->_12 = -cy*sr+sy*sp*cr; Out->_13 = sy*cp; Out->_14 = 0.0f;
	Out->_21 = sr*cp;           Out->_22 = cr*cp;           Out->_23 = -sp;   Out->_24 = 0.0f;
	Out->_31 = -sy*cr+cy*sp*sr; Out->_32 = sr*sy+cy*sp*cr;  Out->_33 = cy*cp; Out->_34 = 0.0f;
	Out->_41 = 0.0f;            Out->_42 = 0.0f;            Out->_43 = 0.0f;  Out->_44 = 1.0f;
}

// Napisane na podstawie ksi¹¿ki "3D Math Primer for Graphics and Game Development".
// Niestety nie dzia³a.
// Po wygenerowaniu macierzy za pomoc¹ funkcji RotationYawPitchRoll (próbowa³em te¿ RotationYawPitchRollInv)
// i wyci¹gniêciu z powrotem k¹tów Eulera wychodz¹ zupe³nie inne liczby - nie przeciwne, nie podobne,
// nie zerowe czy INF, po prostu zupe³nie inne i nie wiem dlaczego :(
/*void RotationMatrixToEulerAngles(const MATRIX &m, float *Yaw, float *Pitch, float *Roll)
{
	// Extract pitch from m23, being careful for domain errors with asin().
	// We could have values slightly out of range due to floating point arithmetic.
	float sp = -m._23;
	if (sp <= -1.0f)
		*Pitch = -PI_2;
	else if (sp >= 1.0f)
		*Pitch = PI_2;
	else
		*Pitch = asinf(sp);

	// Check for the Gimbal lock case, giving a slight tolerance for numerical imprecision
	if (sp > 0.9999f)
	{
		// We are looking straight up or down.
		// Slam roll to zero and host set yaw.
		*Roll = 0.0f;
		*Yaw = atan2f(-m._31, m._11);
	}
	else
	{
		// Compute yaw from m13 and m33
		*Yaw = atan2f(m._13, m._33);
		// Compute roll from m21 and m22
		*Roll = atan2f(m._21, m._22);
	}
}*/

void OrthoLH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar)
{
	Out->_11 = 2.0f/Width;  Out->_12 = 0.0f;        Out->_13 = 0.0f;               Out->_14 = 0.0f;
	Out->_21 = 0.0f;        Out->_22 = 2.0f/Height; Out->_23 = 0.0f;               Out->_24 = 0.0f;
	Out->_31 = 0.0f;        Out->_32 = 0.0f;        Out->_33 = 1.0f/(ZFar-ZNear);  Out->_34 = 0.0f;
	Out->_41 = 0.0f;        Out->_42 = 0.0f;        Out->_43 = ZNear/(ZNear-ZFar); Out->_44 = 1.0f;
}

void OrthoRH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar)
{
	Out->_11 = 2.0f/Width;  Out->_12 = 0.0f;        Out->_13 = 0.0f;               Out->_14 = 0.0f;
	Out->_21 = 0.0f;        Out->_22 = 2.0f/Height; Out->_23 = 0.0f;               Out->_24 = 0.0f;
	Out->_31 = 0.0f;        Out->_32 = 0.0f;        Out->_33 = 1.0f/(ZNear-ZFar);  Out->_34 = 0.0f;
	Out->_41 = 0.0f;        Out->_42 = 0.0f;        Out->_43 = ZNear/(ZNear-ZFar); Out->_44 = 1.0f;
}

void OrthoOffCenterLH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar)
{
	Out->_11 = 2.0f/(Right-Left);         Out->_12 = 0.0f;                      Out->_13 = 0.0f;               Out->_14 = 0.0f;
	Out->_21 = 0.0f;                      Out->_22 = 2.0f/(Top-Bottom);         Out->_23 = 0.0f;               Out->_24 = 0.0f;
	Out->_31 = 0.0f;                      Out->_32 = 0.0f;                      Out->_33 = 1.0f/(ZFar-ZNear);  Out->_34 = 0.0f;
	Out->_41 = (Left+Right)/(Left-Right); Out->_42 = (Top+Bottom)/(Bottom-Top); Out->_43 = ZNear/(ZNear-ZFar); Out->_44 = 1.0f;
}

void OrthoOffCenterRH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar)
{
	Out->_11 = 2.0f/(Right-Left);         Out->_12 = 0.0f;                      Out->_13 = 0.0f;               Out->_14 = 0.0f;
	Out->_21 = 0.0f;                      Out->_22 = 2.0f/(Top-Bottom);         Out->_23 = 0.0f;               Out->_24 = 0.0f;
	Out->_31 = 0.0f;                      Out->_32 = 0.0f;                      Out->_33 = 1.0f/(ZNear-ZFar);  Out->_34 = 0.0f;
	Out->_41 = (Left+Right)/(Left-Right); Out->_42 = (Top+Bottom)/(Bottom-Top); Out->_43 = ZNear/(ZNear-ZFar); Out->_44 = 1.0f;
}

void PerspectiveLH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar)
{
	Out->_11 = 2.0f*ZNear/Width; Out->_12 = 0.0f;              Out->_13 = 0.0f;                    Out->_14 = 0.0f;
	Out->_21 = 0.0f;             Out->_22 = 2.0f*ZNear/Height; Out->_23 = 0.0f;                    Out->_24 = 0.0f;
	Out->_31 = 0.0f;             Out->_32 = 0.0f;              Out->_33 = ZFar/(ZFar-ZNear);       Out->_34 = 1.0f;
	Out->_41 = 0.0f;             Out->_42 = 0.0f;              Out->_43 = ZNear*ZFar/(ZNear-ZFar); Out->_44 = 0.0f;
}

void PerspectiveRH(MATRIX *Out, float Width, float Height, float ZNear, float ZFar)
{
	Out->_11 = 2.0f*ZNear/Width; Out->_12 = 0.0f;              Out->_13 = 0.0f;                    Out->_14 =  0.0f;
	Out->_21 = 0.0f;             Out->_22 = 2.0f*ZNear/Height; Out->_23 = 0.0f;                    Out->_24 =  0.0f;
	Out->_31 = 0.0f;             Out->_32 = 0.0f;              Out->_33 = ZFar/(ZNear-ZFar);       Out->_34 = -1.0f;
	Out->_41 = 0.0f;             Out->_42 = 0.0f;              Out->_43 = ZNear*ZFar/(ZNear-ZFar); Out->_44 =  0.0f;
}

void PerspectiveFovLH(MATRIX *Out, float FovY, float Aspect, float ZNear, float ZFar)
{
	float YScale = 1.0f/tanf(FovY*0.5f); // cot
	float XScale = YScale / Aspect;
	Out->_11 = XScale; Out->_12 = 0.0f;   Out->_13 = 0.0f;                     Out->_14 = 0.0f;
	Out->_21 = 0.0f;   Out->_22 = YScale; Out->_23 = 0.0f;                     Out->_24 = 0.0f;
	Out->_31 = 0.0f;   Out->_32 = 0.0f;   Out->_33 = ZFar/(ZFar-ZNear);        Out->_34 = 1.0f;
	Out->_41 = 0.0f;   Out->_42 = 0.0f;   Out->_43 = -ZNear*ZFar/(ZFar-ZNear); Out->_44 = 0.0f;
}

void PerspectiveFovRH(MATRIX *Out, float FovY, float Aspect, float ZNear, float ZFar)
{
	float YScale = 1.0f/tanf(FovY*0.5f); // cot
	float XScale = YScale / Aspect;
	Out->_11 = XScale; Out->_12 = 0.0f;   Out->_13 = 0.0f;                     Out->_14 =  0.0f;
	Out->_21 = 0.0f;   Out->_22 = YScale; Out->_23 = 0.0f;                     Out->_24 =  0.0f;
	Out->_31 = 0.0f;   Out->_32 = 0.0f;   Out->_33 = ZFar/(ZNear-ZFar);        Out->_34 = -1.0f;
	Out->_41 = 0.0f;   Out->_42 = 0.0f;   Out->_43 = ZNear*ZFar/(ZNear-ZFar);  Out->_44 =  0.0f;
}

void PerspectiveOffCenterLH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar)
{
	Out->_11 = 2.0f*ZNear/(Right-Left);   Out->_12 = 0.0f;                      Out->_13 = 0.0f;                    Out->_14 = 0.0f;
	Out->_21 = 0.0f;                      Out->_22 = 2.0f*ZNear/(Top-Bottom);   Out->_23 = 0.0f;                    Out->_24 = 0.0f;
	Out->_31 = (Left+Right)/(Left-Right); Out->_32 = (Top+Bottom)/(Bottom-Top); Out->_33 = ZFar/(ZFar-ZNear);       Out->_34 = 1.0f;
	Out->_41 = 0.0f;                      Out->_42 = 0.0f;                      Out->_43 = ZNear*ZFar/(ZNear-ZFar); Out->_44 = 0.0f;
}

void PerspectiveOffCenterRH(MATRIX *Out, float Left, float Right, float Bottom, float Top, float ZNear, float ZFar)
{
	Out->_11 = 2.0f*ZNear/(Right-Left);   Out->_12 = 0.0f;                      Out->_13 = 0.0f;                    Out->_14 =  0.0f;
	Out->_21 = 0.0f;                      Out->_22 = 2.0f*ZNear/(Top-Bottom);   Out->_23 = 0.0f;                    Out->_24 =  0.0f;
	Out->_31 = (Left+Right)/(Right-Left); Out->_32 = (Top+Bottom)/(Top-Bottom); Out->_33 = ZFar/(ZNear-ZFar);       Out->_34 = -1.0f;
	Out->_41 = 0.0f;                      Out->_42 = 0.0f;                      Out->_43 = ZNear*ZFar/(ZNear-ZFar); Out->_44 =  0.0f;
}

void PerspectiveFovLH_Inf(MATRIX *Out, float FovY, float Aspect, float ZNear)
{
	float YScale = 1.0f/tanf(FovY*0.5f);
	float XScale = YScale / Aspect;
	Out->_11 = XScale; Out->_12 = 0.0f;   Out->_13 = 0.0f;   Out->_14 = 0.0f;
	Out->_21 = 0.0f;   Out->_22 = YScale; Out->_23 = 0.0f;   Out->_24 = 0.0f;
	Out->_31 = 0.0f;   Out->_32 = 0.0f;   Out->_33 = 1.0f;   Out->_34 = 1.0f;
	Out->_41 = 0.0f;   Out->_42 = 0.0f;   Out->_43 = -ZNear; Out->_44 = 0.0f;
}

void MatrixShadow(MATRIX *Out, const VEC4 &Light, const PLANE &Plane)
{
	float D = Dot(Plane, Light);
	Out->_11 = Plane.a * Light.x + D; Out->_12 = Plane.a * Light.y;     Out->_13 = Plane.a * Light.z;     Out->_14 = Plane.a * Light.w;
	Out->_21 = Plane.b * Light.x;     Out->_22 = Plane.b * Light.y + D; Out->_23 = Plane.b * Light.z;     Out->_24 = Plane.b * Light.w;
	Out->_31 = Plane.c * Light.x;     Out->_32 = Plane.c * Light.y;     Out->_33 = Plane.c * Light.z + D; Out->_34 = Plane.c * Light.w;
	Out->_41 = Plane.d * Light.x;     Out->_42 = Plane.d * Light.y;     Out->_43 = Plane.d * Light.z;     Out->_44 = Plane.d * Light.w + D;
}

void MatrixReflect(MATRIX *Out, const PLANE &p)
{
	Out->_11 = -2.0f * p.a * p.a + 1.0f; Out->_12 = -2.0f * p.b * p.a;        Out->_13 = -2.0f * p.c * p.a;        Out->_14 = 0.0f;
	Out->_21 = -2.0f * p.a * p.b;        Out->_22 = -2.0f * p.b * p.b + 1.0f; Out->_23 = -2.0f * p.c * p.b;        Out->_24 = 0.0f;
	Out->_31 = -2.0f * p.a * p.c;        Out->_32 = -2.0f * p.b * p.c;        Out->_33 = -2.0f * p.c * p.c + 1.0f; Out->_34 = 0.0f;
	Out->_41 = -2.0f * p.a * p.d;        Out->_42 = -2.0f * p.b * p.d;        Out->_43 = -2.0f * p.c * p.d;        Out->_44 = 1.0f;
}

void Transpose(MATRIX *m)
{
	std::swap(m->_12, m->_21);
	std::swap(m->_13, m->_31);
	std::swap(m->_14, m->_41);
	std::swap(m->_23, m->_32);
	std::swap(m->_24, m->_42);
	std::swap(m->_34, m->_43);
}

void Transpose(MATRIX *Out, const MATRIX &m)
{
    *Out = MATRIX(
        m._11, m._21, m._31, m._41,
        m._12, m._22, m._32, m._42,
        m._13, m._23, m._33, m._43,
        m._14, m._24, m._34, m._44);
}

void Lerp(MATRIX *Out, const MATRIX &m1, const MATRIX &m2, float t)
{
	Out->_11 = Lerp(m1._11, m2._11, t);
	Out->_12 = Lerp(m1._12, m2._12, t);
	Out->_13 = Lerp(m1._13, m2._13, t);
	Out->_14 = Lerp(m1._14, m2._14, t);

	Out->_21 = Lerp(m1._21, m2._21, t);
	Out->_22 = Lerp(m1._22, m2._22, t);
	Out->_23 = Lerp(m1._23, m2._23, t);
	Out->_24 = Lerp(m1._24, m2._24, t);

	Out->_31 = Lerp(m1._31, m2._31, t);
	Out->_32 = Lerp(m1._32, m2._32, t);
	Out->_33 = Lerp(m1._33, m2._33, t);
	Out->_34 = Lerp(m1._34, m2._34, t);

	Out->_41 = Lerp(m1._41, m2._41, t);
	Out->_42 = Lerp(m1._42, m2._42, t);
	Out->_43 = Lerp(m1._43, m2._43, t);
	Out->_44 = Lerp(m1._44, m2._44, t);
}

float Det(const MATRIX &m)
{
	return
		(m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) - (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) +
		(m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)) + (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) -
		(m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3));
}

bool Inverse(MATRIX *Out, const MATRIX &m)
{
	float d = Det(m);
	if (d == 0.0f) return false;
	d = 1.0f / d;
	(*Out)(0, 0) = d * (m(1, 1) * (m(2, 2) * m(3, 3) - m(3, 2) * m(2, 3)) + m(2, 1) * (m(3, 2) * m(1, 3) - m(1, 2) * m(3, 3)) + m(3, 1) * (m(1, 2) * m(2, 3) - m(2, 2) * m(1, 3)));
	(*Out)(1, 0) = d * (m(1, 2) * (m(2, 0) * m(3, 3) - m(3, 0) * m(2, 3)) + m(2, 2) * (m(3, 0) * m(1, 3) - m(1, 0) * m(3, 3)) + m(3, 2) * (m(1, 0) * m(2, 3) - m(2, 0) * m(1, 3)));
	(*Out)(2, 0) = d * (m(1, 3) * (m(2, 0) * m(3, 1) - m(3, 0) * m(2, 1)) + m(2, 3) * (m(3, 0) * m(1, 1) - m(1, 0) * m(3, 1)) + m(3, 3) * (m(1, 0) * m(2, 1) - m(2, 0) * m(1, 1)));
	(*Out)(3, 0) = d * (m(1, 0) * (m(3, 1) * m(2, 2) - m(2, 1) * m(3, 2)) + m(2, 0) * (m(1, 1) * m(3, 2) - m(3, 1) * m(1, 2)) + m(3, 0) * (m(2, 1) * m(1, 2) - m(1, 1) * m(2, 2)));
	(*Out)(0, 1) = d * (m(2, 1) * (m(0, 2) * m(3, 3) - m(3, 2) * m(0, 3)) + m(3, 1) * (m(2, 2) * m(0, 3) - m(0, 2) * m(2, 3)) + m(0, 1) * (m(3, 2) * m(2, 3) - m(2, 2) * m(3, 3)));
	(*Out)(1, 1) = d * (m(2, 2) * (m(0, 0) * m(3, 3) - m(3, 0) * m(0, 3)) + m(3, 2) * (m(2, 0) * m(0, 3) - m(0, 0) * m(2, 3)) + m(0, 2) * (m(3, 0) * m(2, 3) - m(2, 0) * m(3, 3)));
	(*Out)(2, 1) = d * (m(2, 3) * (m(0, 0) * m(3, 1) - m(3, 0) * m(0, 1)) + m(3, 3) * (m(2, 0) * m(0, 1) - m(0, 0) * m(2, 1)) + m(0, 3) * (m(3, 0) * m(2, 1) - m(2, 0) * m(3, 1)));
	(*Out)(3, 1) = d * (m(2, 0) * (m(3, 1) * m(0, 2) - m(0, 1) * m(3, 2)) + m(3, 0) * (m(0, 1) * m(2, 2) - m(2, 1) * m(0, 2)) + m(0, 0) * (m(2, 1) * m(3, 2) - m(3, 1) * m(2, 2)));
	(*Out)(0, 2) = d * (m(3, 1) * (m(0, 2) * m(1, 3) - m(1, 2) * m(0, 3)) + m(0, 1) * (m(1, 2) * m(3, 3) - m(3, 2) * m(1, 3)) + m(1, 1) * (m(3, 2) * m(0, 3) - m(0, 2) * m(3, 3)));
	(*Out)(1, 2) = d * (m(3, 2) * (m(0, 0) * m(1, 3) - m(1, 0) * m(0, 3)) + m(0, 2) * (m(1, 0) * m(3, 3) - m(3, 0) * m(1, 3)) + m(1, 2) * (m(3, 0) * m(0, 3) - m(0, 0) * m(3, 3)));
	(*Out)(2, 2) = d * (m(3, 3) * (m(0, 0) * m(1, 1) - m(1, 0) * m(0, 1)) + m(0, 3) * (m(1, 0) * m(3, 1) - m(3, 0) * m(1, 1)) + m(1, 3) * (m(3, 0) * m(0, 1) - m(0, 0) * m(3, 1)));
	(*Out)(3, 2) = d * (m(3, 0) * (m(1, 1) * m(0, 2) - m(0, 1) * m(1, 2)) + m(0, 0) * (m(3, 1) * m(1, 2) - m(1, 1) * m(3, 2)) + m(1, 0) * (m(0, 1) * m(3, 2) - m(3, 1) * m(0, 2)));
	(*Out)(0, 3) = d * (m(0, 1) * (m(2, 2) * m(1, 3) - m(1, 2) * m(2, 3)) + m(1, 1) * (m(0, 2) * m(2, 3) - m(2, 2) * m(0, 3)) + m(2, 1) * (m(1, 2) * m(0, 3) - m(0, 2) * m(1, 3)));
	(*Out)(1, 3) = d * (m(0, 2) * (m(2, 0) * m(1, 3) - m(1, 0) * m(2, 3)) + m(1, 2) * (m(0, 0) * m(2, 3) - m(2, 0) * m(0, 3)) + m(2, 2) * (m(1, 0) * m(0, 3) - m(0, 0) * m(1, 3)));
	(*Out)(2, 3) = d * (m(0, 3) * (m(2, 0) * m(1, 1) - m(1, 0) * m(2, 1)) + m(1, 3) * (m(0, 0) * m(2, 1) - m(2, 0) * m(0, 1)) + m(2, 3) * (m(1, 0) * m(0, 1) - m(0, 0) * m(1, 1)));
	(*Out)(3, 3) = d * (m(0, 0) * (m(1, 1) * m(2, 2) - m(2, 1) * m(1, 2)) + m(1, 0) * (m(2, 1) * m(0, 2) - m(0, 1) * m(2, 2)) + m(2, 0) * (m(0, 1) * m(1, 2) - m(1, 1) * m(0, 2)));
	return true;
}

void Matrix33ToMatrix(MATRIX *Out, const MATRIX33 &m)
{
	Out->_11 = m._11; Out->_12 = m._12; Out->_13 = m._13; Out->_14 = 0.0f;
	Out->_21 = m._21; Out->_22 = m._22; Out->_23 = m._23; Out->_24 = 0.0f;
	Out->_31 = m._31; Out->_32 = m._32; Out->_33 = m._33; Out->_34 = 0.0f;
	Out->_41 = 0.0f;  Out->_42 = 0.0f;  Out->_43 = 0.0f;  Out->_44 = 1.0f;
}

void MatrixToMatrix33(MATRIX33 *Out, const MATRIX &m)
{
	Out->_11 = m._11; Out->_12 = m._12; Out->_13 = m._13;
	Out->_21 = m._21; Out->_22 = m._22; Out->_23 = m._23;
	Out->_31 = m._31; Out->_32 = m._32; Out->_33 = m._33;
}

void Identity(MATRIX33 *Out)
{
	Out->_11 = 1.0f; Out->_12 = 0.0f; Out->_13 = 0.0f;
	Out->_21 = 0.0f; Out->_22 = 1.0f; Out->_23 = 0.0f;
	Out->_31 = 0.0f; Out->_32 = 0.0f; Out->_33 = 1.0f;
}

void Transpose(MATRIX33 *m)
{
	std::swap(m->_12, m->_21);
	std::swap(m->_13, m->_31);
	std::swap(m->_23, m->_32);
}

void Transpose(MATRIX33 *Out, const MATRIX33 &m)
{
	Out->_11 = m._11; Out->_12 = m._21; Out->_13 = m._31;
	Out->_21 = m._12; Out->_22 = m._22; Out->_23 = m._32;
	Out->_31 = m._13; Out->_32 = m._23; Out->_33 = m._33;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// KWATERNION

const QUATERNION QUATERNION_ZERO = QUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
const QUATERNION QUATERNION_IDENTITY = QUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

void CalcQuaternionAxis(VEC3 *Out, const QUATERNION &Q)
{
	// Compute sin^2(theta/2). Remember that w = cos(theta/2),
	// and sin^2(x) + cos^2(x) = 1
	float SinThetaOver25q = 1.0f - Q.w * Q.w;
	// Protect against numerical imprecision
	if (SinThetaOver25q <= 0.0f)
	{
		// Identity quaternion, or numerical imprecision. Just
		// return any valid vector, since it doesn't matter
		Out->x = 0.0f;
		Out->y = 0.0f;
		Out->z = 0.0f;
		return;
	}
	// Compite 1 / sin(theta/2)
	float OneOverSinThetaOver2 = 1.0f / sqrtf(SinThetaOver25q);
	// Return axis of rotation
	Out->x = Q.x * OneOverSinThetaOver2;
	Out->y = Q.y * OneOverSinThetaOver2;
	Out->z = Q.z * OneOverSinThetaOver2;
}

// == Do klasy QUATERNION ==
// Wersja stara sprzed odwrócenia kolejnoœci:
// QUATERNION operator * (const QUATERNION &q) const { return QUATERNION(q.x * w + q.y * z - q.z * y + q.w * x, q.y * w - q.x * z + q.w * y + q.z * x, q.z * w + q.w * z + q.x * y - q.y * x, q.w * w - q.z * z - q.y * y - q.x * x); }
// QUATERNION & operator *= (const QUATERNION &q) { QUATERNION R; R.x = q.x * w + q.y * z - q.z * y + q.w * x; R.y = q.y * w - q.x * z + q.w * y + q.z * x; R.z = q.z * w + q.w * z + q.x * y - q.y * x; R.w = q.w * w - q.z * z - q.y * y - q.x * x; *this = R; return *this; }


void Minus(QUATERNION *q)
{
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
	q->w = -q->w;
}

void Add(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2)
{
	Out->x = q1.x + q2.x;
	Out->y = q1.y + q2.y;
	Out->z = q1.z + q2.z;
	Out->w = q1.w + q2.w;
}

void Sub(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2)
{
	Out->x = q1.x - q2.x;
	Out->y = q1.y - q2.y;
	Out->z = q1.z - q2.z;
	Out->w = q1.w - q2.w;
}

void Mul(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2)
{
/* Stara wersja - chyba identyczna, ale to ni¿ej napisane ³adnie
	Out->x = q1.w * q2.x + q1.z * q2.y - q1.y * q2.z + q1.x * q2.w;
	Out->y = q1.w * q2.y - q1.z * q2.x + q1.y * q2.w + q1.x * q2.z;
	Out->z = q1.w * q2.z + q1.z * q2.w + q1.y * q2.x - q1.x * q2.y;
	Out->w = q1.w * q2.w - q1.z * q2.z - q1.y * q2.y - q1.x * q2.x;

	Out->x = q1.w*q2.x + q1.x*q2.w + q1.z*q2.y - q1.y*q2.z;
	Out->y = q1.w*q2.y + q1.y*q2.w + q1.x*q2.z - q1.z*q2.x;
	Out->z = q1.w*q2.z + q1.z*q2.w + q1.y*q2.x - q1.x*q2.y;
	Out->w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;

	Nowa wersja odwrócona, ¿eby mno¿enie by³o nie od koñca jak w teorii tylko tak jak w macierzach.
*/

	Out->x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	Out->y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z;
	Out->z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
	Out->w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
}

void Mul(QUATERNION *q, float v)
{
	q->x *= v;
	q->y *= v;
	q->z *= v;
	q->w *= v;
}

void Div(QUATERNION *q, float v)
{
	v = 1.0f / v;
	q->x *= v;
	q->y *= v;
	q->z *= v;
	q->w *= v;
}

void AxisToQuaternion(QUATERNION *Out, const VEC3 &Axis, float Angle)
{
	Angle *= 0.5f;
	float Sin = sinf(Angle);
	Out->x = Sin*Axis.x;
	Out->y = Sin*Axis.y;
	Out->z = Sin*Axis.z;
	Out->w = cosf(Angle);
}

void RotationMatrixToQuaternion(QUATERNION *Out, const MATRIX &RotationMatrix)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	float Trace = RotationMatrix(0, 0) + RotationMatrix(1, 1) + RotationMatrix(2, 2);
	float Root;

	if (Trace > 0.0f)
	{
		Root = sqrtf(Trace + 1.0f);
		Out->w = 0.5f * Root;
		Root = 0.5f/Root;
		Out->x = -(RotationMatrix(2, 1)-RotationMatrix(1, 2)) * Root;
		Out->y = -(RotationMatrix(0, 2)-RotationMatrix(2, 0)) * Root;
		Out->z = -(RotationMatrix(1, 0)-RotationMatrix(0, 1)) * Root;
	}
	else
	{
		static uint Next[3] = { 1, 2, 0 };
		uint i = 0;
		if (RotationMatrix(1, 1) > RotationMatrix(0, 0))
			i = 1;
		if (RotationMatrix(2, 2) > RotationMatrix(i, i))
			i = 2;
		uint j = Next[i];
		uint k = Next[j];

		Root = sqrtf(RotationMatrix(i, i) - RotationMatrix(j, j) - RotationMatrix(k, k) + 1.0f);
		float *apkQuat[3] = { &Out->x, &Out->y, &Out->z };
		*apkQuat[i] = -0.5f * Root;
		Root = 0.5f / Root;
		Out->w = (RotationMatrix(k, j)-RotationMatrix(j, k)) * Root;
		*apkQuat[j] = -(RotationMatrix(j, i)+RotationMatrix(i, j))*Root;
		*apkQuat[k] = -(RotationMatrix(k, i)+RotationMatrix(i, k))*Root;
	}
}

/*
======== Wersja 1 ========

Poprawna, ale wolniejsza.

	Out->_11 = 1.0f-2.0f*q.y*q.y-2.0f*q.z*q.z; Out->_12 = 2.0f*q.x*q.y+2.0f*q.w*q.z;      Out->_13 = 2.0f*q.x*q.z-2.0f*q.w*q.y;      Out->_14 = 0.0f;
	Out->_21 = 2.0f*q.x*q.y-2.0f*q.w*q.z;      Out->_22 = 1.0f-2.0f*q.x*q.x-2.0f*q.z*q.z; Out->_23 = 2.0f*q.y*q.z+2.0f*q.w*q.x;      Out->_24 = 0.0f;
	Out->_31 = 2.0f*q.x*q.z+2.0f*q.w*q.y;      Out->_32 = 2.0f*q.y*q.z-2.0f*q.w*q.x;      Out->_33 = 1.0f-2.0f*q.x*q.x-2.0f*q.y*q.y; Out->_34 = 0.0f;
	Out->_41 = 0.0f;                           Out->_42 = 0.0f;                           Out->_43 = 0.0f;                           Out->_44 = 1.0f;

======== Wersja 2 ========

Ta co jest aktualnie niezakomentowana.

Dzia³a tak samo jak wersja 1 (wynik testu: 99.9% przypadków), ale jest
optymalniejsza.

Na podstawie kodu z ksi¹¿ki "Game Programming Gems", tom 5, rozdz. 1.6
"Improved Frustum Culling", Frank Puig Placeres.
*/

void QuaternionToRotationMatrix(MATRIX *Out, const QUATERNION &q)
{
	float
		xx = q.x * q.x,
		yy = q.y * q.y,
		zz = q.z * q.z,
		xy = q.x * q.y,
		xz = q.x * q.z,
		yz = q.y * q.z,
		wx = q.w * q.x,
		wy = q.w * q.y,
		wz = q.w * q.z;

	Out->_11 = 1.0f - 2.0f * ( yy + zz ); Out->_12 = 2.0f * ( xy + wz );        Out->_13 = 2.0f * ( xz - wy );        Out->_14 = 0.0f;
	Out->_21 = 2.0f * ( xy - wz );        Out->_22 = 1.0f - 2.0f * ( xx + zz ); Out->_23 = 2.0f * ( yz + wx );        Out->_24 = 0.0f;
	Out->_31 = 2.0f * ( xz + wy );        Out->_32 = 2.0f * ( yz - wx );        Out->_33 = 1.0f - 2.0f * ( xx + yy ); Out->_34 = 0.0f;
	Out->_41 = 0.0f;                      Out->_42 = 0.0f;                      Out->_43 = 0.0f;                      Out->_44 = 1.0f;
}

/*
======== Wersja 1 ========

Poprawna, ale wolniejsza.

	QUATERNION q_inv; Inverse(&q_inv, q);
	QUATERNION q_p = QUATERNION(p.x, p.y, p.z, 0.0f);
	QUATERNION tmp1; Mul(&tmp1, q_inv, q_p);
	QUATERNION tmp2; Mul(&tmp2, tmp1, q);
	Out->x = tmp2.x;
	Out->y = tmp2.y;
	Out->z = tmp2.z;

======== Wersja 2 ========

Ta co jest aktualnie niezakomentowana.

Sprawdzone, ¿e dzia³a tak samo jak wersja 1 (wynik testu: 100% przypadków), ale
jest optymalniejsza.

U¿ywa tak naprawdê rozpisanego przekszta³cenia na macierz obrotu i pomno¿enia
wektora przez t¹ macierz.

Na podstawie kodu z ksi¹¿ki "Game Programming Gems", tom 5, rozdz. 1.6
"Improved Frustum Culling", Frank Puig Placeres.
*/

void QuaternionTransform(VEC3 *Out, const VEC3 &p, const QUATERNION &q)
{
	float
		xx = q.x * q.x,
		yy = q.y * q.y,
		zz = q.z * q.z,
		xy = q.x * q.y,
		xz = q.x * q.z,
		yz = q.y * q.z,
		wx = q.w * q.x,
		wy = q.w * q.y,
		wz = q.w * q.z;

	Out->x = (1.0f - 2.0f * ( yy + zz ))*p.x + (2.0f * ( xy - wz ))       *p.y  + (2.0f * ( xz + wy ))       *p.z;
	Out->y = (2.0f * ( xy + wz ))       *p.x + (1.0f - 2.0f * ( xx + zz ))*p.y  + (2.0f * ( yz - wx ))       *p.z;
	Out->z = (2.0f * ( xz - wy ))       *p.x + (2.0f * ( yz + wx ))       *p.y  + (1.0f - 2.0f * ( xx + yy ))*p.z;
}

void EulerAnglesToQuaternionO2I(QUATERNION *Out, float Yaw, float Pitch, float Roll)
{
	Yaw *= 0.5f; Pitch *= 0.5f; Roll *= 0.5f;

	float sy = sinf(Yaw), cy = cosf(Yaw);
	float sp = sinf(Pitch), cp = cosf(Pitch);
	float sr = sinf(Roll), cr = cosf(Roll);

	Out->x = cy*sp*cr + sy*cp*sr;
	Out->y = sy*cp*cr - cy*sp*sr;
	Out->z = cy*cp*sr - sy*sp*cr;
	Out->w = cy*cp*cr + sy*sp*sr;
}

void EulerAnglesToQuaternionI2O(QUATERNION *Out, float Yaw, float Pitch, float Roll)
{
	Yaw *= 0.5f; Pitch *= 0.5f; Roll *= 0.5f;

	float sy = sinf(Yaw), cy = cosf(Yaw);
	float sp = sinf(Pitch), cp = cosf(Pitch);
	float sr = sinf(Roll), cr = cosf(Roll);

	Out->x = -cy*sp*cr - sy*cp*sr;
	Out->y = cy*sp*sr - sy*cp*cr;
	Out->z = sy*sp*cr - cy*cp*sr;
	Out->w = cy*cp*cr + sy*sp*sr;
}

void QuaternionO2IToEulerAngles(float *Yaw, float *Pitch, float *Roll, const QUATERNION &q)
{
	// Extract sin(Pitch)
	float sp = -2.0f * (q.y*q.z - q.w*q.x);

	// Check for Gimbal lock, giving slight tolerance for numerical imprecision
	if (float_equal(sp, 1.0f))
	{
		// Looking straight up or down
		*Pitch = PI_2 * sp;
		// Compute yaw, slam roll to zero
		*Yaw = atan2f(-q.x*q.z + q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		*Roll = 0.0f;
	}
	else
	{
		// Compute angles
		*Pitch = asinf(sp);
		*Yaw = atan2f(q.x*q.z + q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
		*Roll = atan2f(q.x*q.y + q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
	}
}

void QuaternionI2OToEulerAngles(float *Yaw, float *Pitch, float *Roll, const QUATERNION &q)
{
	// Extract sin(Pitch)
	float sp = -2.0f * (q.y*q.z + q.w*q.x);

	// Check for Gimbal lock, giving slight tolerance for numerical imprecision
	if (float_equal(sp, 1.0f))
	{
		// Looking straight up or down
		*Pitch = PI_2 * sp;
		// Compute yaw, slam roll to zero
		*Yaw = atan2f(-q.x*q.z - q.w*q.y, 0.5f - q.y*q.y - q.z*q.z);
		*Roll = 0.0f;
	}
	else
	{
		// Compute angles
		*Pitch = asinf(sp);
		*Yaw = atan2f(q.x*q.z - q.w*q.y, 0.5f - q.x*q.x - q.y*q.y);
		*Roll = atan2f(q.x*q.y - q.w*q.z, 0.5f - q.x*q.x - q.z*q.z);
	}
}

void QuaternionRotationX(QUATERNION *Out, float a)
{
	a *= 0.5f;
	Out->x = sinf(a);
	Out->y = 0.0f;
	Out->z = 0.0f;
	Out->w = cosf(a);
}

void QuaternionRotationY(QUATERNION *Out, float a)
{
	a *= 0.5f;
	Out->x = 0.0f;
	Out->y = sinf(a);
	Out->z = 0.0f;
	Out->w = cosf(a);
}

void QuaternionRotationZ(QUATERNION *Out, float a)
{
	a *= 0.5f;
	Out->x = 0.0f;
	Out->y = 0.0f;
	Out->z = sinf(a);
	Out->w = cosf(a);
}

void QuaternionDiff(QUATERNION *Out, const QUATERNION &a, const QUATERNION &b)
{
	// Ta funkcja dzia³a tak a nie inaczej dlatego ¿e mno¿enie mamy odwrotnie, inaczej by³by inny wzór.
	QUATERNION a_inv; Inverse(&a_inv, a);
	Mul(Out, a_inv, b);
}

void Conjugate(QUATERNION *q)
{
	q->x = -q->x;
	q->y = -q->y;
	q->z = -q->z;
}

void Inverse(QUATERNION *Out, const QUATERNION &q)
{
	float NormRc = 1.0f / sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	Out->w = q.w * NormRc; 
	Out->x = - q.x * NormRc; 
	Out->y = - q.y * NormRc; 
	Out->z = - q.z * NormRc; 
}

void Normalize(QUATERNION *Out, const QUATERNION &q)
{
	float n = 1.0f / sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	Out->x = q.x * n;
	Out->y = q.y * n;
	Out->z = q.z * n;
	Out->w = q.w * n;
}

void Normalize(QUATERNION *InOut)
{
	float n = 1.0f / Length(*InOut);
	InOut->x *= n;
	InOut->y *= n;
	InOut->z *= n;
	InOut->w *= n;
}

float Dot(const QUATERNION &q1, const QUATERNION &q2)
{
	return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
}

float LengthSq(const QUATERNION &q)
{
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}

float Length(const QUATERNION &q)
{
	return sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

void Log(QUATERNION *Out, const QUATERNION &q)
{
	Out->w = 0.0f;
	float Theta = acosf(q.w);
	float SinTheta = sinf(Theta);
	if (FLOAT_ALMOST_ZERO(SinTheta))
	{
		Out->x = q.x;
		Out->y = q.y;
		Out->z = q.z;
	}
	else
	{
		float Factor = Theta / SinTheta;
		Out->x = q.x * Factor;
		Out->y = q.y * Factor;
		Out->z = q.z * Factor;
	}
}

void Exp(QUATERNION *Out, const QUATERNION &q)
{
	float Angle = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z);
	float Sin = sinf(Angle);

	Out->w = cosf(Angle);

	if (FLOAT_ALMOST_ZERO(Sin))
	{
		Out->x = q.x;
		Out->y = q.y;
		Out->z = q.z;
	}
	else
	{
		float Coeff = Sin / Angle;
		Out->x = Coeff * q.x;
		Out->y = Coeff * q.y;
		Out->z = Coeff * q.z;
	}
}

void Pow(QUATERNION *InOut, float t)
{
	// Kwaternion identycznioœciowy ma daæ w wyniku ten sam kwaternion,
	// dlatego trzeba to sprawdzaæ szczególnie, bo inaczej by³oby dzielenie przez 0.
	if (!float_equal(InOut->w, 1.0f))
	{
		// Extract the half angle alpha (alpha = theta/2)
		float alpha = acosf(InOut->w);
		// Compute new alpha value
		float newAlpha = alpha * t;
		// Compute new w value
		InOut->w = cosf(newAlpha);
		// Compute new xyz values
		float mult = sinf(newAlpha) / sinf(alpha);
		InOut->x *= mult;
		InOut->y *= mult;
		InOut->z *= mult;
	}
}

/*
INTERPOLACJA SFERYCZNA KWATERNIONÓW

======== Wersja 1 ========

Liczy³a Ÿle, dawa³a wyniki inne ni¿ D3DX.

	float costheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
	float theta = acosf(costheta);
	if (abs(theta) < 0.01f)
	{
		Out->w = q1.w;
		Out->x = q1.x;
		Out->y = q1.y;
		Out->z = q1.z;
	}
	float sinTheta = sqrtf(1.0f - costheta*costheta);
	if (fabsf(sinTheta) < 0.01f)
	{
		Out->w = (q1.w * 0.5f + q2.w * 0.5f);
		Out->x = (q1.x * 0.5f + q2.x * 0.5f);
		Out->y = (q1.y * 0.5f + q2.y * 0.5f);
		Out->z = (q1.z * 0.5f + q2.z * 0.5f);
	}
	float ratioA = sin((1.0f - t) * theta) / sinTheta;
	float ratioB = sin(t * theta) / sinTheta;

	Out->w = (q1.w * ratioA + q2.w * ratioB);
	Out->x = (q1.x * ratioA + q2.x * ratioB);
	Out->y = (q1.y * ratioA + q2.y * ratioB);
	Out->z = (q1.z * ratioA + q2.z * ratioB);

======== Wersja 2 ========

Ta która jest poni¿ej niezakomentowana.

Mia³a w nag³ówku taki komentarz (niezgodny z prawd¹):

// (Ta wersja, pochodz¹ca z ksi¹¿ki "3D Math Primer for Graphics and Game Development"
// daje wyniki zgodne z D3DX w dok³adnie po³owie przypadów - nie wiem dlaczego.
// Próbowa³em zakomentowaæ sekcje "if (cosOmega < 0.0f)", "if (cosOmega > 0.9999f)",
// ich kombinacje i wci¹¿ to samo.)

======== Wersja 3 ========

Na podstawie kodu z ksi¹¿ki "Game Programming Gems", tom 5, rozdz. 1.6
"Improved Frustum Culling", Frank Puig Placeres.

Wygl¹da ³adnie, ale daje wyniki kompletnie niezgodne z D3DX.

	float cosfine = (q0.x * q1.x) + (q0.y * q1.y) * (q0.z * q1.z) * (q0.w * q1.w);
	float angleR = acosf(cosfine);

	float lower_weight;
	float upper_weight;

	// set up our weights
	if (FLOAT_ALMOST_ZERO(angleR))
	{
		// numerically unstable when angle is close to 0, lerp instead
		lower_weight = t;
		upper_weight = 1.f - t;
	}
	else
	{
		// this will also fall apart if w approaches k*pi/2 for k = [1, 2, ...]
		float i_sinf_angle = 1.f / sinf(angleR);
		lower_weight = sinf(angleR - angleR * t) * i_sinf_angle;
		upper_weight = sinf(angleR * t)          * i_sinf_angle;
	}

	Out->w = (q0.w * lower_weight) + (q1.w * upper_weight);
	Out->x = (q0.x * lower_weight) + (q1.x * upper_weight);
	Out->y = (q0.y * lower_weight) + (q1.y * upper_weight);
	Out->z = (q0.z * lower_weight) + (q1.z * upper_weight);

========

Porównanie zgodnoœci wyników moich funkcji z D3DXQuaternionSlerp dla losowych
kwaternionów:

                       Wersja 1   Wersja 2   Wersja 3
Nie normalizowane       50 %      100 %        0 %
Normalizowane wejœcie   80 %      100 %        0 %
Normalizowane wyjœcie   50 %      100 %        0 %
Normalizowane we i wy   80 %      100 %        0 %
*/

void Slerp(QUATERNION *Out, const QUATERNION &q0, const QUATERNION &q1, float t)
{

	// Compute the "cosine of the angle" between the quaternions, using the dot product
	float cosOmega = Dot(q0, q1);
	// If negative dot, negate one of the input quaternions to take the shorter 4D "arc"
	QUATERNION new_q1 = q1;
	if (cosOmega < 0.0f)
	{
		new_q1.x = -new_q1.x;
		new_q1.y = -new_q1.y;
		new_q1.z = -new_q1.z;
		new_q1.w = -new_q1.w;
		cosOmega = -cosOmega;
	}

	// Check if they are very close together to protect against divide-by-zero
	float k0, k1;
	if (cosOmega > 0.9999f)
	{
		// Very close - jost use linear interpolation
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		// Compute the sin of the angle using the trig identity sin^x(omega)+cos^2(omega) = 1
		float sinOmega = sqrtf(1.0f - cosOmega*cosOmega);
		// Compute the angle from its sin and cosine
		float omega = atan2f(sinOmega, cosOmega);
		// Compute the inverse of denominator, so we only have to divide once
		float oneOverSinOmega = 1.0f / sinOmega;
		// Compute interpolation parameters
		k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sinf(t*omega) * oneOverSinOmega;
	}
	// Interpolate
	Out->x = q0.x*k0 + new_q1.x*k1;
	Out->y = q0.y*k0 + new_q1.y*k1;
	Out->z = q0.z*k0 + new_q1.z*k1;
	Out->w = q0.w*k0 + new_q1.w*k1;
}

void Lerp(QUATERNION *Out, const QUATERNION &q1, const QUATERNION &q2, float t)
{
	float w1 = t;
	float w2 = 1.f - t;

	Out->x = (q1.x * w1) + (q2.x * w2);
	Out->y = (q1.y * w1) + (q2.y * w2);
	Out->z = (q1.z * w1) + (q2.z * w2);
	Out->w = (q1.w * w1) + (q2.w * w2);
}

void Squad(QUATERNION *Out, const QUATERNION &Q1, const QUATERNION &A, const QUATERNION &B, const QUATERNION &C, float t)
{
	QUATERNION Tmp1; Slerp(&Tmp1, Q1, C, t);
	QUATERNION Tmp2; Slerp(&Tmp2, A, B, t);
	Slerp(Out, Tmp1, Tmp2, 2.0f*t*(1.0f-t));
}

void SquadSetup(QUATERNION *OutA, QUATERNION *OutB, QUATERNION *OutC, const QUATERNION &Q0, const QUATERNION &Q1, const QUATERNION &Q2, const QUATERNION &Q3)
{
	QUATERNION q0 = LengthSq(Q0+Q1) < LengthSq(Q0-Q1) ? -Q0 : Q0;
	QUATERNION q2 = LengthSq(Q1+Q2) < LengthSq(Q1-Q2) ? -Q2 : Q2;
	QUATERNION q3 = LengthSq(Q2+Q3) < LengthSq(Q2-Q3) ? -Q3 : Q3;

	QUATERNION tmp1, tmp2, tmp3, tmp4, tmp33, tmp44, q1_exp, q2_exp;
	Exp(&q1_exp, Q1);
	Exp(&q2_exp, q2);

	Mul(&tmp33, q1_exp, q2);
	Mul(&tmp44, q1_exp, q0);
	Log(&tmp3, tmp33);
	Log(&tmp4, tmp44);
	Add(&tmp2, tmp3, tmp4);
	Mul(&tmp2, -0.25f);
	Exp(&tmp1, tmp2);
	Mul(OutA, Q1, tmp1);

	Mul(&tmp33, q2_exp, q3);
	Mul(&tmp44, q2_exp, Q1);
	Log(&tmp3, tmp33);
	Log(&tmp4, tmp44);
	Add(&tmp2, tmp3, tmp4);
	Mul(&tmp2, -0.25f);
	Exp(&tmp1, tmp2);
	Mul(OutB, q2, tmp1);

	*OutC = q2;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// AFFINE2D

const AFFINE2D AFFINE2D_ZERO = AFFINE2D(
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f);
const AFFINE2D AFFINE2D_IDENTITY = AFFINE2D(
	1.f, 0.f, 0.f,
	0.f, 1.f, 0.f);

void Affine2dToStr(tstring *Out, const AFFINE2D &aff)
{
	tstring s;

	FloatToStr(&s, aff.a); *Out  = s; *Out += _T(',');
	FloatToStr(&s, aff.b); *Out += s; *Out += _T(',');
	FloatToStr(&s, aff.c); *Out += s; *Out += _T(';');

	FloatToStr(&s, aff.d); *Out += s; *Out += _T(',');
	FloatToStr(&s, aff.e); *Out += s; *Out += _T(',');
	FloatToStr(&s, aff.f); *Out += s;
}

bool StrToAffine2d(AFFINE2D *Out, const tstring &Str)
{
	tstring Parts[2], Numbers[3], Dupa;
	size_t Index = 0;
	tstring Delimiter1 = _T(";"), Delimiter2 = _T(",");
	if (!Split(Str, Delimiter1, &Parts[0], &Index)) return false;
	if (!Split(Str, Delimiter1, &Parts[1], &Index)) return false;
	if ( Split(Str, Delimiter1, &Dupa,     &Index)) return false;

	for (uint PartI = 0; PartI < 2; PartI++)
	{
		Index = 0;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[0], &Index)) return false;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[1], &Index)) return false;
		if (!Split(Parts[PartI], Delimiter2, &Numbers[2], &Index)) return false;
		if ( Split(Parts[PartI], Delimiter2, &Dupa,       &Index)) return false;

		if (StrToFloat(&Out->operator ()(PartI, 0), Numbers[0]) != 0) return false;
		if (StrToFloat(&Out->operator ()(PartI, 1), Numbers[1]) != 0) return false;
		if (StrToFloat(&Out->operator ()(PartI, 2), Numbers[2]) != 0) return false;
	}

	return true;
}

void Lerp(AFFINE2D &out, const AFFINE2D &lhs, const AFFINE2D &rhs, float t)
{
	out.a = Lerp(lhs.a, rhs.a, t);
	out.b = Lerp(lhs.b, rhs.b, t);
	out.c = Lerp(lhs.c, rhs.c, t);
	out.d = Lerp(lhs.d, rhs.d, t);
	out.e = Lerp(lhs.e, rhs.e, t);
	out.f = Lerp(lhs.f, rhs.f, t);
}

void Inverse(AFFINE2D &out, const AFFINE2D &aff)
{
	float detInv = 1.f / (aff.a*aff.e - aff.b*aff.d);
	out.a =   detInv * aff.e;
	out.b = - detInv * aff.b;
	out.c =   detInv * (aff.b*aff.f - aff.c*aff.e);
	out.d = - detInv * aff.d;
	out.e =   detInv * aff.a;
	out.f = - detInv * (aff.a*aff.f - aff.d*aff.c);
}

void Affine2dToMatrix(MATRIX &out, const AFFINE2D &aff)
{
	out = MATRIX(
		aff.a, aff.d, 0.f, 0.f,
		aff.b, aff.e, 0.f, 0.f,
		0.f,   0.f,   1.f, 0.f,
		aff.c, aff.f, 0.f, 1.f);
}

void MatrixToAffine2d(AFFINE2D &out, const MATRIX &m)
{
	out = AFFINE2D(
		m._11, m._21, m._41,
		m._12, m._22, m._42);
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// FRUSTUM

void FRUSTUM_PLANES::Set(const MATRIX &WorldViewProj)
{
	// Left clipping plane
	Planes[0].a = WorldViewProj._14 + WorldViewProj._11;
	Planes[0].b = WorldViewProj._24 + WorldViewProj._21;
	Planes[0].c = WorldViewProj._34 + WorldViewProj._31;
	Planes[0].d = WorldViewProj._44 + WorldViewProj._41;
	// Right clipping plane
	Planes[1].a = WorldViewProj._14 - WorldViewProj._11;
	Planes[1].b = WorldViewProj._24 - WorldViewProj._21;
	Planes[1].c = WorldViewProj._34 - WorldViewProj._31;
	Planes[1].d = WorldViewProj._44 - WorldViewProj._41;
	// Top clipping plane
	Planes[2].a = WorldViewProj._14 - WorldViewProj._12;
	Planes[2].b = WorldViewProj._24 - WorldViewProj._22;
	Planes[2].c = WorldViewProj._34 - WorldViewProj._32;
	Planes[2].d = WorldViewProj._44 - WorldViewProj._42;
	// Bottom clipping plane
	Planes[3].a = WorldViewProj._14 + WorldViewProj._12;
	Planes[3].b = WorldViewProj._24 + WorldViewProj._22;
	Planes[3].c = WorldViewProj._34 + WorldViewProj._32;
	Planes[3].d = WorldViewProj._44 + WorldViewProj._42;
	// Near clipping plane
	Planes[4].a = WorldViewProj._13;
	Planes[4].b = WorldViewProj._23;
	Planes[4].c = WorldViewProj._33;
	Planes[4].d = WorldViewProj._43;
	// Far clipping plane
	Planes[5].a = WorldViewProj._14 - WorldViewProj._13;
	Planes[5].b = WorldViewProj._24 - WorldViewProj._23;
	Planes[5].c = WorldViewProj._34 - WorldViewProj._33;
	Planes[5].d = WorldViewProj._44 - WorldViewProj._43;
}

void FRUSTUM_PLANES::Set(const FRUSTUM_POINTS &FrustumPoints)
{
	PointsToPlane(&Planes[PLANE_NEAR],
		FrustumPoints[FRUSTUM_POINTS::NEAR_RIGHT_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::NEAR_RIGHT_TOP],
		FrustumPoints[FRUSTUM_POINTS::NEAR_LEFT_TOP]);
	PointsToPlane(&Planes[PLANE_FAR],
		FrustumPoints[FRUSTUM_POINTS::FAR_LEFT_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::FAR_LEFT_TOP],
		FrustumPoints[FRUSTUM_POINTS::FAR_RIGHT_TOP]);
	PointsToPlane(&Planes[PLANE_LEFT],
		FrustumPoints[FRUSTUM_POINTS::NEAR_LEFT_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::NEAR_LEFT_TOP],
		FrustumPoints[FRUSTUM_POINTS::FAR_LEFT_TOP]);
	PointsToPlane(&Planes[PLANE_RIGHT],
		FrustumPoints[FRUSTUM_POINTS::FAR_RIGHT_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::FAR_RIGHT_TOP],
		FrustumPoints[FRUSTUM_POINTS::NEAR_RIGHT_TOP]);
	PointsToPlane(&Planes[PLANE_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::NEAR_LEFT_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::FAR_LEFT_BOTTOM],
		FrustumPoints[FRUSTUM_POINTS::FAR_RIGHT_BOTTOM]);
	PointsToPlane(&Planes[PLANE_TOP],
		FrustumPoints[FRUSTUM_POINTS::FAR_LEFT_TOP],
		FrustumPoints[FRUSTUM_POINTS::NEAR_LEFT_TOP],
		FrustumPoints[FRUSTUM_POINTS::NEAR_RIGHT_TOP]);
}

void FRUSTUM_PLANES::Normalize()
{
	common::Normalize(&Planes[0]);
	common::Normalize(&Planes[1]);
	common::Normalize(&Planes[2]);
	common::Normalize(&Planes[3]);
	common::Normalize(&Planes[4]);
	common::Normalize(&Planes[5]);
}

void FRUSTUM_POINTS::Set(const FRUSTUM_PLANES &FrustumPlanes)
{
	Intersect3Planes(FrustumPlanes.Planes[4], FrustumPlanes.Planes[0], FrustumPlanes.Planes[3], &Points[0]);
	Intersect3Planes(FrustumPlanes.Planes[4], FrustumPlanes.Planes[1], FrustumPlanes.Planes[3], &Points[1]);
	Intersect3Planes(FrustumPlanes.Planes[4], FrustumPlanes.Planes[0], FrustumPlanes.Planes[2], &Points[2]);
	Intersect3Planes(FrustumPlanes.Planes[4], FrustumPlanes.Planes[1], FrustumPlanes.Planes[2], &Points[3]);
	Intersect3Planes(FrustumPlanes.Planes[5], FrustumPlanes.Planes[0], FrustumPlanes.Planes[3], &Points[4]);
	Intersect3Planes(FrustumPlanes.Planes[5], FrustumPlanes.Planes[1], FrustumPlanes.Planes[3], &Points[5]);
	Intersect3Planes(FrustumPlanes.Planes[5], FrustumPlanes.Planes[0], FrustumPlanes.Planes[2], &Points[6]);
	Intersect3Planes(FrustumPlanes.Planes[5], FrustumPlanes.Planes[1], FrustumPlanes.Planes[2], &Points[7]);
}

void FRUSTUM_POINTS::Set(const MATRIX &WorldViewProjInv)
{
	VEC3 P[] = {
		VEC3(-1.f, -1.f, 0.f), VEC3(+1.f, -1.f, 0.f),
		VEC3(-1.f, +1.f, 0.f), VEC3(+1.f, +1.f, 0.f),
		VEC3(-1.f, -1.f, 1.f), VEC3(+1.f, -1.f, 1.f),
		VEC3(-1.f, +1.f, 1.f), VEC3(+1.f, +1.f, 1.f) };

	TransformCoord(&Points[0], P[0], WorldViewProjInv);
	TransformCoord(&Points[1], P[1], WorldViewProjInv);
	TransformCoord(&Points[2], P[2], WorldViewProjInv);
	TransformCoord(&Points[3], P[3], WorldViewProjInv);
	TransformCoord(&Points[4], P[4], WorldViewProjInv);
	TransformCoord(&Points[5], P[5], WorldViewProjInv);
	TransformCoord(&Points[6], P[6], WorldViewProjInv);
	TransformCoord(&Points[7], P[7], WorldViewProjInv);
}

void FRUSTUM_POINTS::CalcCenter(VEC3 *Out) const
{
	// Œrodek near clipping plane
	VEC3 NearCenter = (Points[NEAR_LEFT_TOP] + Points[NEAR_RIGHT_BOTTOM]) * 0.5f;
	// Œrodek far clipping plane
	VEC3 FarCenter = (Points[FAR_LEFT_BOTTOM] + Points[FAR_RIGHT_TOP]) * 0.5f;
	// Punkt w po³owie odleg³oœci miêdzy nimi to œrodek sfery
	*Out = (NearCenter + FarCenter) * 0.5f;
}

void FRUSTUM_POINTS::CalcBoundingBox(BOX *Box) const
{
	Box->Min = Box->Max = Points[0];

	for (uint i = 1; i < 8; i++)
	{
		Min(&Box->Min, Box->Min, Points[i]);
		Max(&Box->Max, Box->Max, Points[i]);
	}
}

void FRUSTUM_POINTS::CalcBoundingSphere(VEC3 *SphereCenter, float *SphereRadius) const
{
	// Œrodek frustuma to œrodek sfery
	CalcCenter(SphereCenter);
	// Promieñ sfery to odleg³oœæ tego punktu od jakiegoœ punktu z rogu far clipping plane
	*SphereRadius = Distance(*SphereCenter, Points[FAR_LEFT_TOP]);
}

void FRUSTUM_RADAR::SetFovAndAspect(float FovY, float Aspect)
{
	float FovY_half = FovY * 0.5f;
	float FovY_half_tan = tanf(FovY_half);
	float FovY_half_tan_aspect = FovY_half_tan * Aspect;

	m_UFactor = FovY_half_tan;
	m_RFactor = FovY_half_tan_aspect;

	m_USphereFactor = 1.f / cosf(FovY_half);
	m_RSphereFactor = 1.f / cosf(atanf(FovY_half_tan_aspect));
}

void FRUSTUM_RADAR::SetProjection(float FovY, float Aspect, float ZNear, float ZFar)
{
	m_ZNear = ZNear;
	m_ZFar  = ZFar;
	
	SetFovAndAspect(FovY, Aspect);
}

void FRUSTUM_RADAR::SetView(const VEC3 &Eye, const VEC3 &Forward, const VEC3 &Up, const VEC3 &Right)
{
	m_Eye = Eye;
	m_Forward = Forward;
	m_Up = Up;
	m_Right = Right;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// KAPSU£A

float CapsuleVolume(const VEC3 &p1, const VEC3 &p2, float R)
{
	return PI * R * R * (
		4.0f/3.0f * R + // kula
		Distance(p1, p2)); // walec
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// INNE

void SphericalToCartesian(VEC3 *Out, float Yaw, float Pitch, float R)
{
	float sy = sinf(Yaw), cy = cosf(Yaw);
	float sp = sinf(Pitch), cp = cosf(Pitch);

	Out->x = R * cp * cy;
	Out->y = R * sp;
	Out->z = - R * cp * sy;
}

void CartesianToSpherical(float *OutYaw, float *OutPitch, float *OutR, const VEC3 &Pos)
{
	float R = Length(Pos);

	if (OutR)
		*OutR = R;
	if (OutYaw)
		*OutYaw = atan2f(-Pos.z, Pos.x);
	if (OutPitch)
		*OutPitch = asinf(Pos.y / R);
}

struct ConvexHull2D_IsLeftSort
{
private:
	VEC2 m_PivotPt;

public:
	static float IsLeft(const VEC2 &p0, const VEC2 &p1, const VEC2 &p2)
	{
		return (p1.x-p0.x)*(p2.y-p0.y) - (p2.x-p0.x)*(p1.y-p0.y);
	}

	ConvexHull2D_IsLeftSort(const VEC2 &PivotPt) : m_PivotPt(PivotPt) { }

	bool operator() (const VEC2 &p1, const VEC2 &p2)
	{
		return ( IsLeft(m_PivotPt, p1, p2) >= 0 );
	}
};

void ConvexHull2D(std::vector<VEC2> *OutPoints, const VEC2 InPoints[], size_t InPointCount)
{
	assert(InPointCount > 0);

	OutPoints->clear();

	if (InPointCount == 1)
	{
		OutPoints->push_back(InPoints[0]);
		return;
	}

	OutPoints->reserve(InPointCount); // wiêcej ni¿ potrzeba

	VEC2 Pivot = InPoints[0];
	std::vector<VEC2> PointSet; // Stores all points except the pivot
	PointSet.reserve(InPointCount-1);

	for (size_t i = 1; i < InPointCount; i++)
	{
		VEC2 Tmp = InPoints[i];
		if ( (Tmp.y < Pivot.y) || (Tmp.y == Pivot.y && Tmp.x > Pivot.x) )
		{
			PointSet.push_back(Pivot);
			Pivot = Tmp;
		}
		else
			PointSet.push_back(Tmp);
	}

	std::vector<VEC2>::iterator PtEnd = std::unique(PointSet.begin(), PointSet.end()); // Tego nie rozumiem po co to ani jak mo¿e wystêpowaæ przed sort zamiast po, ale w koñcu ja tu nie jestem od rozumienia tylko od kopiowania dobrego kodu od innych ;)
	PointSet.erase(PtEnd, PointSet.end());

	std::sort(PointSet.begin(), PointSet.end(), ConvexHull2D_IsLeftSort(Pivot));

	OutPoints->push_back(Pivot);
	OutPoints->push_back(PointSet[0]);

	size_t cnt = 1;
	while (cnt < PointSet.size())
	{
		const VEC2 & PT1 = (*OutPoints)[OutPoints->size()-1];
		const VEC2 & PT2 = (*OutPoints)[OutPoints->size()-2];
		const VEC2 & PK  = PointSet[cnt];
		float LeftTest = ConvexHull2D_IsLeftSort::IsLeft(PT2, PT1, PK);
		if (LeftTest > 0.f)
		{
			OutPoints->push_back(PK);
			cnt++;
		}
		else if (LeftTest == 0.f)
		{
			cnt++;
			VEC2 DiffVec0 = PK - PT2;
			VEC2 DiffVec1 = PT1 - PT2;
			if ( Dot(DiffVec0, DiffVec0) > Dot(DiffVec1, DiffVec1) )
				(*OutPoints)[OutPoints->size()-1] = PK;
		}
		else
			OutPoints->pop_back();
	}
}

const MATRIX HL2_TO_TANGENT_MATRIX = MATRIX(
	-0.408248290463863f, -0.707106781186547f, 0.577350269189626f, 0.0f,
	-0.408248290463863f,  0.707106781186547f, 0.577350269189626f, 0.0f,
	 0.816496580927726f,  0.0f,               0.577350269189626f, 0.0f,
	 0.0f,                0.0f,               0.0f,               1.0f);

const MATRIX TANGENT_TO_HL2_MATRIX = MATRIX(
	-0.408248290463863f, -0.408248290463863f, 0.816496580927726f, 0.0f,
	-0.707106781186547f,  0.707106781186547f, 0.0f,               0.0f,
	 0.577350269189626f,  0.577350269189626f, 0.577350269189626f, 0.0f,
	 0.0f,                0.0f,               0.0f,               1.0f);

const VEC3 HL2_BASIS_VECTORS[3] = {
	VEC3(-0.408248290463863f, -0.707106781186547f, 0.577350269189626f),
	VEC3(-0.408248290463863f,  0.707106781186547f, 0.577350269189626f),
	VEC3( 0.816496580927726f,  0.0f,               0.577350269189626f),
};


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Curves

float CalcCubicBezierCurveLength(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d, float t1, float t2)
{
	// Bode's rule: I_x1_x5 f(x)dx = h/45 * [14*f1 + 64*f2 + 24*f3 + 64*f4 + 14*f5]
	float h = (t2-t1)*0.25f;
	VEC3 v;
	float vLen[5];
	EvalCubicBezierCurveDiff<VEC3>(&v, a, b, c, d, t1);       vLen[0] = Length(v);
	EvalCubicBezierCurveDiff<VEC3>(&v, a, b, c, d, t1+h);     vLen[1] = Length(v);
	EvalCubicBezierCurveDiff<VEC3>(&v, a, b, c, d, t1+h*2.f); vLen[2] = Length(v);
	EvalCubicBezierCurveDiff<VEC3>(&v, a, b, c, d, t1+h*3.f); vLen[3] = Length(v);
	EvalCubicBezierCurveDiff<VEC3>(&v, a, b, c, d, t2);       vLen[4] = Length(v);
	return h * (
		(14.f/45.f) * vLen[0] +
		(64.f/45.f) * vLen[1] +
		(24.f/45.f) * vLen[2] +
		(64.f/45.f) * vLen[3] +
		(14.f/45.f) * vLen[4] );
}

float GetCubicBezierCurveArcLengthParam(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d, float len, float epsilon)
{
	float t1 = 0.0f, t2 = 1.0f, sum = 0.0f;
	for (;;)
	{
		float t = (t1 + t2) * 0.5f;
		float segLen = CalcCubicBezierCurveLength(a,b,c,d, t1,t);
		if (segLen <= epsilon)
			return t;
		float total = sum + segLen;
		float diff = len - total;
		if (diff > epsilon)
		{
			sum += segLen;
			t1 = t;
		}
		else if (diff < -epsilon)
			t2 = t;
		else
			return t;
	}
}

bool IntersectCubicBezierCurve2DWithX(const VEC2 curve[4], float x, float eps, float *outY, float *outT, float t0, float t1)
{
	// based on Casteljau algorithm
	float xmin, xmax;
	xmin = xmax = curve[0].x;
	for(int i=1; i<4; ++i)
	{
		xmin = std::min(xmin, curve[i].x);
		xmax = std::max(xmax, curve[i].x);
	}
	if (xmin <= x && x <= xmax)
	{
		// split in two subcurves
		if (xmax - xmin > eps)
		{
			VEC2 pa[4], pb[4];
			SplitCubicBezierCurve<VEC2>(pa, pb, curve[0], curve[1], curve[2], curve[3]);
			
			float tmed = (t0+t1) * 0.5f;
			if (IntersectCubicBezierCurve2DWithX(pa, x, eps, outY, outT, t0, tmed))
				return true;
			else
				return IntersectCubicBezierCurve2DWithX(pb, x, eps, outY, outT, tmed, t1);
		}
		else
		{
			float tt[4];
			tt[0] = t0;
			tt[1] = t0 + 1.f/3.f * (t1-t0);
			tt[2] = t0 + 2.f/3.f * (t1-t0);
			tt[3] = t1;

			for(int i=0; i<3; ++i)
			{
				const VEC2 *i0, *i1;
				float it0, it1;

				if (curve[i].x > curve[i+1].x)
				{
					i0 = &curve[i+1]; 
					i1= &curve[i];
					it0 = tt[i+1];
					it1 = tt[i];
				}
				else
				{
					i0 = &curve[i]; 
					i1 = &curve[i+1];
					it0 = tt[i];
					it1 = tt[i+1];
				}

				if (i0->x <= x && x <= i1->x)
				{
					float f = (x - i0->x) / (i1->x - i0->x);
					*outY =  i0->y + f*(i1->y - i0->y);
					*outT = it0 + f*(it1 - it0);
					return true;
				}
			}
			return false;
		}
	}
	else
		return false;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// KOLIZJE


float Orient2D(const VEC2 &a, const VEC2 &b, const VEC2 &c)
{
	/*
	Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson
	Liczy wyznacznik:
	| ax ay 1 |
	| bx by 1 |
	| cx cy 1 |
	*/
	return
		(a.x - c.x) * (b.y - c.y) -
		(a.y - c.y) * (b.x - c.x);
}

float Orient3D(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d)
{
	/*
	Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson
	Liczy wyznacznik:
	| ax ay az 1 |
	| bx by bz 1 |
	| cx cy cz 1 |
	| dx dy dz 1 |
	TODO - rozpisaæ optymalniej.
	*/
	VEC3 Tmp; Cross(&Tmp, b - d, c - d);
	return Dot(a - d, Tmp);
}

static inline bool _leftOfLine(const VEC2 &p, const VEC2 &A, const VEC2 &B)
{
	return (p.x-A.x)*(p.y-B.y) < (p.y-A.y)*(p.x-B.x);
}
static inline bool _rightOfLine(const VEC2 &p, const VEC2 &A, const VEC2 &B)
{
	return (p.x-A.x)*(p.y-B.y) > (p.y-A.y)*(p.x-B.x);
}
static inline bool _onLine(const VEC2 &p, const VEC2 &A, const VEC2 &B)
{
	return (p.x-A.x)*(p.y-B.y) == (p.y-A.y)*(p.x-B.x);
}

bool PointInTriangle(const VEC2 &p, const VEC2 &a, const VEC2 &b, const VEC2 &c)
{
	if (!_leftOfLine(p, a, b))
		// triangle is oriented CCW
		return !_leftOfLine(p, b, c) && !_leftOfLine(p, c, a);
	else
		// triangle is oriented CW
		return !_rightOfLine(p, b, c) && !_rightOfLine(p, c, a);

	/* Old algorithm:
	float bary_a, bary_b, bary_c;
	CalcBarycentric(p, a, b, c, &bary_a, &bary_b, &bary_c);
	return bary_b >= 0.0f && bary_c >= 0.0f && (bary_b + bary_c) <= 1.0f;
	*/
}

bool IsQuadConvex(const VEC2 &a, const VEC2 &b, const VEC2 &c, const VEC2 &d)
{
	return SegmentToSegment(a, c, b, d);
}

bool IsQuadConvex(const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson
	VEC3 d_minus_b = d - b;
	VEC3 bda; Cross(&bda, d_minus_b, a - b);
	VEC3 bdc; Cross(&bdc, d_minus_b, c - b);
	if (Dot(bda, bdc) >= 0.0f) return false;
	VEC3 acd; Cross(&acd, c - a, d - a);
	VEC3 acb; Cross(&acb, c - a, b - a);
	return Dot(acd, acb) < 0.0f;
}

float ClosestPointOnLine(const VEC3 &p, const VEC3 &RayOrig, const VEC3 &RayDir)
{
	// Napisane na podstawie ksi¹¿ki "3D Math Primer for Graphics and Game Development", str. 278.
	return Dot(RayDir, p-RayOrig);
}

float ClosestPointSegmentSegment(
	const VEC3 &Seg1_Beg, const VEC3 &Seg1_End, const VEC3 &Seg2_Beg, const VEC3 &Seg2_End,
	float &OutSeg1_t, float &OutSeg2_t, VEC3 &OutSeg1_pt, VEC3 &OutSeg2_pt)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 d1 = Seg1_End - Seg1_Beg; // Direction vector of segment S1
	VEC3 d2 = Seg2_End - Seg2_Beg; // Direction vector of segment S2
	VEC3 r = Seg1_Beg - Seg2_Beg;
	float a = Dot(d1, d1); // Squared length of segment S1, always nonnegative
	float e = Dot(d2, d2); // Squared length of segment S2, always nonnegative
	float f = Dot(d2, r);

	// Check if either or both segments degenerate into points
	if (a <= FLOAT_EPSILON && e <= FLOAT_EPSILON) {
		// Both segments degenerate into points
		OutSeg1_t = OutSeg2_t = 0.0f;
		OutSeg1_pt = Seg1_Beg;
		OutSeg2_pt = Seg2_Beg;
		return Dot(OutSeg1_pt - OutSeg2_pt, OutSeg1_pt - OutSeg2_pt);
	}
	if (a <= FLOAT_EPSILON) {
		// First segment degenerates into a point
		OutSeg1_t = 0.0f;
		OutSeg2_t = f / e; // OutSeg1_t = 0 => OutSeg2_t = (b*OutSeg1_t + f) / e = f / e
		OutSeg2_t = minmax(0.0f, OutSeg2_t, 1.0f);
	} else {
		float c = Dot(d1, r);
		if (e <= FLOAT_EPSILON) {
			// Second segment degenerates into a point
			OutSeg2_t = 0.0f;
			OutSeg1_t = minmax(0.0f, -c / a, 1.0f); // OutSeg2_t = 0 => OutSeg1_t = (b*OutSeg2_t - c) / a = -c / a
		} else {
			// The general nondegenerate case starts here
			float b = Dot(d1, d2);
			float denom = a*e-b*b; // Always nonnegative

			// If segments not parallel, compute closest point on L1 to L2, and
			// clamp to segment S1. Else pick arbitrary OutSeg1_t (here 0)
			if (denom != 0.0f) {
				OutSeg1_t = minmax(0.0f, (b*f - c*e) / denom, 1.0f);
			} else OutSeg1_t = 0.0f;

			// Compute point on L2 closest to S1(OutSeg1_t) using
			// OutSeg2_t = Dot((P1+D1*OutSeg1_t)-P2,D2) / Dot(D2,D2) = (b*OutSeg1_t + f) / e
			OutSeg2_t = (b*OutSeg1_t + f) / e;

			// If OutSeg2_t in [0,1] done. Else clamp OutSeg2_t, recompute OutSeg1_t for the new value
			// of OutSeg2_t using OutSeg1_t = Dot((P2+D2*OutSeg2_t)-P1,D1) / Dot(D1,D1)= (OutSeg2_t*b - c) / a
			// and clamp OutSeg1_t to [0, 1]
			if (OutSeg2_t < 0.0f) {
				OutSeg2_t = 0.0f;
				OutSeg1_t = minmax(0.0f, -c / a, 1.0f);
			} else if (OutSeg2_t > 1.0f) {
				OutSeg2_t = 1.0f;
				OutSeg1_t = minmax(0.0f, (b - c) / a, 1.0f);
			}
		}
	}

	OutSeg1_pt = Seg1_Beg + d1 * OutSeg1_t;
	OutSeg2_pt = Seg2_Beg + d2 * OutSeg2_t;
	return Dot(OutSeg1_pt - OutSeg2_pt, OutSeg1_pt - OutSeg2_pt);
}

float PointToLineDistance(const VEC3 &P, const VEC3 &LineOrig, const VEC3 &LineDir)
{
	// Wzór z netu (Worlfram MathWorld): http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

	VEC3 vsub; Sub(&vsub, LineOrig, P);
	VEC3 vcr; Cross(&vcr, LineDir, vsub);
	return Length(vcr);
}

float PointToSegmentDistanceSq(const VEC2 &p, const VEC2 &segment_p1, const VEC2 &segment_p2)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC2 ab = segment_p2 - segment_p1, ac = p - segment_p1, bc = p - segment_p2;
	float e = Dot(ac, ab);
	// Handle cases where p projects outside ab
	if (e <= 0.0f) return Dot(ac, ac);
	float f = Dot(ab, ab);
	if (e >= f) return Dot(bc, bc);
	// Handle cases where p projects onto ab
	return Dot(ac, ac) - e * e / f;
}

float PointToSegmentDistanceSq(const VEC3 &p, const VEC3 &segment_p1, const VEC3 &segment_p2)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 ab = segment_p2 - segment_p1, ac = p - segment_p1, bc = p - segment_p2;
	float e = Dot(ac, ab);
	// Handle cases where p projects outside ab
	if (e <= 0.0f) return Dot(ac, ac);
	float f = Dot(ab, ab);
	if (e >= f) return Dot(bc, bc);
	// Handle cases where p projects onto ab
	return Dot(ac, ac) - e * e / f;
}

bool PointOnLine(const VEC3 &P, const VEC3 &LineOrig, const VEC3 &LineDir)
{
	// Wzór z netu (Worlfram MathWorld): http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

	VEC3 vsub; Sub(&vsub, LineOrig, P);
	VEC3 vcr; Cross(&vcr, LineDir, vsub);
	return FLOAT_ALMOST_ZERO(LengthSq(vcr));
}

float PointToLine(const VEC3 &P, const VEC3 &LineOrig, const VEC3 &LineDir)
{
	// Algorytm wymyœli³em sam.

	float abs_dir_x = fabsf(LineDir.x);
	float abs_dir_y = fabsf(LineDir.y);
	float abs_dir_z = fabsf(LineDir.z);

	if (abs_dir_x > abs_dir_y && abs_dir_x > abs_dir_z)
		return (P.x - LineOrig.x) / LineDir.x;
	else if (abs_dir_y > abs_dir_z)
		return (P.y - LineOrig.y) / LineDir.y;
	else
		return (P.z - LineOrig.z) / LineDir.z;
}

bool PointInLineSegment(const VEC3 &P, const VEC3 &SegmentBeg, const VEC3 &SegmentEnd)
{
	// Algorytm wymyœli³em sam.

	float minx = std::min(SegmentBeg.x, SegmentEnd.x);
	float maxx = std::max(SegmentBeg.x, SegmentEnd.x);
	float miny = std::min(SegmentBeg.y, SegmentEnd.y);
	float maxy = std::max(SegmentBeg.y, SegmentEnd.y);
	float minz = std::min(SegmentBeg.z, SegmentEnd.z);
	float maxz = std::max(SegmentBeg.z, SegmentEnd.z);

	float abs_diff_x = maxx - minx;
	float abs_diff_y = maxy - miny;
	float abs_diff_z = maxz - minz;

	if (abs_diff_x > abs_diff_y && abs_diff_x > abs_diff_z)
		return (P.x >= minx && P.x <= maxx);
	else if (abs_diff_y > abs_diff_z)
		return (P.y >= miny && P.y <= maxy);
	else
		return (P.z >= minz && P.z <= maxz);
}

float ClosestPointOnSegment(VEC2 *Out, const VEC2 &p, const VEC2 &p1, const VEC2 &p2)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	float t;
	VEC2 ab = p2 - p1;
	// Project p onto ab, but deferring divide by Dot(ab, ab)
	t = Dot(p - p1, ab);
	if (t <= 0.0f)
	{
		// p projects outside the [p1,p2] interval, on the p1 side; clamp to p1
		t = 0.0f;
		if (Out) *Out = p1;
	}
	else
	{
		float denom = Dot(ab, ab); // Always nonnegative since denom = ||ab||^2
		if (t >= denom)
		{
			// p projects outside the [p1,p2] inverval, on the p2 side; clamp to p2
			t = 1.0f;
			if (Out) *Out = p2;
		}
		else
		{
			// p projects inside the [p1,p2] inverval; must do deferred divide now
			t /= denom;
			if (Out) *Out = p1 + t * ab;
		}
	}
	return t;
}

float ClosestPointOnSegment(VEC3 *Out, const VEC3 &p, const VEC3 &p1, const VEC3 &p2)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	float t;
	VEC3 ab = p2 - p1;
	// Project p onto ab, but deferring divide by Dot(ab, ab)
	t = Dot(p - p1, ab);
	if (t <= 0.0f)
	{
		// p projects outside the [p1,p2] interval, on the p1 side; clamp to p1
		t = 0.0f;
		if (Out) *Out = p1;
	}
	else
	{
		float denom = Dot(ab, ab); // Always nonnegative since denom = ||ab||^2
		if (t >= denom)
		{
			// p projects outside the [p1,p2] inverval, on the p2 side; clamp to p2
			t = 1.0f;
			if (Out) *Out = p2;
		}
		else
		{
			// p projects inside the [p1,p2] inverval; must do deferred divide now
			t /= denom;
			if (Out) *Out = p1 + t * ab;
		}
	}
	return t;
}

float ClosestPointOnRay(VEC2 *Out, const VEC2 &p, const VEC2 &RayOrig, const VEC2 &RayDir)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	float t;
	// Project p onto ab, but deferring divide by Dot(ab, ab)
	t = Dot(p - RayOrig, RayDir);
	if (t <= 0.0f)
	{
		// p projects outside the [p1,p2] interval, on the p1 side; clamp to p1
		t = 0.0f;
		if (Out) *Out = RayOrig;
	}
	else
	{
		t /= Dot(RayDir, RayDir);
		if (Out) *Out = RayOrig + t * RayDir;
	}
	return t;
}

float ClosestPointOnRay(VEC3 *Out, const VEC3 &p, const VEC3 &RayOrig, const VEC3 &RayDir)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	float t;
	// Project p onto ab, but deferring divide by Dot(ab, ab)
	t = Dot(p - RayOrig, RayDir);
	if (t <= 0.0f)
	{
		// p projects outside the [p1,p2] interval, on the p1 side; clamp to p1
		t = 0.0f;
		if (Out) *Out = RayOrig;
	}
	else
	{
		t /= Dot(RayDir, RayDir);
		if (Out) *Out = RayOrig + t * RayDir;
	}
	return t;
}

void ClosestPointInBox(VEC3 *Out, const BOX &Box, const VEC3 &p)
{
	Out->x = minmax(Box.Min.x, p.x, Box.Max.x);
	Out->y = minmax(Box.Min.y, p.y, Box.Max.y);
	Out->z = minmax(Box.Min.z, p.z, Box.Max.z);
}

void ClosestPointInObb(VEC3 *OutPoint, const VEC3 &Point, const OBB &Obb)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 d = Point - Obb.Center;
	// Start result at center of box; make steps from there
	*OutPoint = Obb.Center;
	// For each OBB axis...
	for (int i = 0; i < 3; i++)
	{
		// ...project d onto that axis to get the distance
		// along the axis of d from the box center
		float dist = Dot(d, Obb.Axes[i]);
		// If distance farther than the box extents, clamp to the box
		if (dist > Obb.HalfSize[i]) dist = Obb.HalfSize[i];
		if (dist < -Obb.HalfSize[i]) dist = -Obb.HalfSize[i];
		// Step that distance along the axis to get world coordinate
		*OutPoint += dist * Obb.Axes[i];
	}
}

void ClosestPointInRect3d(VEC3 *OutPoint, const VEC3 &Point, const RECT3D &Rect3d)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 d = Point - Rect3d.Center;
	// Start result at center of rect; make steps from there
	*OutPoint = Rect3d.Center;
	// For each rect axis...
	for (int i = 0; i < 2; i++) {
		// ...project d onto that axis to get the distance
		// along the axis of d from the rect center
		float dist = Dot(d, Rect3d.Axes[i]);
		// If distance farther than the rect extents, clamp to the rect
		if (dist > Rect3d.HalfSize[i]) dist = Rect3d.HalfSize[i];
		if (dist < -Rect3d.HalfSize[i]) dist = -Rect3d.HalfSize[i];
		// Step that distance along the axis to get world coordinate
		*OutPoint += dist * Rect3d.Axes[i];
	}
}

void ClosestPointInRect3d(VEC3 *OutPoint, const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c)
{
	VEC3 ab = b - a; // vector across rect
	VEC3 ac = c - a; // vector down rect
	VEC3 d = p - a;
	// Start result at top-left corner of rect; make steps from there
	*OutPoint = a;
	// Clamp p’ (projection of p to plane of r) to rectangle in the across direction
	float dist = Dot(d, ab);
	float maxdist = Dot(ab, ab);
	if (dist >= maxdist)
		*OutPoint += ab;
	else if (dist > 0.0f)
		*OutPoint += (dist / maxdist) * ab;
	// Clamp p’ (projection of p to plane of r) to rectangle in the down direction
	dist = Dot(d, ac);
	maxdist = Dot(ac, ac);
	if (dist >= maxdist)
		*OutPoint += ac;
	else if (dist > 0.0f)
		*OutPoint += (dist / maxdist) * ac;
}

void ClosestPointInTriangle(VEC3 *OutPoint, const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Check if P in vertex region outside A
	VEC3 ab = b - a;
	VEC3 ac = c - a;
	VEC3 ap = p - a;
	float d1 = Dot(ab, ap);
	float d2 = Dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) {
		*OutPoint = a; // barycentric coordinates (1,0,0)
		return;
	}

	// Check if P in vertex region outside B
	VEC3 bp = p - b;
	float d3 = Dot(ab, bp);
	float d4 = Dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) {
		*OutPoint = b; // barycentric coordinates (0,1,0)
		return;
	}

	// Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		*OutPoint = a + v * ab; // barycentric coordinates (1-v,v,0)
		return;
	}

	// Check if P in vertex region outside C
	VEC3 cp = p - c;
	float d5 = Dot(ab, cp);
	float d6 = Dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) {
		*OutPoint = c; // barycentric coordinates (0,0,1)
		return;
	}

	// Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		*OutPoint = a + w * ac; // barycentric coordinates (1-w,0,w)
		return;
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*OutPoint = b + w * (c - b); // barycentric coordinates (0,1-w,w)
		return;
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*OutPoint = a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

void ClosestPointInTriangle(VEC2 *out, const VEC2 &pt, const VEC2 &triA, const VEC2 &triB, const VEC2 &triC)
{
	VEC2 goodTri[3];
	if (_leftOfLine(triA, triB, triC))
	{
		goodTri[0] = triC;
		goodTri[1] = triB;
		goodTri[2] = triA;
	}
	else
	{
		goodTri[0] = triA;
		goodTri[1] = triB;
		goodTri[2] = triC;
	}

	bool leftSide[3];
	leftSide[0] = _leftOfLine(pt, goodTri[0], goodTri[1]);
	leftSide[1] = _leftOfLine(pt, goodTri[1], goodTri[2]);
	leftSide[2] = _leftOfLine(pt, goodTri[2], goodTri[0]);

	if (!leftSide[0] && !leftSide[1] && !leftSide[2])
	{
		// point in triangle -- no need to project
		*out = pt;
		return;
	}
	
	if (leftSide[0] && !leftSide[1] && !leftSide[2])
	{
		// point on left side of line0 only -- project on line0
		ClosestPointOnSegment(out, pt, goodTri[0], goodTri[1]);
		return;
	}

	if (!leftSide[0] && leftSide[1] && !leftSide[2])
	{
		// point on left side of line1 only -- project on line1
		ClosestPointOnSegment(out, pt, goodTri[1], goodTri[2]);
		return;
	}

	if (!leftSide[0] && !leftSide[1] && leftSide[2])
	{
		// point on left side of line2 only -- project on line2
		ClosestPointOnSegment(out, pt, goodTri[2], goodTri[0]);
		return;
	}

	if (leftSide[2] && leftSide[0]) { *out = goodTri[0]; return; }
	if (leftSide[0] && leftSide[1]) { *out = goodTri[1]; return; }
	if (leftSide[1] && leftSide[2]) { *out = goodTri[2]; return; }

	assert(0 && "WTF?!");
}

/// \internal
// Test if point p lies outside plane through abc
int PointOutsideOfPlane(const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c)
{
	VEC3 Tmp;
	Cross(&Tmp, b - a, c - a);
    return Dot(p - a, Tmp) >= 0.0f; // [AP AB AC] >= 0
}

void ClosestPointInTetrahedron(VEC3 *OutPoint, const VEC3 &p, const VEC3 &a, const VEC3 &b, const VEC3 &c, const VEC3 &d)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Start out assuming point inside all halfspaces, so closest to itself
	*OutPoint = p;
	float bestSqDist = FLT_MAX;
	// If point outside face abc then compute closest point on abc
	if (PointOutsideOfPlane(p, a, b, c)) {
		VEC3 q;
		ClosestPointInTriangle(&q, p, a, b, c);
		float sqDist = Dot(q - p, q - p);
		// Update best closest point if (squared) distance is less than current best
		if (sqDist < bestSqDist) bestSqDist = sqDist, *OutPoint = q;
	}
	// Repeat test for face acd
	if (PointOutsideOfPlane(p, a, c, d)) {
		VEC3 q;
		ClosestPointInTriangle(&q, p, a, c, d);
		float sqDist = Dot(q - p, q - p);
		if (sqDist < bestSqDist) bestSqDist = sqDist, *OutPoint = q;
	}
	// Repeat test for face adb
	if (PointOutsideOfPlane(p, a, d, b)) {
		VEC3 q;
		ClosestPointInTriangle(&q, p, a, d, b);
		float sqDist = Dot(q - p, q - p);
		if (sqDist < bestSqDist) bestSqDist = sqDist, *OutPoint = q;
	}
	// Repeat test for face bdc
	if (PointOutsideOfPlane(p, b, d, c)) {
		VEC3 q;
		ClosestPointInTriangle(&q, p, b, d, c);
		float sqDist = Dot(q - p, q - p);
		if (sqDist < bestSqDist) bestSqDist = sqDist, *OutPoint = q;
	}
}

float PointToRectDistanceSq(const VEC2 &p, const RECTF &rect)
{
	float v, SqDist = 0.f;
	
	v = p.x;
	if (v < rect.Min.x)
		SqDist += sqr(rect.Min.x - v);
	else if (v > rect.Max.x)
		SqDist += sqr(v - rect.Max.x);

	v = p.y;
	if (v < rect.Min.y)
		SqDist += sqr(rect.Min.y - v);
	else if (v > rect.Max.y)
		SqDist += sqr(v - rect.Max.y);

	return SqDist;
}

float PointToRectManhattanDistance(const VEC2 &p, const RECTF &rect)
{
	float v, Dist = 0.f;

	v = p.x;
	if (v < rect.Min.x)
		Dist += fabsf(rect.Min.x - v);
	else if (v > rect.Max.x)
		Dist += fabsf(v - rect.Max.x);

	v = p.y;
	if (v < rect.Min.y)
		Dist += fabsf(rect.Min.y - v);
	else if (v > rect.Max.y)
		Dist += fabsf(v - rect.Max.y);

	return Dist;
}

float PointToRectMaxDistance(const VEC2 &p, const RECTF &rect)
{
	float v;
	VEC2 Dist;

	v = p.x;
	if (v < rect.Min.x)
		Dist.x = fabsf(rect.Min.x - v);
	else if (v > rect.Max.x)
		Dist.x = fabsf(v - rect.Max.x);
	else
		Dist.x = 0.f;

	v = p.y;
	if (v < rect.Min.y)
		Dist.y = fabsf(rect.Min.y - v);
	else if (v > rect.Max.y)
		Dist.y = fabsf(v - rect.Max.y);
	else
		Dist.y = 0.f;

	return std::max(Dist.x, Dist.y);
}

float PointToBoxDistanceSq(const VEC3 &p, const BOX &box)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson
	
	float SqDist = 0.0f;
	for (unsigned i = 0; i < 3; i++)
	{
		// For each axis count any excess distance outside box extents
		float v = p[i];
		if (v < box.Min[i])
			SqDist += (box.Min[i] - v) * (box.Min[i] - v);
		if (v > box.Max[i])
			SqDist += (v - box.Max[i]) * (v - box.Max[i]);
	}
	return SqDist;
}

float PointToBoxManhattanDistance(const VEC3 &p, const BOX &box)
{
	float Dist = 0.0f;
	for (unsigned i = 0; i < 3; i++)
	{
		float v = p[i];
		if (v < box.Min[i])
			Dist += fabsf(box.Min[i] - v);
		if (v > box.Max[i])
			Dist += fabsf(v - box.Max[i]);
	}
	return Dist;
}

float PointToBoxMaxDistance(const VEC3 &p, const BOX &box)
{
	VEC3 Dist;
	for (unsigned i = 0; i < 3; i++)
	{
		float v = p[i];
		if (v < box.Min[i])
			Dist[i] = fabsf(box.Min[i] - v);
		else if (v > box.Max[i])
			Dist[i] = fabsf(v - box.Max[i]);
		else
			Dist[i] = 0.f;
	}
	return max3(Dist.x, Dist.y, Dist.z);
}

float RectToRectDistanceSq(const RECTF &rect1, const RECTF &rect2)
{
	float SqDist = 0.f;

	if (rect1.Max.x < rect2.Min.x)
		SqDist += sqr(rect2.Min.x - rect1.Max.x);
	else if (rect1.Min.x > rect2.Max.x)
		SqDist += sqr(rect1.Min.x - rect2.Max.x);

	if (rect1.Max.y < rect2.Min.y)
		SqDist += sqr(rect2.Min.y - rect1.Max.y);
	else if (rect1.Min.y > rect2.Max.y)
		SqDist += sqr(rect1.Min.y - rect2.Max.y);

	return SqDist;
}

float RectToRectManhattanDistance(const RECTF &rect1, const RECTF &rect2)
{
	float Dist = 0.f;

	if (rect1.Max.x < rect2.Min.x)
		Dist += fabsf(rect2.Min.x - rect1.Max.x);
	else if (rect1.Min.x > rect2.Max.x)
		Dist += fabsf(rect1.Min.x - rect2.Max.x);

	if (rect1.Max.y < rect2.Min.y)
		Dist += fabsf(rect2.Min.y - rect1.Max.y);
	else if (rect1.Min.y > rect2.Max.y)
		Dist += fabsf(rect1.Min.y - rect2.Max.y);

	return Dist;
}

float RectToRectMaxDistance(const RECTF &rect1, const RECTF &rect2)
{
	VEC2 Dist;

	if (rect1.Max.x < rect2.Min.x)
		Dist.x = fabsf(rect2.Min.x - rect1.Max.x);
	else if (rect1.Min.x > rect2.Max.x)
		Dist.x = fabsf(rect1.Min.x - rect2.Max.x);
	else
		Dist.x = 0.f;

	if (rect1.Max.y < rect2.Min.y)
		Dist.y = fabsf(rect2.Min.y - rect1.Max.y);
	else if (rect1.Min.y > rect2.Max.y)
		Dist.y = fabsf(rect1.Min.y - rect2.Max.y);
	else
		Dist.y = 0.f;

	return std::max(Dist.x, Dist.y);
}

float BoxToBoxDistanceSq(const BOX &box1, const BOX &box2)
{
	float SqDist = 0.f;
	for (int i = 0; i < 3; i++)
	{
		if (box1.Max[i] < box2.Min[i])
			SqDist += sqr(box2.Min[i] - box1.Max[i]);
		else if (box1.Min[i] > box2.Max[i])
			SqDist += sqr(box1.Min[i] - box2.Max[i]);
	}
	return SqDist;
}

float BoxToBoxManhattanDistance(const BOX &box1, const BOX &box2)
{
	float Dist = 0.f;
	for (int i = 0; i < 3; i++)
	{
		if (box1.Max[i] < box2.Min[i])
			Dist += fabsf(box2.Min[i] - box1.Max[i]);
		else if (box1.Min[i] > box2.Max[i])
			Dist += fabsf(box1.Min[i] - box2.Max[i]);
	}
	return Dist;
}

float BoxToBoxMaxDistance(const BOX &box1, const BOX &box2)
{
	VEC3 Dist;
	for (int i = 0; i < 3; i++)
	{
		if (box1.Max[i] < box2.Min[i])
			Dist[i] = fabsf(box2.Min[i] - box1.Max[i]);
		else if (box1.Min[i] > box2.Max[i])
			Dist[i] = fabsf(box1.Min[i] - box2.Max[i]);
		else
			Dist[i] = 0.f;
	}
	return max3(Dist.x, Dist.y, Dist.z);
}

float PointToObbDistanceSq(const VEC3 &Point, const OBB &Obb)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

    VEC3 v = Point - Obb.Center;
    float sqDist = 0.0f;
    for (uint i = 0; i < 3; i++) {
        // Project vector from box center to Point on each axis, getting the distance
        // of Point along that axis, and count any excess distance outside box extents
		float d = Dot(v, Obb.Axes[i]), excess = 0.0f;
        if (d < -Obb.HalfSize[i])
            excess = d + Obb.HalfSize[i];
        else if (d > Obb.HalfSize[i])
            excess = d - Obb.HalfSize[i];
        sqDist += excess * excess;
    }
    return sqDist;
}

bool CircleToRect(const VEC2 &circleCenter, float circleRadius, const RECTF &rect)
{
	return PointToRectDistanceSq(circleCenter, rect) <= sqr(circleRadius);
}

bool PointInSphere(const VEC3 &p, const VEC3 &SphereCenter, float SphereRadius)
{
	//return Distance(p, SphereCenter) <= SphereRadius;
	// Nowe, szybsze, mam nadziejê ¿e poprawne
	return DistanceSq(p, SphereCenter) <= SphereRadius * SphereRadius;
}

float PointToSphereDistance(const VEC3 &p, const VEC3 &SphereCenter, float SphereRadius)
{
	return Distance(p, SphereCenter) - SphereRadius;
}

void ClosestPointOnSphere(VEC3 *Out, const VEC3 &SphereCenter, float SphereRadius, const VEC3 &p)
{
	// Napisane na podstawie ksi¹¿ki "3D Math Primer for Graphics and Game Development", str. 280.
	VEC3 d = SphereCenter - p;
	float length_d = Length(d);
	*Out = p + d * ((length_d - SphereRadius) / length_d);
}

bool PointOnPlane(const VEC3 &p, const PLANE &plane)
{
	return FLOAT_ALMOST_ZERO(DotCoord(plane, p));
}

void ClosestPointOnPlane(VEC3 *Out, const PLANE &Plane, const VEC3 &p)
{
	// Napisane na podstawie ksi¹¿ki "3D Math Primer for Graphics and Game Development", str. 279.
	*Out = p - GetPlaneNormal(Plane) * DotCoord(Plane, p);
}

bool PointInTriangle(const VEC3 &point, const VEC3 &pa, const VEC3 &pb, const VEC3 &pc)
{
	// ród³o: Improved Collision detection and Response, Kasper Fauerby

	VEC3 e10 = pb-pa;
	VEC3 e20 = pc-pa;
	float a = Dot(e10, e10);
	float b = Dot(e10, e20);
	float c = Dot(e20, e20);
	float ac_bb = (a*c) - (b*b);
	VEC3 vp(point.x-pa.x, point.y-pa.y, point.z-pa.z);
	float d = Dot(vp, e10);
	float e = Dot(vp, e20);
	float x = (d*c) - (e*b);
	float y = (e*a) - (d*b);
	float z = x+y - ac_bb;
	return (( absolute_cast<unsigned>(z) & ~(absolute_cast<unsigned>(x)|absolute_cast<unsigned>(y)) ) & 0x80000000) != 0;
}

bool PointInFrustum(const VEC3 &p, const FRUSTUM_PLANES &Frustum)
{
	if (DotCoord(Frustum.Planes[0], p) <= 0.0f) return false;
	if (DotCoord(Frustum.Planes[1], p) <= 0.0f) return false;
	if (DotCoord(Frustum.Planes[2], p) <= 0.0f) return false;
	if (DotCoord(Frustum.Planes[3], p) <= 0.0f) return false;
	if (DotCoord(Frustum.Planes[4], p) <= 0.0f) return false;
	if (DotCoord(Frustum.Planes[5], p) <= 0.0f) return false;

	return true;
}

bool PointInFrustum(const VEC3 &p, const FRUSTUM_RADAR &Frustum)
{
	VEC3 OP = p - Frustum.GetEye();

	float f = Dot(OP, Frustum.GetForward());
	if (f < Frustum.GetZNear() || f > Frustum.GetZFar())
		return false;

	float r = Dot(OP, Frustum.GetRight());
	float RLimit = Frustum.GetRFactor() * f;
	if (r < -RLimit || r > RLimit)
		return false;

	float u = Dot(OP, Frustum.GetUp());
	float ULimit = Frustum.GetUFactor() * f;
	if (u < -ULimit || u > ULimit)
		return false;

	return true;
}

bool ClosestPointsOnLines(float *OutT1, float *OutT2, const VEC3 &RayOrig1, const VEC3 &RayDir1, const VEC3 &RayOrig2, const VEC3 &RayDir2)
{
	// Napisane na podstawie ksi¹¿ki "3D Math Primer for Graphics and Game Development", str. 283.

	VEC3 orig_diff = RayOrig2-RayOrig1;
	VEC3 dcr; Cross(&dcr, RayDir1, RayDir2);
	float denom = LengthSq(dcr);
	if (FLOAT_ALMOST_ZERO(denom))
		return false;
	float denom_inv = 1.0f / denom;

	VEC3 cr; Cross(&cr, orig_diff, RayDir2);
	*OutT1 = Dot(cr, dcr) * denom_inv;
	Cross(&cr, orig_diff, RayDir1);
	*OutT2 = Dot(cr, dcr) * denom_inv;

	return true;
}

bool RayToBox(float *OutT, const VEC3 &RayOrig, const VEC3 &RayDir, const BOX &Box)
{
	bool inside = true;
	float xt;

	if (RayOrig.x < Box.Min.x)
	{
		xt = Box.Min.x - RayOrig.x;
		xt /= RayDir.x;
		inside = false;
	}
	else if (RayOrig.x > Box.Max.x)
	{
		xt = Box.Max.x - RayOrig.x;
		xt /= RayDir.x;
		inside = false;
	}
	else
		xt = -1.0f;

	float yt;

	if (RayOrig.y < Box.Min.y)
	{
		yt = Box.Min.y - RayOrig.y;
		yt /= RayDir.y;
		inside = false;
	}
	else if (RayOrig.y > Box.Max.y)
	{
		yt = Box.Max.y - RayOrig.y;
		yt /= RayDir.y;
		inside = false;
	}
	else
		yt = -1.0f;

	float zt;

	if (RayOrig.z < Box.Min.z)
	{
		zt = Box.Min.z - RayOrig.z;
		zt /= RayDir.z;
		inside = false;
	}
	else if (RayOrig.z > Box.Max.z)
	{
		zt = Box.Max.z - RayOrig.z;
		zt /= RayDir.z;
		inside = false;
	}
	else
		zt = -1.0f;

	if (inside)
	{
		*OutT = 0.0f;
		return true;
	}

	// Select the farthest plane - this is the plane of intersection
	int plane = 0;

	float t = xt;
	if (yt > t)
	{
		plane = 1;
		t = yt;
	}

	if (zt > t)
	{
		plane = 2;
		t = zt;
	}

	// Check if the point of intersection lays within the box face

	switch(plane)
	{
	case 0: // ray intersects with yz plane
		{
			float y = RayOrig.y + RayDir.y * t;
			if (y < Box.Min.y || y > Box.Max.y) return false;
			float z = RayOrig.z + RayDir.z * t;
			if (z < Box.Min.z || z > Box.Max.z) return false;
		}
		break;
	case 1: // ray intersects with xz plane
		{
			float x = RayOrig.x + RayDir.x * t;
			if (x < Box.Min.x || x > Box.Max.x) return false;
			float z = RayOrig.z + RayDir.z * t;
			if (z < Box.Min.z || z > Box.Max.z) return false;
		}
		break;
	default:
		case 2: // ray intersects with xy plane
		{
			float x = RayOrig.x + RayDir.x * t;
			if (x < Box.Min.x || x > Box.Max.x) return false;
			float y = RayOrig.y + RayDir.y * t;
			if (y < Box.Min.y || y > Box.Max.y) return false;
		}
		break;
	}

	*OutT = t;
	return true;
}

bool RayToSphere(const VEC3 &RayOrig, const VEC3 &RayDir, const VEC3 &SphereCenter, float SphereRadius, float *OutT)
{
	// Równanie kwadratowe.
	// Napisane samodzielnie z ma³¹ pomoc¹:
	// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter1.htm
	// link znaleziony na:
	// http://www.realtimerendering.com/int/

	VEC3 RayOrig_minus_SphereCenter = RayOrig - SphereCenter;
	float a = Dot(RayDir, RayDir);
	float b = 2.f * Dot(RayDir, RayOrig_minus_SphereCenter);
	float c = Dot(RayOrig_minus_SphereCenter, RayOrig_minus_SphereCenter) - (SphereRadius * SphereRadius);
	float Delta = b * b - 4.f * a * c;

	if (Delta < 0.f)
		return false;

	float a_2 = 2.f * a;
	float minus_b = - b;
	float sqrt_Delta = sqrtf(Delta);

	// Pierwszy pierwiastek - ten mniejszy
	*OutT = (minus_b - sqrt_Delta) / a_2;
	// Przypadek ¿e ca³a sfera jest przed RayOrig - pierwiastek mniejszy to wynik
	if (*OutT >= 0.f)
		return true;
	// Drugi pierwiastek - ten wiêkszy
	*OutT = (minus_b + sqrt_Delta) / a_2;
	// Przypadek ¿e poczatek promienia jest wewn¹trz sfery
	if (*OutT >= 0.f)
	{
		*OutT = 0.f;
		return true;
	}
	// Przypadek ¿e sfera jest z ty³u za promieniem
	return true;
}

/*
Wersja stara, która liczy Ÿle! Nie wiem sk¹d pochodzi.

bool RayToSphere(const VEC3 &RayOrigin, const VEC3 &RayDir, const VEC3 &SphereCenter, float SphereRadius, float *t)
{
	VEC3 Q = SphereCenter - RayOrigin;
	float c = Length(Q);
	float v = Dot(Q, RayDir);
	float d = SphereRadius*SphereRadius - (c*c - v*v);
	// There was no intersection
	if (d < 0.0f)
		return false;
	// Return the distance to the [first] intersecting point
	*t = v - sqrtf(d);
	return true;
}

Jakiœ algorytm na to jest te¿ w ksi¹¿ce "3D Math Primer for Graphics and Game Development", str. 288.
*/

bool RayToPlane(const VEC3 &RayOrig, const VEC3 &RayDir, const PLANE &Plane, float *OutT, float *OutVD)
{
	// Napisane na podstawie:
	// http://www.siggraph.org/education/materials/HyperGraph/raytrace/rayplane_intersection.htm

	//       A xo + B yo + C zo + D
	// t = - ----------------------
	//         A xd + B yd + C zd

	// Ten sam wzór jest w ksi¹¿ce "3D Math Primer for Graphics and Game Development", str. 284.
	// Inna wersja dostêpna jest w ksi¹¿ce: "3D Game Engine Programming", Stefan Zerbst with Oliver Duvel, str. 136.

	*OutVD = Plane.a * RayDir.x + Plane.b * RayDir.y + Plane.c * RayDir.z;
	if (*OutVD == 0.0f)
		return false;
	
	*OutT = - (Plane.a * RayOrig.x + Plane.b * RayOrig.y + Plane.c * RayOrig.z + Plane.d) / *OutVD;
	
	return true;
}

bool RayToTriangle(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 &p0, const VEC3 &p1, const VEC3 &p2,
	bool BackfaceCulling,
	float *OutT)
{
	////// Nowy, piêkny algorytm :)
	// Znaleziony w Google Code Search:
	// http://www.google.com/codesearch?hl=en&q=+RayTriangleIntersect+show:M63-4o6bYUI:fUr9QIwtaKY:Dw059DARM5E&sa=N&cd=1&ct=rc&cs_p=http://www.angstrom-distribution.org/unstable/sources/ode-snapshot-20060210.tar.bz2&cs_f=ode-snapshot-20060210/ode/src/collision_trimesh_trimesh.cpp#first
	// Dopisa³em do niego Backface Culling, dzia³a bez problemu :D

	VEC3 tvec, pvec, qvec;

	// find vectors for two edges sharing vert0
	VEC3 edge1 = p1 - p0;
	VEC3 edge2 = p2 - p0;

	// begin calculating determinant - also used to calculate U parameter
	Cross(&pvec, RayDir, edge2);

	// if determinant is near zero, ray lies in plane of triangle
	float det = Dot(edge1, pvec);
	if (BackfaceCulling && det < 0.0f)
		return false;
	if (FLOAT_ALMOST_ZERO(det))
		return false;
	float inv_det = 1.0f / det;

	// calculate distance from vert0 to ray origin
	tvec = RayOrig - p0;

	// calculate U parameter and test bounds
	float u = Dot(tvec, pvec) * inv_det;
	if (u < 0.0f || u > 1.0f)
		return false;

	// prepare to test V parameter
	Cross(&qvec, tvec, edge1);

	// calculate V parameter and test bounds
	float v = Dot(RayDir, qvec) * inv_det;
	if (v < 0.0f || u + v > 1.0f)
		return false;

	// calculate t, ray intersects triangle
	if (OutT != NULL)
		*OutT = Dot(edge2, qvec) * inv_det;
	return true;

	////// Stary algorytm: Moller i Trumbore
	// Na podstawie ksi¹¿ki: "3D Game Engine Programming", Stefan Zerbst with Oliver Duvel, str. 134.
	// Dobry, ale niestety nie obs³uguje on Backface Culling i nijak nie uda³o mi siê tego do niego wprowadziæ.

/*	VEC3 pvec, tvec, qvec;

	VEC3 edge1 = p1 - p0;
	VEC3 edge2 = p2 - p0;

	Cross(&pvec, RayDir, edge2);

	// If close to 0 ray os parallel
	float det = Dot(edge1, pvec);

	//if (Cull && det < 0.0f)
	if (det < 0.0f)
		return false;
	if (FLOAT_ALMOST_ZERO(det))
		return false;

	// Distance to plane, < 0 means aray behind the plane
	tvec = RayOrig - p0;
	float u = Dot(tvec, pvec);
	if (u < 0.0f || u > det)
		return false;

	Cross(&qvec, tvec, edge1);
	float v = Dot(RayDir, qvec);
	if (v < 0.0f || u+v > det)
		return false;

	if (OutT)
	{
		*OutT = Dot(edge2, qvec);
		float InvDet = 1.0f / det;
		*OutT *= InvDet;
	}
	return true;*/

	// Jakiœ algorytm, du¿o d³u¿szy, jest te¿ w ksi¹¿ce "3D Math Primer for Graphics and Game Development", str. 294.
}

bool RayToQuad(const VEC3 &RayOrig, const VEC3 &RayDir, const VEC3 QuadPoints[4], VEC3 *OutPoint)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 pa = QuadPoints[0] - RayOrig;
	VEC3 pb = QuadPoints[1] - RayOrig;
	VEC3 pc = QuadPoints[2] - RayOrig;
	// Determine which triangle to test against by testing against diagonal first
	VEC3 m; Cross(&m, pc, RayDir);
	float v = Dot(pa, m); // ScalarTriple(RayDir, pa, pc);
	if (v >= 0.0f) {
		// Test intersection against triangle abc
		float u = -Dot(pb, m); // ScalarTriple(RayDir, pc, pb);
		if (u < 0.0f) return false;
		float w = ScalarTripleProduct(RayDir, pb, pa);
		if (w < 0.0f) return false;
		// Compute *OutPoint, *OutPoint = u*QuadPoints[0] + v*QuadPoints[1] + w*QuadPoints[2], from barycentric coordinates (u, v, w)
		float denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom; // w = 1.0f - u - v;
		*OutPoint = u*QuadPoints[0] + v*QuadPoints[1] + w*QuadPoints[2];
	} else {
		// Test intersection against triangle dac
		VEC3 pd = QuadPoints[3] - RayOrig;
		float u = Dot(pd, m); // ScalarTriple(RayDir, pd, pc);
		if (u < 0.0f) return false;
		float w = ScalarTripleProduct(RayDir, pa, pd);
		if (w < 0.0f) return false;
		v = -v;
		// Compute *OutPoint, *OutPoint = u*QuadPoints[0] + v*QuadPoints[3] + w*QuadPoints[2], from barycentric coordinates (u, v, w)
		float denom = 1.0f / (u + v + w);
		u *= denom;
		v *= denom;
		w *= denom; // w = 1.0f - u - v;
		*OutPoint = u*QuadPoints[0] + v*QuadPoints[3] + w*QuadPoints[2];
	}
	return true;
}

bool RayToFrustum(const VEC3 &RayOrig, const VEC3 &RayDir, const FRUSTUM_PLANES &Frustum, float *t_near, float *t_far)
{
	// To jest uniwersalny algorytm na kolizjê promienia/prostej/odcinka z wieloœcianem wypuk³ym.
	// Na podstawie Graphics Gems II, rozdzia³ V.1.

	*t_near = MINFLOAT;
	*t_far = MAXFLOAT;
	float vn, vd, t;

	for (uint i = 0; i < 6; i++)
	{
		vn = DotCoord(Frustum.Planes[i], RayOrig);
		vd = DotNormal(Frustum.Planes[i], RayDir);

		// Nie ma przeciêcia z p³aszczyzn¹
		if (FLOAT_ALMOST_ZERO(vd))
		{
			// Promieñ na zewn¹trz - na pewno nie ma przeciêcia
			if (vn < 0.0f)
				return false;
		}
		else
		{
			t = -vn / vd;

			if (vd > 0.0f)
				*t_near = std::max(*t_near, t);
			else
				*t_far = std::min(*t_far, t);

			if (*t_near > *t_far)
				return false;
		}
	}
	return (*t_near != MINFLOAT && *t_far != MAXFLOAT);
}

bool RayToCylinder(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 &CylinderBeg, const VEC3 &CylinderEnd, float CylinderRadius,
	float *OutT)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 d = CylinderEnd - CylinderBeg, m = RayOrig - CylinderBeg;
	float md = Dot(m, d);
	float nd = Dot(RayDir, d);
	float dd = Dot(d, d);
	// Test if segment fully outside either endcap of cylinder
	//if (md < 0.0f && md + nd < 0.0f) return false; // Segment outside ‘CylinderBeg’ side of cylinder
	//if (md > dd && md + nd > dd) return false;     // Segment outside ‘CylinderEnd’ side of cylinder
	float nn = Dot(RayDir, RayDir);
	float mn = Dot(m, RayDir);
	float a = dd * nn - nd * nd;
	float k = Dot(m, m) - CylinderRadius * CylinderRadius;
	float c = dd * k - md * md;
	if (fabsf(a) < FLOAT_EPSILON) {
		// Segment runs parallel to cylinder axis
		if (c > 0.0f) return false; // ‘a’ and thus the segment lie outside cylinder
		// Now known that segment intersects cylinder; figure out how it intersects
		if (md < 0.0f) *OutT = -mn / nn; // Intersect segment against ‘CylinderBeg’ endcap
		else if (md > dd) *OutT = (nd - mn) / nn; // Intersect segment against ‘CylinderEnd’ endcap
		else *OutT = 0.0f; // ‘a’ lies inside cylinder
		return true;
	}
	float b = dd * mn - nd * md;
	float discr = b * b - a * c;
	if (discr < 0.0f) return false; // No real roots; no intersection
	*OutT = (-b - sqrtf(discr)) / a;
	//if (*OutT < 0.0f || *OutT > 1.0f) return false; // Intersection lies outside segment
	if (md + *OutT * nd < 0.0f) {
		// Intersection outside cylinder on ‘CylinderBeg’ side
		if (nd <= 0.0f) return false; // Segment pointing away from endcap
		*OutT = -md / nd;
		// Keep intersection if Dot(S(*OutT) - CylinderBeg, S(*OutT) - CylinderBeg) <= CylinderRadius^2
		return k + 2 * *OutT * (mn + *OutT * nn) <= 0.0f;
	} else if (md + *OutT * nd > dd) {
		// Intersection outside cylinder on ‘CylinderEnd’ side
		if (nd >= 0.0f) return false; // Segment pointing away from endcap
		*OutT = (dd - md) / nd;
		// Keep intersection if Dot(S(*OutT) - CylinderEnd, S(*OutT) - CylinderEnd) <= CylinderRadius^2
		return k + dd - 2 * md + *OutT * (2 * (mn - nd) + *OutT * nn) <= 0.0f;
	}
	// Segment intersects cylinder between the end-caps; *OutT is correct
	return true;
}

bool PointInConvexPolyhedron(const VEC3 &Point, const PLANE PolyhedronPlanes[], size_t PolyhedronPlaneCount)
{
	return PointInConvexPolyhedron(Point, PolyhedronPlanes, PolyhedronPlaneCount, sizeof(PLANE));
}

bool PointInConvexPolyhedron(const VEC3 &Point, const void *PolyhedronPlanes, size_t PolyhedronPlaneCount, ptrdiff_t PolyhedronPlaneStride)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	const char *PolyhedronPlaneBytes = (const char*)PolyhedronPlanes;
	for (size_t Plane_i = 0; Plane_i < PolyhedronPlaneCount; Plane_i++)
	{
		const PLANE &Plane = *(const PLANE*)PolyhedronPlaneBytes;
		if (DotCoord(Plane, Point) < 0.0f)
			return false;
		PolyhedronPlaneBytes += PolyhedronPlaneStride;
	}
	return true;
}

bool RayToConvexPolyhedron(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const PLANE PolyhedronPlanes[], size_t PolyhedronPlaneCount,
	float *OutBegT, float *OutEndT)
{
	return RayToConvexPolyhedron(RayOrig, RayDir, PolyhedronPlanes, PolyhedronPlaneCount, sizeof(PLANE), OutBegT, OutEndT);
}

bool RayToConvexPolyhedron(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const void *PolyhedronPlanes, size_t PolyhedronPlaneCount, ptrdiff_t PolyhedronPlaneStride,
	float *OutBegT, float *OutEndT)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Set initial interval to being the whole segment. For RayOrig ray, *OutEndT should be
	// set to +FLT_MAX. For RayOrig line, additionally *OutBegT should be set to –FLT_MAX
	*OutBegT = FLT_MIN; //0.0f;
	*OutEndT = FLT_MAX; //1.0f;
	// Intersect segment against each plane 
	const char *PolyhedronPlaneBytes = (const char*)PolyhedronPlanes;
	for (size_t i = 0; i < PolyhedronPlaneCount; i++)
	{
		const PLANE &p = *(PLANE*)PolyhedronPlaneBytes;

		float denom = DotNormal(p, -RayDir);
		float dist = DotCoord(p, RayOrig);
		// Test if segment runs parallel to the plane
		if (denom == 0.0f) {
			// If so, return "no intersection" if segment lies outside plane
			if (dist > 0.0f) return false;
		} else {
			// Compute parameterized t value for intersection with current plane
			float t = dist / denom;
			if (denom < 0.0f) {
				// When entering halfspace, update *OutBegT if t is larger
				if (t > *OutBegT) *OutBegT = t;
			} else {
				// When exiting halfspace, update *OutEndT if t is smaller
				if (t < *OutEndT) *OutEndT = t;
			}
			// Exit with “no intersection” if intersection becomes empty
			if (*OutBegT > *OutEndT) return false;
		}

		PolyhedronPlaneBytes += PolyhedronPlaneStride;
	}
	// A nonzero logical intersection, so the segment intersects the polyhedron
	return true;
}

bool SphereToBox(const VEC3 &SphereCenter, float SphereRadius, const BOX &Box)
{
	VEC3 PointInBox;
	ClosestPointInBox(&PointInBox, Box, SphereCenter);
	return DistanceSq(SphereCenter, PointInBox) < SphereRadius*SphereRadius;
}

bool SphereToObb(const VEC3 &SphereCenter, float SphereRadius, const OBB &Obb)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 p;
	ClosestPointInObb(&p, SphereCenter, Obb);
	VEC3 v = p - SphereCenter;
	return Dot(v, v) <= SphereRadius * SphereRadius;
}

bool SphereToTriangle(const VEC3 &SphereCenter, float SphereRadius, const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 p;
	ClosestPointInTriangle(&p, SphereCenter, TriA, TriB, TriC);
	VEC3 v = p - SphereCenter;
	return Dot(v, v) <= SphereRadius * SphereRadius;
}

// Zaczête, niedokoñczone...
/*bool SphereToPolygon(
	const VEC3 &SphereCenter, float SphereRadius,
	const VEC3 PolygonPoints[], size_t PolygonPointCount, const PLANE *PolygonPlane)
{
	return SphereToPolygon(SphereCenter, SphereRadius, PolygonPoints, PolygonPointCount, sizeof(VEC3), PolygonPlane);
}

bool SphereToPolygon(
	const VEC3 &SphereCenter, float SphereRadius,
	const void *PolygonPoints, size_t PolygonPointCount, int PolygonPointStride, const PLANE *PolygonPlane)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Compute normal for the plane of the polygon
	VEC3 *n = PolygonNormal;
	VEC3 tmp_n;
	if (n == NULL)
	{
		Cross(&tmp_n, p.v[1] – p.v[0], p.v[2] – p.v[0]);
		Normalize(&tmp_n);
		n = &tmp_n;
	}
	// Compute the plane equation for p
	PLANE m; GetPlaneNormal(m) = n; m.d = -Dot(n, p.v[0]);
	// No intersection if sphere not intersecting plane of polygon
	if (!SphereToPlane(SphereCenter, SphereRadius, m)) return false;
	// Test to see if any one of the polygon edges pierces the sphere   
	for (int k = PolygonPointCount, i = 0, j = k - 1; i < k; j = i, i++) {
		float t;
		// Test if edge (p.v[j], p.v[i]) intersects s
		if (RayToSphere(p.v[j], p.v[i] – p.v[j], SphereCenter, SphereRadius, &t) && t >= 0.0f && t <= 1.0f)
			return true;
	}
	// Test if the orthogonal projection q of the sphere center onto m is inside p
	VEC3 q ; ClosestPointOnPlane(&q, m, SphereCenter);
	return PointInPolygon(q, p);
}*/

bool SphereInBox(const VEC3 &SphereCenter, float SphereRadius, const BOX &Box)
{
	return
		(SphereCenter.x - SphereRadius >= Box.Min.x) &&
		(SphereCenter.y - SphereRadius >= Box.Min.y) &&
		(SphereCenter.z - SphereRadius >= Box.Min.z) &&
		(SphereCenter.x + SphereRadius <= Box.Max.x) &&
		(SphereCenter.y + SphereRadius <= Box.Max.y) &&
		(SphereCenter.z + SphereRadius <= Box.Max.z);
}

bool BoxInSphere(const BOX &Box, const VEC3 &SphereCenter, float SphereRadius)
{
	VEC3 Corner;
	for (uint i = 0; i < 8; i++)
	{
		Box.GetCorner(&Corner, i);
		if (!PointInSphere(Corner, SphereCenter, SphereRadius))
			return false;
	}
	return true;
}

int ClassifyBoxToPlane(const PLANE &plane, const BOX &box)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 c; box.GetCenter(&c); // Compute AABB center
	VEC3 e = box.Max - c; // Compute positive extents

	// Compute the projection inverval radius of b onto L(t) = b.c + t * p.n
	float r = e.x * fabsf(plane.a) + e.y * fabsf(plane.b) + e.z * fabsf(plane.c);
	// Compute distance of box center from plane
	float s = DotCoord(plane, c);
	
	if (s < -r) return -1;
	else if (s < r) return 0;
	else return 1;
}

int ClassifyObbToPlane(const PLANE &Plane, const OBB &Obb)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Compute the projection interval radius of Obb onto L(t) = Obb.c + t * Plane.n
	float r =
		Obb.HalfSize[0]*fabsf(Dot(GetPlaneNormal(Plane), Obb.Axes[0])) +
		Obb.HalfSize[1]*fabsf(Dot(GetPlaneNormal(Plane), Obb.Axes[1])) +
		Obb.HalfSize[2]*fabsf(Dot(GetPlaneNormal(Plane), Obb.Axes[2]));
	// Compute distance of box center from plane
	float s = DotCoord(Plane, Obb.Center);
	// Intersection occurs when distance s falls within [-r,+r] interval
	if (s < -r) return -1;
	else if (s < r) return 0;
	else return 1;
}

bool TriangleInBox(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, const BOX &Box)
{
	return PointInBox(p1, Box) && PointInBox(p2, Box) && PointInBox(p3, Box);
}

/// \internal
#define FINDMINMAX(x0,x1,x2,min,max) \
  min = max = x0;   \
  if(x1<min) min=x1;\
  if(x1>max) max=x1;\
  if(x2<min) min=x2;\
  if(x2>max) max=x2;

/// \internal
int TriangleToBox_planeBoxOverlap(const VEC3 &normal, const VEC3 &vert, const VEC3 &maxbox)	// -NJMP-
{
  uint q;
  VEC3 vmin,vmax;
  float v;
  for(q=0;q<=2;q++)
  {
    v=vert[q];					// -NJMP-
    if(normal[q]>0.0f)
    {
      vmin[q]=-maxbox[q] - v;	// -NJMP-
      vmax[q]= maxbox[q] - v;	// -NJMP-
    }
    else
    {
      vmin[q]= maxbox[q] - v;	// -NJMP-
      vmax[q]=-maxbox[q] - v;	// -NJMP-
    }
  }
  if(Dot(normal,vmin)>0.0f) return 0;	// -NJMP-
  if(Dot(normal,vmax)>=0.0f) return 1;	// -NJMP-
  
  return 0;
}


/*======================== X-tests ========================*/
/// \internal
#define AXISTEST_X01(a, b, fa, fb)			   \
	p0 = a*v0.y - b*v0.z;			       	   \
	p2 = a*v2.y - b*v2.z;			       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize.y + fb * boxhalfsize.z;   \
	if(min>rad || max<-rad) return false;

/// \internal
#define AXISTEST_X2(a, b, fa, fb)			   \
	p0 = a*v0.y - b*v0.z;			           \
	p1 = a*v1.y - b*v1.z;			       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize.y + fb * boxhalfsize.z;   \
	if(min>rad || max<-rad) return false;

/*======================== Y-tests ========================*/
/// \internal
#define AXISTEST_Y02(a, b, fa, fb)			   \
	p0 = -a*v0.x + b*v0.z;		      	   \
	p2 = -a*v2.x + b*v2.z;	       	       	   \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
	rad = fa * boxhalfsize.x + fb * boxhalfsize.z;   \
	if(min>rad || max<-rad) return false;

/// \internal
#define AXISTEST_Y1(a, b, fa, fb)			   \
	p0 = -a*v0.x + b*v0.z;		      	   \
	p1 = -a*v1.x + b*v1.z;	     	       	   \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize.x + fb * boxhalfsize.z;   \
	if(min>rad || max<-rad) return false;
/*======================== Z-tests ========================*/
/// \internal
#define AXISTEST_Z12(a, b, fa, fb)			   \
	p1 = a*v1.x - b*v1.y;			           \
	p2 = a*v2.x - b*v2.y;			       	   \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
	rad = fa * boxhalfsize.x + fb * boxhalfsize.y;   \
	if(min>rad || max<-rad) return false;

/// \internal
#define AXISTEST_Z0(a, b, fa, fb)			   \
	p0 = a*v0.x - b*v0.y;				   \
	p1 = a*v1.x - b*v1.y;			           \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
	rad = fa * boxhalfsize.x + fb * boxhalfsize.y;   \
	if(min>rad || max<-rad) return false;

bool TriangleToBox(const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC, const BOX &Box)
{
	// Autor algorytmu: Tomas Akenine-Moller, http://www.ce.chalmers.se/staff/tomasm/code/

	VEC3 boxcenter;
	Box.GetCenter(&boxcenter);
	VEC3 boxhalfsize;
	Box.GetHalfSize(&boxhalfsize);
  // use separating axis theorem to test overlap between triangle and box
  // need to test for overlap in these directions:
  // 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle
  //    we do not even need to test these)
  // 2) normal of the triangle
  // 3) crossproduct(edge from tri, {x,y,z}-directin)
  //    this gives 3x3=9 more tests
   VEC3 v0,v1,v2;
//   float axis[3];
   float min,max,p0,p1,p2,rad,fex,fey,fez;		// -NJMP- "d" local variable removed
   VEC3 normal,e0,e1,e2;

   /* This is the fastest branch on Sun */
   /* move everything so that the boxcenter is in (0,0,0) */
   v0 = TriA - boxcenter;
   v1 = TriB - boxcenter;
   v2 = TriC - boxcenter;

   /* compute triangle edges */
   e0 = v1 - v0;      /* tri edge 0 */
   e1 = v2 - v1;      /* tri edge 1 */
   e2 = v0 - v2;      /* tri edge 2 */

   /* Bullet 3:  */
   /*  test the 9 tests first (this was faster) */
   fex = fabsf(e0.x);
   fey = fabsf(e0.y);
   fez = fabsf(e0.z);
   AXISTEST_X01(e0.z, e0.y, fez, fey);
   AXISTEST_Y02(e0.z, e0.x, fez, fex);
   AXISTEST_Z12(e0.y, e0.x, fey, fex);

   fex = fabsf(e1.x);
   fey = fabsf(e1.y);
   fez = fabsf(e1.z);
   AXISTEST_X01(e1.z, e1.y, fez, fey);
   AXISTEST_Y02(e1.z, e1.x, fez, fex);
   AXISTEST_Z0(e1.y, e1.x, fey, fex);

   fex = fabsf(e2.x);
   fey = fabsf(e2.y);
   fez = fabsf(e2.z);
   AXISTEST_X2(e2.z, e2.y, fez, fey);
   AXISTEST_Y1(e2.z, e2.x, fez, fex);
   AXISTEST_Z12(e2.y, e2.x, fey, fex);

   /* Bullet 1: */
   /*  first test overlap in the {x,y,z}-directions */
   /*  find min, max of the triangle each direction, and test for overlap in */
   /*  that direction -- this is equivalent to testing a minimal AABB around */
   /*  the triangle against the AABB */

   /* test in X-direction */
   FINDMINMAX(v0.x,v1.x,v2.x,min,max);
   if(min>boxhalfsize.x || max<-boxhalfsize.x) return false;

   /* test in Y-direction */
   FINDMINMAX(v0.y,v1.y,v2.y,min,max);
   if(min>boxhalfsize.y || max<-boxhalfsize.y) return false;

   /* test in Z-direction */
   FINDMINMAX(v0.z,v1.z,v2.z,min,max);
   if(min>boxhalfsize.z || max<-boxhalfsize.z) return false;

   /* Bullet 2: */
   /*  test if the box intersects the plane of the triangle */
   /*  compute plane equation of triangle: normal*x+d=0 */
   Cross(&normal,e0,e1);
   // -NJMP- (line removed here)
   if(!TriangleToBox_planeBoxOverlap(normal,v0,boxhalfsize)) return false;	// -NJMP-

   return true;   /* box and triangle overlaps */
}

#undef FINDMINMAX
#undef AXISTEST_X01
#undef AXISTEST_X2
#undef AXISTEST_Y02
#undef AXISTEST_Y1
#undef AXISTEST_Z12
#undef AXISTEST_Z0

bool BoxToFrustum_Fast(const BOX &Box, const FRUSTUM_PLANES &Frustum)
{
	// Na podstawie ksi¹¿ki "3D Game Engine Programming", Stefan Zerbst with Oliver Duvel

	VEC3 vmin;
	for (uint i = 0; i < 6; i++)
	{
		if (Frustum.Planes[i].a <= 0.0f)
			vmin.x = Box.Min.x;
		else
			vmin.x = Box.Max.x;

		if (Frustum.Planes[i].b <= 0.0f)
			vmin.y = Box.Min.y;
		else
			vmin.y = Box.Max.y;

		if (Frustum.Planes[i].c <= 0.0f)
			vmin.z = Box.Min.z;
		else
			vmin.z = Box.Max.z;

		if (DotCoord(Frustum.Planes[i], vmin) < 0.0f)
			return false;
	}
	return true;
}

bool BoxToFrustum(const BOX &Box, const FRUSTUM_PLANES &FrustumPlanes, const FRUSTUM_POINTS *OptFrustumPoints)
{
	// Najpierw test podstawowy
	if (!BoxToFrustum_Fast(Box, FrustumPlanes))
		return false;

	// Wylicz Frustum Points jeœli trzeba
	FRUSTUM_POINTS MyFrustumPoints;
	const FRUSTUM_POINTS *FrustumPointsPtr = OptFrustumPoints;
	if (FrustumPointsPtr == NULL)
	{
		MyFrustumPoints.Set(FrustumPlanes);
		FrustumPointsPtr = &MyFrustumPoints;
	}

	// Dodatkowy test wg Separating Axis Theorem

	VEC3 BoxCenter; Box.GetCenter(&BoxCenter);
	VEC3 FrustumCenter; FrustumPointsPtr->CalcCenter(&FrustumCenter);
	VEC3 BoxCorner;
	BoxCorner.x = (FrustumCenter.x > BoxCenter.x ? Box.Max.x : Box.Min.x);
	BoxCorner.y = (FrustumCenter.y > BoxCenter.y ? Box.Max.y : Box.Min.y);
	BoxCorner.z = (FrustumCenter.z > BoxCenter.z ? Box.Max.z : Box.Min.z);

	VEC3 BoxToFrustumVec = FrustumCenter - BoxCenter;
	VEC3 BoxToFrustumDir; Normalize(&BoxToFrustumDir, BoxToFrustumVec);

	PLANE SeparatingPlane;
	PointNormalToPlane(&SeparatingPlane, BoxCorner, BoxToFrustumDir);

	if (ClassifyFrustumToPlane(*FrustumPointsPtr, SeparatingPlane) > 0)
		return false;

	return true;
}

bool BoxToFrustum(const BOX &Box, const FRUSTUM_RADAR &Frustum)
{
	// Algorytm na podstawie kodu z ksi¹¿ki "Game Programming Gems", tom 5, rozdz. 1.6
	// "Improved Frustum Culling", Frank Puig Placeres

	VEC3 P;
	int OutOfLeft = 0, OutOfRight = 0, OutOfFar = 0, OutOfNear = 0, OutOfTop = 0, OutOfBottom = 0;
	bool IsInRightTest, IsInUpTest, IsInFrontTest;

	VEC3 Corners[2];
	Corners[0] = Box.Min - Frustum.GetEye();
	Corners[1] = Box.Max - Frustum.GetEye();

	for (uint i = 0; i < 8; i++)
	{
		IsInRightTest = IsInUpTest = IsInFrontTest = false;
		P.x = Corners[ i        & 1 ].x;
		P.y = Corners[ (i >> 2) & 1 ].y;
		P.z = Corners[ (i >> 1) & 1 ].z;

		float r = Dot(P, Frustum.GetRight());
		float u = Dot(P, Frustum.GetUp());
		float f = Dot(P, Frustum.GetForward());

		if (r < -Frustum.GetRFactor() * f)
			OutOfLeft++;
		else if (r > Frustum.GetRFactor() * f)
			OutOfRight++;
		else
			IsInRightTest = true;

		if (u < -Frustum.GetUFactor() * f)
			OutOfBottom++;
		else if (u > Frustum.GetUFactor() * f)
			OutOfTop++;
		else
			IsInUpTest = true;

		if (f < -Frustum.GetZNear())
			OutOfNear++;
		else if (f > Frustum.GetZFar())
			OutOfFar++;
		else
			IsInUpTest = true;

		if (IsInRightTest && IsInFrontTest && IsInUpTest)
			return true;
	}

	if (OutOfLeft == 8 || OutOfRight == 8 || OutOfFar == 8 || OutOfNear == 8 || OutOfTop == 8 || OutOfBottom == 8)
		return false;
	return true;
}

bool BoxInFrustum(const BOX &Box, const FRUSTUM_PLANES &Frustum)
{
	// Ka¿dy punkt AABB musi le¿eæ w jego wnêtrzu
	if (!PointInFrustum(Box.Min, Frustum)) return false;
	if (!PointInFrustum(VEC3(Box.Max.x, Box.Min.y, Box.Min.z), Frustum)) return false;
	if (!PointInFrustum(VEC3(Box.Min.x, Box.Max.y, Box.Min.z), Frustum)) return false;
	if (!PointInFrustum(VEC3(Box.Max.x, Box.Max.y, Box.Min.z), Frustum)) return false;
	if (!PointInFrustum(VEC3(Box.Min.x, Box.Min.y, Box.Max.z), Frustum)) return false;
	if (!PointInFrustum(VEC3(Box.Max.x, Box.Min.y, Box.Max.z), Frustum)) return false;
	if (!PointInFrustum(VEC3(Box.Min.x, Box.Max.y, Box.Max.z), Frustum)) return false;
	if (!PointInFrustum(Box.Max, Frustum)) return false;
	return true;
}

bool SphereToSphere(const VEC3 &Center1, float Radius1, const VEC3 &Center2, float Radius2)
{
	float x = Radius1 + Radius2;
	return DistanceSq(Center1, Center2) <= x*x;
}

bool SphereToPlane(const VEC3 &SphereCenter, float SphereRadius, const PLANE &Plane)
{
	return fabsf(DotCoord(Plane, SphereCenter)) <= SphereRadius;
}

int ClassifySphereToPlane(const VEC3 &SphereCenter, float SphereRadius, const PLANE &Plane)
{
	float d = DotCoord(Plane, SphereCenter);
	if (d > SphereRadius)
		return +1;
	else if (d < -SphereRadius)
		return -1;
	else
		return 0;
}

bool CapsuleToPlane(const VEC3 &CapsuleBeg, const VEC3 &CapsuleEnd, float CapsuleRadius, const PLANE &Plane)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Get the distance for both CapsuleBeg and CapsuleEnd from plane Plane
	float adist = DotCoord(Plane, CapsuleBeg);
	float bdist = DotCoord(Plane, CapsuleEnd);
	// Intersects if on different sides of plane (distances have different signs)
	if (adist * bdist < 0.0f) return true;
	// Intersects if start or end position within radius from plane
	if (fabsf(adist) <= CapsuleRadius || fabsf(bdist) <= CapsuleRadius) return true;
	// No intersection
	return false;
}

bool SphereToFrustum_Fast(const VEC3 &SphereCenter, float SphereRadius, const FRUSTUM_PLANES &Frustum)
{
	SphereRadius = -SphereRadius;
	if (DotCoord(Frustum.Planes[0], SphereCenter) <= SphereRadius) return false;
	if (DotCoord(Frustum.Planes[1], SphereCenter) <= SphereRadius) return false;
	if (DotCoord(Frustum.Planes[2], SphereCenter) <= SphereRadius) return false;
	if (DotCoord(Frustum.Planes[3], SphereCenter) <= SphereRadius) return false;
	if (DotCoord(Frustum.Planes[4], SphereCenter) <= SphereRadius) return false;
	if (DotCoord(Frustum.Planes[5], SphereCenter) <= SphereRadius) return false;

	return true;
}

bool SphereToFrustum(
	const VEC3 &SphereCenter, float SphereRadius,
	const FRUSTUM_PLANES &FrustumPlanes, const FRUSTUM_POINTS *OptFrustumPoints)
{
	if (!SphereToFrustum_Fast(SphereCenter, SphereRadius, FrustumPlanes))
		return false;

	// Wylicz Frustum Points jeœli trzeba
	FRUSTUM_POINTS MyFrustumPoints;
	const FRUSTUM_POINTS *FrustumPointsPtr = OptFrustumPoints;
	if (FrustumPointsPtr == NULL)
	{
		MyFrustumPoints.Set(FrustumPlanes);
		FrustumPointsPtr = &MyFrustumPoints;
	}

	// Dodatkowy test wg Separating Axis Theorem

	VEC3 FrustumCenter; FrustumPointsPtr->CalcCenter(&FrustumCenter);
	VEC3 SphereToFrustumVec = FrustumCenter - SphereCenter;
	VEC3 SphereToFrustumDir; Normalize(&SphereToFrustumDir, SphereToFrustumVec);

	PLANE SeparatingPlane;
	PointNormalToPlane(&SeparatingPlane, SphereCenter + SphereToFrustumDir * SphereRadius, SphereToFrustumDir);

	if (ClassifyFrustumToPlane(*FrustumPointsPtr, SeparatingPlane) > 0)
		return false;

	return true;
}

bool SphereToFrustum(const VEC3 &SphereCenter, float SphereRadius, const FRUSTUM_RADAR &Frustum)
{
	VEC3 OP = SphereCenter - Frustum.GetEye();

	float f = Dot(OP, Frustum.GetForward());
	if (f < Frustum.GetZNear() - SphereRadius || f > Frustum.GetZFar() + SphereRadius)
		return false;

	// dr i du to jest poprawka, której nie uwzglêdnia oryginalny artyku³ o reprezentacji radarowej
	// w ksi¹¿ce "Game Programming Gems 5", tom 5, rozdz. 1.6 - "Improved Frustum Culling",
	// Frank Puig Placeres, ale uwzglêdnia j¹ autor "View Frustum Culling Tutorial",
	// António Ramires Fernandes, http://www.lighthouse3d.com/opengl/viewfrustum/index.php

	float dr = SphereRadius * Frustum.GetRSphereFactor();
	float r = Dot(OP, Frustum.GetRight());
	float RLimit = Frustum.GetRFactor() * f;
	if (r < -RLimit - dr || r > RLimit + dr)
		return false;

	float du = SphereRadius * Frustum.GetUSphereFactor();
	float u = Dot(OP, Frustum.GetUp());
	float ULimit = Frustum.GetUFactor() * f;
	if (u < -ULimit - du || u > ULimit + du)
		return false;

	return true;
}

bool SphereInFrustum(const VEC3 &SphereCenter, float SphereRadius, const FRUSTUM_PLANES &Frustum)
{
	if (DotCoord(Frustum.Planes[0], SphereCenter) < SphereRadius) return false;
	if (DotCoord(Frustum.Planes[1], SphereCenter) < SphereRadius) return false;
	if (DotCoord(Frustum.Planes[2], SphereCenter) < SphereRadius) return false;
	if (DotCoord(Frustum.Planes[3], SphereCenter) < SphereRadius) return false;
	if (DotCoord(Frustum.Planes[4], SphereCenter) < SphereRadius) return false;
	if (DotCoord(Frustum.Planes[5], SphereCenter) < SphereRadius) return false;

	return true;
}

bool Intersect2Planes(const PLANE &Plane1, const PLANE &Plane2, VEC3 *OutLineOrig, VEC3 *OutLineDir)
{
	// Nowa wersja
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Compute direction of intersection line
	Cross(OutLineDir, GetPlaneNormal(Plane1), GetPlaneNormal(Plane2));

	// If d is (near) zero, the planes are parallel (and separated)
	// or coincident, so they’re not considered intersecting
	float denom = Dot(*OutLineDir, *OutLineDir);
	if (denom < FLOAT_EPSILON) return false;

	// Compute point on intersection line
	Cross(OutLineOrig, GetPlaneNormal(Plane1)*Plane2.d - GetPlaneNormal(Plane2)*Plane1.d, *OutLineDir);
	// (Ksi¹¿ka mówi, ¿e gdyby p³aszczyzny by³y znormalizowane, niepotrzebne jest to dzielenie przez denom.)
	*OutLineOrig /= denom;
	return true;


	// Stara wersja, zamotana
/*	float dot = Dot(GetPlaneNormal(Plane1), GetPlaneNormal(Plane2));
	if (FLOAT_ALMOST_ZERO(dot))
		return false;
	Cross(LineDir, GetPlaneNormal(Plane1), GetPlaneNormal(Plane2));
	float abs;
	float maxabs = fabsf(LineDir->x);
	int index = 0;
	if ((abs = fabsf(LineDir->y)) > maxabs) { maxabs = abs; index = 1; }
	if ((abs = fabsf(LineDir->z)) > maxabs) { maxabs = abs; index = 2; }

	switch (index)
	{
	case 0:
		LineOrig->x = 0.0f;
		LineOrig->y = (Plane1.c*Plane2.d - Plane2.c*Plane1.d) / LineDir->x;
		LineOrig->z = (Plane2.b*Plane1.d - Plane1.b*Plane2.d) / LineDir->x;
		break;
	case 1:
		LineOrig->x = (Plane2.c*Plane1.d - Plane1.c*Plane2.d) / LineDir->y;
		LineOrig->y = 0.0f;
		LineOrig->z = (Plane1.a*Plane2.d - Plane2.a*Plane1.d) / LineDir->y;
		break;
	case 2:
		LineOrig->x = (Plane1.b*Plane2.d - Plane2.b*Plane1.d) / LineDir->z;
		LineOrig->y = (Plane2.a*Plane1.d - Plane1.a*Plane2.d) / LineDir->z;
		LineOrig->z = 0.0f;
		break;
	}
	return true;*/
}

/* Stara wersja, sprawdzone ¿e dzia³a tak samo jak powy¿sza tylko wolniej i zwraca
inny punkt na prostej (ale ten mo¿e byæ dowolny).

bool PlaneToPlane(const PLANE &Plane1, const PLANE &Plane2, VEC3 *OutRayOrig, VEC3 *OutRayDir)
{
	VEC3 cr;
	float lsq;
	// If cross product equals 0 planes are parallel
	Cross(&cr, Plane1.GetNormal(), Plane2.GetNormal());
	lsq = LengthSq(cr);
	if (FLOAT_ALMOST_ZERO(lsq))
		return false;

	// Intersection line if needed
	if (OutRayOrig && OutRayDir)
	{
		float N00 = LengthSq(Plane1.GetNormal());
		float N01 = Dot(Plane1.GetNormal(), Plane2.GetNormal());
		float N11 = LengthSq(Plane2.GetNormal());
		float Det = N00*N11 - N01*N01;
		if (FLOAT_ALMOST_ZERO(Det))
			return false;
		float InvDet = 1.0f / Det;
		// Tu musia³em odwróciæ sk³adniki odejmowania celem skorygowania D,
		// bo by³o uwzglêdniane odwrotnie ni¿ ja stosujê w swojej reprezentacji p³aszczyzny.
		float C0 = (N01*Plane2.d - N11*Plane1.d) * InvDet;
		float C1 = (N01*Plane1.d - N00*Plane2.d) * InvDet;
		*OutRayDir = cr;
		*OutRayOrig = Plane1.GetNormal() * C0 + Plane2.GetNormal() * C1;
	}

	return true;
}*/

bool Intersect3Planes(const PLANE &P1, const PLANE &P2, const PLANE &P3, VEC3 *OutP)
{
	// Nowy algorytm
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 u; Cross(&u, GetPlaneNormal(P2), GetPlaneNormal(P3));
	float denom = Dot(GetPlaneNormal(P1), u);
	if (fabsf(denom) < FLOAT_EPSILON) return false; // Planes do not intersect in a point
	VEC3 CrossTmp; Cross(&CrossTmp, GetPlaneNormal(P1), P2.d * GetPlaneNormal(P3) - P3.d * GetPlaneNormal(P2));
	*OutP = (CrossTmp - P1.d * u) / denom;
	return true;

	// Stary algorytm, bardziej skomplikowany.
/*	// Get the intersection point of three planes.
	//                    -1
	// |x|     | n1.x n1.y n1.z |     |d1|
	// |y|  =  | n2.x n2.y n2.z |  X  |d2|
	// |z|     | n3.x n3.y n3.z |     |d3|

	// Inny algorytm (albo inaczej zapisany) jest te¿ w ksi¹¿ce "3D Math Primer for Graphics and Game Development", str. 286.

	float fDet;
    float MN[9]  = { P1.a, P1.b, P1.c, P2.a, P2.b, P2.c, P3.a, P3.b, P3.c };
    float IMN[9] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float MD[3]  = { -P1.d, -P2.d , -P3.d };

    IMN[0] =   MN[4] * MN[8] - MN[5] * MN[7];
    IMN[3] = -(MN[3] * MN[8] - MN[5] * MN[6]);
    IMN[6] =   MN[3] * MN[7] - MN[4] * MN[6];

    fDet = MN[0] * IMN[0] + MN[1] * IMN[3] + MN[2] * IMN[6];

    if(FLOAT_ALMOST_ZERO(fDet))
        return false;

    IMN[1] = -(MN[1] * MN[8] - MN[2] * MN[7]);
    IMN[4] =   MN[0] * MN[8] - MN[2] * MN[6];
    IMN[7] = -(MN[0] * MN[7] - MN[1] * MN[6]);
    IMN[2] =   MN[1] * MN[5] - MN[2] * MN[4];
    IMN[5] = -(MN[0] * MN[5] - MN[2] * MN[3]);
    IMN[8] =   MN[0] * MN[4] - MN[1] * MN[3];

    fDet = 1.0f / fDet;

    IMN[0] *= fDet;
    IMN[1] *= fDet;
    IMN[2] *= fDet;
    IMN[3] *= fDet;
    IMN[4] *= fDet;
    IMN[5] *= fDet;
    IMN[6] *= fDet;
    IMN[7] *= fDet;
    IMN[8] *= fDet;

    OutP->x = IMN[0] * MD[0] + IMN[1] * MD[1] + IMN[2] * MD[2];
    OutP->y = IMN[3] * MD[0] + IMN[4] * MD[1] + IMN[5] * MD[2];
    OutP->z = IMN[6] * MD[0] + IMN[7] * MD[1] + IMN[8] * MD[2];

    return true;*/
}

bool PlaneToTriangle(const PLANE &Plane, const VEC3 &p0, const VEC3 &p1, const VEC3 &p2)
{
	float F = DotCoord(Plane, p0);
	if (F == 0.0f)
		return true;
	if (DotCoord(Plane, p1)*F <= 0.0f || DotCoord(Plane, p2)*F <= 0.0f)
		return true;
	return false;
}

int ClassifyPlaneToTriangle(const PLANE &Plane, const VEC3 &p0, const VEC3 &p1, const VEC3 &p2)
{
	float F = DotCoord(Plane, p0);
	if (F == 0.0f)
		return 0;
	if (DotCoord(Plane, p1)*F <= 0.0f || DotCoord(Plane, p2)*F <= 0.0f)
		return 0;
	return (F > 0.0f) ? +1 : -1;
}

int ClassifyFrustumToPlane(const FRUSTUM_POINTS &Frustum, const PLANE &Plane)
{
	float a = DotCoord(Plane, Frustum.Points[0]); if (a == 0.0f) return 0;
	float b = DotCoord(Plane, Frustum.Points[1]); if (b*a <= 0.0f) return 0;
	b = DotCoord(Plane, Frustum.Points[2]); if (b*a <= 0.0f) return 0;
	b = DotCoord(Plane, Frustum.Points[3]); if (b*a <= 0.0f) return 0;
	b = DotCoord(Plane, Frustum.Points[4]); if (b*a <= 0.0f) return 0;
	b = DotCoord(Plane, Frustum.Points[5]); if (b*a <= 0.0f) return 0;
	b = DotCoord(Plane, Frustum.Points[6]); if (b*a <= 0.0f) return 0;
	b = DotCoord(Plane, Frustum.Points[7]); if (b*a <= 0.0f) return 0;
	return (a > 0.0f) ? +1 : -1;
}

/// \internal
// Returns 2 times the signed triangle area. The result is positive if
// abc is ccw, negative if abc is cw, zero if abc is degenerate.
float SegmentToSegment_Signed2DTriArea(const VEC2 &a, const VEC2 &b, const VEC2 &c)
{
    return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

bool SegmentToSegment(const VEC2 &Seg1_Beg, const VEC2 &Seg1_End, const VEC2 &Seg2_Beg, const VEC2 &Seg2_End, float *OutSeg1_t)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Sign of areas correspond to which side of ab points Seg2_Beg and Seg2_End are
	float a1 = SegmentToSegment_Signed2DTriArea(Seg1_Beg, Seg1_End, Seg2_End); // Compute winding of abd (+ or -)
	float a2 = SegmentToSegment_Signed2DTriArea(Seg1_Beg, Seg1_End, Seg2_Beg); // To intersect, must have sign opposite of a1

	// If Seg2_Beg and Seg2_End are on different sides of ab, areas have different signs
	if (a1 * a2 < 0.0f) {
		// Compute signs for Seg1_Beg and Seg1_End with respect to segment cd
		float a3 = SegmentToSegment_Signed2DTriArea(Seg2_Beg, Seg2_End, Seg1_Beg); // Compute winding of cda (+ or -)
		// Since area is constant a1-a2 = a3-a4, or a4=a3+a2-a1
		//      float a4 = Signed2DTriArea(Seg2_Beg, Seg2_End, Seg1_End); // Must have opposite sign of a3
		float a4 = a3 + a2 - a1;
		// Points Seg1_Beg and Seg1_End on different sides of cd if areas have different signs
		if (a3 * a4 < 0.0f) {
			// Segments intersect. Find intersection point along L(t)=Seg1_Beg+t*(Seg1_End-Seg1_Beg).
			// Given height h1 of Seg1_Beg over cd and height h2 of Seg1_End over cd,
			// t = h1 / (h1 - h2) = (Seg1_End*h1/2) / (Seg1_End*h1/2 - Seg1_End*h2/2) = a3 / (a3 - a4),
			// where Seg1_End (the base of the triangles cda and cdb, i.e., the length
			// of cd) cancels out.
			if (OutSeg1_t) *OutSeg1_t = a3 / (a3 - a4);
			return true;
		}
	}

	// Segments not intersecting (or collinear)
	return false;
}

bool IntersectLines(const VEC2 &p1, const VEC2 &p2, const VEC2 &p3, const VEC2 &p4)
{
	// NIE TESTOWANA

	// test 1 - bouding boksy
	RECTF r1, r2;

	r1.Min.x = std::min(p1.x, p2.x);
	r1.Max.x = std::max(p1.x, p2.x);
	r1.Min.y = std::min(p1.y, p2.y);
	r1.Max.y = std::max(p1.y, p2.y);

	r2.Min.x = std::min(p3.x, p4.x);
	r2.Max.x = std::max(p3.x, p4.x);
	r2.Min.y = std::min(p3.y, p4.y);
	r2.Max.y = std::max(p3.y, p4.y);

	if (!RectToRect(r1, r2))
		return false;

	// test 2 - odcinek 1 z 2
	VEC2 v, v1, v2;
	float d1, d2;
	v = p2 - p1;

	v1 = p3 - p1;
	v2 = p4 - p1;
	d1 = v.x*v1.y - v.y*v1.x;
	d2 = v.x*v2.y - v.y*v2.x;

	if (d1*d2 > 0.0f)
		return false;

	// test 3 - odcinek 2 z 1
	v = p4 - p3;

	v1 = p1 - p3;
	v2 = p2 - p3;
	d1 = v.x*v1.y - v.y*v1.x;
	d2 = v.x*v2.y - v.y*v2.x;

	if (d1*d2 > 0.0f)
		return false;

	// wszystko OK
	return true;
}

/* Pocz¹tek algorytmu TriangleToTriangle
Autorzy: P. Guigue, O. Devillers */

#define CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) {\
  v1=p2-q1;\
  v2=p1-q1;\
  Cross(&N1,v1,v2);\
  v1=q2-q1;\
  if (Dot(v1,N1) > 0.0f) return false;\
  v1=p2-p1;\
  v2=r1-p1;\
  Cross(&N1,v1,v2);\
  v1=r2-p1; \
  if (Dot(v1,N1) > 0.0f) return false;\
  else return true; }

#define ORIENT_2D(a, b, c)  ((a[0]-c[0])*(b[1]-c[1])-(a[1]-c[1])*(b[0]-c[0]))

#define INTERSECTION_TEST_VERTEX(P1, Q1, R1, P2, Q2, R2) {\
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f)\
    if (ORIENT_2D(R2,Q2,Q1) <= 0.0f)\
      if (ORIENT_2D(P1,P2,Q1) > 0.0f) {\
	if (ORIENT_2D(P1,Q2,Q1) <= 0.0f) return true; \
	else return false;} else {\
	if (ORIENT_2D(P1,P2,R1) >= 0.0f)\
	  if (ORIENT_2D(Q1,R1,P2) >= 0.0f) return true; \
	  else return false;\
	else return false;}\
    else \
      if (ORIENT_2D(P1,Q2,Q1) <= 0.0f)\
	if (ORIENT_2D(R2,Q2,R1) <= 0.0f)\
	  if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) return true; \
	  else return false;\
	else return false;\
      else return false;\
  else\
    if (ORIENT_2D(R2,P2,R1) >= 0.0f) \
      if (ORIENT_2D(Q1,R1,R2) >= 0.0f)\
	if (ORIENT_2D(P1,P2,R1) >= 0.0f) return true;\
	else return false;\
      else \
	if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) {\
	  if (ORIENT_2D(R2,R1,Q2) >= 0.0f) return true; \
	  else return false; }\
	else return false; \
    else  return false; \
 };

#define INTERSECTION_TEST_EDGE(P1, Q1, R1, P2, Q2, R2) { \
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f) {\
    if (ORIENT_2D(P1,P2,Q1) >= 0.0f) { \
        if (ORIENT_2D(P1,Q1,R2) >= 0.0f) return true; \
        else return false;} else { \
      if (ORIENT_2D(Q1,R1,P2) >= 0.0f){ \
	if (ORIENT_2D(R1,P1,P2) >= 0.0f) return true; else return false;} \
      else return false; } \
  } else {\
    if (ORIENT_2D(R2,P2,R1) >= 0.0f) {\
      if (ORIENT_2D(P1,P2,R1) >= 0.0f) {\
	if (ORIENT_2D(P1,R1,R2) >= 0.0f) return true;  \
	else {\
	  if (ORIENT_2D(Q1,R1,R2) >= 0.0f) return true; else return false;}}\
      else  return false; }\
    else return false; }}

/* Permutation in a canonical form of T2's vertices */
#define TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
  if (dp2 > 0.0f) { \
     if (dq2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2) \
     else if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
     else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) }\
  else if (dp2 < 0.0f) { \
    if (dq2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
    else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
    else CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
  } else { \
    if (dq2 < 0.0f) { \
      if (dr2 >= 0.0f)  CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
      else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2)\
    } \
    else if (dq2 > 0.0f) { \
      if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
      else  CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
    } \
    else  { \
      if (dr2 > 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
      else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2)\
      else return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1,N2);\
     }}}
  
static bool ccw_tri_tri_intersection_2d(
	const VEC2 &p1, const VEC2 &q1, const VEC2 &r1, 
	const VEC2 &p2, const VEC2 &q2, const VEC2 &r2) {
  if ( ORIENT_2D(p2,q2,p1) >= 0.0f ) {
    if ( ORIENT_2D(q2,r2,p1) >= 0.0f ) {
      if ( ORIENT_2D(r2,p2,p1) >= 0.0f ) return 1;
      else INTERSECTION_TEST_EDGE(p1,q1,r1,p2,q2,r2)
    } else {  
      if ( ORIENT_2D(r2,p2,p1) >= 0.0f ) 
	INTERSECTION_TEST_EDGE(p1,q1,r1,r2,p2,q2)
      else INTERSECTION_TEST_VERTEX(p1,q1,r1,p2,q2,r2)}}
  else {
    if ( ORIENT_2D(q2,r2,p1) >= 0.0f ) {
      if ( ORIENT_2D(r2,p2,p1) >= 0.0f ) 
	INTERSECTION_TEST_EDGE(p1,q1,r1,q2,r2,p2)
      else  INTERSECTION_TEST_VERTEX(p1,q1,r1,q2,r2,p2)}
    else INTERSECTION_TEST_VERTEX(p1,q1,r1,r2,p2,q2)}
};

static bool tri_tri_overlap_test_2d(
	const VEC2 &p1, const VEC2 &q1, const VEC2 &r1, 
	const VEC2 &p2, const VEC2 &q2, const VEC2 &r2) {
  if ( ORIENT_2D(p1,q1,r1) < 0.0f )
    if ( ORIENT_2D(p2,q2,r2) < 0.0f )
      return ccw_tri_tri_intersection_2d(p1,r1,q1,p2,r2,q2);
    else
      return ccw_tri_tri_intersection_2d(p1,r1,q1,p2,q2,r2);
  else
    if ( ORIENT_2D(p2,q2,r2) < 0.0f )
      return ccw_tri_tri_intersection_2d(p1,q1,r1,p2,r2,q2);
    else
      return ccw_tri_tri_intersection_2d(p1,q1,r1,p2,q2,r2);
};

static bool coplanar_tri_tri3d(
	const VEC3 &p1, const VEC3 &q1, const VEC3 &r1,
	const VEC3 &p2, const VEC3 &q2, const VEC3 &r2,
	const VEC3 &normal_1, const VEC3 &normal_2)
{
  float P1[2],Q1[2],R1[2];
  float P2[2],Q2[2],R2[2];

  float n_x, n_y, n_z;

  n_x = ((normal_1[0]<0)?-normal_1[0]:normal_1[0]);
  n_y = ((normal_1[1]<0)?-normal_1[1]:normal_1[1]);
  n_z = ((normal_1[2]<0)?-normal_1[2]:normal_1[2]);

  /* Projection of the triangles in 3D onto 2D such that the area of
     the projection is maximized. */
  if (( n_x > n_z ) && ( n_x >= n_y )) {
    // Project onto plane YZ
      P1[0] = q1[2]; P1[1] = q1[1];
      Q1[0] = p1[2]; Q1[1] = p1[1];
      R1[0] = r1[2]; R1[1] = r1[1]; 
      P2[0] = q2[2]; P2[1] = q2[1];
      Q2[0] = p2[2]; Q2[1] = p2[1];
      R2[0] = r2[2]; R2[1] = r2[1]; 
  } else if (( n_y > n_z ) && ( n_y >= n_x )) {
    // Project onto plane XZ
    P1[0] = q1[0]; P1[1] = q1[2];
    Q1[0] = p1[0]; Q1[1] = p1[2];
    R1[0] = r1[0]; R1[1] = r1[2]; 
    P2[0] = q2[0]; P2[1] = q2[2];
    Q2[0] = p2[0]; Q2[1] = p2[2];
    R2[0] = r2[0]; R2[1] = r2[2]; 
  } else {
    // Project onto plane XY
    P1[0] = p1[0]; P1[1] = p1[1]; 
    Q1[0] = q1[0]; Q1[1] = q1[1]; 
    R1[0] = r1[0]; R1[1] = r1[1]; 
    P2[0] = p2[0]; P2[1] = p2[1]; 
    Q2[0] = q2[0]; Q2[1] = q2[1]; 
    R2[0] = r2[0]; R2[1] = r2[1]; 
  }
  return tri_tri_overlap_test_2d(P1,Q1,R1,P2,Q2,R2);
};

bool TriangleToTriangle(
	const VEC3 &Tri1_A,const VEC3 &Tri1_B,const VEC3 &Tri1_C,
	const VEC3 &Tri2_A,const VEC3 &Tri2_B,const VEC3 &Tri2_C)
{
  float dp1, dq1, dr1, dp2, dq2, dr2;
  VEC3 v1, v2;
  VEC3 N1, N2; 
  
  /* Compute distance signs  of Tri1_A, Tri1_B and Tri1_C to the plane of
     triangle(Tri2_A,Tri2_B,Tri2_C) */
  v1 = Tri2_A - Tri2_C;
  v2 = Tri2_B - Tri2_C;
  Cross(&N2,v1,v2);

  v1 = Tri1_A - Tri2_C;
  dp1 = Dot(v1,N2);
  v1 = Tri1_B - Tri2_C;
  dq1 = Dot(v1,N2);
  v1 = Tri1_C - Tri2_C;
  dr1 = Dot(v1,N2);
  
  if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f))  return false; 

  /* Compute distance signs  of Tri2_A, Tri2_B and Tri2_C to the plane of
     triangle(Tri1_A,Tri1_B,Tri1_C) */
  v1=Tri1_B-Tri1_A;
  v2=Tri1_C-Tri1_A;
  Cross(&N1,v1,v2);

  v1=Tri2_A-Tri1_C;
  dp2 = Dot(v1,N1);
  v1=Tri2_B-Tri1_C;
  dq2 = Dot(v1,N1);
  v1=Tri2_C-Tri1_C;
  dr2 = Dot(v1,N1);
  
  if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) return 0;

  /* Permutation in a canonical form of T1's vertices */
  if (dp1 > 0.0f) {
    if (dq1 > 0.0f) TRI_TRI_3D(Tri1_C,Tri1_A,Tri1_B,Tri2_A,Tri2_C,Tri2_B,dp2,dr2,dq2)
    else if (dr1 > 0.0f) TRI_TRI_3D(Tri1_B,Tri1_C,Tri1_A,Tri2_A,Tri2_C,Tri2_B,dp2,dr2,dq2)	
    else TRI_TRI_3D(Tri1_A,Tri1_B,Tri1_C,Tri2_A,Tri2_B,Tri2_C,dp2,dq2,dr2)
  } else if (dp1 < 0.0f) {
    if (dq1 < 0.0f) TRI_TRI_3D(Tri1_C,Tri1_A,Tri1_B,Tri2_A,Tri2_B,Tri2_C,dp2,dq2,dr2)
    else if (dr1 < 0.0f) TRI_TRI_3D(Tri1_B,Tri1_C,Tri1_A,Tri2_A,Tri2_B,Tri2_C,dp2,dq2,dr2)
    else TRI_TRI_3D(Tri1_A,Tri1_B,Tri1_C,Tri2_A,Tri2_C,Tri2_B,dp2,dr2,dq2)
  } else {
    if (dq1 < 0.0f) {
      if (dr1 >= 0.0f) TRI_TRI_3D(Tri1_B,Tri1_C,Tri1_A,Tri2_A,Tri2_C,Tri2_B,dp2,dr2,dq2)
      else TRI_TRI_3D(Tri1_A,Tri1_B,Tri1_C,Tri2_A,Tri2_B,Tri2_C,dp2,dq2,dr2)
    }
    else if (dq1 > 0.0f) {
      if (dr1 > 0.0f) TRI_TRI_3D(Tri1_A,Tri1_B,Tri1_C,Tri2_A,Tri2_C,Tri2_B,dp2,dr2,dq2)
      else TRI_TRI_3D(Tri1_B,Tri1_C,Tri1_A,Tri2_A,Tri2_B,Tri2_C,dp2,dq2,dr2)
    }
    else  {
      if (dr1 > 0.0f) TRI_TRI_3D(Tri1_C,Tri1_A,Tri1_B,Tri2_A,Tri2_B,Tri2_C,dp2,dq2,dr2)
      else if (dr1 < 0.0f) TRI_TRI_3D(Tri1_C,Tri1_A,Tri1_B,Tri2_A,Tri2_C,Tri2_B,dp2,dr2,dq2)
      else return coplanar_tri_tri3d(Tri1_A,Tri1_B,Tri1_C,Tri2_A,Tri2_B,Tri2_C,N1,N2);
    }
  }
}

#undef CHECK_MIN_MAX
#undef TRI_TRI_3D
#undef ORIENT_2D
#undef INTERSECTION_TEST_VERTEX
#undef INTERSECTION_TEST_EDGE

/* Koniec algorytmu TriangleToTriangle */

bool TriangleInFrustum(const VEC3 &t1, const VEC3 &t2, const VEC3 &t3, const FRUSTUM_PLANES &Frustum)
{
	return
		PointInFrustum(t1, Frustum) &&
		PointInFrustum(t2, Frustum) &&
		PointInFrustum(t3, Frustum);
}

bool TriangleToFrustum(
	const VEC3 &t1, const VEC3 &t2, const VEC3 &t3, const PLANE *OptTrianglePlane,
	const FRUSTUM_PLANES &FrustumPlanes, const FRUSTUM_POINTS *OptFrustumPoints)
{
	// Algorytm wymyœli³em sam na podstawie Separating Axis Theorem.

	// Etap 1

	// Ca³y trójk¹t le¿y po ujemnej stronie jednej z p³aszczyzn frustuma - odrzuæ
	for (uint i = 0; i < 6; i++)
	{
		if (
			DotCoord(FrustumPlanes[i], t1) < 0.f &&
			DotCoord(FrustumPlanes[i], t2) < 0.f &&
			DotCoord(FrustumPlanes[i], t3) < 0.f)
		{
			return false;
		}
	}

	// Etap 2

	// Wylicz TrianglePlane
	const PLANE *TrianglePlanePtr = OptTrianglePlane;
	PLANE TrianglePlane;
	if (TrianglePlanePtr == NULL)
	{
		PointsToPlane(&TrianglePlane, t1, t2, t3);
		TrianglePlanePtr = &TrianglePlane;
	}

	// Wylicz FrustumPoints
	const FRUSTUM_POINTS *FrustumPointsPtr = OptFrustumPoints;
	FRUSTUM_POINTS FrustumPoints;
	if (FrustumPointsPtr == NULL)
	{
		FrustumPoints.Set(FrustumPlanes);
		FrustumPointsPtr = &FrustumPoints;
	}

	// Frustum nie przecina p³aszczyzny trójk¹ta - odrzuæ
	if (ClassifyFrustumToPlane(*FrustumPointsPtr, *TrianglePlanePtr) != 0)
		return false;

	return true;
}

bool FrustumToFrustum(
	const FRUSTUM_PLANES &Frustum1_Planes, const FRUSTUM_POINTS &Frustum1_Points,
	const FRUSTUM_PLANES &Frustum2_Planes, const FRUSTUM_POINTS &Frustum2_Points)
{
	// Na podstawie Separating Axis Theorem:
	// Dwa frustumy nie zachodz¹ na siebie wtedy i tylko wtedy, gdy wszystkie punkty
	// jednego z nich le¿¹ po ujemnej stronie jednej z p³aszczyzn drugiego.

	for (uint i = 0; i < 6; i++)
		if (ClassifyFrustumToPlane(Frustum2_Points, Frustum1_Planes[i]) < 0)
			return false;
	for (uint i = 0; i < 6; i++)
		if (ClassifyFrustumToPlane(Frustum1_Points, Frustum2_Planes[i]) < 0)
			return false;

	return true;

	// Inny, ale niekoniecznie szybszy algorytm znajduje siê w ksi¹¿ce ShaderX, tom 3, rozdz. 5.4, str. 437.
}

bool SphereToCapsule(
	const VEC3 &SphereCenter, float SphereRadius,
	const VEC3 &CapsuleBeg, const VEC3 &CapsuleEnd, float CapsuleRadius)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Compute (squared) distance between sphere center and capsule line segment
	float dist2 = PointToSegmentDistanceSq(SphereCenter, CapsuleBeg, CapsuleEnd);

	// If (squared) distance smaller than (squared) sum of radii, they collide
	float radius = SphereRadius + CapsuleRadius;
	return dist2 <= radius * radius;
}

bool CapsuleToCapsule(
	const VEC3 &Capsule1_Beg, const VEC3 &Capsule1_End, float Capsule1_Radius,
	const VEC3 &Capsule2_Beg, const VEC3 &Capsule2_End, float Capsule2_Radius)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Compute (squared) distance between the inner structures of the capsules
	float s, t;
	VEC3 c1, c2;
	float dist2 = ClosestPointSegmentSegment(Capsule1_Beg, Capsule1_End, Capsule2_Beg, Capsule2_End, s, t, c1, c2);
	// if (squared) distance smaller than (squared) sum of radii, they collide
	float radius = Capsule1_Radius + Capsule2_Radius;
	return dist2 <= radius * radius;
}

bool PointInConvexPolygon(const VEC2 &Point, const VEC2 PolygonPoints[], size_t PolygonPointCount)
{
	return PointInConvexPolygon(Point, PolygonPoints, PolygonPointCount, sizeof(VEC2));
}

bool PointInConvexPolygon(const VEC2 &Point, const void *PolygonPoints, size_t PolygonPointCount, ptrdiff_t PolygonPointStride)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson
	// Algorytm ma z³o¿onoœæ O(log n) !!!
	
	assert(PolygonPointCount >= 3);
	
	// Do binary search over polygon vertices to find the fan triangle
	// (v[0], v[low], v[high]) the point Point lies within the near sides of
	size_t low = 0, high = PolygonPointCount;
	do {
		size_t mid = (low + high) / 2;
		if (TriangleIsCW(
			*(VEC2*)((const char*)PolygonPoints+0  *PolygonPointStride),
			*(VEC2*)((const char*)PolygonPoints+mid*PolygonPointStride),
			Point))
			low = mid;
		else
			high = mid;
	} while (low + 1 < high);

	// If point outside last (or first) edge, then it is not inside the n-gon
	if (low == 0 || high == PolygonPointCount) return false;

	// Point is inside the polygon if it is left of
	// the directed edge from v[low] to v[high]
	return TriangleIsCW(
		*(VEC2*)((const char*)PolygonPoints+low *PolygonPointStride),
		*(VEC2*)((const char*)PolygonPoints+high*PolygonPointStride),
		Point);
}

bool PointInPolygon(const VEC2 &Point, const VEC2 PolygonPoints[], size_t PolygonPointCount)
{
	return PointInPolygon(Point, PolygonPoints, PolygonPointCount, sizeof(VEC2));
}

bool PointInPolygon(const VEC2 &Point, const void *PolygonPoints, size_t PolygonPointCount, ptrdiff_t PolygonPointStride)
{
	// Na podstawie:
	// Point-In-Polygon Algorithm — Determining Whether A Point Is Inside A Complex Polygon. Darel Rex Finley.
	// http://www.alienryderflex.com/polygon/

	assert(PolygonPointCount >= 3);

	size_t i, j = 0;
	bool OddNodes = false;
	const char *PolygonPointData = (const char*)PolygonPoints;
	for (i = 0; i < PolygonPointCount; i++)
	{
		j++;
		if (j == PolygonPointCount) j = 0;
		const VEC2 &pi = *(const VEC2*)(PolygonPointData+PolygonPointStride*i);
		const VEC2 &pj = *(const VEC2*)(PolygonPointData+PolygonPointStride*j);

		if (
			pi.y < Point.y && pj.y >= Point.y ||
			pj.y < Point.y && pi.y >= Point.y)
		{
			if (pi.x + (Point.y - pi.y) / (pj.y - pi.y) * (pj.x - pi.x) < Point.x)
				OddNodes = !OddNodes;
		}
	}
	return OddNodes;
}

bool RayToConvexPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 PolygonPoints[], size_t PolygonPointCount,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane)
{
	return RayToConvexPolygon(
		RayOrig, RayDir,
		PolygonPoints, PolygonPointCount, sizeof(VEC3),
		BackfaceCulling, OutT, OutPoint, PolygonPlane);
}

bool RayToConvexPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const void *PolygonPoints, size_t PolygonPointCount, ptrdiff_t PolygonPointStride,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane)
{
	assert(PolygonPointCount >= 3);

	const char *PolygonPointBytes = (const char*)PolygonPoints;

	const PLANE *Plane = PolygonPlane;
	PLANE MyPlane;
	if (Plane == NULL)
	{
		PointsToPlane(&MyPlane,
			*(const VEC3*)(PolygonPointBytes + 0*PolygonPointStride),
			*(const VEC3*)(PolygonPointBytes + 1*PolygonPointStride),
			*(const VEC3*)(PolygonPointBytes + 2*PolygonPointStride));
		Plane = &MyPlane;
	}
	
	float VD;
	if (!RayToPlane(RayOrig, RayDir, *Plane, OutT, &VD))
		return false;
	if (BackfaceCulling && VD > 0.0f)
		return false;

	// Algorytm skopiowany z PointInConvexPolygon
	
	*OutPoint = RayOrig + RayDir * (*OutT);
	float fnx = fabsf(Plane->a);
	float fny = fabsf(Plane->b);
	float fnz = fabsf(Plane->c);
	size_t low = 0, high = PolygonPointCount;

	// Najwiêkszy jest X - rzut na p³aszczyznê YZ
	if (fnx > fny && fnx > fnz)
	{
		do {
			size_t mid = (low + high) / 2;
			if (TriangleIsCW(
				VEC2( ((VEC3*)((const char*)PolygonPoints+0  *PolygonPointStride))->y, ((VEC3*)((const char*)PolygonPoints+0  *PolygonPointStride))->z ),
				VEC2( ((VEC3*)((const char*)PolygonPoints+mid*PolygonPointStride))->y, ((VEC3*)((const char*)PolygonPoints+mid*PolygonPointStride))->z ),
				VEC2(OutPoint->y, OutPoint->z)))
				low = mid;
			else
				high = mid;
		} while (low + 1 < high);

		// If point outside last (or first) edge, then it is not inside the n-gon
		if (low == 0 || high == PolygonPointCount) return false;

		// *OutPoint is inside the polygon if it is left of
		// the directed edge from v[low] to v[high]
		return TriangleIsCW(
			VEC2( ((VEC3*)((const char*)PolygonPoints+low *PolygonPointStride))->y, ((VEC3*)((const char*)PolygonPoints+low *PolygonPointStride))->z ),
			VEC2( ((VEC3*)((const char*)PolygonPoints+high*PolygonPointStride))->y, ((VEC3*)((const char*)PolygonPoints+high*PolygonPointStride))->z ),
			VEC2(OutPoint->y, OutPoint->z));
	}
	// Najwiêkszy jest Z - rzut na p³aszczyznê XY
	else if (fnz > fny)
	{
		do {
			size_t mid = (low + high) / 2;
			if (TriangleIsCW(
				VEC2( ((VEC3*)((const char*)PolygonPoints+0  *PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+0  *PolygonPointStride))->y ),
				VEC2( ((VEC3*)((const char*)PolygonPoints+mid*PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+mid*PolygonPointStride))->y ),
				VEC2(OutPoint->x, OutPoint->y)))
				low = mid;
			else
				high = mid;
		} while (low + 1 < high);

		// If point outside last (or first) edge, then it is not inside the n-gon
		if (low == 0 || high == PolygonPointCount) return false;

		// *OutPoint is inside the polygon if it is left of
		// the directed edge from v[low] to v[high]
		return TriangleIsCW(
			VEC2( ((VEC3*)((const char*)PolygonPoints+low *PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+low *PolygonPointStride))->y ),
			VEC2( ((VEC3*)((const char*)PolygonPoints+high*PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+high*PolygonPointStride))->y ),
			VEC2(OutPoint->x, OutPoint->y));
	}
	// Najwiêkszy jest Y - rzut na p³aszczyznê XZ
	else
	{
		do {
			size_t mid = (low + high) / 2;
			if (TriangleIsCW(
				VEC2( ((VEC3*)((const char*)PolygonPoints+0  *PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+0  *PolygonPointStride))->z ),
				VEC2( ((VEC3*)((const char*)PolygonPoints+mid*PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+mid*PolygonPointStride))->z ),
				VEC2(OutPoint->x, OutPoint->z)))
				low = mid;
			else
				high = mid;
		} while (low + 1 < high);

		// If point outside last (or first) edge, then it is not inside the n-gon
		if (low == 0 || high == PolygonPointCount) return false;

		// *OutPoint is inside the polygon if it is left of
		// the directed edge from v[low] to v[high]
		return TriangleIsCW(
			VEC2( ((VEC3*)((const char*)PolygonPoints+low *PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+low *PolygonPointStride))->z ),
			VEC2( ((VEC3*)((const char*)PolygonPoints+high*PolygonPointStride))->x, ((VEC3*)((const char*)PolygonPoints+high*PolygonPointStride))->z ),
			VEC2(OutPoint->x, OutPoint->z));
	}
}

bool RayToPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const VEC3 PolygonPoints[], size_t PolygonPointCount,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane)
{
	return RayToPolygon(
		RayOrig, RayDir,
		PolygonPoints, PolygonPointCount, sizeof(VEC3),
		BackfaceCulling, OutT, OutPoint, PolygonPlane);
}

/*
Funkcja by³a testowana tylko dla przypadku gdy abs(Plane.Normal.y) najwiêksze czyli p³aszczyzna le¿y w XZ,
ale jeœli siê nigdzie nie pomyli³em przy zamianie wspó³rzêdnych przy kopiowaniu to powinna dzia³aæ wszêdzie.
*/
bool RayToPolygon(
	const VEC3 &RayOrig, const VEC3 &RayDir,
	const void *PolygonPoints, size_t PolygonPointCount, ptrdiff_t PolygonPointStride,
	bool BackfaceCulling, float *OutT, VEC3 *OutPoint, const PLANE *PolygonPlane)
{
	assert(PolygonPointCount >= 3);

	const char *PolygonPointBytes = (const char*)PolygonPoints;

	const PLANE *Plane = PolygonPlane;
	PLANE MyPlane;
	if (Plane == NULL)
	{
		PointsToPlane(&MyPlane,
			*(const VEC3*)(PolygonPointBytes + 0*PolygonPointStride),
			*(const VEC3*)(PolygonPointBytes + 1*PolygonPointStride),
			*(const VEC3*)(PolygonPointBytes + 2*PolygonPointStride));
		Plane = &MyPlane;
	}
	
	float VD;
	if (!RayToPlane(RayOrig, RayDir, *Plane, OutT, &VD))
		return false;
	if (BackfaceCulling && VD > 0.0f)
		return false;

	// Algorytm skopiowany z PointInPolygon
	
	*OutPoint = RayOrig + RayDir * (*OutT);
	float fnx = fabsf(Plane->a);
	float fny = fabsf(Plane->b);
	float fnz = fabsf(Plane->c);
	size_t i, j = 0;
	bool OddNodes = false;

	// Najwiêkszy jest X - rzut na p³aszczyznê YZ
	if (fnx > fny && fnx > fnz)
	{
		for (i = 0; i < PolygonPointCount; i++)
		{
			j++;
			if (j == PolygonPointCount) j = 0;
			const VEC3 &pi = *(const VEC3*)(PolygonPointBytes + i*PolygonPointStride);
			const VEC3 &pj = *(const VEC3*)(PolygonPointBytes + j*PolygonPointStride);
			if (
				pi.y < OutPoint->y && pj.y >= OutPoint->y ||
				pj.y < OutPoint->y && pi.y >= OutPoint->y)
			{
				if (pi.z + (OutPoint->y - pi.y) / (pj.y - pi.y) * (pj.z - pi.z) < OutPoint->z)
				{
					OddNodes = !OddNodes;
				}
			}
		}
		return OddNodes;
	}
	// Najwiêkszy jest Z - rzut na p³aszczyznê XY
	else if (fnz > fny)
	{
		for (i = 0; i < PolygonPointCount; i++)
		{
			j++;
			if (j == PolygonPointCount) j = 0;
			const VEC3 &pi = *(const VEC3*)(PolygonPointBytes + i*PolygonPointStride);
			const VEC3 &pj = *(const VEC3*)(PolygonPointBytes + j*PolygonPointStride);
			if (
				pi.y < OutPoint->y && pj.y >= OutPoint->y ||
				pj.y < OutPoint->y && pi.y >= OutPoint->y)
			{
				if (pi.x + (OutPoint->y - pi.y) / (pj.y - pi.y) * (pj.x - pi.x) < OutPoint->x)
				{
					OddNodes = !OddNodes;
				}
			}
		}
		return OddNodes;
	}
	// Najwiêkszy jest Y - rzut na p³aszczyznê XZ
	else
	{
		for (i = 0; i < PolygonPointCount; i++)
		{
			j++;
			if (j == PolygonPointCount) j = 0;
			const VEC3 &pi = *(const VEC3*)(PolygonPointBytes + i*PolygonPointStride);
			const VEC3 &pj = *(const VEC3*)(PolygonPointBytes + j*PolygonPointStride);
			if (
				pi.z < OutPoint->z && pj.z >= OutPoint->z ||
				pj.z < OutPoint->z && pi.z >= OutPoint->z)
			{
				if (pi.x + (OutPoint->z - pi.z) / (pj.z - pi.z) * (pj.x - pi.x) < OutPoint->x)
				{
					OddNodes = !OddNodes;
				}
			}
		}
		return OddNodes;
	}
}

bool SweptSphereToPlane(const VEC3 &SphereCenter, float SphereRadius, const VEC3 &SphereSweepDir, const PLANE &Plane, float *OutT0, float *OutT1)
{
	// Inna wersja, ale zwracaj¹ca tylko jedno t, jest w ksi¹¿ce:
	// Real-Time Collision Detection, Christer Ericson, rozdz. 5.5.3, str. 221.

	float b_dot_n = DotCoord(Plane, SphereCenter);
	float d_dot_n = DotNormal(Plane, SphereSweepDir);

	if (d_dot_n == 0.f)
	{
		if (fabsf(b_dot_n) <= SphereRadius)
		{
			if (OutT0) *OutT0 = 0.f;
			if (OutT1) *OutT1 = MAXFLOAT;
			return true;
		}
		else
			return false;
	}
	else
	{
		if (OutT0 || OutT1)
		{
			d_dot_n = 1.f / d_dot_n;
			float Tmp0 = ( SphereRadius - b_dot_n) * d_dot_n;
			float Tmp1 = (-SphereRadius - b_dot_n) * d_dot_n;
			if (OutT0) *OutT0 = std::min(Tmp0, Tmp1);
			if (OutT1) *OutT1 = std::max(Tmp0, Tmp1);
		}
		return true;
	}
}

bool SweptSphereToFrustum(const VEC3 &SphereCenter, float SphereRadius, const VEC3 &SphereSweepDir, const FRUSTUM_PLANES &Frustum)
{
	// Funkcja na podstawie: PracticalPSM - przyk³adowy program autorstwa nVidia Coroporation.
	// Ale znacznie ulepszona, bo tam by³a napisana beznadziejnie!

    // algorithm -- get all 12 intersection points of the swept sphere with the view frustum
    // for all points >0, displace sphere along the sweep driection.  if the displaced sphere
    // is inside the frustum, return TRUE.  else, return FALSE

	float a, b;
	VEC3 DisplacedSphereCenter;
	float DisplacedSphereRadius = SphereRadius; // * 1.1f; // <- tak by³o w oryginale, ale WTF???

	for (size_t i = 0; i < 6; i++)
	{
		if (SweptSphereToPlane(SphereCenter, SphereRadius, SphereSweepDir, Frustum[i], &a, &b))
		{
			if (a >= 0.f)
			{
				DisplacedSphereCenter = SphereCenter + SphereSweepDir * a;
				if (SphereToFrustum_Fast(DisplacedSphereCenter, DisplacedSphereRadius, Frustum))
					return true;
			}
			if (b >= 0.f)
			{
				DisplacedSphereCenter = SphereCenter + SphereSweepDir * b;
				if (SphereToFrustum_Fast(DisplacedSphereCenter, DisplacedSphereRadius, Frustum))
					return true;
			}
		}
	}

	return false;
}

bool SweptSphereToSphere(
	const VEC3 &SphereCenter1, float SphereRadius1,
	const VEC3 &SphereCenter2, float SphereRadius2,
	const VEC3 &SphereSweepDir1,
	float *OutT1, float *OutT2)
{
	// Inny algorytm, ale zwracaj¹cy tylko jedno t, jest w ksi¹¿ce:
	// Real-Time Collision Detection, Christer Ericson, rozdz. 5.5.5, str. 224.

	// Algorytm na podstawie artyku³u "Simple Intersection Tests For Games", Miguel Gomez.
	// Gamasutra, October 18, 1999
	// http://www.gamasutra.com/features/19991018/Gomez_1.htm
	// Dzia³a w oparciu o jakieœtam równanie kwadratowe.

	// vector from A0 to B0
	VEC3 AB = SphereCenter1 - SphereCenter2;

	// Relative velocity
	const VEC3 &vab = SphereSweepDir1;

	float rab = SphereRadius1 + SphereRadius2;

	// u*u coefficient
	float a = Dot(vab, vab);

	// u coefficient
	float b = 2.0f * Dot(vab, AB);

	// constant term
	float c = Dot(AB, AB) - rab * rab;

	// (To by³o bez sensu, a bez tego te¿ dzia³a i to bardziej prawid³owo.)
	//// check if they're currently overlapping
	//if (Dot(AB, AB) <= rab * rab)
	//{
	//	*OutT1 = *OutT2 = 0.f;
	//	return true;
	//}

	// check if they hit each other
	// during the frame
	if (QuadraticEquation(a, b, c, OutT1, OutT2) > 0)
	{
		if (*OutT1 > *OutT2)
			std::swap(*OutT1, *OutT2);
		return true;
	}

	return false;
}

bool SweptBoxToBox(const BOX &Box1, const BOX &Box2, const VEC3 &SweepDirBox1, float *OutT1, float *OutT2)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	// Exit early if ‘Box1’ and ‘Box2’ initially overlapping
	if (BoxToBox(Box1, Box2)) {
		*OutT1 = *OutT2 = 0.0f;
		return true;
	}

	// Use relative velocity; effectively treating 'Box1' as stationary
	//Vector SweepDirBox1 = vb - va;

	// Initialize times of first and last contact
	*OutT1 = 0.0f;
	*OutT2 = FLT_MAX;

	// For each axis, determine times of first and last contact, if any
	for (uint i = 0; i < 3; i++) {
		if (SweepDirBox1[i] < 0.0f) {
			if (Box1.Max[i] < Box2.Min[i]) return false; // Nonintersecting and moving apart
			if (Box2.Max[i] < Box1.Min[i]) *OutT1 = std::max((Box2.Max[i] - Box1.Min[i]) / SweepDirBox1[i], *OutT1);
			if (Box1.Max[i] > Box2.Min[i]) *OutT2  = std::min((Box2.Min[i] - Box1.Max[i]) / SweepDirBox1[i], *OutT2);
		}
		if (SweepDirBox1[i] > 0.0f) {
			if (Box1.Min[i] > Box2.Max[i]) return false; // Nonintersecting and moving apart
			if (Box1.Max[i] < Box2.Min[i]) *OutT1 = std::max((Box2.Min[i] - Box1.Max[i]) / SweepDirBox1[i], *OutT1);
			if (Box2.Max[i] > Box1.Min[i]) *OutT2 = std::min((Box2.Max[i] - Box1.Min[i]) / SweepDirBox1[i], *OutT2);
		}

		// No overlap possible if time of first contact occurs after time of last contact
		if (*OutT1 > *OutT2) return false;
	}

	return true;

/*	// Zmniejsz Box1 do pojedynczego poruszaj¹cego sie punktu (promienia),
	// a Box2 rozszerz o rozmiar Box1 (suma minkowskiego)
	// W ten sposób problem sprowadza siê do kolizji Ray-Box.
	// Algorytm wymyœli³em sam.

	VEC3 RayOrig; Box1.CalcCenter(&RayOrig);
	VEC3 BoxExtend; Box1.GetSize(&BoxExtend);
	BoxExtend *= 0.5f;
	BOX Box;
	Box.Min.x = Box2.Min.x - BoxExtend.x;
	Box.Min.y = Box2.Min.y - BoxExtend.y;
	Box.Min.z = Box2.Min.z - BoxExtend.z;
	Box.Max.x = Box2.Max.x + BoxExtend.x;
	Box.Max.y = Box2.Max.y + BoxExtend.y;
	Box.Max.z = Box2.Max.z + BoxExtend.z;

	return RayToBox(OutT, RayOrig, SweepDirBox1, Box);*/
}

// Function have not been tested yet!
int RayToCone(const VEC3 &rayOrig, const VEC3 & rayDir, const VEC3 &coneVertex, const VEC3 &coneDirVec, float coneHalfAngle, VEC3 intersections[2])
{
	const float	sigma=cosf(coneHalfAngle);
	const float sigma2=sigma*sigma;

	const VEC3 deltaVec=rayOrig-coneVertex; // DELTA

	const float dotAD=Dot(coneDirVec, rayDir);
	const float dotADELTA=Dot(coneDirVec, deltaVec);
	
	const float c2=dotAD*dotAD - sigma2;
	const float c1=dotAD*dotADELTA - sigma2*Dot(rayDir, deltaVec);
	const float c0=dotADELTA*dotADELTA - sigma2*Dot(deltaVec, deltaVec);

	if(fabsf(c2)>FLT_EPSILON)
	{
		float delta=c1*c1 - c0*c2;

		if(delta<0)
		{
			// nie ma przeciec
			return 0;
		}
		else if(delta>0) 
		{
			// mozliwe 2 przeciecia
			float sqrt_delta=sqrtf(delta);
			float inv_c2=1.0f/c2;
			float t1=(-c1+sqrt_delta)*inv_c2;
			float t2=(-c1-sqrt_delta)*inv_c2;
			int numInter=0;
			VEC3 vecX=rayOrig + rayDir*t1;
			
			if(Dot(coneDirVec, vecX-coneVertex)>=0)
			{
				// jest przeciecie w dobrej czesci podwojnego stozka
				intersections[numInter]=vecX;
				numInter++;
			}

			vecX=rayOrig + rayDir*t2;

			if(Dot(coneDirVec, vecX-coneVertex)>=0)
			{
				// jest przeciecie w dobrej czesci podwojnego stozka
				intersections[numInter]=vecX;
				numInter++;
			}

			return numInter;
		}
		else
		{
			// mozliwe jedno przeciecie (styczna do stozka, lub przechodzi przez coneVertex)
			float t=-c1/c2;
			VEC3 vecX=rayOrig + rayDir*t;

			if(Dot(coneDirVec, vecX-coneVertex)>=0)
			{
				// jest przeciecie w dobrej czesci podwojnego stozka
				intersections[0]=vecX;
				return 1;
			}
		}
	}
	else if(fabsf(c1)>FLT_EPSILON)
	{
		// c2==0 i c1!=0
		float t=-0.5f*c0/c1;
		VEC3 vecX=rayOrig + rayDir*t;

		if(Dot(coneDirVec, vecX-coneVertex)>=0)
		{
			// jest przeciecie w dobrej czesci podwojnego stozka
			intersections[0]=vecX;
			return 1;
		}
	}
	else if(fabsf(c0)>FLT_EPSILON)
	{
		// c2==0 i c1==0 i c0!=0
		return 0;
	}
	else
	{
		// c2==0 i c1==0 i c0==0
		// linia zawiera sie w powierzchni stozka, ale zwracamy 0
		return 0;
	}

	return 0;
}

// Function have not been tested yet!
bool PointInCone(const VEC3 &pt, const VEC3 &coneVertex, const VEC3 &coneDirVec, float coneHalfAngle)
{
	assert(coneHalfAngle<PI_2);

	float sigma=cosf(coneHalfAngle);
	VEC3 v = pt-coneVertex;
	Normalize(&v);
	return Dot(coneDirVec, v) > sigma;
}

// Function have not been tested yet!
bool SegmentIntersectsOrIsContainedInCone(const VEC3 & segmentPoint1, const VEC3 & segmentPoint2, const VEC3 & coneVertex, const VEC3 & coneDirVec, const float coneHalfAngle, VEC3 intersections[2], int &numIntersectionsFound)
{
	assert(coneHalfAngle<PI_2);

	// na poczatek ustawiamy 0 znalezionych przeciec
	numIntersectionsFound=0;
	VEC3 v;

	const float	sigma=cosf(coneHalfAngle);

	v = segmentPoint1-coneVertex;
	Normalize(&v);
	bool res1=Dot(coneDirVec, v)>sigma;
	
	v = segmentPoint2-coneVertex;
	Normalize(&v);
	bool res2=Dot(coneDirVec, v)>sigma;

	if(res1 && res2)
	{
		// odcinek zawiera sie w stozku
		return true;
	}

	VEC3 linePoint=segmentPoint1;
	VEC3 vecP1toP2=segmentPoint2-segmentPoint1;
	assert(LengthSq(vecP1toP2)>FLT_EPSILON);
	VEC3 lineDirVec=vecP1toP2;
	Normalize(&lineDirVec);
	
	int numInter=RayToCone(linePoint, lineDirVec, coneVertex, coneDirVec, coneHalfAngle, intersections);
	
	// czy znalazl przeciecia z linia?
	if(numInter<1)
		return false;

	VEC3 minPt(std::min(segmentPoint1.x, segmentPoint2.x), std::min(segmentPoint1.y, segmentPoint2.y), std::min(segmentPoint1.z, segmentPoint2.z));
	VEC3 maxPt(std::max(segmentPoint1.x, segmentPoint2.x), std::max(segmentPoint1.y, segmentPoint2.y), std::max(segmentPoint1.z, segmentPoint2.z));

	bool change=false;
	int correctedNumInter=numInter;

	// jesli punkt przeciecia nie nalezy do odcinka
	if(!PointInBox(intersections[0], BOX(minPt, maxPt)))
	{
		correctedNumInter--;
		change=true;
	}

	// jesli byly 2 przeciecia
	if(numInter>1)
	{
		// jesli punkt przeciecia nie nalezy do odcinka
		if(!PointInBox(intersections[1], BOX(minPt, maxPt)))
			correctedNumInter--;			

		// jesli jedno przeciecie i trzeba zamienic
		if(correctedNumInter==1 && change)
			intersections[0]=intersections[1];
	}

	numIntersectionsFound=correctedNumInter;
	return numIntersectionsFound>0;
}

// Function have not been tested yet!
bool SegmentIntersectsOrIsContainedInCone(const VEC3 & segmentPoint1, const VEC3 & segmentPoint2, const VEC3 & coneVertex, const VEC3 & coneDirVec, const float coneHalfAngle)
{
	assert(coneHalfAngle<PI_2);

	VEC3 intersections[2];
	const float	sigma=cosf(coneHalfAngle);

	VEC3 v;

	v = segmentPoint1-coneVertex;
	Normalize(&v);
	bool res1=Dot(coneDirVec, v)>sigma;

	v = segmentPoint2-coneVertex;
	Normalize(&v);
	bool res2=Dot(coneDirVec, v)>sigma;

	if(res1 || res2)
	{
		// odcinek zawiera sie w stozku lub go przecina
		return true;
	}

	const VEC3 linePoint=segmentPoint1;
	const VEC3 vecP1toP2=segmentPoint2-segmentPoint1;
	assert(LengthSq(vecP1toP2)>FLT_EPSILON);
	VEC3 lineDirVec=vecP1toP2;
	Normalize(&lineDirVec);
	
	int numInter=RayToCone(linePoint, lineDirVec, coneVertex, coneDirVec, coneHalfAngle, intersections);
	
	// czy znalazl przeciecia z linia?
	if(numInter<1)
		return false;

	VEC3 minPt(std::min(segmentPoint1.x, segmentPoint2.x), std::min(segmentPoint1.y, segmentPoint2.y), std::min(segmentPoint1.z, segmentPoint2.z));
	VEC3 maxPt(std::max(segmentPoint1.x, segmentPoint2.x), std::max(segmentPoint1.y, segmentPoint2.y), std::max(segmentPoint1.z, segmentPoint2.z));		

	int correctedNumInter=numInter;

	// jesli punkt przeciecia nie nalezy do odcinka
	if(!PointInBox(intersections[0], BOX(minPt, maxPt)))
		correctedNumInter--;

	// jesli byly 2 przeciecia
	if(numInter>1)
	{
		// jesli punkt przeciecia nie nalezy do odcinka
		if(!PointInBox(intersections[1], BOX(minPt, maxPt)))
			correctedNumInter--;			
	}				
	
	return correctedNumInter>0;
}

size_t FurthestPoint(const VEC2 &p, const VEC2 Points[], size_t PointCount, float *OutDistance)
{
	return FurthestPoint(p, Points, PointCount, sizeof(VEC2), OutDistance);
}

size_t FurthestPoint(const VEC2 &p, const void *Data, size_t PointCount, ptrdiff_t Stride, float *OutDistance)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)Data;
	size_t FoundIndex = 0;
	float NewDistSq, FoundDistSq = DistanceSq(p, *(const VEC2*)Bytes);
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewDistSq = DistanceSq(p, *(const VEC2*)Bytes);
		if (NewDistSq > FoundDistSq)
		{
			FoundIndex = i;
			FoundDistSq = NewDistSq;
		}
		Bytes += Stride;
	}

	if (OutDistance) *OutDistance = sqrtf(FoundDistSq);
	return FoundIndex;
}

size_t FurthestPoint(const VEC3 &p, const VEC3 Points[], size_t PointCount, float *OutDistance)
{
	return FurthestPoint(p, Points, PointCount, sizeof(VEC3), OutDistance);
}

size_t FurthestPoint(const VEC3 &p, const void *Data, size_t PointCount, ptrdiff_t Stride, float *OutDistance)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)Data;
	size_t FoundIndex = 0;
	float NewDistSq, FoundDistSq = DistanceSq(p, *(const VEC3*)Bytes);
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewDistSq = DistanceSq(p, *(const VEC3*)Bytes);
		if (NewDistSq > FoundDistSq)
		{
			FoundIndex = i;
			FoundDistSq = NewDistSq;
		}
		Bytes += Stride;
	}

	if (OutDistance) *OutDistance = sqrtf(FoundDistSq);
	return FoundIndex;
}

size_t ClosestPoint(const VEC2 &p, const VEC2 Points[], size_t PointCount, float *OutDistance)
{
	return ClosestPoint(p, Points, PointCount, sizeof(VEC2), OutDistance);
}

size_t ClosestPoint(const VEC2 &p, const void *Data, size_t PointCount, ptrdiff_t Stride, float *OutDistance)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)Data;
	size_t FoundIndex = 0;
	float NewDistSq, FoundDistSq = DistanceSq(p, *(const VEC2*)Bytes);
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewDistSq = DistanceSq(p, *(const VEC2*)Bytes);
		if (NewDistSq < FoundDistSq)
		{
			FoundIndex = i;
			FoundDistSq = NewDistSq;
		}
		Bytes += Stride;
	}

	if (OutDistance) *OutDistance = sqrtf(FoundDistSq);
	return FoundIndex;
}

size_t ClosestPoint(const VEC3 &p, const VEC3 Points[], size_t PointCount, float *OutDistance)
{
	return ClosestPoint(p, Points, PointCount, sizeof(VEC3), OutDistance);
}

size_t ClosestPoint(const VEC3 &p, const void *Data, size_t PointCount, ptrdiff_t Stride, float *OutDistance)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)Data;
	size_t FoundIndex = 0;
	float NewDistSq, FoundDistSq = DistanceSq(p, *(const VEC3*)Bytes);
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewDistSq = DistanceSq(p, *(const VEC3*)Bytes);
		if (NewDistSq < FoundDistSq)
		{
			FoundIndex = i;
			FoundDistSq = NewDistSq;
		}
		Bytes += Stride;
	}

	if (OutDistance) *OutDistance = sqrtf(FoundDistSq);
	return FoundIndex;
}

size_t FurthestPointForDir(const VEC2 &Dir, const VEC2 Points[], size_t PointCount)
{
	return FurthestPointForDir(Dir, Points, PointCount, sizeof(VEC2));
}

size_t FurthestPointForDir(const VEC2 &Dir, const void *Data, size_t PointCount, ptrdiff_t Stride)
{
	assert(PointCount > 0);
	
	const char *Bytes = (const char*)Data;
	size_t FoundIndex = 0;
	float NewDot, FoundDot = Dot(Dir, *(const VEC2*)Bytes);
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewDot = Dot(Dir, *(const VEC2*)Bytes);
		if (NewDot > FoundDot)
		{
			FoundDot = NewDot;
			FoundIndex = i;
		}
		Bytes += Stride;
	}

	return FoundIndex;
}

size_t FurthestPointForDir(const VEC3 &Dir, const VEC3 Points[], size_t PointCount)
{
	return FurthestPointForDir(Dir, Points, PointCount, sizeof(VEC3));
}

size_t FurthestPointForDir(const VEC3 &Dir, const void *Data, size_t PointCount, ptrdiff_t Stride)
{
	assert(PointCount > 0);
	
	const char *Bytes = (const char*)Data;
	size_t FoundIndex = 0;
	float NewDot, FoundDot = Dot(Dir, *(const VEC3*)Bytes);
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewDot = Dot(Dir, *(const VEC3*)Bytes);
		if (NewDot > FoundDot)
		{
			FoundDot = NewDot;
			FoundIndex = i;
		}
		Bytes += Stride;
	}

	return FoundIndex;
}

void RectBoundingPoints(RECTF *OutRect, const VEC2 Points[], size_t PointCount)
{
	RectBoundingPoints(OutRect, Points, PointCount, sizeof(VEC2));
}

void RectBoundingPoints(RECTF *OutRect, const void *Data, size_t PointCount, ptrdiff_t Stride)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)Data;

	OutRect->Min = OutRect->Max = *(const VEC2*)Bytes;
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		OutRect->Min.x = std::min(OutRect->Min.x, ((const VEC2*)Bytes)->x);
		OutRect->Max.x = std::max(OutRect->Max.x, ((const VEC2*)Bytes)->x);
		OutRect->Min.y = std::min(OutRect->Min.y, ((const VEC2*)Bytes)->y);
		OutRect->Max.y = std::max(OutRect->Max.y, ((const VEC2*)Bytes)->y);
		Bytes += Stride;
	}
}

void BoxBoundingPoints(BOX *box, const VEC3 points[], size_t PointCount)
{
	BoxBoundingPoints(box, points, PointCount, sizeof(VEC3));
}

void BoxBoundingPoints(BOX *box, const void *Data, size_t PointCount, ptrdiff_t Stride)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)Data;

	box->Min = box->Max = *(const VEC3*)Bytes;
	Bytes += Stride;

	for (size_t i = 1; i < PointCount; i++)
	{
		Min(&box->Min, box->Min, *(const VEC3*)Bytes);
		Max(&box->Max, box->Max, *(const VEC3*)Bytes);
		Bytes += Stride;
	}
}

void SphereBoundingSpheres(VEC3 *OutCenter, float *OutRadius, const VEC3 &Center1, float Radius1, const VEC3 &Center2, float Radius2)
{
	// Na podstawie ksi¹¿ki: Real-Time Collision Detection, Christer Ericson

	VEC3 d = Center2 - Center1;
	float dist2 = LengthSq(d);

	if (sqr(Radius2 - Radius1) >= dist2)
	{
		if (Radius2 >= Radius1)
			*OutCenter = Center2, *OutRadius = Radius2;
		else
			*OutCenter = Center1, *OutRadius = Radius1;
	}
	else
	{
		float dist = sqrtf(dist2);
		*OutRadius = (dist + Radius1 + Radius2) * 0.5f;
		*OutCenter = Center1;
		if (dist > FLOAT_EPSILON)
			*OutCenter += ((*OutRadius - Radius1) / dist) * d;
	}
}

void CircleBoundingPoints(VEC2 *OutSphereCenter, float *OutSphereRadius, const VEC2 Points[], size_t PointCount)
{
	CircleBoundingPoints(OutSphereCenter, OutSphereRadius, Points, PointCount, sizeof(VEC2));
}

void CircleBoundingPoints(VEC2 *OutSphereCenter, float *OutSphereRadius, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)PointData;
	*OutSphereRadius = 0.f;
	*OutSphereCenter = *(const VEC2*)Bytes;

	VEC2 CVec;
	float d, r, scale;

	for (size_t i = 1; i < PointCount; i++)
	{
		const VEC2 &Tmp = *(const VEC2*)Bytes;
		CVec = Tmp - *OutSphereCenter;
		d = Dot(CVec, CVec);
		if (d > (*OutSphereRadius)*(*OutSphereRadius))
		{
			d = sqrtf(d);
			r = 0.5f * (d + *OutSphereRadius);
			scale = (r - *OutSphereRadius) / d;
			*OutSphereCenter += scale * CVec;
			*OutSphereRadius = r;
		}
		Bytes += PointStride;
	}
}

void SphereBoundingPoints(VEC3 *OutSphereCenter, float *OutSphereRadius, const VEC3 Points[], size_t PointCount)
{
	SphereBoundingPoints(OutSphereCenter, OutSphereRadius, Points, PointCount, sizeof(VEC3));
}

void SphereBoundingPoints(VEC3 *OutSphereCenter, float *OutSphereRadius, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)PointData;
	*OutSphereRadius = 0.f;
	*OutSphereCenter = *(const VEC3*)Bytes;

	VEC3 CVec;
	float d, r, scale;

	for (size_t i = 1; i < PointCount; i++)
	{
		const VEC3 &Tmp = *(const VEC3*)Bytes;
		CVec = Tmp - *OutSphereCenter;
		d = Dot(CVec, CVec);
		if (d > (*OutSphereRadius)*(*OutSphereRadius))
		{
			d = sqrtf(d);
			r = 0.5f * (d + *OutSphereRadius);
			scale = (r - *OutSphereRadius) / d;
			*OutSphereCenter += scale * CVec;
			*OutSphereRadius = r;
		}
		Bytes += PointStride;
	}
}

void OriginCircleBoundingPoints(float *OutRadius, const VEC2 Points[], size_t PointCount)
{
	OriginCircleBoundingPoints(OutRadius, Points, PointCount, sizeof(VEC2));
}

void OriginCircleBoundingPoints(float *OutRadius, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)PointData;

	*OutRadius = LengthSq(*(const VEC2*)Bytes);
	Bytes += PointStride;
	float NewLengthSq;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewLengthSq = LengthSq(*(const VEC2*)Bytes);
		if (NewLengthSq > *OutRadius)
			*OutRadius = NewLengthSq;
		Bytes += PointStride;
	}

	*OutRadius = sqrtf(*OutRadius);
}

void OriginSphereBoundingPoints(float *OutRadius, const VEC3 Points[], size_t PointCount)
{
	OriginSphereBoundingPoints(OutRadius, Points, PointCount, sizeof(VEC3));
}

void OriginSphereBoundingPoints(float *OutRadius, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	assert(PointCount > 0);

	const char *Bytes = (const char*)PointData;

	*OutRadius = LengthSq(*(const VEC3*)Bytes);
	Bytes += PointStride;
	float NewLengthSq;

	for (size_t i = 1; i < PointCount; i++)
	{
		NewLengthSq = LengthSq(*(const VEC3*)Bytes);
		if (NewLengthSq > *OutRadius)
			*OutRadius = NewLengthSq;
		Bytes += PointStride;
	}

	*OutRadius = sqrtf(*OutRadius);
}

void CalcCentroid(VEC2 *OutCentroid, const VEC2 Points[], size_t PointCount)
{
	CalcCentroid(OutCentroid, Points, PointCount, sizeof(VEC2));
}

void CalcCentroid(VEC2 *OutCentroid, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	assert(PointCount > 0);
	const char *PointBytes = (const char*)PointData;
	*OutCentroid = *(const VEC2*)PointBytes;
	PointBytes += PointStride;
	
	for (size_t i = 1; i < PointCount; i++)
	{
		*OutCentroid += *(const VEC2*)PointBytes;
		PointBytes += PointStride;
	}

	*OutCentroid /= (float)PointCount;
}

void CalcCentroid(VEC3 *OutCentroid, const VEC3 Points[], size_t PointCount)
{
	CalcCentroid(OutCentroid, Points, PointCount, sizeof(VEC3));
}

void CalcCentroid(VEC3 *OutCentroid, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	assert(PointCount > 0);
	const char *PointBytes = (const char*)PointData;
	*OutCentroid = *(const VEC3*)PointBytes;
	PointBytes += PointStride;
	
	for (size_t i = 1; i < PointCount; i++)
	{
		*OutCentroid += *(const VEC3*)PointBytes;
		PointBytes += PointStride;
	}

	*OutCentroid /= (float)PointCount;
}

void CalcCovarianceMatrix(MATRIX33 *OutCov, const VEC3 Points[], size_t PointCount)
{
	CalcCovarianceMatrix(OutCov, Points, PointCount, sizeof(VEC3));
}

void CalcCovarianceMatrix(MATRIX33 *OutCov, const void *PointData, size_t PointCount, ptrdiff_t PointStride)
{
	/* Na podstawie ksi¹¿ki:
	Real-Time Collision Detection, Christer Ericson */

	assert(PointCount > 0);
	const char *PointBytes = (const char*)PointData;

	float oon = 1.0f / (float)PointCount;
	float e00, e11, e22, e01, e02, e12;

	// Compute the center of mass (centroid) of the points
	VEC3 c = *(const VEC3*)PointBytes;
	PointBytes += PointStride;
	for (size_t i = 1; i < PointCount; i++)
	{
		c += *(const VEC3*)PointBytes;
		PointBytes += PointStride;
	}
	c *= oon;

	// Compute covariance elements
	e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
	PointBytes = (const char*)PointData;
	VEC3 p;
	for (size_t i = 0; i < PointCount; i++)
	{
		// Translate points so center of mass is at origin
		p = *(const VEC3*)PointBytes - c;
		// Compute covariance of translated points
		e00 += p.x * p.x;
		e11 += p.y * p.y;
		e22 += p.z * p.z;
		e01 += p.x * p.y;
		e02 += p.x * p.z;
		e12 += p.y * p.z;
	
		PointBytes += PointStride;
	}
	
	// Fill the covariance matrix elements
	OutCov->_11 = e00 * oon;
	OutCov->_22 = e11 * oon;
	OutCov->_33 = e22 * oon;
	OutCov->_12 = OutCov->_21 = e01 * oon;
	OutCov->_13 = OutCov->_31 = e02 * oon;
	OutCov->_23 = OutCov->_32 = e12 * oon;
}

/* Funkcja pomocnicza, wewnêtrzna
Na podstawie ksi¹¿ki:
Real-Time Collision Detection, Christer Ericson */
void SymSchur2(const MATRIX33 &a, int p, int q, float &c, float &s)
{
	if (fabsf(a(p, q)) > 0.0001f)
	{
		float r = (a(q, q) - a(p, p)) / (2.0f * a(p, q));
		float t;
		if (r >= 0.0f)
			t = 1.0f / (r + sqrtf(1.0f + r*r));
		else
			t = -1.0f / (-r + sqrtf(1.0f + r*r));
		c = 1.0f / sqrtf(1.0f + t*t);
		s = t*c;
	}
	else
		c = 1.0f, s = 0.0f;
}

/*
Na podstawie ksi¹¿ki:
Real-Time Collision Detection, Christer Ericson

Computes the eigenvectors and eigenvalues of the symmetric matrix A using
the classic Jacobi method of iteratively updating A as A = J^T * A * J,
where J = J(p, q, theta) is the Jacobi rotation matrix.
See Golub, Van Load, Matrix Computations, 3rd ed, p428
*/
void CalcEigenvaluesAndEigenvectors(MATRIX33 *InCovOutEigenvalues, MATRIX33 *OutEigenvectors)
{
	uint i, j, n, p, q;
	float prevoff = 0.0f, c, s;
	MATRIX33 J, J_transpose, b, t;

	// Initialize (*OutEigenvectors) to identify matrix
	Identity(OutEigenvectors);

	// Repeat for some maximum number of iterations
	const int MAX_ITERATIONS = 50;
	for (n = 0; n < MAX_ITERATIONS; n++)
	{
		// Find the largest off-diagonal absolute element (*InCovOutEigenvalues)[p][q]
		p = 0, q = 1;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				if (i == j) continue;
				if (fabsf((*InCovOutEigenvalues)(i, j)) > fabsf((*InCovOutEigenvalues)(p, q)))
					p = i, q = j;
			}
		}

		// Compute the Jacobi rotation matrix J(p, q, theta)
		// (This code can be optimized for the three different cases of rotation)
		SymSchur2((*InCovOutEigenvalues), p, q, c, s);
		Identity(&J);
		J(p, p) =  c; J(p, q) = s;
		J(q, p) = -s; J(q, q) = c;

		// Cumulate rotations into what will contain the eigenvectors
		(*OutEigenvectors) *= J;

		// Make '(*InCovOutEigenvalues)' more diagonal, until just eigenvalues remain on diagonal
		Transpose(&J_transpose, J);
		*InCovOutEigenvalues = J_transpose * (*InCovOutEigenvalues) * J;

		// Compute 'norm' of off-diagonal elements
		float off = 0.0f;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				if (i == j) continue;
				off += (*InCovOutEigenvalues)(i, j) * (*InCovOutEigenvalues)(i, j);
			}
		}
		// off = sqrtf(off); not needed for norm comparison

		// Stop when norm no longer decreasing
		if (n > 2 && off >= prevoff)
			return;

		prevoff = off;
	}
}

void BoxBoundingSphere(VEC3 *SphereCenter, float *SphereRadius, const BOX &Box)
{
	Box.GetCenter(SphereCenter);
	float Diagonal = Length(Box.Max - Box.Min); // Przek¹tna
	*SphereRadius = Diagonal / 2.f;
}

void SphereBoundingBox(BOX *Out, const VEC3 &SphereCenter, float SphereRadius)
{
	Out->Min = SphereCenter;
	Out->Min.x -= SphereRadius;
	Out->Min.y -= SphereRadius;
	Out->Min.z -= SphereRadius;
	Out->Max = SphereCenter;
	Out->Max.x += SphereRadius;
	Out->Max.y += SphereRadius;
	Out->Max.z += SphereRadius;
}

void BoxBoundingSpheres(BOX *OutBox, const VEC3 SpheresCenter[], const float SpheresRadius[], size_t SphereCount)
{
	assert(SphereCount > 0);

	OutBox->Min = OutBox->Max = SpheresCenter[0];
	float r = SpheresRadius[0];
	OutBox->Min.x -= r; OutBox->Min.y -= r; OutBox->Min.z -= r;
	OutBox->Max.x += r; OutBox->Max.y += r; OutBox->Max.z += r;

	for (size_t i = 1; i < SphereCount; i++)
	{
		const VEC3 &c = SpheresCenter[i];
		r = SpheresRadius[i];

		OutBox->Min.x = std::min(OutBox->Min.x, c.x - r);
		OutBox->Min.y = std::min(OutBox->Min.y, c.y - r);
		OutBox->Min.z = std::min(OutBox->Min.z, c.z - r);
		OutBox->Max.x = std::max(OutBox->Max.x, c.x + r);
		OutBox->Max.y = std::max(OutBox->Max.y, c.y + r);
		OutBox->Max.z = std::max(OutBox->Max.z, c.z + r);
	}
}

void RandomPointInUnitSphere(VEC3 *Out, RandomGenerator &Rand)
{
	// Dla szeœcianu (-1,-1,-1)..(1,1,1) objêtoœæ to 8, dla sfery o promieniu 1 objêtoœæ to 4/3*PI ~= 4.18.
	// Tak wiêc prawdopodobieñstwo wylosowania liczby spoza zakresu wynosi 0.4775.
	// Tak wiêc oczekiwana liczba iteracji tutaj wynosi nieco poni¿ej 2.
	do
	{
		Out->x = Rand.RandFloat(-1.0f, 1.0f);
		Out->y = Rand.RandFloat(-1.0f, 1.0f);
		Out->z = Rand.RandFloat(-1.0f, 1.0f);
	}
	while (Length(*Out) > 1.0f);
}

void RandomPointInUnitCircle(VEC2 *Out, RandomGenerator &Rand)
{
	// Dla kwadatu (-1,-1)..(1,1) powierzchnia to 4, dla ko³a o promieniu 1 powierzchnia to PI ~= 3.14.
	// Tak wiêc prawdopodobieñstwo wylosowania liczby spoza zakresu wynosi 0.215.
	// Tak wiêc oczekiwana liczba iteracji tutaj wynosi poni¿ej 1.5.
	do
	{
		Out->x = Rand.RandFloat(-1.0f, 1.0f);
		Out->y = Rand.RandFloat(-1.0f, 1.0f);
	}
	while (Length(*Out) > 1.0f);
}

void RandomPointInCapsule(VEC3 *Out, const VEC3 &p1, const VEC3 &p2, float R, RandomGenerator &Rand)
{
	VEC3 p_vec = p2 - p1; // wektor p1-p2
	float Help = PI * R * R;
	float SphereVolume = 4.0f/3.0f * R * Help;
	float CylinderVolume = Help * Length(p_vec);
	// Prawdopodobieñstwo wylosowania z kuli jest proporcjonalne do stosunku objêtoœci kuli do objêtoœci walca.
	float SphereProbability = SphereVolume / (SphereVolume + CylinderVolume);
	float RandomNumber = Rand.RandFloat();
	// Punkt z jednej z pó³kul
	if (RandomNumber <= SphereProbability)
	{
		// Losowy wektor
		VEC3 RandomVec; RandomPointInUnitSphere(&RandomVec, Rand);
		// Pó³kula przy p2
		if (Dot(RandomVec, p_vec) > 0.0f)
			*Out = p2 + RandomVec * R;
		// Pó³kula przy p1
		else
			*Out = p1 + RandomVec * R;
	}
	// Punkt z walca
	else
	{
		Lerp(Out, p1, p2, Rand.RandFloat());

		VEC3 dir1, dir2;
		PerpedicularVectors(&dir1, &dir2, p_vec);
		Normalize(&dir1);
		Normalize(&dir2);
		dir1 *= R;
		dir2 *= R;
		VEC2 RandomVec; RandomPointInUnitCircle(&RandomVec, Rand);
		*Out += dir1 * RandomVec.x + dir2 * RandomVec.y;
	}
}

void RandomPointInUnitSphere(VEC3 *Out) { RandomPointInUnitSphere(Out, common::g_Rand); }
void RandomPointInUnitCircle(VEC2 *Out) { RandomPointInUnitCircle(Out, common::g_Rand); }
void RandomPointInCapsule(VEC3 *Out, const VEC3 &p1, const VEC3 &p2, float R) { RandomPointInCapsule(Out, p1, p2, R, common::g_Rand); }

void CalcBillboardOrientation(
	VEC3 *OutRight, VEC3 *OutUp,
	unsigned DegreesOfFreedom, bool UseRealDir,
	const VEC3 &DefinedRight, const VEC3 &DefinedUp,
	const VEC3 &CamRightDir, const VEC3 &CamRealUpDir, const VEC3 &CameraToObjectDir)
{
	if (DegreesOfFreedom == 0)
	{
		*OutRight = DefinedRight;
		*OutUp = DefinedUp;
	}
	else if (DegreesOfFreedom == 1)
	{
		*OutUp = DefinedUp;

		if (UseRealDir)
		{
			Cross(OutRight, CameraToObjectDir, DefinedUp);
			*OutRight = - *OutRight;
			Normalize(OutRight);
		}
		else
			*OutRight = CamRightDir;
	}
	else // (DegreesOfFreedom == 2)
	{
		if (UseRealDir)
		{
			// Tu Up wykorzystany tylko tymczasowo, wyliczam Right
			*OutUp = VEC3_POSITIVE_Y;
			if (fabsf(Dot(*OutUp, CameraToObjectDir)) > 0.99f)
				*OutUp = VEC3_POSITIVE_Z;
			Cross(OutRight, CameraToObjectDir, *OutUp);
			*OutRight = - *OutRight;
			Normalize(OutRight);

			// Tu wyliczam docelowe Up
			Cross(OutUp, CameraToObjectDir, *OutRight);
			Normalize(OutUp);
		}
		else
		{
			*OutRight = CamRightDir;
			*OutUp = CamRealUpDir;
		}
	}
}

void RayToTriangleCalc::Init(const VEC3 &TriA, const VEC3 &TriB, const VEC3 &TriC)
{
	VEC3 n;
	Cross(&n, TriB-TriA, TriC-TriA);
	PointNormalToPlane(&m_Plane, TriA, n);

	VEC3 tmp;
	Cross(&tmp, n, TriC-TriB);
	PointNormalToPlane(&m_EdgePlaneBC, TriB, tmp);
	Cross(&tmp, n, TriA-TriC);
	PointNormalToPlane(&m_EdgePlaneCA, TriC, tmp);

	m_EdgePlaneBC /= DotCoord(m_EdgePlaneBC, TriA);
	m_EdgePlaneCA /= DotCoord(m_EdgePlaneCA, TriB);
}

bool RayToTriangleCalc::RayToTriangle(
	const VEC3 &RayOrig, const VEC3 &RayDir, bool BackfaceCulling,
	float *OutT, float *OutU, float *OutV, float *OutW, VEC3 *OutPoint) const
{
	// Compute distance of RayOrig to triangle plane. Exit if RayOrig lies behind plane
	float distp = DotCoord(m_Plane, RayOrig);
	if (BackfaceCulling && distp < 0.0f) return false;

	// Compute distance of q to triangle plane. Exit if q lies in front of plane
	float distq = DotCoord(m_Plane, RayOrig+RayDir);
	//if (distq >= 0.0f) return false;

	// Compute *OutT value and point *OutPoint of intersection with triangle plane
	float denom = distp - distq;
	*OutT = distp / denom;
	*OutPoint = RayOrig + *OutT * RayDir;

	// Compute the barycentric coordinate OutU; exit if outside 0..1 range
	*OutU = DotCoord(m_EdgePlaneBC, *OutPoint);
	if (*OutU < 0.0f || *OutU > 1.0f) return false;
	// Compute the barycentric coordinate OutV; exit if negative
	*OutV = DotCoord(m_EdgePlaneCA, *OutPoint);
	if (*OutV < 0.0f) return false;
	// Compute the barycentric coordinate OutW; exit if negative
	*OutW = 1.0f - *OutU - *OutV;
	if (*OutW < 0.0f) return false;

	// Segment intersects tri at distance *OutT in position *OutPoint (*OutPoint = *OutU*A + *OutV*B + *OutW*C)
	return true;
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Poisson Disc

const float POISSON_DISC_1D[] = {
	0.919736f,
	0.305094f,
	0.0208441f,
	0.605945f,
	0.44319f,
	0.146458f,
	0.76278f,
	0.528336f,
	0.676595f,
	0.214301f,
	0.833399f,
	0.997833f,
	0.37315f,
	0.0885037f,
	0.714835f,
	0.87286f,
	0.487899f,
	0.264931f,
	0.957305f,
	0.567492f,
	0.643178f,
	0.794427f,
	0.406964f,
	0.0525834f,
	0.335887f,
	0.118015f,
	0.185827f,
	0.739921f,
	0.894559f,
	0.240577f,
	0.465804f,
	0.586474f,
	0.694205f,
	0.625568f,
	0.0719626f,
	0.00463881f,
	0.93582f,
	0.164312f,
	0.546831f,
	0.388989f,
	0.280923f,
	0.352916f,
	0.426099f,
	0.979339f,
	0.855708f,
	0.507614f,
	0.81634f,
	0.660543f,
	0.0367138f,
	0.322001f,
	0.252602f,
	0.780908f,
	0.133732f,
	0.103427f,
	0.227821f,
	0.727378f,
	0.1995f,
	0.907102f,
	0.576495f,
	0.496841f,
	0.557146f,
	0.806116f,
	0.752403f,
	0.36195f,
	0.948119f,
	0.844874f,
	0.294046f,
	0.454604f,
	0.883755f,
	0.518418f,
	0.703299f,
	0.968993f,
	0.596851f,
	0.478439f,
	0.615375f,
	0.173589f,
	0.0624409f,
	0.537614f,
	0.416211f,
	0.988372f,
	0.382153f,
	0.635884f,
	0.207587f,
	0.928922f,
	0.0137028f,
	0.683432f,
	0.0281381f,
	0.314188f,
	0.825678f,
	0.155858f,
	0.863887f,
	0.110508f,
	0.651326f,
	0.772576f,
	0.43614f,
	0.328715f,
	0.669637f,
	0.272896f,
	0.0793481f,
	0.125278f,
};

const VEC2 POISSON_DISC_2D[] = {
	VEC2(0.203192f, 0.761162f),
	VEC2(0.938597f, 0.789483f),
	VEC2(0.255654f, 0.0919828f),
	VEC2(0.93878f, 0.150029f),
	VEC2(0.659322f, 0.441084f),
	VEC2(0.563066f, 0.887509f),
	VEC2(0.0140385f, 0.390149f),
	VEC2(0.609149f, 0.0334788f),
	VEC2(0.374218f, 0.47908f),
	VEC2(0.996857f, 0.483566f),
	VEC2(0.0172735f, 0.983978f),
	VEC2(0.245369f, 0.97467f),
	VEC2(0.718131f, 0.225562f),
	VEC2(0.687124f, 0.685415f),
	VEC2(0.03296f, 0.00997955f),
	VEC2(0.794366f, 0.955504f),
	VEC2(0.217475f, 0.332926f),
	VEC2(0.425062f, 0.270974f),
	VEC2(0.0307321f, 0.610034f),
	VEC2(0.44969f, 0.69805f),
	VEC2(0.832057f, 0.590991f),
	VEC2(0.432081f, 0.0149541f),
	VEC2(0.828913f, 0.37553f),
	VEC2(0.833796f, 0.00650044f),
	VEC2(0.424482f, 0.974395f),
	VEC2(0.18656f, 0.573443f),
	VEC2(0.990905f, 0.945189f),
	VEC2(0.0121158f, 0.794153f),
	VEC2(0.0946684f, 0.22364f),
	VEC2(0.337626f, 0.869503f),
	VEC2(0.525071f, 0.482131f),
	VEC2(0.0847194f, 0.495804f),
	VEC2(0.597797f, 0.163976f),
	VEC2(0.338237f, 0.365032f),
	VEC2(0.302713f, 0.24073f),
	VEC2(0.813349f, 0.793939f),
	VEC2(0.125034f, 0.886349f),
	VEC2(0.597674f, 0.314829f),
	VEC2(0.310862f, 0.633992f),
	VEC2(0.630726f, 0.57033f),
	VEC2(0.461257f, 0.823573f),
	VEC2(0.979003f, 0.278817f),
	VEC2(0.999756f, 0.644673f),
	VEC2(0.990204f, 0.0241707f),
	VEC2(0.687918f, 0.809961f),
	VEC2(0.78106f, 0.116184f),
	VEC2(0.571429f, 0.757225f),
	VEC2(0.652577f, 0.977691f),
	VEC2(0.840388f, 0.23072f),
	VEC2(0.38432f, 0.136387f),
	VEC2(0.137059f, 0.0767846f),
	VEC2(0.00473037f, 0.145268f),
	VEC2(0.214301f, 0.456923f),
	VEC2(0.915677f, 0.524338f),
	VEC2(0.47557f, 0.564318f),
	VEC2(0.486129f, 0.342875f),
	VEC2(0.779931f, 0.507309f),
	VEC2(0.899594f, 0.69985f),
	VEC2(0.520341f, 0.0552385f),
	VEC2(0.965423f, 0.386212f),
	VEC2(0.550096f, 0.614093f),
	VEC2(0.10062f, 0.723808f),
	VEC2(0.315561f, 0.734184f),
	VEC2(0.146001f, 0.98529f),
	VEC2(0.467208f, 0.184576f),
	VEC2(0.737785f, 0.00854518f),
	VEC2(0.919065f, 0.880947f),
	VEC2(0.201331f, 0.000274667f),
	VEC2(0.691366f, 0.330729f),
	VEC2(0.127293f, 0.355632f),
	VEC2(0.802423f, 0.675588f),
	VEC2(0.522416f, 0.257057f),
	VEC2(0.18366f, 0.186682f),
	VEC2(0.749687f, 0.41905f),
	VEC2(0.893704f, 0.99588f),
	VEC2(0.00601215f, 0.266945f),
	VEC2(0.333323f, 0.0261238f),
	VEC2(0.283578f, 0.520096f),
	VEC2(0.219947f, 0.8511f),
	VEC2(0.684927f, 0.140873f),
	VEC2(0.890378f, 0.306436f),
	VEC2(0.72457f, 0.579485f),
	VEC2(0.384869f, 0.581103f),
	VEC2(0.518693f, 0.969878f),
	VEC2(0.715079f, 0.895688f),
	VEC2(0.898038f, 0.0681478f),
	VEC2(0.206763f, 0.671834f),
	VEC2(0.778466f, 0.297281f),
	VEC2(0.007416f, 0.893704f),
	VEC2(0.00149541f, 0.548143f),
	VEC2(0.940001f, 0.599353f),
	VEC2(0.0575884f, 0.0792566f),
	VEC2(0.205664f, 0.253487f),
	VEC2(0.362682f, 0.782617f),
	VEC2(0.840693f, 0.469527f),
	VEC2(0.544328f, 0.405286f),
	VEC2(0.125553f, 0.650655f),
	VEC2(0.331095f, 0.941038f),
	VEC2(0.444563f, 0.0819117f),
	VEC2(0.260933f, 0.40672f),
};

const VEC3 POISSON_DISC_3D[] = {
	VEC3(0.700583f, 0.131199f, 0.789819f),
	VEC3(0.233589f, 0.330454f, 0.194098f),
	VEC3(0.323435f, 0.725242f, 0.798456f),
	VEC3(0.256874f, 0.894192f, 0.209754f),
	VEC3(0.906308f, 0.554521f, 0.244118f),
	VEC3(0.815455f, 0.715476f, 0.946867f),
	VEC3(0.837245f, 0.0255135f, 0.125004f),
	VEC3(0.0839564f, 0.0171819f, 0.839473f),
	VEC3(0.726646f, 0.968963f, 0.46498f),
	VEC3(0.877041f, 0.433424f, 0.623371f),
	VEC3(0.322825f, 0.0265511f, 0.416272f),
	VEC3(0.60625f, 0.454146f, 0.00573748f),
	VEC3(0.797998f, 0.887051f, 0.0751366f),
	VEC3(0.0401013f, 0.426466f, 0.99118f),
	VEC3(0.103122f, 0.441237f, 0.553697f),
	VEC3(0.0527055f, 0.976287f, 0.715323f),
	VEC3(0.499863f, 0.501389f, 0.54799f),
	VEC3(0.39491f, 0.344371f, 0.900418f),
	VEC3(0.0426344f, 0.0268258f, 0.0708945f),
	VEC3(0.426862f, 0.0350658f, 0.0274361f),
	VEC3(0.991272f, 0.00253304f, 0.5009f),
	VEC3(0.145543f, 0.523942f, 0.00283822f),
	VEC3(0.948027f, 0.94058f, 0.643757f),
	VEC3(0.986511f, 0.0397656f, 0.820368f),
	VEC3(0.00936918f, 0.0529191f, 0.4691f),
	VEC3(0.395917f, 0.141636f, 0.66921f),
	VEC3(0.881924f, 0.440077f, 0.92291f),
	VEC3(0.283242f, 0.727531f, 0.509934f),
	VEC3(0.65157f, 0.905728f, 0.797876f),
	VEC3(0.00549333f, 0.878903f, 0.444105f),
	VEC3(0.420087f, 0.59801f, 0.275155f),
	VEC3(0.591845f, 0.0939055f, 0.334056f),
	VEC3(0.00106815f, 0.77221f, 0.964019f),
	VEC3(0.347179f, 0.934629f, 0.98703f),
	VEC3(0.734153f, 0.678121f, 0.597736f),
	VEC3(0.981292f, 0.382641f, 0.0160222f),
	VEC3(0.0285958f, 0.972198f, 0.0133366f),
	VEC3(0.329753f, 0.990692f, 0.621815f),
	VEC3(0.514328f, 0.996826f, 0.133396f),
	VEC3(0.977508f, 0.981109f, 0.299387f),
	VEC3(0.0110477f, 0.671529f, 0.203833f),
	VEC3(0.93936f, 0.256508f, 0.375286f),
	VEC3(0.549669f, 0.572619f, 0.976684f),
	VEC3(0.396435f, 0.0260323f, 0.994507f),
	VEC3(0.540117f, 0.310892f, 0.340465f),
	VEC3(0.55034f, 0.274789f, 0.575945f),
	VEC3(0.404767f, 0.529832f, 0.0286569f),
	VEC3(0.698233f, 0.371899f, 0.800012f),
	VEC3(0.709311f, 0.0685446f, 0.549516f),
	VEC3(0.435102f, 0.540239f, 0.792352f),
	VEC3(0.574664f, 0.723045f, 0.192877f),
	VEC3(0.674001f, 0.511368f, 0.290658f),
	VEC3(0.724326f, 0.249947f, 0.118259f),
	VEC3(0.198126f, 0.567431f, 0.351665f),
	VEC3(0.227638f, 0.951903f, 0.418958f),
	VEC3(0.231819f, 0.208045f, 0.786248f),
	VEC3(0.0803858f, 0.657399f, 0.511856f),
	VEC3(0.772149f, 0.927274f, 0.980499f),
	VEC3(0.289285f, 0.39903f, 0.674703f),
	VEC3(0.998291f, 0.796106f, 0.454329f),
	VEC3(0.000305185f, 0.330424f, 0.361736f),
	VEC3(0.00509659f, 0.249672f, 0.824488f),
	VEC3(0.913053f, 0.765923f, 0.244758f),
	VEC3(0.0798669f, 0.259682f, 0.0281991f),
	VEC3(0.482681f, 0.276833f, 0.136448f),
	VEC3(0.132969f, 0.7322f, 0.0426954f),
	VEC3(0.507218f, 0.825861f, 0.573046f),
	VEC3(0.217719f, 0.540452f, 0.87875f),
	VEC3(0.92056f, 0.196936f, 0.94705f),
	VEC3(0.986053f, 0.641041f, 0.0386975f),
	VEC3(0.019013f, 0.987854f, 0.261727f),
	VEC3(0.305643f, 0.211951f, 0.0111393f),
	VEC3(0.961547f, 0.689322f, 0.768426f),
	VEC3(0.500931f, 0.898343f, 0.35548f),
	VEC3(0.956511f, 0.21952f, 0.704184f),
	VEC3(0.231025f, 0.264748f, 0.485916f),
	VEC3(0.726218f, 0.00341807f, 0.996734f),
	VEC3(0.99939f, 0.183935f, 0.149022f),
	VEC3(0.218604f, 0.00259407f, 0.628071f),
	VEC3(0.995911f, 0.879482f, 0.976959f),
	VEC3(0.361888f, 0.75808f, 0.0860622f),
	VEC3(0.0212409f, 0.229408f, 0.613819f),
	VEC3(0.697012f, 0.421552f, 0.494552f),
	VEC3(0.0376904f, 0.983367f, 0.932524f),
	VEC3(0.591632f, 0.269265f, 0.996246f),
	VEC3(0.204443f, 0.119053f, 0.235054f),
	VEC3(0.739799f, 0.7481f, 0.39143f),
	VEC3(0.945524f, 0.584063f, 0.463088f),
	VEC3(0.646077f, 0.578784f, 0.775597f),
	VEC3(0.0684835f, 0.651051f, 0.737968f),
	VEC3(0.519395f, 0.801508f, 0.945128f),
	VEC3(0.735343f, 0.616077f, 0.104343f),
	VEC3(0.00216681f, 0.464278f, 0.149785f),
	VEC3(0.160558f, 0.242653f, 0.984741f),
	VEC3(0.530534f, 0.975036f, 0.984619f),
	VEC3(0.221137f, 0.0102847f, 0.000579852f),
	VEC3(0.999451f, 0.724662f, 0.942351f),
	VEC3(0.344554f, 0.459304f, 0.213965f),
	VEC3(0.522324f, 0.659688f, 0.010712f),
	VEC3(0.435011f, 0.0114139f, 0.235969f),
};


} // namespace common
