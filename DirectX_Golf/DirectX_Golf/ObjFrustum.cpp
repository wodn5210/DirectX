#include "ObjFrustum.h"


ObjFrustum::ObjFrustum()
{

	// 투영행렬까지 거치면 모든 3차원 월드좌표의 점은 (-1,-1,0) ~ (1,1,1)사이의 값으로 바뀐다.
	// m_projVTX에 이 동차공간의 경계값을 넣어둔다.
	m_projVTX[0].x = -1.0f;		m_projVTX[0].y = -1.0f;		m_projVTX[0].z = 0.0f;
	m_projVTX[1].x = 1.0f;		m_projVTX[1].y = -1.0f;		m_projVTX[1].z = 0.0f;
	m_projVTX[2].x = 1.0f;		m_projVTX[2].y = -1.0f;		m_projVTX[2].z = 1.0f;
	m_projVTX[3].x = -1.0f;		m_projVTX[3].y = -1.0f;		m_projVTX[3].z = 1.0f;
	m_projVTX[4].x = -1.0f;		m_projVTX[4].y = 1.0f;		m_projVTX[4].z = 0.0f;
	m_projVTX[5].x = 1.0f;		m_projVTX[5].y = 1.0f;		m_projVTX[5].z = 0.0f;
	m_projVTX[6].x = 1.0f;		m_projVTX[6].y = 1.0f;		m_projVTX[6].z = 1.0f;
	m_projVTX[7].x = -1.0f;		m_projVTX[7].y = 1.0f;		m_projVTX[7].z = 1.0f;

	ZeroMemory(m_plane, sizeof(m_plane[0]) * 6);

}

ObjFrustum::~ObjFrustum()
{
	if (m_pVB != NULL)
		m_pVB->Release();
	if (m_pIB != NULL)
		m_pIB->Release();
}

HRESULT ObjFrustum::Create(LPDIRECT3DDEVICE9 pDev)
{
	if (pDev == NULL)
		return E_FAIL;

	m_device = pDev;

	if (FAILED(_InitVB()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitIB()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT ObjFrustum::_InitVB()
{
	if (FAILED(m_device->CreateVertexBuffer(8 * sizeof(D3DXVECTOR3),
		0, ObjFrustum::_FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		
		return E_FAIL;
	}
	return S_OK;
}
HRESULT ObjFrustum::_InitIB()
{
	TRI_IDX indices[] =
	{
	{0, 1, 2}, {0, 2, 3},
	{4, 6, 5}, {4, 7, 6 },
	{0, 3, 7}, {0, 7, 4},
	{1, 5, 6}, {1, 6, 2},
	{3, 6, 2}, {3, 7, 6},
	{0, 4, 5}, {0, 5, 1},
	};


	if (FAILED(m_device->CreateIndexBuffer(12 * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;

	if (FAILED(m_pIB->Lock(0, sizeof(indices), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, indices, sizeof(indices));
	m_pIB->Unlock();

	return S_OK;
}
HRESULT ObjFrustum::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	m_device->SetMaterial(&mtrl);

	return S_OK;
}


HRESULT ObjFrustum::_UpdateVB()
{
	// Fill the vertex buffer.
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(m_vtx), (void**)& pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, m_vtx, sizeof(m_vtx));

	m_pVB->Unlock();

	return S_OK;
}

// 카메라(view) * 프로젝션(projection)행렬을 입력받아 6개의 평면을 만든다.
BOOL ObjFrustum::Update(D3DXMATRIXA16* pmatViewProj, D3DXVECTOR3* pos)
{
	//카메라 좌표 설정
	m_vPos = *pos;	

	int				i;
	D3DXMATRIXA16	matInv;

	// view * proj의 역행렬을 구한다.
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj);

	// Vertex_final = Vertex_local * Matrix_world * Matrix_view * Matrix_Proj 기본변환에서 프러스텀좌표 유도
	// (Vertex_world = Vertex_local * Matrix_world)

	// Vertex_final = Vertex_world * ( Matrix_view * Matrix_Proj )
	// Vertex_World = Vertex_final * 역행렬( Matrix_view * Matrix_Proj )^-1
	// Vertex_World가 결국 프러스텀 정점임	
	for (i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vtx[i], &m_projVTX[i], &matInv);



	// 얻어진 월드좌표로 프러스텀 평면을 만든다
	// 벡터가 프러스텀 안쪽에서 바깥쪽으로 나가는 평면들이다.
	D3DXPlaneFromPoints(&m_plane[0], m_vtx + 4, m_vtx + 7, m_vtx + 6);	// 상 평면(top)
	D3DXPlaneFromPoints(&m_plane[1], m_vtx, m_vtx + 1, m_vtx + 2);		// 하 평면(bottom)
	D3DXPlaneFromPoints(&m_plane[2], m_vtx, m_vtx + 4, m_vtx + 5);		// 근 평면(near)
	D3DXPlaneFromPoints(&m_plane[3], m_vtx + 2, m_vtx + 6, m_vtx + 7);	// 원 평면(far)
	D3DXPlaneFromPoints(&m_plane[4], m_vtx, m_vtx + 3, m_vtx + 7);		// 좌 평면(left)
	D3DXPlaneFromPoints(&m_plane[5], m_vtx + 1, m_vtx + 5, m_vtx + 6);	// 우 평면(right)

	_UpdateVB();

	return TRUE;
}

// 프러스텀을 화면에 그려준다.
BOOL	ObjFrustum::Draw()
{
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(D3DXVECTOR3));
	m_device->SetFVF(ObjFrustum::_FVF);
	m_device->SetIndices(m_pIB);
	m_device->SetTexture(0, NULL);
	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);



	D3DXMatrixIdentity(&m_matWorld);
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	// 파란색으로 상,하 평면을 그린다.
	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);

	_InitMtrl();

	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return TRUE;

}


/*
	프러스텀 컬링을 이용한 
	렌더링 최적화 방법에 사용할 코드
*/

//프러스텀 안에 있으면 True
BOOL	ObjFrustum::IsIn(D3DXVECTOR3* pv)
{
	for (int i = 0; i < 6; i++)
	{
		//프러스텀 평면과 정점pv를 이용해서 내부 or 외부 판단
		float dist = D3DXPlaneDotCoord(&m_plane[i], pv);

		//평면의 벡터가 바깥쪽으로 나가게 되어있기때문에 양수면 외부임
		if (dist > PLANE_EPSILON)
			return FALSE;
	}
	return TRUE;
}


//경계구가 프러스텀 안에 있음?
BOOL	ObjFrustum::IsInSphere(D3DXVECTOR3* pv, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		float dist = D3DXPlaneDotCoord(&m_plane[i], pv);

		//경계구가 완전히 밖으로 나가야 FALSE임
		//조금이라도 걸쳐있다면 TRUE
		if (dist > (radius + PLANE_EPSILON))
			return FALSE;
	}
	return TRUE;
}
