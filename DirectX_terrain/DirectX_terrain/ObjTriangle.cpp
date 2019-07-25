#include "ObjTriangle.h"


ObjTriangle::ObjTriangle(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	m_fvf = D3DFVF_XYZ | D3DFVF_NORMAL;
}
ObjTriangle::~ObjTriangle()
{
	if (m_pIB != NULL)
		m_pIB->Release();

	if (m_pVB != NULL)
		m_pVB->Release();
}

void ObjTriangle::DrawMain()
{
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(m_fvf);
	m_device->SetIndices(m_pIB);
	_InitMtrl();
	m_matWorld = m_scale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);

}
void ObjTriangle::DrawMap()
{
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(m_fvf);
	m_device->SetIndices(m_pIB);
	_InitMtrl();
	m_matWorld = m_bigScale * m_translation;
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);

}





HRESULT ObjTriangle::Create(D3DXVECTOR3 pos[3])
{
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


HRESULT ObjTriangle::_InitVB(D3DXVECTOR3 pos[3])
{
	m_center = (pos[0] + pos[1] + pos[2]) / 3.0f;
	pos[0] -= m_center;
	pos[1] -= m_center;
	pos[2] -= m_center;

	D3DXMatrixIdentity(&m_translation);
	m_translation._41 = m_center.x;
	m_translation._42 = m_center.y;
	m_translation._43 = m_center.z;
	
	CUSTOMVERTEX gm_Vertices[] =
	{
		{ pos[0],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		{ pos[1],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		{ pos[2],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
	};
	/// 정점버퍼 생성
	if (FAILED(m_device->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, m_fvf,
		D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}

	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(gm_Vertices), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, gm_Vertices, sizeof(gm_Vertices));
	m_pVB->Unlock();

	return S_OK;
}
HRESULT ObjTriangle::_InitIB()
{
	MYINDEX indices[] =
	{
		{0, 1, 2},
	};

	if (FAILED(m_device->CreateIndexBuffer(1 * sizeof(MYINDEX), 0,
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
HRESULT ObjTriangle::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}