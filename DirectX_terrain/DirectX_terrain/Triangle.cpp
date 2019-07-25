#include "Triangle.h"


Triangle::Triangle(LPDIRECT3DDEVICE9 device)
{
	m_device = device;
	_fvf = D3DFVF_XYZ | D3DFVF_NORMAL;
	D3DXMatrixIdentity(&_matWorld);
	D3DXMatrixIdentity(&_translation);
	D3DXMatrixIdentity(&_rotation);
	D3DXMatrixIdentity(&_scale);
}
Triangle::~Triangle()
{
	if (m_pIB != NULL)
		m_pIB->Release();

	if (m_pVB != NULL)
		m_pVB->Release();
}

void Triangle::DrawObj()
{
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(CUSTOMVERTEX));
	m_device->SetFVF(_fvf);
	m_device->SetIndices(m_pIB);
	_InitMtrl();
	_matWorld = _scale * _translation;
	m_device->SetTransform(D3DTS_WORLD, &_matWorld);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);

}





HRESULT Triangle::Create(D3DXVECTOR3 pos[3])
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


HRESULT Triangle::_InitVB(D3DXVECTOR3 pos[3])
{
	D3DXVECTOR3 center = (pos[0] + pos[1] + pos[2]) / 3.0f;
	pos[0] -= center;
	pos[1] -= center;
	pos[2] -= center;

	D3DXMatrixIdentity(&_translation);
	_translation._41 = center.x;
	_translation._42 = center.y;
	_translation._43 = center.z;
	
	CUSTOMVERTEX g_Vertices[] =
	{
		{ pos[0],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		{ pos[1],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		{ pos[2],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
	};
	/// 정점버퍼 생성
	if (FAILED(m_device->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, _fvf,
		D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}

	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(g_Vertices), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	m_pVB->Unlock();

	return S_OK;
}
HRESULT Triangle::_InitIB()
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
HRESULT Triangle::_InitMtrl()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	m_device->SetMaterial(&mtrl);
	return S_OK;
}