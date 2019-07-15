#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)
struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct MYINDEX
{
	WORD _0, _1, _2;
};

class Engine
{
private:
	LPDIRECT3D9             g_pD3D;
	LPDIRECT3DDEVICE9       g_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
	LPDIRECT3DINDEXBUFFER9	g_pIB = NULL;

public:
	Engine();
	~Engine();

	HRESULT InitD3D(HWND hWnd);
	HRESULT InitVB();
	HRESULT InitIB();
	VOID Render();
	HRESULT InitLight();
	HRESULT InitMaterial();
	VOID SetupMatrices();
};

