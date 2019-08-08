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
	int m_hitCount;

	CamMain* m_pCamMain;
	ID3DXFont* m_pFont;
	ID3DXSprite* m_pSprite;

private:
	VOID _ReadyRender();
	int _BntSearch(int x, int y);

	HRESULT _InitCam();
	HRESULT _InitLight();
	HRESULT _InitObj();

	VOID _RenderText();

public:
	~SceneEnd();

	HRESULT Create(LPDIRECT3DDEVICE9 device, HWND hWnd, int hitCount);

	
	VOID Rendering();
	
	int MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

