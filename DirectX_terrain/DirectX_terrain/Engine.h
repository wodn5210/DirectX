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
		

	//Ŭ���� �ﰢ�� ��¿�
	ObjTriangle* m_tri;
	ObjBall* m_ball;

	CamMain* m_CamMain;
	CamMiniMap* m_CamMap;

	Terrain* g_pTerrain;
	Frustum* g_pFrustum;

	BOOL					g_bHideFrustum = TRUE;	// Frustum�� �ȱ׸� ���ΰ�?
	BOOL					g_bLockFrustum = FALSE;	// Frustum�� ������ ���ΰ�?
	BOOL					g_bWireframe = FALSE;	// ���̾����������� �׸����ΰ�?
	BOOL					g_bSelectTriOn = FALSE;	// ������ �ﰢ�� ���������� �׸���
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

