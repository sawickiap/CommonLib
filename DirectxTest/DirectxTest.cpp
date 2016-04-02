#include "../Common/Base.hpp"
#include "../Common/Math.hpp"
#include <cassert>
#include <windows.h>
#include <d3dx9.h>

using namespace common;

enum MOUSE_BUTTON { MOUSE_LEFT, MOUSE_MIDDLE, MOUSE_RIGHT };
enum MOUSE_EVENT_ { MOUSE_DOWN, MOUSE_UP, MOUSE_DBLCLK };

struct VERTEX
{
	VEC3 Pos;
	VEC3 Normal;
};

const char * const CLASS_NAME = "COMMON_LIB_DX_TEST";
const char * const WINDOW_TITLE = "CommonLib DirectX Test";
const unsigned SCREEN_CX = 800;
const unsigned SCREEN_CY = 600;
const float CAM_YAW_RADIANS_PER_PIXEL = 0.01f;
const float CAM_PITCH_RADIANS_PER_PIXEL = 0.01f;
const float CAM_DIST_UNITS_PER_PIXEL = 0.02f;
const float Z_NEAR = 0.5f, Z_FAR = 100.0f;
const VEC3 DIR_TO_LIGHT = VEC3(0.49147318718299f, 0.245736593591495f, -0.835504418211084f); // W przestrzeni widoku

const unsigned TRANSFORM_INDEX_COUNT = 2;
const char * TRANSFORM_INDEX_NAMES[TRANSFORM_INDEX_COUNT] = {
	"x",
	"y",
};

HINSTANCE g_Instance = NULL;
HWND g_Wnd = NULL;
IDirect3D9 *g_D3D = NULL;
IDirect3DDevice9 *g_Dev = NULL;
ID3DXFont *g_Font = NULL;
ID3DXEffect *g_GeometricPrimitivesEffect = NULL;
ID3DXMesh *g_BoxMesh = NULL, *g_SphereMesh = NULL, *g_CylinderMesh = NULL;
float g_CamYaw, g_CamPitch, g_CamDist;
MATRIX g_View, g_Proj;
bool g_CamRotMode = false, g_CamZoomMode = false, g_TransformMode = false;
int g_CursorLastX, g_CursorLastY;
unsigned g_TransformIndex = 0;

////// Dane aktualnego testu
namespace {

VEC2 g_Point = VEC2(0.f, 0.f);


} // namespace


void CreateFont()
{
	HRESULT hr = D3DXCreateFont(
		g_Dev,
		-12,
		0,
		FW_BOLD,
		0,
		FALSE,
		EASTEUROPE_CHARSET,
		OUT_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DECORATIVE,
		"Arial",
		&g_Font);
	assert(SUCCEEDED(hr));
}

void DestroyFont()
{
	SAFE_RELEASE(g_Font);
}

void CreateEffects()
{
	HRESULT hr = D3DXCreateEffectFromFile(
		g_Dev,
		"GeometricPrimitives.fxo",
		NULL,
		NULL,
		0,
		NULL,
		&g_GeometricPrimitivesEffect,
		NULL);
	assert(SUCCEEDED(hr));
}

void DestroyEffects()
{
	SAFE_RELEASE(g_GeometricPrimitivesEffect);
}

void RecalcViewMatrix()
{
	VEC3 CamEye;
	SphericalToCartesian(&CamEye, g_CamYaw, g_CamPitch, g_CamDist);
	LookAtLH(&g_View, CamEye, -CamEye, VEC3_POSITIVE_Y);
}

void InitCamera()
{
	PerspectiveFovLH(
		&g_Proj,
		DegToRad(55.0f),
		(float)SCREEN_CX / (float)SCREEN_CY,
		Z_NEAR, Z_FAR);

	g_CamYaw = DegToRad(15.0f);
	g_CamPitch = DegToRad(15.0f);
	g_CamDist = 10.0f;
	RecalcViewMatrix();
}

