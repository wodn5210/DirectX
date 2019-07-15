#include "ObjCube.h"
ObjCube::ObjCube()
{
	_fvf = D3DFVF_XYZ | D3DFVF_NORMAL;
}
ObjCube::~ObjCube()
{
	if (g_pIB != NULL)
		g_pIB->Release();

	if (g_pVB != NULL)
		g_pVB->Release();
}

void ObjCube::DrawObj(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;

	//정점버퍼 스트림 설정
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(_fvf);
	g_pd3dDevice->SetIndices(g_pIB);

	InitMtrl(g_pd3dDevice);
	D3DXMatrixRotationY(&_rotation, fAngle);
	_matWorld = _rotation * _scale* _translation;
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &_matWorld);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
}
HRESULT ObjCube::Create(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	if (FAILED(InitVB(g_pd3dDevice)))
	{
		return E_FAIL;
	}
	if (FAILED(InitIB(g_pd3dDevice)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT ObjCube::InitVB(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	CUSTOMVERTEX g_Vertices[] =
	{
		//큐브 만들기 위한 삼각형들
			{	D3DXVECTOR3(-1,  1,  1),			D3DXVECTOR3(0, 1, 0)},		//v0
			{	D3DXVECTOR3( 1,  1,  1),			D3DXVECTOR3(0, 1, 0)},		//v1
			{	D3DXVECTOR3( 1,  1, -1),			D3DXVECTOR3(1, 0, 0)},		//v2
			{	D3DXVECTOR3(-1,  1, -1),		D3DXVECTOR3(1, 0, 0)},	//v3

			{	D3DXVECTOR3(-1, -1,  1),		D3DXVECTOR3(0, -1, 0)},	//v4
			{	D3DXVECTOR3( 1, -1,  1),			D3DXVECTOR3(0, -1, 0)},		//v5
			{	D3DXVECTOR3( 1, -1, -1),			D3DXVECTOR3(1, 0, 1)},	//v6
			{	D3DXVECTOR3(-1, -1, -1),		D3DXVECTOR3(0, 0, 1)},	//v7
	};

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
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
HRESULT ObjCube::InitIB(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	MYINDEX indices[] =
	{
	{0, 1, 2}, {0, 2, 3},
	{4, 6, 5}, {4, 7, 6 },
	{0, 3, 7}, {0,7, 4},
	{1, 5, 6}, {1, 6, 2},
	{3, 2, 6}, {3, 6, 7},
	{0, 4, 5}, {0, 5, 1},
	};

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(12 * sizeof(MYINDEX), 0,
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
HRESULT ObjCube::InitMtrl(LPDIRECT3DDEVICE9 g_pd3dDevice)
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

	mtrl.Diffuse.r = mtrl.Ambient.r = _material.r;
	mtrl.Diffuse.g = mtrl.Ambient.g = _material.g;
	mtrl.Diffuse.b = mtrl.Ambient.b = _material.b;
	mtrl.Diffuse.a = mtrl.Ambient.a = _material.a;
	g_pd3dDevice->SetMaterial(&mtrl);
	return S_OK;
}