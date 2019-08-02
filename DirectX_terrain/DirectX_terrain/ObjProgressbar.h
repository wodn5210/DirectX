#pragma once
#include "ObjGraphic.h"
#include "define.h"

class ObjProgressbar :
	public ObjGraphic
{
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB_guage;
	LPDIRECT3DINDEXBUFFER9	m_pIB_edge;

	BAR_VTX m_projVTX[20];
	BAR_VTX	m_vtx[20];

	

public:
	ObjProgressbar(LPDIRECT3DDEVICE9 device = NULL);
	~ObjProgressbar();

	void DrawMain(D3DXMATRIXA16* pmatViewProj, float energy);

	HRESULT Create();


private:
	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitMtrlEdge();
	HRESULT _InitMtrlGauge();
};
