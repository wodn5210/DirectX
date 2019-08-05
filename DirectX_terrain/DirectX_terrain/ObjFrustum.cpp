#include "ObjFrustum.h"


ObjFrustum::ObjFrustum(LPDIRECT3DDEVICE9 pDev)
{
	m_device = pDev;

	// ������ı��� ��ġ�� ��� 3���� ������ǥ�� ���� (-1,-1,0) ~ (1,1,1)������ ������ �ٲ��.
	// m_projVTX�� �� ���������� ��谪�� �־�д�.
	m_projVTX[0].x = -1.0f;		m_projVTX[0].y = -1.0f;		m_projVTX[0].z = 0.0f;
	m_projVTX[1].x = 1.0f;		m_projVTX[1].y = -1.0f;		m_projVTX[1].z = 0.0f;
	m_projVTX[2].x = 1.0f;		m_projVTX[2].y = -1.0f;		m_projVTX[2].z = 1.0f;
	m_projVTX[3].x = -1.0f;		m_projVTX[3].y = -1.0f;		m_projVTX[3].z = 1.0f;
	m_projVTX[4].x = -1.0f;		m_projVTX[4].y = 1.0f;		m_projVTX[4].z = 0.0f;
	m_projVTX[5].x = 1.0f;		m_projVTX[5].y = 1.0f;		m_projVTX[5].z = 0.0f;
	m_projVTX[6].x = 1.0f;		m_projVTX[6].y = 1.0f;		m_projVTX[6].z = 1.0f;
	m_projVTX[7].x = -1.0f;		m_projVTX[7].y = 1.0f;		m_projVTX[7].z = 1.0f;

	//InitIB();

	ZeroMemory(m_plane, sizeof(m_plane[0]) * 6);

}


HRESULT ObjFrustum::Init()
{
	if (FAILED(m_device->CreateVertexBuffer(8 * sizeof(D3DXVECTOR3),
		0, ObjFrustum::_FVF, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	FRS_IDX indices[] =
	{
	{0, 1, 2}, {0, 2, 3},
	{4, 6, 5}, {4, 7, 6 },
	{0, 3, 7}, {0, 7, 4},
	{1, 5, 6}, {1, 6, 2},
	{3, 6, 2}, {3, 7, 6},
	{0, 4, 5}, {0, 5, 1},
	};


	if (FAILED(m_device->CreateIndexBuffer(12 * sizeof(FRS_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;

	if (FAILED(g_pIB->Lock(0, sizeof(indices), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, indices, sizeof(indices));
	g_pIB->Unlock();
	return S_OK;
}




HRESULT ObjFrustum::FillVB()
{
	// Fill the vertex buffer.
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(m_vtx), (void**)& pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, m_vtx, sizeof(m_vtx));

	g_pVB->Unlock();

	return S_OK;
}

// ī�޶�(view) * ��������(projection)����� �Է¹޾� 6���� ����� �����.
BOOL ObjFrustum::Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3* pos)
{

	m_vPos = *pos;	//ī�޶� ��ǥ ����

	int				i;
	D3DXMATRIXA16	matInv;

	// view * proj�� ������� ���Ѵ�.
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj);

	// Vertex_final = Vertex_local * Matrix_world * Matrix_view * Matrix_Proj �⺻��ȯ���� ����������ǥ ����
	// (Vertex_world = Vertex_local * Matrix_world)

	// Vertex_final = Vertex_world * ( Matrix_view * Matrix_Proj )
	// Vertex_World = Vertex_final * �����( Matrix_view * Matrix_Proj )^-1
	// Vertex_World�� �ᱹ �������� ������	
	for (i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vtx[i], &m_projVTX[i], &matInv);



	// ����� ������ǥ�� �������� ����� �����
	// ���Ͱ� �������� ���ʿ��� �ٱ������� ������ �����̴�.
	D3DXPlaneFromPoints(&m_plane[0], m_vtx + 4, m_vtx + 7, m_vtx + 6);		// �� ���(top)
	D3DXPlaneFromPoints(&m_plane[1], m_vtx, m_vtx + 1, m_vtx + 2);		// �� ���(bottom)
	D3DXPlaneFromPoints(&m_plane[2], m_vtx, m_vtx + 4, m_vtx + 5);		// �� ���(near)
	D3DXPlaneFromPoints(&m_plane[3], m_vtx + 2, m_vtx + 6, m_vtx + 7);	// �� ���(far)
	D3DXPlaneFromPoints(&m_plane[4], m_vtx, m_vtx + 3, m_vtx + 7);		// �� ���(left)
	D3DXPlaneFromPoints(&m_plane[5], m_vtx + 1, m_vtx + 5, m_vtx + 6);	// �� ���(right)

	//IB ó�� �ѹ����ϸ� ���� �ñ� -> �ȴ�
	FillVB();
	//InitIB();
	return TRUE;
}

//�������� �ȿ� ������ True
BOOL	ObjFrustum::IsIn(D3DXVECTOR3* pv)
{
	for (int i = 0; i < 6; i++)
	{
		//�������� ���� ����pv�� �̿��ؼ� ���� or �ܺ� �Ǵ�
		float dist = D3DXPlaneDotCoord(&m_plane[i], pv);

		//����� ���Ͱ� �ٱ������� ������ �Ǿ��ֱ⶧���� ����� �ܺ���
		if (dist > PLANE_EPSILON)
			return FALSE;
	}
	return TRUE;
}


//��豸�� �������� �ȿ� ����?
BOOL	ObjFrustum::IsInSphere(D3DXVECTOR3* pv, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		float dist = D3DXPlaneDotCoord(&m_plane[i], pv);

		//��豸�� ������ ������ ������ FALSE��
		//�����̶� �����ִٸ� TRUE
		if (dist > (radius + PLANE_EPSILON))
			return FALSE;
	}
	return TRUE;
}

// ���������� ȭ�鿡 �׷��ش�.
BOOL	ObjFrustum::Draw()
{
	m_device->SetStreamSource(0, g_pVB, 0, sizeof(D3DXVECTOR3));
	m_device->SetFVF(ObjFrustum::_FVF);
	m_device->SetIndices(g_pIB);
	m_device->SetTexture(0, NULL);
	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);


	
	D3DXMatrixIdentity(&m_matWorld);
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	// �Ķ������� ��,�� ����� �׸���.
	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	m_device->SetMaterial(&mtrl);

	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return TRUE;

}