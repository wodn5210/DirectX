#include "CamMiniMap.h"



CamMiniMap::CamMiniMap(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	m_vUp = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	//if(device == NULL)
	//		¿¡·¯
}

VOID CamMiniMap::SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat)
{
	Cam::SetView(pvEye, pvLookat);
	D3DXMatrixOrthoLH(&m_matProj, 140.0f, 140.0f, 1.0f, 1000.0f);
	m_device->SetTransform(D3DTS_PROJECTION, &m_matProj);
}