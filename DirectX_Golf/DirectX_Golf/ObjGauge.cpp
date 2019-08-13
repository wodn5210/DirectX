#include "ObjGauge.h"

ObjGauge::ObjGauge()
{


	m_projVTX[0].p = D3DXVECTOR3(-0.7f, -0.8f, EPSILON);
	m_projVTX[1].p = D3DXVECTOR3(0.7f, -0.8f, EPSILON);
	m_projVTX[2].p = D3DXVECTOR3(-0.7f, -0.95f, EPSILON);
	m_projVTX[3].p = D3DXVECTOR3(0.7f, -0.95f, EPSILON);

	m_projVTX[4].p = D3DXVECTOR3(-0.72f, -0.78f, EPSILON);
	m_projVTX[5].p = D3DXVECTOR3(0.72f, -0.78f, EPSILON);
	m_projVTX[6].p = D3DXVECTOR3(-0.72f, -0.8f, EPSILON);
	m_projVTX[7].p = D3DXVECTOR3(0.72f, -0.8f, EPSILON);

	m_projVTX[8].p = D3DXVECTOR3(-0.72f, -0.95f, EPSILON);
	m_projVTX[9].p = D3DXVECTOR3(0.72f, -0.95f, EPSILON);
	m_projVTX[10].p = D3DXVECTOR3(-0.72f, -0.97f, EPSILON);
	m_projVTX[11].p = D3DXVECTOR3(0.72f, -0.97f, EPSILON);


	m_projVTX[12].p = D3DXVECTOR3(-0.72f, -0.78f, EPSILON);
	m_projVTX[13].p = D3DXVECTOR3(-0.7f, -0.78f, EPSILON);
	m_projVTX[14].p = D3DXVECTOR3(-0.72f, -0.97f, EPSILON);
	m_projVTX[15].p = D3DXVECTOR3(-0.7f, -0.97f, EPSILON);

	m_projVTX[16].p = D3DXVECTOR3(0.7f, -0.78f, EPSILON);
	m_projVTX[17].p = D3DXVECTOR3(0.72f, -0.78f, EPSILON);
	m_projVTX[18].p = D3DXVECTOR3(0.7f, -0.97f, EPSILON);
	m_projVTX[19].p = D3DXVECTOR3(0.72f, -0.97f, EPSILON);
}
ObjGauge::~ObjGauge()
{
	if (m_pVB != NULL)
		m_pVB->Release();
	if (m_pIB_guage != NULL)
		m_pIB_guage->Release();
	if (m_pIB_edge != NULL)
		m_pIB_guage->Release();
}

HRESULT ObjGauge::ReadyRender(D3DXMATRIXA16 * pmatViewProj, float energy)
{
	m_projVTX[1].p.x = -0.7f + energy * 2.0f;
	m_projVTX[3].p.x = -0.7f + energy * 2.0f;


	D3DXMATRIXA16	matInv;
	// view * proj의 역행렬을 구한다.
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj);

	for (int i = 0; i < 20; i++)
	{
		D3DXVec3TransformCoord(&m_worldVTX[i].p, &m_projVTX[i].p, &matInv);
	}


	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(m_worldVTX), (void**)& pVertices, 0)))
	{
		printf("할당 오류\n");
		return E_FAIL;
	}

	memcpy(pVertices, m_worldVTX, sizeof(m_worldVTX));
	m_pVB->Unlock();

	return S_OK;
}

void ObjGauge::DrawMain()
{
	
	m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);		// 환경광원(ambient light)의 값 설정
	D3DXMatrixIdentity(&m_matWorld);
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_device->SetStreamSource(0, m_pVB, 0, sizeof(BAR_VTX));
	m_device->SetFVF(BAR_VTX::FVF);

	m_device->SetIndices(m_pIB_guage);
	_InitMtrlGauge();
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	m_device->SetIndices(m_pIB_edge);
	_InitMtrlEdge();
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 16, 0, 8);


}

HRESULT ObjGauge::Create(LPDIRECT3DDEVICE9 device)
{
	m_device = device;

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



HRESULT ObjGauge::_InitVB()
{
	/// 정점버퍼 생성
	if (FAILED(m_device->CreateVertexBuffer(4 * sizeof(SKY_VTX),
		0, BAR_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}



	return S_OK;
}
HRESULT ObjGauge::_InitIB()
{
	TRI_IDX guage[] =
	{
		{0, 1, 2}, {2, 1, 3}
	};

	if (FAILED(m_device->CreateIndexBuffer(2 * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB_guage, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(m_pIB_guage->Lock(0, sizeof(guage), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, guage, sizeof(guage));
	m_pIB_guage->Unlock();


	TRI_IDX edge[] =
	{
	{4, 5, 6}, {6, 5, 7},
	{8, 9, 10}, {10, 9, 11},
	{12, 13, 14}, {14, 13, 15},
	{16, 17, 18}, {18, 17, 19}
	};
	if (FAILED(m_device->CreateIndexBuffer(8 * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB_edge, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices2;
	if (FAILED(m_pIB_edge->Lock(0, sizeof(edge), (void**)& pIndices2, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices2, edge, sizeof(edge));
	m_pIB_edge->Unlock();

	return S_OK;
}
HRESULT ObjGauge::_InitMtrlEdge()
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

HRESULT ObjGauge::_InitMtrlGauge()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}