#include "ObjBallGori.h"

ObjBallGori::ObjBallGori()
{

}
ObjBallGori::~ObjBallGori()
{

	if (m_pVB != NULL)
		m_pVB->Release();

	if (m_pIB != NULL)
		m_pIB->Release();
}

void ObjBallGori::DrawMain()
{
	//m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);		// 환경광원(ambient light)의 값 설정
	D3DXMatrixIdentity(&m_matWorld);
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);


	m_device->SetIndices(m_pIB);

	_InitMtrl();
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(GORI_VTX));
	m_device->SetFVF(GORI_VTX::FVF);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 22, 0, 20);

}

HRESULT ObjBallGori::ReadyRender(D3DXVECTOR3* pos, D3DXVECTOR3* dir)
{
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXVec3Normalize(&cross, &cross);
	GORI_VTX temp[22];
	for (int i = 0; i < 22; i++)
		temp[i] = m_vtx[i];
	for (int i = 2; i < 22; i++) 
		m_vtx[i] = temp[i - 2];
	

	m_vtx[0].p = *pos - cross * 0.02f;
	m_vtx[1].p = *pos + cross * 0.02f;


	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, 22 * sizeof(GORI_VTX), (void**)& pVertices, 0)))
	{
		printf("할당 오류\n");
		return E_FAIL;
	}

	memcpy(pVertices, &m_vtx, 22 * sizeof(GORI_VTX));
	m_pVB->Unlock();

	return true;
}


HRESULT ObjBallGori::Create(LPDIRECT3DDEVICE9 device, D3DXVECTOR3* pos)
{
	m_device = device;

	if (FAILED(_InitVB(pos)))
	{
		return E_FAIL;
	}
	if (FAILED(_InitIB()))
	{
		return E_FAIL;
	}

	

	return S_OK;
}
HRESULT ObjBallGori::Init(D3DXVECTOR3* pos)
{
	for (int i = 0; i < 22; i++)
	{
		m_vtx[i].p = *pos;
	}

	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, 22 * sizeof(GORI_VTX), (void**)& pVertices, 0)))
	{
		printf("할당 오류\n");
		return E_FAIL;
	}

	memcpy(pVertices, &m_vtx, 22 * sizeof(GORI_VTX));
	m_pVB->Unlock();

	return S_OK;
}

HRESULT ObjBallGori::_InitVB(D3DXVECTOR3* pos)
{
	/// 정점버퍼 생성

	if (FAILED(m_device->CreateVertexBuffer(22 * sizeof(GORI_VTX),
		0, GORI_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}
	for (int i = 0; i < 22; i++)
	{
		m_vtx[i].p = *pos;
	}
	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, 22 * sizeof(GORI_VTX), (void**)& pVertices, 0)))
	{
		printf("할당 오류\n");
		return E_FAIL;
	}

	memcpy(pVertices, &m_vtx, 22 * sizeof(GORI_VTX));
	m_pVB->Unlock();



	return S_OK;
}
HRESULT ObjBallGori::_InitIB()
{
	TRI_IDX idx[20] =
	{
		{0, 1, 2}, {2, 1, 3}
	};
	for (int i = 2; i < 20; i += 2)
	{
		idx[i]._0 = idx[0]._0 + i;
		idx[i]._1 = idx[0]._1 + i;
		idx[i]._2 = idx[0]._2 + i;
		idx[i + 1]._0 = idx[1]._0 + i;
		idx[i + 1]._1 = idx[1]._1 + i;
		idx[i + 1]._2 = idx[1]._2 + i;
	}

	if (FAILED(m_device->CreateIndexBuffer(20 * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(m_pIB->Lock(0, 20 * sizeof(TRI_IDX), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, idx, 20 * sizeof(TRI_IDX));
	m_pIB->Unlock();

	return S_OK;
}
HRESULT ObjBallGori::_InitMtrl()
{


	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}