void CreateMeshes()
{
	HRESULT hr = D3DXCreateBox(g_Dev, 2.0f, 2.0f, 2.0f, &g_BoxMesh, NULL);
	assert(SUCCEEDED(hr));

	hr = D3DXCreateSphere(g_Dev, 1.0f, 32, 32, &g_SphereMesh, NULL);
	assert(SUCCEEDED(hr));

	hr = D3DXCreateCylinder(g_Dev, 1.0f, 1.0f, 2.0f, 32, 1, &g_CylinderMesh, NULL);
	assert(SUCCEEDED(hr));
}

void DestroyMeshes()
{
	SAFE_RELEASE(g_CylinderMesh);
	SAFE_RELEASE(g_SphereMesh);
	SAFE_RELEASE(g_BoxMesh);
}

void CreateD3D()
{
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	assert(g_D3D);

	D3DPRESENT_PARAMETERS PresentParams = {
		SCREEN_CX, SCREEN_CY,
		D3DFMT_A8R8G8B8,
		0,
		D3DMULTISAMPLE_4_SAMPLES, //D3DMULTISAMPLE_NONE,
		1, // ??
		D3DSWAPEFFECT_DISCARD,
		g_Wnd,
		TRUE,
		TRUE,
		D3DFMT_D24S8,
		D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL,
		0,
		D3DPRESENT_INTERVAL_ONE
	};

	HRESULT hr = g_D3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		g_Wnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&PresentParams,
		&g_Dev);
	assert(SUCCEEDED(hr));

	CreateFont();
	CreateMeshes();
	CreateEffects();
	InitCamera();
}

void DestroyD3D()
{
	DestroyEffects();
	DestroyMeshes();
	DestroyFont();

	if (g_Dev) { g_Dev->Release(); g_Dev = NULL; }
	if (g_D3D) { g_D3D->Release(); g_D3D = NULL; }
}

void OnKeyDown(WPARAM Key)
{
	switch (Key)
	{
	case VK_ESCAPE:
		PostMessage(g_Wnd, WM_CLOSE, 0, 0);
		break;
	case VK_DOWN:
		g_TransformIndex = (g_TransformIndex + 1) % TRANSFORM_INDEX_COUNT;
		break;
	case VK_UP:
		g_TransformIndex = (g_TransformIndex + TRANSFORM_INDEX_COUNT - 1) % TRANSFORM_INDEX_COUNT;
		break;
	case VK_PRIOR:
		if (g_TransformIndex >= 5)
			g_TransformIndex -= 5;
		else
			g_TransformIndex = 0;
		break;
	case VK_NEXT:
		g_TransformIndex = std::min(g_TransformIndex + 5, TRANSFORM_INDEX_COUNT - 1);
		break;
	case VK_HOME:
		g_TransformIndex = 0;
		break;
	case VK_END:
		g_TransformIndex = TRANSFORM_INDEX_COUNT - 1;
		break;
	case VK_RIGHT:
		break;
	case VK_LEFT:
		break;
	}
}

void OnKeyUp(WPARAM Key)
{
}

void OnChar(char Ch)
{
}

void OnMouseButton(LPARAM LParam, MOUSE_BUTTON Button, MOUSE_EVENT_ Event)
{
	int CursorX = (int)(signed short)LOWORD(LParam);
	int CursorY = (int)(signed short)HIWORD(LParam);

	if (Button == MOUSE_LEFT)
	{
		if (Event == MOUSE_DOWN)
		{
			g_CamRotMode = true;
			g_CursorLastX = CursorX;
			g_CursorLastY = CursorY;
		}
		else if (Event == MOUSE_UP)
			g_CamRotMode = false;
	}
	else if (Button == MOUSE_RIGHT)
	{
		if (Event == MOUSE_DOWN)
		{
			g_CamZoomMode = true;
			g_CursorLastX = CursorX;
			g_CursorLastY = CursorY;
		}
		else if (Event == MOUSE_UP)
			g_CamZoomMode = false;
	}
	else
	{
		if (Event == MOUSE_DOWN)
		{
			g_TransformMode = true;
			g_CursorLastX = CursorX;
			g_CursorLastY = CursorY;
		}
		else if (Event == MOUSE_UP)
			g_TransformMode = false;
	}
}

