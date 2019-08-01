#include "ObjHole.h"

ObjHole::ObjHole(LPDIRECT3DDEVICE9 device)
{
	m_device = device;

	m_big = 50;
	SetScale(m_scale);

	D3DXMatrixRotationX(&m_rotation, D3DX_PI / 2);
}

ObjHole::~ObjHole()
{
	m_lpMesh->Release();
}
HRESULT ObjHole::Create(D3DXVECTOR3 pos, float r)
{
	m_r = r;
	SetCenter(pos);
	D3DXCreateCylinder(m_device, m_r, m_r, m_r*2, 10, 10, &m_lpMesh, NULL);
	return S_OK;
}

HRESULT ObjHole::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}

void ObjHole::DrawMain()
{
	_InitMtrl();
	m_matWorld = m_rotation*m_scale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_lpMesh->DrawSubset(0);
}
void ObjHole::DrawMap()
{
	_InitMtrl();
	m_matWorld = m_rotation*m_bigScale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_lpMesh->DrawSubset(0);
}
