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
#include "ObjSkyBox.h"
#include "Frustum.h"
#include "Terrain.h"
#include "Ray.h"


using namespace std;


const string tex_dir[2] = { "src/tile2.tga", "lightmap.tga" };

struct CUSTOM
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};
#define _FVF (D3DFVF_XYZ | D3DFVF_NORMAL)

class Engine
{
private:
	D3DXVECTOR3 temp[2];
	LPDIRECT3DVERTEXBUFFER9 m_tempVB;

	HWND					g_hwnd;
	LPDIRECT3D9             g_pD3D;
	LPDIRECT3DDEVICE9       g_pd3dDevice;

	WORD					m_winSizeX;
	WORD					m_winSizeY;
		

	//클릭한 삼각형 출력용
	ObjTriangle* m_tri;
	ObjBall* m_ball;
	ObjSkyBox* m_skybox;


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
	VOID DrawDirection();


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
	
	VOID SetBallAccAdd(D3DXVECTOR3 acc) { m_ball->SetBallAccAdd(acc); }
	VOID SetBallSpdAdd(D3DXVECTOR3 spd) { m_ball->SetBallSpdAdd(spd); }
	

};

