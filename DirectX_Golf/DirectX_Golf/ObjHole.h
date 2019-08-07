#pragma once
#include "ObjGraphic.h"
class ObjHole :
	public ObjGraphic
{private:
	LPD3DXMESH m_lpMesh;
	float m_r;

	HRESULT _InitMtrl();

public:
	ObjHole();
	~ObjHole();
	void DrawMain();
	void DrawMap();

	//해당위치에
	HRESULT Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3 pos, float r);

	float GetR() { return m_r; }

};

