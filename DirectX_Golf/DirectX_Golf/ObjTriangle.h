#pragma once
#include "ObjGraphic.h"
#include "define.h"
#include "FIleReadObj.h"


class ObjTriangle : public ObjGraphic
{
	
private:
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;


public:
	ObjTriangle(LPDIRECT3DDEVICE9 m_device = NULL);
	~ObjTriangle();

	void DrawMain();
	void DrawMap();
	HRESULT Create(D3DXVECTOR3 pos[3]);
	HRESULT Create();

	BOOL Update(D3DXVECTOR3 pos, D3DXVECTOR3 dir, D3DXVECTOR3 cross);

private:
	HRESULT _InitVB(D3DXVECTOR3 pos[3]);
	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitMtrl();



	


};

