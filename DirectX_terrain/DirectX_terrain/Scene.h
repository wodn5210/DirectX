#pragma once

#include <d3dx9.h>


class Scene
{
	LPDIRECT3DDEVICE9       m_device;

public:
	Scene();
	~Scene();

	virtual HRESULT Create(LPDIRECT3DDEVICE9 device) = 0;

	HRESULT InitCam();
	HRESULT InitLight();
	HRESULT InitObj();
	virtual VOID Rendering() = 0;
	virtual LRESULT MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};