#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

class Engine
{
public:

private:
	D3DXVECTOR3 eye;
	LPDIRECT3D9             _pD3D;
	LPDIRECT3DDEVICE9       _pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 _pVB;
	LPDIRECT3DINDEXBUFFER9	_pIB;






public:
	Engine() {
		eye = D3DXVECTOR3(100.0f, 250.0f, -400.0f);
		_pD3D = NULL, _pd3dDevice = NULL, _pVB = NULL, _pIB = NULL;
	};
	~Engine() {
		if (_pIB != NULL)
			_pIB->Release();
		if (_pVB != NULL)
			_pVB->Release();
		if (_pd3dDevice != NULL)
			_pd3dDevice->Release();
		if (_pD3D != NULL)
			_pD3D->Release();
	};
	HRESULT InitD3D(HWND hWnd);
	HRESULT InitVB();
	HRESULT InitIB();
	VOID Render();

private:
	HRESULT _InitGeometry();
	VOID _SetupMatrices();
	HRESULT _InitLight();
	
	
};

