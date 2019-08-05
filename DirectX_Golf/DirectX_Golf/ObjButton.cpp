#include "ObjButton.h"

ObjButton::ObjButton()
{
}
ObjButton::~ObjButton()
{
	if (m_pIB != NULL)
		m_pIB->Release();

	if (m_pVB != NULL)
		m_pVB->Release();

	if(m_pTex != NULL)
		m_pTex->Release();
}
HRESULT ObjButton::ReadyRender(D3DXMATRIXA16* pmatViewProj)
{
	D3DXMATRIXA16	matInv;
	// view * proj의 역행렬을 구한다.
	D3DXMatrixInverse(&matInv, NULL, pmatViewProj);


	for (int i = 0; i < 4; i++)
	{
		D3DXVec3TransformCoord(&m_drawVtx[i].p, &m_vtx[i].p, &matInv);
	}


	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(m_drawVtx), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, m_drawVtx, sizeof(m_drawVtx));
	m_pVB->Unlock();

	return S_OK;
}
void ObjButton::DrawMain()
{
	m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);		// 환경광원(ambient light)의 값 설정
	D3DXMatrixIdentity(&m_matWorld);
	m_device->SetTransform(D3DTS_WORLD, &m_matWorld);

	m_device->SetStreamSource(0, m_pVB, 0, sizeof(BNT_VTX));
	m_device->SetFVF(BNT_VTX::FVF);

	m_device->SetIndices(m_pIB);
	_InitMtrl();

	m_device->SetTexture(0, m_pTex);
	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	m_device->SetTexture(0, NULL);
}
HRESULT ObjButton::Create(LPDIRECT3DDEVICE9 device, string path)
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
	if (FAILED(_InitTexture(path)))
	{
		return E_FAIL;
	}
	return S_OK;
}
VOID ObjButton::SetDrawPos(D3DXVECTOR2 lt, D3DXVECTOR2 rb)
{
	m_drawVtx[0] = m_vtx[0] = { D3DXVECTOR3(lt.x, lt.y, EPSILON), D3DXVECTOR2(0, 0) };
	m_drawVtx[1] = m_vtx[1] = { D3DXVECTOR3(rb.x, lt.y, EPSILON), D3DXVECTOR2(1, 0) };
	m_drawVtx[2] = m_vtx[2] = { D3DXVECTOR3(lt.x, rb.y, EPSILON), D3DXVECTOR2(0, 1) };
	m_drawVtx[3] = m_vtx[3] = { D3DXVECTOR3(rb.x, rb.y, EPSILON), D3DXVECTOR2(1, 1) };

}

HRESULT ObjButton::_InitVB()
{	
	if (FAILED(m_device->CreateVertexBuffer(4 * sizeof(BNT_VTX),
		0, BNT_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}
	return S_OK;
}
HRESULT ObjButton::_InitIB()
{
	TRI_IDX idx[] =
	{
		{0, 1, 2}, {2, 1, 3}
	};

	if (FAILED(m_device->CreateIndexBuffer(2 * sizeof(TRI_IDX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(m_pIB->Lock(0, sizeof(idx), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, idx, sizeof(idx));
	m_pIB->Unlock();

	return S_OK;
}
HRESULT ObjButton::_InitTexture(string path)
{
	if (FAILED(D3DXCreateTextureFromFile(m_device, path.c_str(), &m_pTex)))
	{
		return E_FAIL;
	}
	return S_OK;
}
HRESULT ObjButton::_InitMtrl()
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