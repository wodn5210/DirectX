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
#include "ObjTree2.h"
#include "ObjProgressbar.h"
#include "ObjFrustum.h"
#include "ObjTerrain.h"
#include "Ray.h"


using namespace std;


const string tex_dir[2] = { "src/tile2.tga", "lightmap.tga" };

class Engine
{
private:
	D3DXVECTOR3 temp[2];
	LPDIRECT3DVERTEXBUFFER9 m_tempVB;

	HWND					m_hwnd;
	LPDIRECT3D9             m_pD3D;
	LPDIRECT3DDEVICE9       m_device;

	WORD					m_winSizeX;
	WORD					m_winSizeY;


	//Ŭ���� �ﰢ�� ��¿�
	ObjTriangle* m_pTri;
	ObjBall* m_pBall;
	ObjSkyBox* m_pSkybox;
	ObjHole* m_pHole;
	ObjTree1* m_pTree1;
	ObjTree2* m_pTree2;
	ObjProgressbar* m_pBar;
	ObjTerrain* m_pTerrain;
	ObjFrustum* m_pFrustum;

	CamMain* m_pCamMain;
	CamMiniMap* m_pCamMap;



	BOOL					m_bHideFrustum = TRUE;	// Frustum�� �ȱ׸� ���ΰ�?
	BOOL					m_bLockFrustum = FALSE;	// Frustum�� ������ ���ΰ�?
	BOOL					m_bWireframe = FALSE;	// ���̾����������� �׸����ΰ�?
	BOOL					m_bSelectTriOn = FALSE;	// ������ �ﰢ�� ���������� �׸���
	BOOL					m_bBallCamera = FALSE;
	BOOL					m_bBallJump = FALSE;
	BOOL					m_bBallEnergyView = FALSE;
	float					m_BallEnergy = 0;
	int						m_BallState = 1;

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
		m_bLockFrustum = !m_bLockFrustum;
		m_bHideFrustum = !m_bLockFrustum;
	}
	VOID SetWire()
	{
		m_bWireframe = !m_bWireframe;
	}
	VOID SetSelectOff()
	{
		m_bSelectTriOn = FALSE;
	}
	VOID SetBallCamera()
	{
		m_bBallCamera = !m_bBallCamera;
		if (m_bBallCamera)
		{
			D3DXVECTOR3* ballPos = m_pBall->GetCenter();
			m_pCamMain->SetBallPosP(ballPos);
			m_pCamMain->SetBallView(&D3DXVECTOR3(0, 1, 0));

		}
	}
	VOID SetCameraMoveZ(float dist);
	VOID SetCameraMoveX(float dist);

	VOID MouseMove(WORD x, WORD y);

	VOID SetBallSpdAdd(D3DXVECTOR3 spd) { m_pBall->SetBallSpdAdd(spd); }


	//ball �������� ���õ� ī�޶��� ȸ��
	VOID SetBallCamRotateY(float degree);
	VOID SetBallCamRotateX(float degree);
	VOID SetBallJump(float energy);
	VOID SetEnergyView(float energy)
	{
		if (energy >= 0.7 || m_bBallJump)
			return;
		m_bBallEnergyView = TRUE;
		m_BallEnergy = energy;
	}
	;

	BOOL ISBallInHole();

};