/*void RecalcObbAxes()
{
	g_ObbAngles1.y = minmax(-PI_2*0.99f, g_ObbAngles1.y, PI_2*0.99f);

	MATRIX Rot;

	RotationYawPitchRoll(&Rot, g_ObbAngles1);
	g_Obb1.Axes[0] = VEC3(Rot._11, Rot._12, Rot._13);
	g_Obb1.Axes[1] = VEC3(Rot._21, Rot._22, Rot._23);
	g_Obb1.Axes[2] = VEC3(Rot._31, Rot._32, Rot._33);
}*/

void DoMouseTransform(int CursorPixelsDeltaX, int CursorPixelsDeltaY)
{
	static const float TRANS_FACTOR = 0.01f;
	static const float ANGLE_FACTOR = 0.01f;

	int SumDelta = CursorPixelsDeltaX + CursorPixelsDeltaY;

	switch (g_TransformIndex)
	{
	case  0: g_Point.x += SumDelta * TRANS_FACTOR; break;
	case  1: g_Point.y += SumDelta * TRANS_FACTOR; break;
	}
}

void OnMouseMove(LPARAM LParam)
{
	int CursorX = (int)(signed short)LOWORD(LParam);
	int CursorY = (int)(signed short)HIWORD(LParam);

	if (g_CamRotMode)
	{
		g_CamYaw   += (CursorX - g_CursorLastX) * CAM_YAW_RADIANS_PER_PIXEL;
		g_CamPitch += (CursorY - g_CursorLastY) * CAM_PITCH_RADIANS_PER_PIXEL;
		g_CamPitch = minmax(-PI_2*0.99f, g_CamPitch, PI_2*0.99f);
		RecalcViewMatrix();
		g_CursorLastX = CursorX;
		g_CursorLastY = CursorY;
	}
	else if (g_CamZoomMode)
	{
		g_CamDist -= ( (CursorX - g_CursorLastX) + (CursorY - g_CursorLastY) ) * CAM_DIST_UNITS_PER_PIXEL;
		g_CamDist = std::max(Z_NEAR, g_CamDist);
		RecalcViewMatrix();
		g_CursorLastX = CursorX;
		g_CursorLastY = CursorY;
	}
	else if (g_TransformMode)
	{
		DoMouseTransform(CursorX - g_CursorLastX, CursorY - g_CursorLastY);
		g_CursorLastX = CursorX;
		g_CursorLastY = CursorY;
	}
}

void SetupDraw(COLOR Color, const MATRIX *World)
{
	VEC3 DirToLight_World;
	UntransformNormal(&DirToLight_World, DIR_TO_LIGHT, g_View);
	if (World)
	{
		VEC3 DirToLight_Local;

		//UntransformNormal(&DirToLight_Local, DirToLight_World, *World);

		MATRIX WorldInvT;
		Inverse(&WorldInvT, *World);
		Transpose(&WorldInvT);
		TransformNormal(&DirToLight_Local, DirToLight_World, WorldInvT);

		Normalize(&DirToLight_Local);
		g_GeometricPrimitivesEffect->SetMatrix("g_WorldViewProj", &( (*World) * g_View * g_Proj ));
		g_GeometricPrimitivesEffect->SetVector("g_DirToLight", &VEC4(DirToLight_Local.x, DirToLight_Local.y, DirToLight_Local.z, 1.0f));
	}
	else
	{
		g_GeometricPrimitivesEffect->SetMatrix("g_WorldViewProj", &( g_View * g_Proj ));
		g_GeometricPrimitivesEffect->SetVector("g_DirToLight", &VEC4(DirToLight_World.x, DirToLight_World.y, DirToLight_World.z, 1.0f));
	}

	COLORF Colorf;
	ColorToColorf(&Colorf, Color);
	g_GeometricPrimitivesEffect->SetVector("g_Color", &VEC4(Colorf.R, Colorf.G, Colorf.B, Colorf.A));

	g_GeometricPrimitivesEffect->CommitChanges();
}

