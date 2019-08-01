#include "CamMain.h"

CamMain::CamMain(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	m_ballEye = D3DXVECTOR3(3, 1, 0);
	
}

VOID CamMain::SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp)
{
	Cam::SetView(pvEye, pvLookat, pvUp);

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	m_device->SetTransform(D3DTS_PROJECTION, &m_matProj);

	D3DXVec3Normalize(&m_vView, &(m_vLookat - m_vEye));
	D3DXVec3Cross(&m_vCross, &m_vUp, &m_vView);


}

// 카메라 좌표계의 X축으로 angle만큼 회전한다.
VOID CamMain::RotateLocalX(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vCross, angle);

	D3DXVECTOR3 vNewDst, vNewUp;
	D3DXVec3TransformCoord(&vNewDst, &m_vView, &matRot);	// view * rot로 새로운 dst vector를 구한다.
	vNewDst += m_vEye;										// 실제 dst position =  eye Position + dst vector

	SetView(&m_vEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 Y축으로 angle만큼 회전한다.
VOID CamMain::RotateLocalY(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord(&vNewDst, &m_vView, &matRot);	// view * rot로 새로운 dst vector를 구한다.
	vNewDst += m_vEye;		// 실제 dst position =  eye Position + dst vector

	SetView(&m_vEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 X축방향으로 dist만큼 전진한다.(후진은 -dist를 넣으면 된다.)
VOID CamMain::MoveLocalX(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vCross);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	SetView(&vNewEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 Y축방향으로 dist만큼 전진한다.(후진은 -dist를 넣으면 된다.)
VOID CamMain::MoveLocalY(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vUp);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	SetView(&vNewEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 Z축방향으로 dist만큼 전진한다.(후진은 -dist를 넣으면 된다.)
VOID CamMain::MoveLocalZ(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vView);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	SetView(&vNewEye, &vNewDst, &m_vUp);
}

// 월드좌표계의 *pv값의 위치로 카메라를 이동한다.
VOID CamMain::MoveTo(D3DXVECTOR3* pv)
{
	D3DXVECTOR3	dv = *pv - m_vEye;
	m_vEye = *pv;
	m_vLookat += dv;

	SetView(&m_vEye, &m_vLookat, &m_vUp);
}


/*
m_vEye = ball위치 + m_ballEye
*/
VOID CamMain::SetBallView(D3DXVECTOR3* pvUp)
{

	SetView(&(*m_pBallPos + m_ballEye), m_pBallPos, pvUp);
}

// 카메라 좌표계의 Y축으로 angle만큼 회전한다.
VOID CamMain::SetBallViewRotateY(float angle) 
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord(&m_ballEye, &m_ballEye, &matRot);	//y축 기준으로 ballEye 회전시킴

	SetView(&(*m_pBallPos + m_ballEye), m_pBallPos, &D3DXVECTOR3(0, 1, 0));
}