#pragma once
#include "Cam.h"
class CamMiniMap :
	public Cam
{
private:

public:
	CamMiniMap(LPDIRECT3DDEVICE9 m_device = NULL);

	VOID SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat);
	void ResetView() { 
		SetView(&m_vEye, &m_vLookat);
		m_device->SetViewport(&m_vp);
	};

};

