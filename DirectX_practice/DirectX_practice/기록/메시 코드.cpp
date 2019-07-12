#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#define SHOW_HOW_TO_USE_TCI


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice	IDriect3D ��ü����(COM)
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device	IDirect3DDevice9

LPD3DXMESH			g_pMesh = NULL;
D3DMATERIAL9*		g_pMeshMaterials = NULL;
LPDIRECT3DTEXTURE9* g_pMeshTextures = NULL;
DWORD				g_dwNumMaterials = 0L;


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

	//������ ������Ʈ �׸��Ŷ� Z���� �ʿ��� - ����ó���κ�
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,		//����͹�ȣ, ��µ���̽�(�ϵ���� ���� ���� ����̽�), ��Ŀ�� ������			
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,			//���� ���̴� SW����
		&d3dpp, &g_pd3dDevice)))					//����ü ������, IDirect3DDevice9 ����
	{
		return E_FAIL;
	}
	//// Turn off culling, so we see the front and back of the triangle
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Z���� ��� ON
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

HRESULT InitGeometry()
{
	/*
	��� �޽� ������ �������� ���������� �����Ǿ� �ְ�, �������� ������ ������ ������ ����
	������ �������� ��� �޽ø� �������� �κи޽÷� ����
	���ҵ� �κи޽ô� �κи޽ú��� ���� �׸� -> DrawSubset()
	*/

	//���� �ӽ� ���� ����
	LPD3DXBUFFER pD3DXMtrlBuffer;

	//tiger.x���� �޽������� �������� �д´�
	if (FAILED(D3DXLoadMeshFromX(L"Tiger.x", D3DXMESH_SYSTEMMEM, 
		g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh)))
	{
		MessageBox(NULL, L"Could not find Tiger.x", L"Mesh.exe", MB_OK);
		return E_FAIL;
	}

	//���������� �ؽ��� ������ ���� �̾Ƴ���.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();	
	//���� ������ŭ ���� ����ü �迭 ����
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	//���� ������ŭ �ؽ��� �迭 ����
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	
	for (DWORD i = 0; i < g_dwNumMaterials; i++) 
	{		
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;					//���� ���� ����
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;		//�ֺ����� ������ Diffuse ������
		g_pMeshTextures[i] = NULL;										
																 
		if (d3dxMaterials[i].pTextureFilename != NULL && lstrlen((LPCWSTR)d3dxMaterials[i].pTextureFilename) > 0)
		{
			//�ؽ��ĸ� ���Ͽ��� �ε��Ѵ�
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
				L"Tiger.bmp", &g_pMeshTextures[i])))
			{
				MessageBox(NULL, L"Could not find texture map", L"Mesh.exe", MB_OK);
				return E_FAIL;
			}
		}
	}

	//�ӽû��� �������� ����
	pD3DXMtrlBuffer->Release();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pMeshMaterials != NULL)
		delete[] g_pMeshMaterials;

	if (g_pMeshTextures)
	{
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			if (g_pMeshTextures[i])
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if (g_pMesh != NULL)
		g_pMesh->Release();

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
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


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
	////����(material) ���� (Ȯ�걤 + �ֺ���)- ����̽��� �ϳ��� ��������
	//D3DMATERIAL9 mtrl;
	//ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	//mtrl.Diffuse.r = mtrl.Ambient.r = 1.f;
	//mtrl.Diffuse.g = mtrl.Ambient.g = 1.f;
	//mtrl.Diffuse.b = mtrl.Ambient.b = 1.f;
	//g_pd3dDevice->SetMaterial(&mtrl);

	/*
	������ DrawPrimitive() ȣ�� �ÿ� �ϳ��� ��������
	�޽ð� �������� ������ �̷���� �ִٸ� �޽ø� �������� �и��ؾ��� - 2-6���� �޽ôٷ궧 �ٽú���
	x���Ͽ��� �������� ������ �����ص� �ɵ�?
	*/


	//���⼺ ���� ����
	D3DXVECTOR3 vecDir;													//���� ����
	D3DLIGHT9 light;													//���� ����ü
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;									//������ ���� (��, ���⼺, ���� ���ð���)
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
	vecDir = D3DXVECTOR3(cosf(timeGetTime() / 350.f),
		1.f, sinf(timeGetTime() / 350.f));		//��������
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);		//���� ���� ��������

	light.Range = 1000.f;												//���� �ٴٸ� �� �ִ� �ִ����
	g_pd3dDevice->SetLight(0, &light);									//����̽��� 0������ ����
	g_pd3dDevice->LightEnable(0, TRUE);									//0�� ���� ���
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);					//�������� ON

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xaaaaaaaa);			//ȯ�汤 ����
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
		D3DCOLOR_XRGB(0, 0, 128), 1.0f, 0);			//ȭ�� �� ������ �ʱ�ȭ

	SetupLights();
	SetupMatrices();																		//����, ��, �������� ��� ����


	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//������ �׸��ٰ� �˸���
	{
		/*
		�޽ø� �׸��� ������ �� �޽ÿ� �ش��ϴ� �ؽ�ó�� ������ �����Ѵ�.
		*/
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			//�κ����� �޽��� ������ �ؽ��� ����
			g_pd3dDevice->SetMaterial(&g_pMeshMaterials[i]);		
			g_pd3dDevice->SetTexture(0, g_pMeshTextures[i]);		
			
			//�κ����� �޽� ���
			g_pMesh->DrawSubset(i);
		}
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
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

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