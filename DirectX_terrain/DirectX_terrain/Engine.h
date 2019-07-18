#pragma once
#include <d3dx9.h>
#include <string>

#include "Camera.h"
#include "Light.h"
#include "Frustum.h"
#include "Terrian.h"


using namespace std;


const string tex_dir[2] = { "src/tile2.tga", "lightmap.tga" };


class Engine
{
private:
	LPDIRECT3D9             g_pD3D;
	LPDIRECT3DDEVICE9       g_pd3dDevice;

	Camera camera;

public:
	Engine();
	~Engine();

	HRESULT InitD3D(HWND hWnd);
	HRESULT InitView();
	HRESULT InitLight();
	HRESULT InitObj();


	VOID Rendering();
};

