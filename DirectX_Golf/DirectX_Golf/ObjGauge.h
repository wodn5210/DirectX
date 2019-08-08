#pragma once
#include "ObjGraphic.h"


class ObjGauge :
	public ObjGraphic
{
	LPDIRECT3DDEVICE9 m_device;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB_guage;
	LPDIRECT3DINDEXBUFFER9	m_pIB_edge;

	BAR_VTX m_projVTX[20];
	BAR_VTX	m_vtx[20];

	

public:
	ObjGauge();
	~ObjGauge();

	void DrawMain();
	HRESULT ReadyRender(D3DXMATRIXA16* pmatViewProj, float energy);

	HRESULT Create(LPDIRECT3DDEVICE9 device);


private:
	HRESULT _InitVB();
	HRESULT _InitIB();
	HRESULT _InitMtrlEdge();
	HRESULT _InitMtrlGauge();
};