void DrawGeometry(D3DPRIMITIVETYPE PrimitiveType, const VERTEX *Vertices, uint PrimitiveCount, COLOR Color, const MATRIX *World)
{
	SetupDraw(Color, World);
	g_Dev->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, Vertices, sizeof(VERTEX));
}

void DrawTriangle(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, COLOR Color, const MATRIX *World = NULL)
{
	VEC3 Normal;
	Cross(&Normal, p2-p1, p3-p1);
	Normalize(&Normal);
	VERTEX Vertices[3] = {
		{ p1, Normal },
		{ p2, Normal },
		{ p3, Normal },
	};
	DrawGeometry(D3DPT_TRIANGLELIST, Vertices, 1, Color, World);
}

void DrawBox(const BOX &Box, COLOR Color, const MATRIX *World = NULL)
{
	MATRIX scal, trans;
	Scaling(&scal, Box.GetSizeDim(0) * 0.5f, Box.GetSizeDim(1) * 0.5f, Box.GetSizeDim(2) * 0.5f);
	VEC3 BoxCenter;
	Box.GetCenter(&BoxCenter);
	Translation(&trans, BoxCenter);

	if (World)
		SetupDraw(Color, &(scal * trans * (*World)));
	else
		SetupDraw(Color, &(scal * trans));
	
	g_BoxMesh->DrawSubset(0);
}

void DrawObb(const OBB &Obb, COLOR Color)
{
	MATRIX scal, RotTrans;
	Scaling(&scal, Obb.HalfSize);
	AxesToMatrix(&RotTrans, Obb.Axes[0], Obb.Axes[1], Obb.Axes[2]);
	MatrixSetTranslation(&RotTrans, Obb.Center);

	SetupDraw(Color, &(scal * RotTrans));
	
	g_BoxMesh->DrawSubset(0);
}

void DrawSphere(const VEC3 &Center, float Radius, COLOR Color)
{
	MATRIX M;
	Scaling(&M, Radius);
	MatrixSetTranslation(&M, Center);

	SetupDraw(Color, &M);
	
	g_SphereMesh->DrawSubset(0);
}

void DrawCylinder(const VEC3 &p1, const VEC3 &p2, float Radius, COLOR Color)
{
	MATRIX scal, RotTrans;

	Scaling(&scal, Radius, Radius, Distance(p1, p2) * 0.5f);

	VEC3 Axis[3], Up;
	Axis[0] = p2 - p1;
	Normalize(&Axis[0]);
	if (fabsf(Dot(Axis[0], VEC3_POSITIVE_Y)) > 0.99f)
		Up = VEC3_POSITIVE_Z;
	else
		Up = VEC3_POSITIVE_Y;
	Cross(&Axis[1], Axis[0], Up);
	Cross(&Axis[2], Axis[0], Axis[1]);
	Normalize(&Axis[0]);
	Normalize(&Axis[1]);
	Normalize(&Axis[2]);
	AxesToMatrix(&RotTrans, Axis[2], Axis[1], Axis[0]);
	//Transpose(&RotTrans);

	VEC3 mid_point;
	MidPoint(&mid_point, p1, p2);
	MatrixSetTranslation(&RotTrans, mid_point);

	SetupDraw(Color, &(scal * RotTrans));
	
	g_CylinderMesh->DrawSubset(0);
}

