#include "Engine.h"



Engine::Engine()
{
	eye.x = 100.0f;
	eye.y = 250.0f;
	eye.z = -400.0f;
}
Engine::~Engine()
{
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
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

HRESULT Engine::InitObj()
{
	testTiger.Create(g_pd3dDevice);
	for (int i = 0; i < 5; i++) 
	{
		if (FAILED(test[i].Create(g_pd3dDevice)))
		{	
			return E_FAIL;
		}
		else
		{

			D3DCOLORVALUE material = { 0, (i * 50) / 255.f, 0, 1 };
			test[i].SetMaterial(material);

			D3DXMATRIXA16 t, s;
			D3DXMatrixScaling(&s, 30.f, 30.f, 30.f);
			test[i].SetScale(s);
			D3DXMatrixTranslation(&t, 130 - i * 60, 80, -70);
			test[i].SetTranslation(t);
		}
		if (FAILED(testCube[i].Create(g_pd3dDevice)))
		{
			return E_FAIL;
		}
		else
		{
			D3DCOLORVALUE material = { (i * 50) / 255.f, 0, 0, 1 };
			testCube[i].SetMaterial(material);

			D3DXMATRIXA16 t, s;
			D3DXMatrixScaling(&s, 30.f, 30.f, 30.f);
			testCube[i].SetScale(s);
			D3DXMatrixTranslation(&t, 160- i * 80, -80, -70);
			testCube[i].SetTranslation(t);
		}
	}
	

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
	InitView();


	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		testTiger.DrawObj(g_pd3dDevice);
		for (int i = 0; i < 5; i++)
		{
			test[i].DrawObj(g_pd3dDevice);
			testCube[i].DrawObj(g_pd3dDevice);
		}
		
		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

VOID Engine::InitLight()
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

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

}

VOID Engine::InitView()
{
	D3DXVECTOR3 vEyePt(eye.x, eye.y, eye.z);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}