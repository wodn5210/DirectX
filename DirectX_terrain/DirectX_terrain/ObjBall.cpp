#include "ObjBall.h"

ObjBall::ObjBall(LPDIRECT3DDEVICE9 device)
{
	m_device = device;

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_translation);
	D3DXMatrixIdentity(&m_rotation);
	D3DXMatrixIdentity(&m_scale);
	m_big = 5;
	SetScale(m_scale);
}
ObjBall::~ObjBall()
{
	if (m_pMesh != NULL)
		m_pMesh->Release();
}

void ObjBall::DrawMain()
{
	_InitMtrl();
	m_matWorld = m_scale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pMesh->DrawSubset(0);
}
void ObjBall::DrawMap()
{
	_InitMtrl();
	m_matWorld = m_bigScale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_pMesh->DrawSubset(0);
}
HRESULT ObjBall::Create(D3DXVECTOR3 pos, float r)
{
	m_center = pos;
	D3DXMatrixIdentity(&m_translation);
	m_translation._41 = m_center.x;
	m_translation._42 = m_center.y;
	m_translation._43 = m_center.z;

	D3DXCreateSphere(m_device, r, 20, 20, &m_pMesh, 0);
	return S_OK;
}

HRESULT ObjBall::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}