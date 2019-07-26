#pragma once
#include <d3dx9.h>
#include <stdio.h>
/*
메쉬 픽하기 위한 광선
*/

class Ray
{
private:
	LPDIRECT3DDEVICE9       g_pd3dDevice;

	D3DXVECTOR3 m_origin;
	D3DXVECTOR3 m_direction;

public:
	Ray();
	~Ray();

	VOID Create(LPDIRECT3DDEVICE9 g_pd3dDevice, int x = 0, int y = 0);
	VOID Create(LPDIRECT3DDEVICE9 g_pd3dDevice, D3DXVECTOR3* center, D3DXVECTOR3* view);
	D3DXVECTOR3* GetPos() { return &m_origin; }
	D3DXVECTOR3* GetDir() { return &m_direction; }
private:
	VOID _TransformRay();
};

