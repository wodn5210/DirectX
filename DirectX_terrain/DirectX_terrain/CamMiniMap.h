#pragma once
#include "Cam.h"
class CamMiniMap :
	public Cam
{
private:

public:
	CamMiniMap(LPDIRECT3DDEVICE9 m_device = NULL);

	VOID SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp);
	void ResetView() { 
		SetView(&m_vEye, &m_vLookat, &m_vUp);
		m_device->SetViewport(&m_vp);
	};

};

