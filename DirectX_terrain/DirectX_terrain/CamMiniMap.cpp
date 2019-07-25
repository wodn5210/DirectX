#include "CamMiniMap.h"



CamMiniMap::CamMiniMap(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	//if(device == NULL)
	//		¿¡·¯
}

VOID CamMiniMap::SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp)
{
	Cam::SetView(pvEye, pvLookat, pvUp);
	D3DXMatrixOrthoLH(&m_matProj, 140.0f, 140.0f, 1.0f, 1000.0f);
	m_device->SetTransform(D3DTS_PROJECTION, &m_matProj);
}