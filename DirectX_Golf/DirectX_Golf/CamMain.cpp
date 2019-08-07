#include "CamMain.h"

CamMain::CamMain()
{

}

VOID CamMain::Create(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
}

VOID CamMain::SetView(D3DXVECTOR3* pvEye, D3DXVECTOR3* pvLookat)
{
	Cam::SetView(pvEye, pvLookat);

	D3DXMatrixIdentity(&m_matBillBoard);
	m_matBillBoard._11 = m_matView._11;
	m_matBillBoard._13 = m_matView._13;
	m_matBillBoard._31 = m_matView._31;
	m_matBillBoard._33 = m_matView._33;
	D3DXMatrixInverse(&m_matBillBoard, NULL, &m_matBillBoard);

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4, 1.0f, 1.0f, 500.0f);
	m_device->SetTransform(D3DTS_PROJECTION, &m_matProj);

	D3DXVec3Normalize(&m_vView, &(m_vLookat - m_vEye));
	D3DXVec3Cross(&m_vCross, &m_vUp, &m_vView);


}

VOID CamMain::SetPressUp()
{
	MoveLocalZ(0.5f);
}
VOID CamMain::SetPressDown()
{
	MoveLocalZ(-0.5f);
}
VOID CamMain::SetPressLeft()
{
	MoveLocalX(-0.5f);
}
VOID CamMain::SetPressRight()
{
	MoveLocalX(0.5f);
}

// ī�޶� ��ǥ���� X������ angle��ŭ ȸ���Ѵ�.
VOID CamMain::RotateLocalX(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vCross, angle);

	D3DXVECTOR3 vNewDst, vNewUp;
	D3DXVec3TransformCoord(&vNewDst, &m_vView, &matRot);	// view * rot�� ���ο� dst vector�� ���Ѵ�.
	vNewDst += m_vEye;										// ���� dst position =  eye Position + dst vector

	SetView(&m_vEye, &vNewDst);
}

// ī�޶� ��ǥ���� Y������ angle��ŭ ȸ���Ѵ�.
VOID CamMain::RotateLocalY(float angle)
{
	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis(&matRot, &m_vUp, angle);

	D3DXVECTOR3 vNewDst;
	D3DXVec3TransformCoord(&vNewDst, &m_vView, &matRot);	// view * rot�� ���ο� dst vector�� ���Ѵ�.
	vNewDst += m_vEye;		// ���� dst position =  eye Position + dst vector

	SetView(&m_vEye, &vNewDst);
}

// ī�޶� ��ǥ���� X��������� dist��ŭ �����Ѵ�.(������ -dist�� ������ �ȴ�.)
VOID CamMain::MoveLocalX(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vCross);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	SetView(&vNewEye, &vNewDst);
}

// ī�޶� ��ǥ���� Y��������� dist��ŭ �����Ѵ�.(������ -dist�� ������ �ȴ�.)
VOID CamMain::MoveLocalY(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vUp);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	SetView(&vNewEye, &vNewDst);
}

// ī�޶� ��ǥ���� Z��������� dist��ŭ �����Ѵ�.(������ -dist�� ������ �ȴ�.)
VOID CamMain::MoveLocalZ(float dist)
{
	D3DXVECTOR3 vNewEye = m_vEye;
	D3DXVECTOR3 vNewDst = m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove, &m_vView);
	vMove *= dist;
	vNewEye += vMove;
	vNewDst += vMove;

	SetView(&vNewEye, &vNewDst);
}

