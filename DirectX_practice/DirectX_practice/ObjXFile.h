#pragma once
#include "ObjGraphic.h"


class ObjXFile :
	public ObjGraphic
{
private:
	LPD3DXMESH			g_pMesh = NULL;
	D3DMATERIAL9* g_pMeshMaterials = NULL;
	LPDIRECT3DTEXTURE9* g_pMeshTextures = NULL;
	DWORD				g_dwNumMaterials = 0L;


public:
	ObjXFile();
	~ObjXFile();


	void DrawObj(LPDIRECT3DDEVICE9 g_pd3dDevice);
	HRESULT Create(LPDIRECT3DDEVICE9 g_pd3dDevice,
		D3DXVECTOR3 center = { 0, 0, 0 });
};

