#include "ObjBall.h"

ObjBall::ObjBall(LPDIRECT3DDEVICE9 device, Terrain* terrain)
{
	m_device = device;
	m_pTerrain = terrain;

	m_vAcc = D3DXVECTOR3(0, 0, 0);
	m_vSpeed = D3DXVECTOR3(0, 0, 0);


	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_translation);
	D3DXMatrixIdentity(&m_rotation);
	D3DXMatrixIdentity(&m_scale);


	//무게
	m_weight = 100000;

	//미니맵 배율
	m_big = 300;
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
	m_r = r;
	D3DXMatrixIdentity(&m_translation);
	m_translation._41 = m_center.x;
	m_translation._42 = m_center.y;
	m_translation._43 = m_center.z;

	D3DXCreateSphere(m_device, m_r, 20, 20, &m_pMesh, 0);
	return S_OK;
}

HRESULT ObjBall::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}

void ObjBall::MovePhysical(D3DXVECTOR3 terrain[3])
{
	if (0 > m_center.x || m_center.x >= m_pTerrain->GetX() || 
		0 > m_center.z || m_center.z >= m_pTerrain->GetZ())
	{
		//printf("Map Out!!\n");
		this->Reset();
		return;
	}
	
	D3DXVECTOR3 gravity = D3DXVECTOR3(0, -GRAVITY, 0);

	/*if (D3DXVec3Length(&m_vSpeed) < 0.001)
		return;*/

	//현재 위치 충돌했는지 판단
	//했다면 속도방향 바꾸고 return;
	//충돌안했으면  가속도 더하고 return;
	//현재 위치 충돌 판단
		
	D3DXVECTOR3 pos[3];
	float height = m_pTerrain->GetHeight(&m_center, pos);
	D3DXVECTOR3 norm;
	D3DXVec3Cross(&norm, &(pos[1] - pos[0]), &(pos[2] - pos[1]));
	D3DXVec3Normalize(&norm, &norm);
	float n = D3DXVec3Dot(&norm, &(m_vSpeed * -1));

	if (m_center.y <= height + m_r)
	{

		m_center.y = height + m_r/2.0f;
		
		//방향바꾸고
		m_vSpeed += 2 * norm * n;

		//충격에 의한 속도 감소
		float impulse = 2.0f;
		m_vSpeed /= sqrtf(impulse);


		//법선벡터와 중력의 사이각을 알아내서 기울어진 면에서도 자연스럽게 떨어지게
		float costh = D3DXVec3Dot(&norm, &gravity) / (D3DXVec3Length(&norm) * D3DXVec3Length(&gravity));
		float sinth = sqrtf( 1 - (costh * costh));
		gravity *= sinth*3;

		
		//m_vSpeed -= 0.01f * m_vSpeed;

		
	}

	m_vSpeed += gravity + m_vAcc;
	//m_vSpeed -= 0.1f * (m_vSpeed + gravity);


	D3DXVECTOR3 after = m_center + m_vSpeed;
	SetCenter(after);

	
}