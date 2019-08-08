#include "CamBall.h"
CamBall::CamBall()
{
	//m_ballEye = D3DXVECTOR3(3.0f, 1.0f, 0.0f);
	m_ballEye = D3DXVECTOR3(5.0f, 2.0f, 0.0f);
	m_ballLookat = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_angleX = 0;
}
/*
m_vEye = ball위치 + m_ballEye
*/
VOID CamBall::SetView()
{
	CamMain::SetView(&(*m_pBallPos + m_ballEye), &(*m_pBallPos + m_ballLookat));
}

// 카메라 좌표계의 Y축으로 angle만큼 회전한다.
VOID CamBall::RotateLocalY(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVec3TransformCoord(&m_ballEye, &m_ballEye, &matRot);	//y축 기준으로 ballEye 회전시킴

	SetView();
}

// 카메라 좌표계의 X축으로 angle만큼 회전한다. - 
VOID CamBall::RotateLocalX(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, CamMain::GetvCross(), angle);

	//ballLookat과 ballcam을 로컬행렬로 변환시킨후 ballcam기준으로 lookat회전
	//다시 원상복귀 시킨후 렌더링

	m_ballLookat -= m_ballEye;
	D3DXVec3TransformCoord(&m_ballLookat, &m_ballLookat, &matRot);	//x축 기준으로 ballEye 회전시킴
	m_ballLookat += m_ballEye;
	SetView();
}

VOID CamBall::SetPressUp()
{
	if (m_angleX >= -0.3f)
	{
		m_angleX -= 0.1f;
		RotateLocalX(-0.1f);
	}
}
VOID CamBall::SetPressDown()
{
	if (m_angleX <= 0.1f)
	{
		m_angleX += 0.1f;
		RotateLocalX(0.1f);
	}
}
VOID CamBall::SetPressLeft()
{
	RotateLocalY(-0.05f);
}
VOID CamBall::SetPressRight()
{
	RotateLocalY(0.05f);
}