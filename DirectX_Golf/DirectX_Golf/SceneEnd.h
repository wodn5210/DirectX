#pragma once
#include "define.h"
#include "Scene.h"
#include "ObjButton.h"
#include "CamMain.h"

class SceneEnd :
	public Scene
{
private:
	ObjButton* m_pReStartBnt;
	ObjButton* m_pEndBnt;
	D3DXVECTOR2 pos_restart[2] = { {-0.5f, 0.7f}, {0.5f, 0.3f} };
	D3DXVECTOR2 pos_end[2] = { {-0.5f, -0.3f}, {0.5f, -0.7f} };

	CamMain* m_pCamMain;

private:
	VOID _readyRender();
	int _bntSearch(int x, int y);


public:
	~SceneEnd();

	HRESULT Create(LPDIRECT3DDEVICE9 device, HWND hWnd);

	HRESULT InitCam();
	HRESULT InitLight();
	HRESULT InitObj();
	VOID Rendering();
	int MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

