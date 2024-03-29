#pragma once
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include "ObjTriangle.h"
#include "ObjCube.h"
#include "ObjXFile.h"

class Engine
{
private:
	LPDIRECT3D9             g_pD3D;
	LPDIRECT3DDEVICE9       g_pd3dDevice;
	D3DXVECTOR3 eye;

public:
	Engine();
	~Engine();

	HRESULT InitD3D(HWND hWnd);
	HRESULT InitObj();
	VOID Render();
	VOID InitLight();
	VOID InitView();

	ObjTriangle test[5];
	ObjCube testCube[5];
	ObjXFile testTiger;
};

