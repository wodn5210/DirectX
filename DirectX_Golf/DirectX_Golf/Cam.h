#pragma once
#include <d3dx9.h>

class Cam
{
protected:
	LPDIRECT3DDEVICE9 m_device;

	D3DXVECTOR3		m_vEye;
	D3DXVECTOR3		m_vLookat;
	D3DXVECTOR3		m_vUp;

	D3DXMATRIXA16	m_matView;
	D3DXMATRIXA16	m_matProj;

	D3DVIEWPORT9 m_vp;


public:
	Cam() { m_vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f); }

	VOID SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat)
	{
		D3DXMATRIXA16 world;
		D3DXMatrixIdentity(&world);
		m_device->SetTransform(D3DTS_WORLD, &world);

		//�� ��� ����
		m_vEye = *pvEye;
		m_vLookat = *pvLookat;
		
		D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vLookat, &m_vUp);
		m_device->SetTransform(D3DTS_VIEW, &m_matView);

		//perspective�� ortho�� ������ ������ ���ϴ� ��ӿ��� ó��
	};
	void SetViewport(D3DVIEWPORT9 vp) { m_vp = vp;  m_device->SetViewport(&m_vp); };

	virtual void ResetView() = 0;



	D3DXMATRIXA16* GetViewMatrix() { return &m_matView; }
	D3DXMATRIXA16* GetProjMatrix() { return &m_matProj; }
	D3DXVECTOR3* GetEye() { return &m_vEye; }
	D3DXVECTOR3* GetLookat() { return &m_vLookat; }
	D3DXVECTOR3* GetUp() { return &m_vUp; }
};