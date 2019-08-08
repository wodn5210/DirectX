#include "CamBall.h"
CamBall::CamBall()
{
	//m_ballEye = D3DXVECTOR3(3.0f, 1.0f, 0.0f);
	m_ballEye = D3DXVECTOR3(5.0f, 2.0f, 0.0f);
	m_ballLookat = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_angleX = 0;
}
/*
m_vEye = ball��ġ + m_ballEye
*/
VOID CamBall::SetView()
{
	CamMain::SetView(&(*m_pBallPos + m_ballEye), &(*m_pBallPos + m_ballLookat));
}

// ī�޶� ��ǥ���� Y������ angle��ŭ ȸ���Ѵ�.
VOID CamBall::RotateLocalY(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVec3TransformCoord(&m_ballEye, &m_ballEye, &matRot);	//y�� �������� ballEye ȸ����Ŵ

	SetView();
}

// ī�޶� ��ǥ���� X������ angle��ŭ ȸ���Ѵ�. - 
VOID CamBall::RotateLocalX(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, CamMain::GetvCross(), angle);

	//ballLookat�� ballcam�� ������ķ� ��ȯ��Ų�� ballcam�������� lookatȸ��
	//�ٽ� ���󺹱� ��Ų�� ������

	m_ballLookat -= m_ballEye;
	D3DXVec3TransformCoord(&m_ballLookat, &m_ballLookat, &matRot);	//x�� �������� ballEye ȸ����Ŵ
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