void DrawCapsule(const VEC3 &CapsuleA, const VEC3 &CapsuleB, float CapsuleRadius, COLOR Color)
{
	DrawSphere(CapsuleA, CapsuleRadius, Color);
	DrawSphere(CapsuleB, CapsuleRadius, Color);
	DrawCylinder(CapsuleA, CapsuleB, CapsuleRadius, Color);
}

void DrawCircle(const VEC3 &Center, const VEC3 &Dir1, const VEC3 &Dir2, float Radius, COLOR Color)
{
	VEC3 Normal;
	Cross(&Normal, Dir1, Dir2);
	
	static const unsigned CIRCLE_VERTEX_COUNT = 32;
	VERTEX Vertices[CIRCLE_VERTEX_COUNT+2];
	Vertices[0].Pos = Center;
	Vertices[0].Normal = Normal;
	float Angle = 0.0f, AngleStep = PI_X_2 / CIRCLE_VERTEX_COUNT;
	for (unsigned i = 1; i <= CIRCLE_VERTEX_COUNT+1; i++, Angle += AngleStep)
	{
		Vertices[i].Pos = Center + Dir1*(cosf(Angle)*Radius) + Dir2*(sinf(Angle)*Radius);
		Vertices[i].Normal = Normal;
	}
	DrawGeometry(D3DPT_TRIANGLEFAN, Vertices, CIRCLE_VERTEX_COUNT, Color, NULL);
}

void DrawVector(const VEC3 &Begin, const VEC3 &Vec, COLOR Color)
{
	VEC3 End = Begin + Vec;
	DrawCylinder(Begin, End, 0.05f, Color);

	// Grot
	VEC3 Dir1, Dir2;
	PerpedicularVectors(&Dir1, &Dir2, Vec);
	Normalize(&Dir1);
	Normalize(&Dir2);
	
	VEC3 ArrowBegin = Begin + Vec * 0.8f;
	DrawTriangle(End, ArrowBegin + Dir1*0.05f, ArrowBegin - Dir1*0.05f, Color);
	DrawTriangle(End, ArrowBegin + Dir2*0.05f, ArrowBegin - Dir2*0.05f, Color);
}

void DrawRay(const VEC3 &RayOrig, const VEC3 &RayDir, COLOR Color)
{
	VEC3 End = RayOrig + RayDir;
	DrawCylinder(RayOrig, RayOrig + RayDir * 1000.0f, 0.02f, Color);

	// Grot
	VEC3 Dir1, Dir2;
	PerpedicularVectors(&Dir1, &Dir2, RayDir);
	Normalize(&Dir1);
	Normalize(&Dir2);
	
	VEC3 ArrowBegin = RayOrig + RayDir * 0.8f;
	DrawTriangle(End, ArrowBegin + Dir1*0.05f, ArrowBegin - Dir1*0.05f, Color);
	DrawTriangle(End, ArrowBegin + Dir2*0.05f, ArrowBegin - Dir2*0.05f, Color);
}

// Rysuje p³aszczyznê jako kó³ko o du¿ym promieniu
void DrawPlane(const PLANE &Plane, COLOR Color)
{
	VEC3 Center, Dir1, Dir2;
	ClosestPointOnPlane(&Center, Plane, VEC3_ZERO);
	PerpedicularVectors(&Dir1, &Dir2, GetPlaneNormal(Plane));
	Normalize(&Dir1);
	Normalize(&Dir2);

	static const float PLANE_CIRCLE_RADIUS = 5.0f;
	static const COLOR PLANE_NORMAL_COLOR = COLOR_WHITE;

	DrawCircle(Center, Dir1, Dir2, PLANE_CIRCLE_RADIUS, Color);
	DrawVector(Center, GetPlaneNormal(Plane), PLANE_NORMAL_COLOR);
}

