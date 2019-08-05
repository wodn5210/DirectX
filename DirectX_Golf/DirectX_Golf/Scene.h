#pragma once
#include "define.h"

class Scene
{
protected:
	LPDIRECT3DDEVICE9       m_device;
	HWND m_hWnd;


	WORD m_winSizeX, m_winSizeY;

public:


	HRESULT Create(LPDIRECT3DDEVICE9 device, HWND hWnd)
	{
		m_device = device;
		m_hWnd = hWnd;

		RECT	rc;
		GetClientRect(m_hWnd, &rc);
		m_winSizeX = (WORD)(rc.right - rc.left);
		m_winSizeY = (WORD)(rc.bottom - rc.top);


		return S_OK;
	}

	virtual HRESULT InitCam() = 0;
	virtual HRESULT InitLight() = 0;
	virtual HRESULT InitObj() = 0;
	virtual VOID Rendering() = 0;
	virtual int MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};