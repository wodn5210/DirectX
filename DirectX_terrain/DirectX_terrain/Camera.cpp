#include "Camera.h"


// 생성자
Camera::Camera(LPDIRECT3DDEVICE9 _pd3dDevice)
{
	g_pd3dDevice = _pd3dDevice;

	D3DXVECTOR3	eye(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3	lookat(0.0f, 0.0f, -1.0f);
	D3DXVECTOR3	up(0.0f, 1.0f, 0.0f);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matBill);
	// 프로젝션 행렬
	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
	// 프러스텀 컬링용 프로젝션 행렬 - 눈에 보기 쉽게
	//D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 200.0f);

	SetView(&eye, &lookat, &up);
}



// 카메라 행렬을 생성하기위한 기본 벡터값들을 설정한다.
D3DXMATRIXA16* Camera::SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat, D3DXVECTOR3* pvUp)
{
	// 월드 행렬 설정
	D3DXMATRIXA16	matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//뷰 행렬 설정
	m_vEye = *pvEye;
	m_vLookat = *pvLookat;
	m_vUp = *pvUp;
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vLookat, &m_vUp);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);

	
	D3DXVec3Normalize(&m_vView, &(m_vLookat - m_vEye));
	D3DXVec3Cross(&m_vCross, &m_vUp, &m_vView);

	D3DXMatrixInverse(&m_matBill, NULL, &m_matView);
	m_matBill._41 = 0.0f;
	m_matBill._42 = 0.0f;
	m_matBill._43 = 0.0f;
	
	return &m_matView;
}

// 카메라 좌표계의 X축으로 angle만큼 회전한다.
D3DXMATRIXA16* Camera::RotateLocalX(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vCross, angle);

	D3DXVECTOR3 vNewDst, vNewUp;
	D3DXVec3TransformCoord(&vNewDst, &m_vView, &matRot);	// view * rot로 새로운 dst vector를 구한다.
//	D3DXVec3Cross( &vNewUp, &vNewDst, &m_vCross );			// cross( dst, x축)으로 up vector를 구한다.
//	D3DXVec3Normalize( &vNewUp, &vNewUp );					// up vector를 unit vector로...
	vNewDst += m_vEye;										// 실제 dst position =  eye Position + dst vector

	return SetView(&m_vEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 Y축으로 angle만큼 회전한다.
D3DXMATRIXA16* Camera::RotateLocalY(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord(&vNewDst, &m_vView, &matRot);	// view * rot로 새로운 dst vector를 구한다.
	vNewDst += m_vEye;										// 실제 dst position =  eye Position + dst vector

	return SetView(&m_vEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 X축방향으로 dist만큼 전진한다.(후진은 -dist를 넣으면 된다.)
D3DXMATRIXA16* Camera::MoveLocalX(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vCross);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	return SetView(&vNewEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 Y축방향으로 dist만큼 전진한다.(후진은 -dist를 넣으면 된다.)
D3DXMATRIXA16* Camera::MoveLocalY(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vUp);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	return SetView(&vNewEye, &vNewDst, &m_vUp);
}

// 카메라 좌표계의 Z축방향으로 dist만큼 전진한다.(후진은 -dist를 넣으면 된다.)
D3DXMATRIXA16* Camera::MoveLocalZ(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vView);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	return SetView(&vNewEye, &vNewDst, &m_vUp);
}

// 월드좌표계의 *pv값의 위치로 카메라를 이동한다.
D3DXMATRIXA16* Camera::MoveTo(D3DXVECTOR3* pv)
{
	D3DXVECTOR3	dv = *pv - m_vEye;
	m_vEye = *pv;
	m_vLookat += dv;
	return SetView(&m_vEye, &m_vLookat, &m_vUp);
}
