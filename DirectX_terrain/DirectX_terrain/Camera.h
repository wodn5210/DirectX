#pragma once
#include <d3dx9.h>

class Camera
{
	LPDIRECT3DDEVICE9 g_pd3dDevice;

	D3DXVECTOR3		m_vEye;			
	D3DXVECTOR3		m_vLookat;		
	D3DXVECTOR3		m_vUp;			

	D3DXVECTOR3		m_vView;		
	D3DXVECTOR3		m_vCross;		

	D3DXMATRIXA16	m_matView;		
	D3DXMATRIXA16	m_matBill;
	D3DXMATRIXA16	m_matProj;

public:
	
	Camera(LPDIRECT3DDEVICE9 _pd3dDevice = NULL);

	D3DXMATRIXA16*	SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp);
	void			SetEye(D3DXVECTOR3* pv) { m_vEye = *pv; }
	void			SetLookat(D3DXVECTOR3* pv) { m_vLookat = *pv; }
	void			SetUp(D3DXVECTOR3* pv) { m_vUp = *pv; }
	
	D3DXMATRIXA16* GetViewMatrix() { return &m_matView; }
	D3DXMATRIXA16* GetBillMatrix() { return &m_matBill; }
	D3DXMATRIXA16* GetProjMatrix() { return &m_matProj; }
	D3DXVECTOR3* GetEye() { return &m_vEye; }
	D3DXVECTOR3* GetLookat() { return &m_vLookat; }
	D3DXVECTOR3* GetUp() { return &m_vUp; }


	//�� �ٽ� ����
	void			Flush() { SetView(&m_vEye, &m_vLookat, &m_vUp); }

	//XorY������ ȸ��
	D3DXMATRIXA16* RotateLocalX(float angle);
	D3DXMATRIXA16* RotateLocalY(float angle);

	//pv��ġ�� �̵�
	D3DXMATRIXA16* MoveTo(D3DXVECTOR3* pv);

	//dist��ŭ �� �������� �̵�
	D3DXMATRIXA16* MoveLocalX(float dist);
	D3DXMATRIXA16* MoveLocalY(float dist);
	D3DXMATRIXA16* MoveLocalZ(float dist);
};

