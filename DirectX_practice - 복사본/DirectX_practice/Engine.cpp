#include "Engine.h"


Engine::Engine()
{

}
Engine::~Engine()
{

}

HRESULT Engine::InitD3D(HWND hWnd)
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	return S_OK;
}
HRESULT Engine::InitVB()
{
	CUSTOMVERTEX g_Vertices[] =
	{
		{  D3DXVECTOR3(0.0f, 50.0f, 0.0f),		D3DXVECTOR3(0.0f, 50.0f, 0.0f), },	//첫번째 삼각형
		{  D3DXVECTOR3(-50.0f,  0.0f, 0.0f),	D3DXVECTOR3(0, 0.33f, 0), },
		{  D3DXVECTOR3(50.0f,  0.0f, 0.0f),		D3DXVECTOR3(0, 0, 0.33f), },


	};

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
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
HRESULT Engine::InitIB()
{
	MYINDEX indices[] =
	{
		//첫번째 삼각형 인덱스
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

VOID Engine::Render()
{
	D3DXMATRIXA16 matWorld, translation, rotation, scale;
	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

	InitLight();
	SetupMatrices();


	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//정점버퍼 스트림 설정
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->SetIndices(g_pIB);


		//0번 삼각형
		InitMaterial();
		D3DXMatrixRotationX(&rotation, fAngle);
		D3DXMatrixTranslation(&translation, -50, -50, -50);
		matWorld = rotation * translation;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

HRESULT Engine::InitMaterial()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));

	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	g_pd3dDevice->SetMaterial(&mtrl);



	return TRUE;
}
HRESULT Engine::InitLight()
{
	D3DXVECTOR3 vecDir = D3DXVECTOR3(1, 1, 1);
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 0.5f;
	light.Diffuse.g = 0.5f;
	light.Diffuse.b = 0.5f;
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);
	light.Range = 1000.0f;
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Finally, turn on some ambient light.
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xff505050);
	return TRUE;

}

VOID Engine::SetupMatrices()
{
	D3DXVECTOR3 vEyePt(100.0f, 250.0f, -400.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}