#include "ObjTree1.h"


ObjTree1::ObjTree1(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	D3DXMatrixScaling(&m_scale, 10, 10, 10);
	
}
ObjTree1::~ObjTree1()
{
	if (m_pIB != NULL)
		m_pIB->Release();

	if (m_pVB != NULL)
		m_pVB->Release();

	m_pTex->Release();
}

void ObjTree1::DrawMain(D3DXMATRIXA16* matbill)
{
	//// 알파채널을 사용해서 투명텍스처 효과를 낸다
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_device->SetRenderState(D3DRS_ALPHAREF, 0x08);
	m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(TREE1_VTX));
	m_device->SetFVF(TREE1_VTX::FVF);
	m_device->SetIndices(m_pIB);
	_InitMtrl();

	matbill->_41 = m_center.x;
	matbill->_42 = m_center.y;
	matbill->_43 = m_center.z;

	m_matWorld = *matbill;

	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_device->SetTexture(0, m_pTex);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	m_device->SetTexture(0, NULL);

	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

}

HRESULT ObjTree1::Create(D3DXVECTOR3 center, string txt)
{
	SetCenter(center);

	if (FAILED(_InitVB()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitIB()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitTXT(txt)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT ObjTree1::_InitVB()
{
	//TREE1_VTX vts[] =
	//{
	//	{D3DXVECTOR3(-1, 1, 0), D3DXVECTOR2(0, 0)},
	//	{D3DXVECTOR3(1, 1, 0), D3DXVECTOR2(1, 0)},
	//	{D3DXVECTOR3(-1, -1, 0), D3DXVECTOR2(0, 1)},
	//	{D3DXVECTOR3(1, -1, 0), D3DXVECTOR2(1, 1)},
	//};

	///// 정점버퍼 생성
	//if (FAILED(m_device->CreateVertexBuffer(4 * sizeof(TREE1_VTX),
	//	0, SKY_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	//{
	//	return E_FAIL;
	//}

	///// 정점버퍼를 값으로 채운다. 
	//VOID* pVertices;
	//if (FAILED(m_pVB->Lock(0, sizeof(vts), (void**)& pVertices, 0)))
	//	return E_FAIL;
	//memcpy(pVertices, vts, sizeof(vts));
	//m_pVB->Unlock();


	vector<TREE1_VTX> vts =
	{
		{D3DXVECTOR3(-1, 1, 0), D3DXVECTOR2(0, 0)},
		{D3DXVECTOR3(1, 1, 0), D3DXVECTOR2(1, 0)},
		{D3DXVECTOR3(-1, -1, 0), D3DXVECTOR2(0, 1)},
		{D3DXVECTOR3(1, -1, 0), D3DXVECTOR2(1, 1)},
	};

	/// 정점버퍼 생성
	if (FAILED(m_device->CreateVertexBuffer(vts.size() * sizeof(TREE1_VTX),
		0, SKY_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}

	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, vts.size() * sizeof(TREE1_VTX), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, &vts[0], vts.size() * sizeof(TREE1_VTX));
	m_pVB->Unlock();

	return S_OK;
}
HRESULT ObjTree1::_InitIB()
{
	TRI_IDX indices[] =
	{
		{0, 1, 2}, {2, 1, 3}
	};

	if (FAILED(m_device->CreateIndexBuffer(2 * sizeof(TRI_IDX), 0,
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
HRESULT ObjTree1::_InitTXT(string txt)
{
	if(FAILED(D3DXCreateTextureFromFile(m_device, (LPCSTR)txt.c_str(), &m_pTex)))
	{
		printf("에러");
		return E_FAIL;
	}
	return S_OK;
}
HRESULT ObjTree1::_InitMtrl()
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