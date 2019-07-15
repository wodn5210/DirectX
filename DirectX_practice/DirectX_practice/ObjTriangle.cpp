#include "ObjTriangle.h"


ObjTriangle::ObjTriangle()
{
	_fvf = D3DFVF_XYZ | D3DFVF_NORMAL;
	
}
ObjTriangle::~ObjTriangle()
{
	if (g_pIB != NULL)
		g_pIB->Release();

	if (g_pVB != NULL)
		g_pVB->Release();
}

HRESULT ObjTriangle::Create(LPDIRECT3DDEVICE9 _pd3dDevice, D3DXVECTOR3 center)
{

	if (FAILED(InitVB(_pd3dDevice, center)))
	{
		return E_FAIL;
	}
	if (FAILED(InitIB(_pd3dDevice)))
	{
		return E_FAIL;
	}

	return S_OK;
}


void ObjTriangle::DrawObj(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;

	//정점버퍼 스트림 설정
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(_fvf);
	g_pd3dDevice->SetIndices(g_pIB);

	//0번 삼각형
	InitMtrl(g_pd3dDevice);
	D3DXMatrixRotationX(&rotation, fAngle);
	D3DXMatrixTranslation(&translation, -40, -40, -40);
	matWorld = rotation * translation;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
}

HRESULT ObjTriangle::InitMtrl(LPDIRECT3DDEVICE9 _pd3dDevice)
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	_pd3dDevice->SetMaterial(&mtrl);
	return S_OK;
}

HRESULT ObjTriangle::InitVB(LPDIRECT3DDEVICE9 g_pd3dDevice, D3DXVECTOR3 center)
{
	CUSTOMVERTEX g_Vertices[] =
	{
		{  D3DXVECTOR3(center.x + 0.0f, center.y + 42.0f, center.z + 0.0f),		D3DXVECTOR3(0.0f, 50.0f, 0.0f), },	//첫번째 삼각형
		{  D3DXVECTOR3(center.x - 25.0f, center.y + 0.0f, center.z + 0.0f),	D3DXVECTOR3(0, 0.33f, 0), },
		{  D3DXVECTOR3(center.x + 25.0f, center.y + 0.0f, center.z + 0.0f),		D3DXVECTOR3(0, 0, 0.33f), },


	};

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, _fvf, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// Fill the vertex buffer.
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(g_Vertices), (void**)& pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	g_pVB->Unlock();

	return S_OK;
}
HRESULT ObjTriangle::InitIB(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	MYINDEX indices[] =
	{
		{0, 1, 2},
	};

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(1 * sizeof(MYINDEX), 0,
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