void DrawText_()
{
	RECT Rect = { 8, 8, 10, 10 };

	string msg;
	for (unsigned i = 0; i < TRANSFORM_INDEX_COUNT; i++)
	{
		if (g_TransformIndex == i)
			msg += "[#] ";
		msg += TRANSFORM_INDEX_NAMES[i];
		msg += "\n";
	}

	g_Font->DrawText(NULL, msg.data(), msg.length(), &Rect, DT_LEFT | DT_TOP | DT_NOCLIP, 0xFFFFFFFF);
}

void DrawPoint(const VEC3 &Pt, COLOR Color)
{
	static const float POINT_SPHERE_RADIUS = 0.1f;
	DrawSphere(Pt, POINT_SPHERE_RADIUS, Color);
}

void DrawQuad(const VEC3 &p1, const VEC3 &p2, const VEC3 &p3, const VEC3 &p4, COLOR Color, const MATRIX *World = NULL)
{
	VEC3 Normal;
	Cross(&Normal, p2-p1, p3-p1);
	Normalize(&Normal);
	VERTEX Vertices[4] = {
		{ p1, Normal },
		{ p2, Normal },
		{ p3, Normal },
		{ p4, Normal },
	};
	DrawGeometry(D3DPT_TRIANGLEFAN, Vertices, 2, Color, World);
}

void DrawFrustum(const FRUSTUM_POINTS &F, COLOR Color)
{
	DrawQuad(F[FRUSTUM_POINTS::NEAR_LEFT_BOTTOM], F[FRUSTUM_POINTS::NEAR_LEFT_TOP], F[FRUSTUM_POINTS::NEAR_RIGHT_TOP], F[FRUSTUM_POINTS::NEAR_RIGHT_BOTTOM], Color);
	DrawQuad(F[FRUSTUM_POINTS::FAR_LEFT_BOTTOM], F[FRUSTUM_POINTS::FAR_LEFT_TOP], F[FRUSTUM_POINTS::FAR_RIGHT_TOP], F[FRUSTUM_POINTS::FAR_RIGHT_BOTTOM], Color);
	DrawQuad(F[FRUSTUM_POINTS::NEAR_LEFT_BOTTOM], F[FRUSTUM_POINTS::FAR_LEFT_BOTTOM], F[FRUSTUM_POINTS::FAR_LEFT_TOP], F[FRUSTUM_POINTS::NEAR_LEFT_TOP], Color);
	DrawQuad(F[FRUSTUM_POINTS::NEAR_RIGHT_BOTTOM], F[FRUSTUM_POINTS::FAR_RIGHT_BOTTOM], F[FRUSTUM_POINTS::FAR_RIGHT_TOP], F[FRUSTUM_POINTS::NEAR_RIGHT_TOP], Color);
	DrawQuad(F[FRUSTUM_POINTS::NEAR_LEFT_BOTTOM], F[FRUSTUM_POINTS::FAR_LEFT_BOTTOM], F[FRUSTUM_POINTS::FAR_RIGHT_BOTTOM], F[FRUSTUM_POINTS::NEAR_RIGHT_BOTTOM], Color);
	DrawQuad(F[FRUSTUM_POINTS::NEAR_LEFT_TOP], F[FRUSTUM_POINTS::FAR_LEFT_TOP], F[FRUSTUM_POINTS::FAR_RIGHT_TOP], F[FRUSTUM_POINTS::NEAR_RIGHT_TOP], Color);
}

