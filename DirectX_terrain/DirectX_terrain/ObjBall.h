#pragma once
#include "ObjGraphic.h"
class ObjBall :
	public ObjGraphic
{
private:
	ID3DXMesh* m_pMesh;

public:
	ObjBall(LPDIRECT3DDEVICE9 m_device = NULL);
	~ObjBall();

	void DrawMain();
	void DrawMap();
	HRESULT Create(D3DXVECTOR3 pos, float r);
	HRESULT _InitMtrl();
};

