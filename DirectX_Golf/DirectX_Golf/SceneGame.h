#pragma once
#include <typeinfo>
#include <time.h>

#include "define.h"

#include "Scene.h"
#include "CamBall.h"
#include "CamMain.h"
#include "CamMiniMap.h"
#include "ObjFrustum.h"
#include "ObjTerrain.h"
#include "ObjBall.h"
#include "ObjGauge.h"
#include "ObjSkyBox.h"
#include "ObjHole.h"
#include "ObjTreeTexture.h"
#include "ObjTreeObjFile.h"
#include "ObjTriangle.h"

class SceneGame :
	public Scene
{
private:

	CamMain* m_pCamMain;
	CamBall* m_pCamBall;
	CamMiniMap* m_pCamMap;

	//MainView¿Í BallView ¹¹·Î ·»´õ¸µÇÒÁö
	CamMain* m_pNowCam;

	ObjFrustum* m_pFrustum;
	ObjTerrain* m_pTerrain;
	ObjBall* m_pBall;
	ObjGauge* m_pBar;
	ObjSkyBox* m_pSkybox;
	ObjHole* m_pHole;
	ObjTreeTexture* m_pTreeTex;
	ObjTreeObjFile* m_pTreeObj;
	ObjTriangle* m_pTri;

	BOOL m_bRenderFrustum;
	BOOL m_bBallCam;
	BOOL m_bWireRender;
	ObjBall::STATE m_ballState;
	BOOL m_bSpaceBar;
	BOOL m_bEndGame;
	BOOL m_bSelectMeshRender;
	float m_BallEnergy;
	time_t m_sT = 0, m_eT = 0;

	int m_hitCount;

private:
	VOID _ReadyRender();

	HRESULT _InitCam();
	HRESULT _InitLight();
	HRESULT _InitObj();
	BOOL _ISBallInHole();

	VOID _MouseMove(WORD x, WORD y);
	VOID _SetEnergyView();
	VOID _MeshPicking(int x, int y);

public:
	SceneGame();
	~SceneGame();

	HRESULT Create(LPDIRECT3DDEVICE9 device, HWND hWnd);
	VOID Rendering();
	int MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

