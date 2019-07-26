#pragma once
#include "Cam.h"

class CamMain :
	public Cam
{
private:
	D3DXVECTOR3		m_vView;
	D3DXVECTOR3		m_vCross;

	D3DXMATRIXA16	m_matBill;


public:
	CamMain(LPDIRECT3DDEVICE9 m_device);

	VOID SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp);

	//XorY������ ȸ��
	VOID RotateLocalX(float angle);
	VOID RotateLocalY(float angle);

	//pv��ġ�� �̵�
	VOID MoveTo(D3DXVECTOR3* pv);

	//dist��ŭ �� �������� �̵�
	VOID MoveLocalX(float dist);
	VOID MoveLocalY(float dist);
	VOID MoveLocalZ(float dist);

	void ResetView() { 
		SetView(&m_vEye, &m_vLookat, &m_vUp); 
		m_device->SetViewport(&m_vp);
	};

	D3DXMATRIXA16* GetBillMatrix() { return &m_matBill; }
	D3DXVECTOR3* GetvView() { return &m_vView; }
};

