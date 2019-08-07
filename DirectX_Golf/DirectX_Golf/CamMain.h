#pragma once
#include "Cam.h"

class CamMain :
	public Cam
{
private:
	D3DXVECTOR3		m_vView;
	D3DXVECTOR3		m_vCross;
	D3DXMATRIXA16	m_matBillBoard;

public:
	CamMain();

	VOID Create(LPDIRECT3DDEVICE9 device);

	VOID SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat);

	virtual VOID SetPressUp();
	virtual VOID SetPressDown();
	virtual VOID SetPressLeft();
	virtual VOID SetPressRight();

	//XorY축으로 회전
	VOID RotateLocalX(float angle);
	VOID RotateLocalY(float angle);

	//dist만큼 각 방향으로 이동
	VOID MoveLocalX(float dist);
	VOID MoveLocalY(float dist);
	VOID MoveLocalZ(float dist);

	void ResetView() 
	{ 
		SetView(&m_vEye, &m_vLookat); 
		m_device->SetViewport(&m_vp);
	};

	D3DXVECTOR3* GetvView() { return &m_vView; }
	D3DXVECTOR3* GetvCross() { return &m_vCross; }
	D3DXMATRIXA16* GetBillBoard() { return &m_matBillBoard; }

};

