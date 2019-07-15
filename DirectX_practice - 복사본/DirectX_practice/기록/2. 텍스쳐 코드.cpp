#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#define SHOW_HOW_TO_USE_TCI

struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
#ifndef SHOW_HOW_TO_USE_TCI
	FLOAT tu, tv;				//texture ��ǥ
#endif
};
#ifdef SHOW_HOW_TO_USE_TCI
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE)
#else
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE | D3DFVF_TEX1)
#endif
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice	IDriect3D ��ü����(COM)
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device	IDirect3DDevice9
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;								//vertex ����
LPDIRECT3DTEXTURE9 g_pTexture = NULL;								//texture ����


//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))					//IDriect3D ��ü����(COM) ->IDiret3Ddevice9�����ϸ� �ʿ����
		return E_FAIL;


	D3DPRESENT_PARAMETERS d3dpp;													//����̽� ������ ���� ����ü
	ZeroMemory(&d3dpp, sizeof(d3dpp));											//����ü ���� - �ݵ���ؾ���
	d3dpp.Windowed = TRUE;															//â���
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//������ ������Ʈ �׸��Ŷ� Z���� �ʿ��� - �Ʒ��� �����ϴµ� �̰ǹ��� ����
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,		//����͹�ȣ, ��µ���̽�(�ϵ���� ���� ���� ����̽�), ��Ŀ�� ������			
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,			//���� ���̴� SW����
		&d3dpp, &g_pd3dDevice)))					//����ü ������, IDirect3DDevice9 ����
	{
		return E_FAIL;
	}
	// Turn off culling, so we see the front and back of the triangle
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ������ �ִ� ������ ����Ҷ��� ������� ����
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Z���� ��� ON
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

HRESULT InitGeometry()
{
	//banana.bmp�� �̿��ؼ� �ؽ��� �����Ѵ�
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"bananaaa.bmp", &g_pTexture)))
	{
		MessageBox(NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK);
		return E_FAIL;
	}

	
	//CreateVertexBuffer(������, ����ó�����-SW/HW, FVF�÷���, �������� �޸� ��ġ, ��ȯ�� ������ �������̽�) -> �� �ڵ�� GPU�޸𸮿� ����
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//���λ����� �������۴� �����Ⱚ -> Lock���� ������ �����ͼ� ������ ä���� Unlock ����
	CUSTOMVERTEX* pVertices;

	//Lock (Lock�� ������ ������, Lock�� ���� ũ��, �а� �����ִ� �޸𸮿��� ������, Lock�Ҷ� �Բ� ����ϴ� �÷���) -> �׷� ���⼭ pVertices�� GPU�޸��ΰ�?
	//���۰� ũ�� ��� 0 �̸� ������ü
	if (FAILED(g_pVB->Lock(0, 0, (void**)& pVertices, 0)))
		return E_FAIL;

	for (DWORD i = 0; i < 50; i++)
	{
		FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
		pVertices[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.f, cosf(theta));	//�Ʒ��� ������ǥ
		pVertices[2 * i + 0].color = 0xffff0000;

#ifndef SHOW_HOW_TO_USE_TCI																//SHOW_HOW_TO_USE_TCI�� ����Ǿ� ������ �ؽ�����ǥ ���� X -> �ؽ��� ��� X
		pVertices[2 * i + 0].tu = ((FLOAT)i) / (50 - 1);								//�ؽ����� u��ǥ
		pVertices[2 * i + 0].tv = 1.f;													//�ؽ����� v��ǥ
#endif

		pVertices[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.f, cosf(theta));		//���� ������ǥ
		pVertices[2 * i + 1].color = 0xffffffff;

#ifndef SHOW_HOW_TO_USE_TCI
		pVertices[2 * i + 1].tu = ((FLOAT)i) / (50 - 1);								//�ؽ����� u��ǥ
		pVertices[2 * i + 1].tv = 0.f;													//�ؽ����� v��ǥ
#endif
	}

	//Lock�� Unlock�� ������ �����ؾ���
	g_pVB->Unlock();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pTexture != NULL)
		g_pTexture->Release();

	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

