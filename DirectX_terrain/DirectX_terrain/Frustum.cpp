#include "Frustum.h"


Frustum::Frustum(LPDIRECT3DDEVICE9 pDev)
{
	g_pd3dDevice = pDev;

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


HRESULT Frustum::Init()
{
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(8 * sizeof(D3DXVECTOR3),
		0, Frustum::_FVF, D3DPOOL_DEFAULT, &g_pVB, NULL)))
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


	if (FAILED(g_pd3dDevice->CreateIndexBuffer(12 * sizeof(FRS_IDX), 0,
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




HRESULT Frustum::FillVB()
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
BOOL Frustum::Make(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3* pos)
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
BOOL	Frustum::IsIn(D3DXVECTOR3* pv)
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
BOOL	Frustum::IsInSphere(D3DXVECTOR3* pv, float radius)
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
BOOL	Frustum::Draw()
{
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(D3DXVECTOR3));
	g_pd3dDevice->SetFVF(Frustum::_FVF);
	g_pd3dDevice->SetIndices(g_pIB);
	g_pd3dDevice->SetTexture(0, NULL);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);


	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// �Ķ������� ��,�� ����� �׸���.
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	g_pd3dDevice->SetMaterial(&mtrl);

	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return TRUE;

}