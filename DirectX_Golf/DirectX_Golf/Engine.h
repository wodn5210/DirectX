#pragma once
#include "define.h"
#include "SceneStart.h"
#include "SceneGame.h"
#include "SceneEnd.h"


class Engine
{
private:
	LPDIRECT3D9             m_pD3D; 
	LPDIRECT3DDEVICE9       m_device;
	HWND m_hwnd;

	WORD m_winSizeX, m_winSizeY;


	enum PHASE {START, GAME, END};
	PHASE m_nowPhase;
	Scene* m_pNowScene;

	SceneStart* m_pSceneStart;
	SceneGame* m_pSceneGame;
	SceneEnd* m_pSceneEnd;



private:
	VOID _InitScene();




public:
	Engine();
	~Engine();

	HRESULT Init(HWND hWnd);
	VOID MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



	VOID Rendering();
};

