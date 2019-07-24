#include "Ray.h"


Ray::Ray()
{
	
}
Ray::~Ray()
{

}

VOID Ray::Create(LPDIRECT3DDEVICE9 g_pd3dDevice, int x, int y)
{
	this->g_pd3dDevice = g_pd3dDevice;

	D3DVIEWPORT9 vp;
	g_pd3dDevice->GetViewport(&vp);

	D3DXMATRIX proj;
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &proj);

	float px = (((2.0f * x) / vp.Width) - 1.0f) / proj(0, 0);
	float py = (((-2.0f * y) / vp.Height) + 1.0f) / proj(1, 1);

	m_origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_direction = D3DXVECTOR3(px, py, 1.0f);

	_TransformRay();
}

VOID Ray::_TransformRay()
{
	D3DXMATRIX view;
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &view);

	D3DXMATRIX viewInverse;
	D3DXMatrixInverse(&viewInverse, 0, &view);

	D3DXVec3TransformCoord(&m_origin, &m_origin, &viewInverse);

	D3DXVec3TransformNormal(&m_direction, &m_direction, &viewInverse);
	D3DXVec3Normalize(&m_direction, &m_direction);

	//printf("pos = (%.3f %.3f %.3f)\n", m_origin.x, m_origin.y, m_origin.z);
	//printf("dir = (%.3f %.3f %.3f)\n", m_direction.x, m_direction.y, m_direction.z);
}
