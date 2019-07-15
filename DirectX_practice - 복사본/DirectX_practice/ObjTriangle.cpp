#include "ObjTriangle.h"


ObjTriangle::ObjTriangle()
{
	_fvf = D3DFVF_XYZRHW | D3DFVF_NORMAL;
	
}
ObjTriangle::~ObjTriangle()
{
	if (_pIB != NULL)
		_pIB->Release();

	if (_pVB != NULL)
		_pVB->Release();
}

HRESULT ObjTriangle::create(LPDIRECT3DDEVICE9 _pd3dDevice,
	D3DXVECTOR3 center)
{

	if (FAILED(InitVB(_pd3dDevice, center)))
	{
		return E_FAIL;
	}
	/*if (FAILED(InitIB(_pd3dDevice)))
	{
		return E_FAIL;
	}
*/
	return S_OK;
}


void ObjTriangle::drawObj(LPDIRECT3DDEVICE9 _pd3dDevice)
{
	InitMtrl();
	_pd3dDevice->SetStreamSource(0, _pVB, 0, sizeof(CUSTOMVERTEX));
	_pd3dDevice->SetFVF(_fvf);
	_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}

HRESULT ObjTriangle::InitMtrl(D3DCOLORVALUE material) 
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	return S_OK;
}

HRESULT ObjTriangle::InitVB(LPDIRECT3DDEVICE9 _pd3dDevice, D3DXVECTOR3 center) 
{
	CUSTOMVERTEX g_Vertices[] =
	{
		{	D3DXVECTOR3(-10.0f,-10.0f, 0.0f),			D3DXVECTOR3(0, 1.f, 0)},		//v0
		{	D3DXVECTOR3(10.0f,-10.0f, 0.0f),			D3DXVECTOR3(1.f, 0, 0)},		//v1
		{	D3DXVECTOR3(0.0f, 10.0f, 10.0f),			D3DXVECTOR3(-1.f, 0, 0)},		//v2
	};
	if (FAILED(_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, _fvf, D3DPOOL_DEFAULT, &_pVB, NULL)))
	{
		return E_FAIL;
	}

	// Fill the vertex buffer.
	VOID* pVertices;
	if (FAILED(_pVB->Lock(0, sizeof(g_Vertices), (void**)& pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	_pVB->Unlock();

	return S_OK;
}

HRESULT ObjTriangle::InitIB(LPDIRECT3DDEVICE9 _pd3dDevice)
{
	MYINDEX indices[] =
	{
		{0, 1, 2}
	};
	if (FAILED(_pd3dDevice->CreateIndexBuffer(1 * sizeof(MYINDEX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &_pIB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(_pIB->Lock(0, sizeof(indices), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, indices, sizeof(indices));
	_pIB->Unlock();

	return S_OK;
}