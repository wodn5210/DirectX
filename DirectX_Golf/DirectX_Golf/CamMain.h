#pragma once
#include "Cam.h"

class CamMain :
	public Cam
{
private:
	D3DXVECTOR3		m_vView;
	D3DXVECTOR3		m_vCross;



	D3DXVECTOR3 m_ballEye;
	D3DXVECTOR3* m_pBallPos;
	D3DXVECTOR3 m_ballLookat;
	

public:
	CamMain();

	VOID Create(LPDIRECT3DDEVICE9 device);

	VOID SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp);

	//XorY축으로 회전
	VOID RotateLocalX(float angle);
	VOID RotateLocalY(float angle);

	//pv위치로 이동
	VOID MoveTo(D3DXVECTOR3* pv);

	//dist만큼 각 방향으로 이동
	VOID MoveLocalX(float dist);
	VOID MoveLocalY(float dist);
	VOID MoveLocalZ(float dist);

	void ResetView() 
	{ 
		SetView(&m_vEye, &m_vLookat, &m_vUp); 
		m_device->SetViewport(&m_vp);
	};

	void ResetBallView()
	{
		SetView(&(m_ballEye + *m_pBallPos), &(*m_pBallPos + m_ballLookat), &m_vUp);
		m_device->SetViewport(&m_vp);
		//m_ballEye = D3DXVECTOR3(0, 1, -2);

	};

	D3DXVECTOR3* GetvView() { return &m_vView; }


	VOID SetBallView(D3DXVECTOR3* pvUp);
	VOID SetBallViewRotateY(float angle);
	VOID SetBallViewRotateX(float angle);
	VOID SetBallPosP(D3DXVECTOR3* pBallPos) { m_pBallPos = pBallPos; };
	
};

