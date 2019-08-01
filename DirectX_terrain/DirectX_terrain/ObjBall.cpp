#include "ObjBall.h"

ObjBall::ObjBall(LPDIRECT3DDEVICE9 device, Terrain* terrain)
{
	m_device = device;
	m_pTerrain = terrain;

	m_vAcc = D3DXVECTOR3(0, 0, 0);
	m_vSpeed = D3DXVECTOR3(0, 0, 0);


	//�̴ϸ� ����
	m_big = 100;
	SetScale(m_scale);
	
}
ObjBall::~ObjBall()
{
	m_tex->Release();
	m_lpMesh->Release();
}

void ObjBall::DrawMain()
{
	_InitMtrl();
	m_matWorld = m_scale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);		// 0�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���

	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);



	m_device->SetTexture(0, m_tex);
	m_lpMesh->DrawSubset(0);	
	m_device->SetTexture(0, NULL);
}
void ObjBall::DrawMap()
{
	_InitMtrl();
	m_matWorld = m_bigScale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);		// 0�� �ؽ�ó : 0�� �ؽ�ó �ε��� ���

	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);



	m_device->SetTexture(0, m_tex);
	m_lpMesh->DrawSubset(0);
	m_device->SetTexture(0, NULL);
}
HRESULT ObjBall::Create(D3DXVECTOR3 pos, float r)
{
	if (FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/golfBall_Skin01.jpg", &m_tex)))
	{
		printf("dd����\n");
		return S_FALSE;
	}

	m_r = r;
	SetCenter(pos);


	LPD3DXMESH buf_mesh;
	D3DXCreateSphere(m_device, m_r, 20, 20, &buf_mesh, 0);
	
	buf_mesh->CloneMeshFVF(0, BALL_VTX::FVF, m_device, &m_lpMesh);
	buf_mesh->Release();

	BALL_VTX* pVerts;
	if (FAILED(m_lpMesh->LockVertexBuffer(0, (VOID * *)& pVerts)))
	{
		printf("����\n");
			return S_FALSE;
	}
	int numVerts = m_lpMesh->GetNumVertices();

	for (int i = 0; i < numVerts; i++)
	{
		pVerts->t.x = asinf(pVerts->n.x) / D3DX_PI + 0.5F;
		pVerts->t.y = asinf(pVerts->n.y) / D3DX_PI + 0.5F;

		pVerts++;
	}
	m_lpMesh->UnlockVertexBuffer();

	return S_OK;
}

HRESULT ObjBall::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}


void ObjBall::MovePhysical(D3DXVECTOR3 terrain[3])
{
	int m_x = m_pTerrain->GetX() / 2;
	int m_z = m_pTerrain->GetZ() / 2;

	if (-m_x > m_center.x || m_center.x >= m_x ||
		-m_z > m_center.z || m_center.z >= m_z)
	{
		//printf("Map Out!!\n");

		this->Reset();
		//MAP OUT ���ϰ� �༭ ī�޶� ���õ� �ٽ��Ҽ��ְ�
		return;
	}
	
	D3DXVECTOR3 gravity = D3DXVECTOR3(0, -GRAVITY, 0);


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

	bool collision = m_center.y <= height + m_r;

	if (collision)
	{
		m_center.y = height + m_r/1.5f;
		
		//����ٲٰ�
		m_vSpeed += 2 * norm * n;
		m_vSpeed -= 0.001f * m_vSpeed;
		//��ݿ� ���� �ӵ� ����
		float impulse = 2.0f;
		m_vSpeed /= impulse;

		if (D3DXVec3Length(&m_vSpeed) < 0.003f)
		{
			//printf("����");
			m_vSpeed = D3DXVECTOR3(0, 0, 0);
			
			//������� ���ϰ��ֱ�
			return;
		}

		//�������Ϳ� �߷��� ���̰��� �˾Ƴ��� ������ �鿡���� �ڿ������� ��������
		float costh = D3DXVec3Dot(&norm, &gravity) / (D3DXVec3Length(&norm) * D3DXVec3Length(&gravity));
		float sinth = sqrtf( 1 - (costh * costh));
		gravity *= sinth*3;

		
		//m_vSpeed -= 0.01f * m_vSpeed;


	}
	//m_vSpeed -= 0.01f *D3DXVECTOR3(1, 1, 1);
	m_vSpeed += gravity;
	//m_vSpeed -= 0.01f * (m_vSpeed);


	D3DXVECTOR3 after = m_center + m_vSpeed;
	SetCenter(after);

	//����������� ���ϰ� �ֱ�
}


VOID ObjBall::SetBallJump(float energy, D3DXVECTOR3* pView)
{
	D3DXVECTOR3 xzDir = *pView;
	xzDir.y = 0;
	D3DXVec3Normalize(&xzDir, &xzDir);
	D3DXVECTOR3 yDir = D3DXVECTOR3(0, 1, 0);

	D3DXVECTOR3 dir = xzDir + yDir;

	m_vSpeed = dir * energy;


	//ó��ĥ�� �ణ ������ ġ���� - ��ġ�ʴ� ���� ����
	m_center.y += 1;
}