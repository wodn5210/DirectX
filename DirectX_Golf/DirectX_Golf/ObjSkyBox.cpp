#include "ObjSkyBox.h"


ObjSkyBox::ObjSkyBox()
{

}
ObjSkyBox::~ObjSkyBox()
{
	if (m_pIB != NULL)
		m_pIB->Release();

	if (m_pVB != NULL)
		m_pVB->Release();

	for (int i = 0; i < 6; i++) 
		m_pTex[i]->Release();
}

void ObjSkyBox::DrawMain()
{
	
	m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);		// 환경광원(ambient light)의 값 설정
	//사각형 한개를 이용해서 for문 돌면서 6면 간다
	m_device->SetStreamSource(0, m_pVB, 0, sizeof(SKY_VTX));
	m_device->SetFVF(SKY_VTX::FVF);
	m_device->SetIndices(m_pIB);
	_InitMtrl();

	for (int i = 0; i < 6; i++)
	{
		m_matWorld = forTXT[i] * m_bigScale * m_translation;
		m_device->SetTransform(D3DTS_WORLD, &m_matWorld);
		m_device->SetTexture(0, m_pTex[i]);
		m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		m_device->SetTexture(0, NULL);
	}
}
HRESULT ObjSkyBox::Create(LPDIRECT3DDEVICE9 device)
{
	m_device = device;

	//300배 크게 만들자 3
	D3DXMATRIXA16  s;
	m_big = 100;
	D3DXMatrixIdentity(&s);
	SetScale(s);

	if (FAILED(_InitVB()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitIB()))
	{
		return E_FAIL;
	}
	if (FAILED(_InitTXT()))
	{
		return E_FAIL;
	}

	for (int i = 0; i < 4; i++)
	{
		D3DXMatrixRotationY(&forTXT[i], D3DX_PI/2*i);
	}
	D3DXMatrixRotationX(&forTXT[4], -D3DX_PI / 2);
	D3DXMatrixRotationX(&forTXT[5], D3DX_PI / 2);



	return S_OK;
}



HRESULT ObjSkyBox::_InitVB()
{
	SKY_VTX vts[] =
	{
		{D3DXVECTOR3(-1, 1, 1), D3DXVECTOR2(0, 0)},
		{D3DXVECTOR3(1, 1, 1), D3DXVECTOR2(1, 0)},
		{D3DXVECTOR3(-1, -1, 1), D3DXVECTOR2(0, 1)},
		{D3DXVECTOR3(1, -1, 1), D3DXVECTOR2(1, 1)},
	};

	/// 정점버퍼 생성
	if (FAILED(m_device->CreateVertexBuffer(4 * sizeof(SKY_VTX),
		0, SKY_VTX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL)))
	{
		return E_FAIL;
	}

	/// 정점버퍼를 값으로 채운다. 
	VOID* pVertices;
	if (FAILED(m_pVB->Lock(0, sizeof(vts), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, vts, sizeof(vts));
	m_pVB->Unlock();

	return S_OK;
}
HRESULT ObjSkyBox::_InitIB()
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
HRESULT ObjSkyBox::_InitTXT()
{
	if (FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/Skybox/Front.bmp",	&m_pTex[0])) ||
		FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/Skybox/Left.bmp",	&m_pTex[1])) ||
		FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/Skybox/Back.bmp",	&m_pTex[2])) ||
		FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/Skybox/Right.bmp", &m_pTex[3])) ||
		FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/Skybox/Top.bmp",	 &m_pTex[4])) || 		 
		FAILED(D3DXCreateTextureFromFile(m_device, "src/golf/Skybox/Bottom.bmp", &m_pTex[5]))) 
	{
		return E_FAIL;
	}



	return S_OK;
}
HRESULT ObjSkyBox::_InitMtrl()
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