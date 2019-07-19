#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <string>

#include "Camera.h"
#include "Light.h"
#include "Frustum.h"
#include "Terrain.h"


using namespace std;


const string tex_dir[2] = { "src/tile2.tga", "lightmap.tga" };

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;     
	D3DXVECTOR3 normal;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)

class Engine
{
private:
	HWND					g_hwnd;
	LPDIRECT3D9             g_pD3D;
	LPDIRECT3DDEVICE9       g_pd3dDevice;

	Camera*					g_pCamera;

	DWORD					g_dwMouseX = 0;			
	DWORD					g_dwMouseY = 0;			

	//테스트용 코드
	LPDIRECT3DVERTEXBUFFER9 g_pVB; 

	Terrain* g_pTerrain;

public:
	Engine();
	~Engine();

	HRESULT InitD3D(HWND hWnd);
	HRESULT InitView();
	HRESULT InitLight();
	HRESULT InitObj();

	VOID _MouseEvent();
	VOID _KeyEvent();

	VOID _SetBillBoard();

	VOID RenderReady();
	VOID Rendering();
};

