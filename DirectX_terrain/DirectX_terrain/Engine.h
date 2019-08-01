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
#include "ObjHole.h"
#include "ObjSkyBox.h"
#include "ObjTree1.h"
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
		

	//Ŭ���� �ﰢ�� ��¿�
	ObjTriangle* m_tri;
	ObjBall* m_ball;
	ObjSkyBox* m_skybox;
	ObjHole* m_hole;
	ObjTree1* m_tree1;

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


	D3DXMATRIXA16 _CalcBillBoard();

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
		if (g_bBallCamera)
		{
			D3DXVECTOR3* ballPos = m_ball->GetCenter();
			m_CamMain->SetBallPosP(ballPos);
			m_CamMain->SetBallView(&D3DXVECTOR3(0, 1, 0));

		}
	}
	VOID SetCameraMoveZ(float dist);
	VOID SetCameraMoveX(float dist);

	VOID MouseMove(WORD x, WORD y);

	VOID SetBallSpdAdd(D3DXVECTOR3 spd) { m_ball->SetBallSpdAdd(spd); }


	//ball �������� ���õ� ī�޶��� ȸ��
	VOID SetBallCamRotateY(float degree);
	VOID SetBallJump(float energy) { m_ball->SetBallJump(energy, m_CamMain->GetvView()); };

	BOOL ISBallInHole();
};

