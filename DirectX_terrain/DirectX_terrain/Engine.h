#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <string>

#include "CamMain.h"
#include "CamMiniMap.h"
#include "ObjTriangle.h"
#include "ObjBall.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Ray.h"

using namespace std;


const string tex_dir[2] = { "src/tile2.tga", "lightmap.tga" };


class Engine
{
private:
	HWND					g_hwnd;
	LPDIRECT3D9             g_pD3D;
	LPDIRECT3DDEVICE9       g_pd3dDevice;

	WORD					m_winSizeX;
	WORD					m_winSizeY;
		

	//클릭한 삼각형 출력용
	ObjTriangle* m_tri;
	ObjBall* m_ball;

	CamMain* m_CamMain;
	CamMiniMap* m_CamMap;

	Terrain* g_pTerrain;
	Frustum* g_pFrustum;

	BOOL					g_bHideFrustum = TRUE;	// Frustum을 안그릴 것인가?
	BOOL					g_bLockFrustum = FALSE;	// Frustum을 고정할 것인가?
	BOOL					g_bWireframe = FALSE;	// 와이어프레임으로 그릴것인가?
	BOOL					g_bSelectTriOn = FALSE;	// 선택한 삼각형 빨강색으로 그리기
	BOOL					g_bBallCamera = FALSE;

private:


	VOID _SetBillBoard();
	VOID _BallCameraSetup();

public:
	Engine();
	~Engine();


	HRESULT InitD3D(HWND hWnd);
	HRESULT InitCam();
	HRESULT InitLight();
	HRESULT InitObj();

	
	VOID RenderReady();
	VOID Rendering();

	VOID MeshPickingStart(int x, int y);

	VOID SetFrustum()
	{
		g_bLockFrustum = !g_bLockFrustum;
		g_bHideFrustum = !g_bLockFrustum;
	}
	VOID SetWire() 
	{
		g_bWireframe = !g_bWireframe;
	}
	VOID SetSelectOff()
	{
		g_bSelectTriOn = FALSE;
	}
	VOID SetBallCamera()
	{
		g_bBallCamera = !g_bBallCamera;
	}
	VOID SetCameraMoveZ(float dist);
	VOID SetCameraMoveX(float dist);

	VOID MouseMove(WORD x, WORD y);
	


};

