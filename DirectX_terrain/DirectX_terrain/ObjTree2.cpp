#include "ObjTree2.h"


ObjTree2::ObjTree2(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	D3DXMatrixScaling(&m_scale, 0.01f, 0.01f, 0.01f);
	//D3DXMatrixScaling(&m_scale, 20, 20, 20);
}
ObjTree2::~ObjTree2()
{
	if (m_pIB_trunk != NULL)
		m_pIB_trunk->Release();
	if (m_pIB_leaves != NULL)
		m_pIB_leaves->Release();

	if (m_pVB != NULL)
		m_pVB->Release();
}

void ObjTree2::DrawMain()
{
	m_device->SetRenderState(D3DRS_NORMALIZENORMALS, true);

	m_matWorld = m_scale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_device->SetStreamSource(0, m_pVB, 0, sizeof(TREE2_VTX));
	m_device->SetFVF(TREE2_VTX::FVF);
	m_device->SetIndices(m_pIB_leaves);
	_InitMtrl_leaves();
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vt.size(), 0, m_idx_leaves.size());

	m_device->SetIndices(m_pIB_trunk);
	_InitMtrl_trunk();
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vt.size(), 0, m_idx_trunk.size());

	m_device->SetRenderState(D3DRS_NORMALIZENORMALS, false);

}

HRESULT ObjTree2::Create(D3DXVECTOR3 center, string txt)
{
	SetCenter(center);

	if (FIleReadObj::ReadObj(txt, m_vt, &m_idx_leaves, &m_idx_trunk))
		printf("성공");

	if (FAILED(_InitVB()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitIB_trunk()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitIB_leaves()))
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT ObjTree2::_InitVB()
{
	/// 정점버퍼 생성
	if (FAILED(m_device->CreateVertexBuffer(m_vt.size() * sizeof(TREE2_VTX),
		0, SKY_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}

	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, m_vt.size() * sizeof(TREE2_VTX), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, &m_vt[0], m_vt.size() * sizeof(TREE2_VTX));
	m_pVB->Unlock();

	return S_OK;
}
HRESULT ObjTree2::_InitIB_trunk()
{
	if (FAILED(m_device->CreateIndexBuffer(m_idx_trunk.size() * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB_trunk, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(m_pIB_trunk->Lock(0, m_idx_trunk.size() * sizeof(TRI_IDX), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, &m_idx_trunk[0], m_idx_trunk.size()*sizeof(TRI_IDX));
	m_pIB_trunk->Unlock();
	return S_OK;
}
HRESULT ObjTree2::_InitIB_leaves()
{
	if (FAILED(m_device->CreateIndexBuffer(m_idx_leaves.size() * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB_leaves, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(m_pIB_leaves->Lock(0, m_idx_leaves.size() * sizeof(TRI_IDX), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, &m_idx_leaves[0], m_idx_leaves.size() * sizeof(TRI_IDX));
	m_pIB_leaves->Unlock();
	return S_OK;
}
HRESULT ObjTree2::_InitMtrl_trunk()
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
HRESULT ObjTree2::_InitMtrl_leaves()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}