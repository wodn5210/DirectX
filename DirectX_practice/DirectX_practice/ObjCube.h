#pragma once
#include "ObjGraphic.h"
class ObjCube :
	public ObjGraphic
{
private:
	LPDIRECT3DVERTEXBUFFER9 g_pVB;
	LPDIRECT3DINDEXBUFFER9	g_pIB;

public:
	ObjCube();
	~ObjCube();

	void DrawObj(LPDIRECT3DDEVICE9 g_pd3dDevice);
	HRESULT Create(LPDIRECT3DDEVICE9 g_pd3dDevice);


	HRESULT InitVB(LPDIRECT3DDEVICE9 g_pd3dDevice);
	HRESULT InitIB(LPDIRECT3DDEVICE9 g_pd3dDevice);
	HRESULT InitMtrl(LPDIRECT3DDEVICE9 g_pd3dDevice);
};

