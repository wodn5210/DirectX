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


	//����
	m_weight = 100000;

	//�̴ϸ� ����
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

	//���� ��ġ �浹�ߴ��� �Ǵ�
	//�ߴٸ� �ӵ����� �ٲٰ� return;
	//�浹��������  ���ӵ� ���ϰ� return;
	//���� ��ġ �浹 �Ǵ�
		
	D3DXVECTOR3 pos[3];
	float height = m_pTerrain->GetHeight(&m_center, pos);
	D3DXVECTOR3 norm;
	D3DXVec3Cross(&norm, &(pos[1] - pos[0]), &(pos[2] - pos[1]));
	D3DXVec3Normalize(&norm, &norm);
	float n = D3DXVec3Dot(&norm, &(m_vSpeed * -1));

	if (m_center.y <= height + m_r)
	{

		m_center.y = height + m_r/2.0f;
		
		//����ٲٰ�
		m_vSpeed += 2 * norm * n;

		//��ݿ� ���� �ӵ� ����
		float impulse = 2.0f;
		m_vSpeed /= sqrtf(impulse);


		//�������Ϳ� �߷��� ���̰��� �˾Ƴ��� ������ �鿡���� �ڿ������� ��������
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