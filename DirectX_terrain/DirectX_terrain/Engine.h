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
#include "Ray.h"

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

	DWORD					g_dwMouseX = 0;			
	DWORD					g_dwMouseY = 0;			

	//테스트용 코드
	LPDIRECT3DVERTEXBUFFER9 g_pVB; 


	Camera* g_pCamera;
	Terrain* g_pTerrain;
	Frustum* g_pFrustum;

	BOOL					g_bHideFrustum = TRUE;	// Frustum을 안그릴 것인가?
	BOOL					g_bLockFrustum = FALSE;	// Frustum을 고정할 것인가?
	BOOL					g_bWireframe = FALSE;	// 와이어프레임으로 그릴것인가?


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

	VOID MeshPickingStart(int x, int y);

	VOID SetFrustum()
	{
		g_bLockFrustum = !g_bLockFrustum;
		g_bHideFrustum = !g_bLockFrustum;
	}
	VOID SetWire() {
		g_bWireframe = !g_bWireframe;
	}
};