void DrawGeometricPrimitives()
{
	g_Dev->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
	//g_Dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	// Coordinate system origin
	DrawTriangle(VEC3(0.0f, 0.0f, -0.1f), VEC3(0.0f, 0.0f, 0.1f), VEC3(1.0f, 0.0f, 0.0f), COLOR_RED);
	DrawTriangle(VEC3(0.0f, -0.1f, 0.0f), VEC3(0.0f, 0.1f, 0.0f), VEC3(1.0f, 0.0f, 0.0f), COLOR_RED);
	float DepthBias = 1e-6f;
	g_Dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&DepthBias);
	DrawTriangle(VEC3(-0.1f, 0.0f, 0.0f), VEC3(0.1f, 0.0f, 0.0f), VEC3(0.0f, 1.0f, 0.0f), COLOR_LIME);
	DrawTriangle(VEC3(0.0f, 0.0f, -0.1f), VEC3(0.0f, 0.0f, 0.1f), VEC3(0.0f, 1.0f, 0.0f), COLOR_LIME);
	DepthBias = 2.0f * 1e-6f;
	g_Dev->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&DepthBias);
	DrawTriangle(VEC3(0.0f, -0.1f, -0.0f), VEC3(0.0f, 0.1f, 0.0f), VEC3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
	DrawTriangle(VEC3(-0.1f, 0.0f, -0.0f), VEC3(0.1f, 0.0f, 0.0f), VEC3(0.0f, 0.0f, 1.0f), COLOR_BLUE);
	g_Dev->SetRenderState(D3DRS_DEPTHBIAS, 0);

	////// Drawing current test


	VEC2 triA = VEC2(0.f, 0.f);
	VEC2 triB = VEC2(1.f, 1.f);
	VEC2 triC = VEC2(2.f, -1.7f);

	DrawTriangle(
		VEC3(triA.x,triA.y,0.f),
		VEC3(triB.x,triB.y,0.f),
		VEC3(triC.x,triC.y,0.f),
		COLOR_SILVER);

	VEC2 pt2;
	DrawPoint(VEC3(g_Point.x,g_Point.y,0.f), COLOR_BLACK);
	ClosestPointInTriangle(&pt2, g_Point, triA, triB, triC);
	DrawPoint(VEC3(pt2.x,pt2.y,0.f), COLOR_RED);

	// Promieñ
/*
	VEC3 Ray1Dir;
	SphericalToCartesian(&Ray1Dir, g_Ray1DirYaw, g_Ray1DirPitch, 0.5f);
	VEC3 CollisionPt;
*/

	// Jakiœ statyczny frustum
/*	MATRIX FrustumView, FrustumProj;
	LookAtLH(&FrustumView, VEC3(-0.4f, -0.2f, 0.1f), VEC3(0.924500327042048f, 0.369800130816819f, -0.0924500327042048f), VEC3_POSITIVE_Y);
	PerspectiveFovLH(&FrustumProj, DegToRad(55.0f), 4.0f/3.0f, 0.3f, 3.0f);
	FRUSTUM_PLANES FrustumPlanes(FrustumView*FrustumProj);
	FrustumPlanes.Normalize();
	FRUSTUM_POINTS FrustumPoints(FrustumPlanes);*/

	// Jakiœ statyczny cylinder
/*	VEC3 CylinderBeg = VEC3(-0.2f, -1.0f, -0.5f);
	VEC3 CylinderEnd = VEC3( 0.5f,  1.2f, 0.7f);
	float CylinderRadius = 0.3f;*/

/*	// Jakiœ statyczny wielok¹t wypuk³y 2D, CW
	static const VEC2 ConvexPolygonPoints[] = {
		VEC2(-1.0f, -1.0f),
		VEC2(-1.0f, 1.0f),
		VEC2(0.0f, 2.0f),
		VEC2(2.0f, 0.0f),
		VEC2(1.0f, -2.0f),
	};*/

/*	// Jakiœ statyczny wielok¹t wypuk³y 3D, CW
	static const VEC3 ConvexPolygonPoints[] = {
		VEC3(-1.0f, -0.5f, -1.0f),
		VEC3(-1.0f, -0.5f, 1.0f),
		VEC3(0.0f, 0.0f, 2.0f),
		VEC3(2.0f, 1.0f, 0.0f),
		VEC3(1.0f, 0.5f, -2.0f),
	};*/
}