VOID SetupMatrices()
{
	//�������
	D3DXMATRIXA16 matWorld;
	//D3DXMatrixIdentity(&matWorld);								//���ڱ� �̰� ��? ���ص� �ߵǴ���
	D3DXMatrixRotationX(&matWorld, timeGetTime() / 1000.f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	//UINT iTime = timeGetTime() % 1000;						// float���� ���е� ���ؼ� ����������
	//FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;		// 1�ʸ��� �ѹ����� ȸ���ϴ� ���
	//D3DXMatrixRotationY(&matWorld, fAngle);					// Y���� ȸ�������� ȸ�� ����� ����
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);		// ������ ȸ������� ���� ��ķ� ����̽��� ����


	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);						// ����ġ
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);					// ���� �ٶ󺸴� ��ġ
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);						// õ�� ������ ��Ÿ���� ����
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);		// 1, 2, 3�� ������ �� ��� ����
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);				// ������ �� ����� ����̽��� ����

	//�������� ��� ����: �þ߰�(Field Of View), ��Ⱦ��(aspect ratio), Ŭ���� ��� ��
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);		//(�Է��� �������� ���, �þ߰�(45��), ��Ⱦ��, ���� ~ ���Ÿ� Ŭ���� ���
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);					//�������� ����� ����̽��� ����
}

VOID SetupLights()
{
	//����(material) ���� (Ȯ�걤 + �ֺ���)- ����̽��� �ϳ��� ��������
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.f;
	g_pd3dDevice->SetMaterial(&mtrl);

	/*
	������ DrawPrimitive() ȣ�� �ÿ� �ϳ��� ��������
	�޽ð� �������� ������ �̷���� �ִٸ� �޽ø� �������� �и��ؾ��� - 2-6���� �޽ôٷ궧 �ٽú���
	*/


	//���⼺ ���� ����
	D3DXVECTOR3 vecDir;													//���� ����
	D3DLIGHT9 light;													//���� ����ü
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;									//������ ���� (��, ���⼺, ���� ���ð���)
	light.Diffuse.r = 1.f;
	light.Diffuse.g = 1.f;
	light.Diffuse.b = 1.f;
	vecDir = D3DXVECTOR3(cosf(timeGetTime() / 350.f),
		1.f, sinf(timeGetTime() / 350.f));		//��������
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);		//���� ���� ��������

	light.Range = 1000.f;												//���� �ٴٸ� �� �ִ� �ִ����
	g_pd3dDevice->SetLight(0, &light);									//����̽��� 0������ ����
	g_pd3dDevice->LightEnable(0, TRUE);									//0�� ���� ���
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);					//�������� ON

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x000f0f0f);			//ȯ�汤 ����
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;

	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);			//ȭ�� �� ������ �ʱ�ȭ

	//SetupLights();
	SetupMatrices();																		//����, ��, �������� ��� ����


	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//������ �׸��ٰ� �˸���
	{
		g_pd3dDevice->SetTexture(0, g_pTexture);												
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);


		//TCI(Texture Coord Index) �ؽ��� ��ǥ �ε����� ����Ͽ� x, y, z ��ǥ�� u, v �ؽ�ó��ǥ�� ��ȯ�Ѵ�.
#ifdef SHOW_HOW_TO_USE_TCI	
		D3DXMATRIXA16 mat;
		mat._11 = 0.25f;	mat._12 = 0.00f;	mat._13 = 0.00f;	mat._14 = 0.00f;
		mat._21 = 0.00f;	mat._22 = -0.25f;	mat._23 = 0.00f;	mat._24 = 0.00f;
		mat._31 = 0.00f;	mat._32 = 0.00f;	mat._33 = 1.00f;	mat._34 = 0.00f;
		mat._41 = 0.50f;	mat._42 = 0.50f;	mat._43 = 0.00f;	mat._44 = 1.00f;

		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &mat);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
#endif

		// �ﰢ�� �׸��� ������
		//1. ���������� ����ִ� �������۸� ��� ��Ʈ������ �Ҵ�									�������� ����̽��� ����
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//2. D3D���� ���� ���̴� ���� ���� - ��κ� FVF�� ����										���� ������ ����̽��� ����
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//3. �������� ����ϴ� DrawPrimitive() �Լ� ȣ��											���� ���� �������� DrawIndexedPrimitive()								���������� ������ �׸�
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);

		// End the scene
		g_pd3dDevice->EndScene();																	//������ �� �׸�
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);												//ȭ�鿡 ��Ÿ���� (�ĸ������ ������ ������۷� ����)
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"D3D Tutorial", NULL
	};
	RegisterClassEx(&wc);

	// Create the application's window
	HWND hWnd = CreateWindow(L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice",
		WS_OVERLAPPEDWINDOW, 100, 100, 400, 400,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//���� ���� �ʱ�ȭ
		if (SUCCEEDED(InitGeometry()))
		{
			// Show the window ������ ���
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			// Enter the message loop
			MSG msg;							//�޼��� ������ OS�� �޼����� ������ ��� ����� �����쿡 �����°�
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)			//���� �޼��� �ƴϸ� ��ӵ���
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{	//�޼��� �߰��ϸ� �޼���ó��
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					Render();
				}
			}
		}
	}

	UnregisterClass(L"D3D Tutorial", wc.hInstance);
	return 0;
}