void OnDraw()
{
	static const DWORD BKG_COLOR = 0xFF808080;
	g_Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, BKG_COLOR, 1.0f, 0);

	DrawText_();

	UINT FooPasses;
	g_GeometricPrimitivesEffect->Begin(&FooPasses, 0);
	g_GeometricPrimitivesEffect->BeginPass(0);

	DrawGeometricPrimitives();

	g_GeometricPrimitivesEffect->EndPass();
	g_GeometricPrimitivesEffect->End();
}

void OnCalc()
{
}

LRESULT CALLBACK WndProc(HWND Wnd, UINT Msg, WPARAM WParam, LPARAM LParam)
{
	switch (Msg)
	{
	case WM_CREATE:
		g_Wnd = Wnd;
		CreateD3D();
		break;
	case WM_DESTROY:
		DestroyD3D();
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		OnKeyDown(WParam);
		return 0;
	case WM_KEYUP:
		OnKeyUp(WParam);
		return 0;
	case WM_CHAR:
		OnChar((char)WParam);
		return 0;
	case WM_LBUTTONDOWN:   OnMouseButton(LParam, MOUSE_LEFT,   MOUSE_DOWN);   SetCapture(g_Wnd); return 0;
	case WM_MBUTTONDOWN:   OnMouseButton(LParam, MOUSE_MIDDLE, MOUSE_DOWN);   SetCapture(g_Wnd); return 0;
	case WM_RBUTTONDOWN:   OnMouseButton(LParam, MOUSE_RIGHT,  MOUSE_DOWN);   SetCapture(g_Wnd); return 0;
	case WM_LBUTTONUP:     OnMouseButton(LParam, MOUSE_LEFT,   MOUSE_UP);    ReleaseCapture(); return 0;
	case WM_MBUTTONUP:     OnMouseButton(LParam, MOUSE_MIDDLE, MOUSE_UP);    ReleaseCapture(); return 0;
	case WM_RBUTTONUP:     OnMouseButton(LParam, MOUSE_RIGHT,  MOUSE_UP);    ReleaseCapture(); return 0;
	case WM_LBUTTONDBLCLK: OnMouseButton(LParam, MOUSE_LEFT,   MOUSE_DBLCLK); return 0;
	case WM_MBUTTONDBLCLK: OnMouseButton(LParam, MOUSE_MIDDLE, MOUSE_DBLCLK); return 0;
	case WM_RBUTTONDBLCLK: OnMouseButton(LParam, MOUSE_RIGHT,  MOUSE_DBLCLK); return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(LParam);
		return 0;
	}
	return DefWindowProc(Wnd, Msg, WParam, LParam);
}

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE, char *CmdLine, int ShowCmd)
{
	g_Instance = Instance;

	WNDCLASSEX WndClass = {
		sizeof(WNDCLASSEX),
		CS_DBLCLKS,
		&WndProc,
		0,
		0,
		Instance,
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW),
		NULL,
		NULL,
		CLASS_NAME,
		NULL
	};
	RegisterClassEx(&WndClass);

	DWORD WindowStyle = (WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX) | WS_VISIBLE;
	DWORD WindowExStyle = 0;
	RECT Rect = { 0, 0, SCREEN_CX, SCREEN_CY };
	AdjustWindowRectEx(&Rect, WindowStyle, FALSE, WindowExStyle);

	g_Wnd = CreateWindowEx(
		WindowExStyle,
		CLASS_NAME,
		WINDOW_TITLE,
		WindowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		Rect.right - Rect.left, Rect.bottom - Rect.top,
		NULL,
		NULL,
		Instance,
		0);

	MSG Msg;
	Msg.message = WM_NULL;
	while (Msg.message != WM_QUIT)
	{
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}

		if (g_Dev)
		{
			HRESULT hr = g_Dev->BeginScene();
			assert(SUCCEEDED(hr));
			OnDraw();
			g_Dev->EndScene();
			OnCalc();
			hr = g_Dev->Present(NULL, NULL, NULL, NULL);
			assert(SUCCEEDED(hr));
		}
	}
	return (int)Msg.wParam;
}
