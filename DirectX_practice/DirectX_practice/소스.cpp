#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#define OBC 3



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice	IDriect3D ��ü����(COM)
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device	IDirect3DDevice9
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;								//vertex ����

struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ| D3DFVF_DIFFUSE)

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


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,		//����͹�ȣ, ��µ���̽�(�ϵ���� ���� ���� ����̽�), ��Ŀ�� ������			
		D3DCREATE_HARDWARE_VERTEXPROCESSING,			//���� ���̴� SW����
		&d3dpp, &g_pd3dDevice)))					//����ü ������, IDirect3DDevice9 ����
	{
		return E_FAIL;
	}
	// Turn off culling, so we see the front and back of the triangle
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Turn off D3D lighting, since we are providing our own vertex colors
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

LRESULT InitVB()
{

	CUSTOMVERTEX vertices[] =
	{
		{ -1.0f,-1.0f, 0.0f,0xffff0000, },
		{  1.0f,-1.0f, 0.0f, 0xff0000ff, },
		{  0.0f, 1.0f, 0.0f, 0xffffffff, },
	};

	//���� ���� ����
	//CreateVertexBuffer(������, ����ó�����-SW/HW, FVF�÷���, �������� �޸� ��ġ, ��ȯ�� ������ �������̽�) -> �� �ڵ�� GPU�޸𸮿� ����
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(OBC * sizeof(CUSTOMVERTEX), 
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//���λ����� �������۴� �����Ⱚ -> Lock���� ������ �����ͼ� ������ ä���� Unlock ����
	VOID* pVertices;

	//Lock (Lock�� ������ ������, Lock�� ���� ũ��, �а� �����ִ� �޸𸮿��� ������, Lock�Ҷ� �Բ� ����ϴ� �÷���) -> �׷� ���⼭ pVertices�� GPU�޸��ΰ�?
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)& pVertices, 0)))
		return E_FAIL;

	//�������� ����
	memcpy(pVertices, vertices, sizeof(vertices));

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

	UINT iTime = timeGetTime() % 1000;						// float���� ���е� ���ؼ� ����������
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;		// 1�ʸ��� �ѹ����� ȸ���ϴ� ���
	D3DXMatrixRotationY(&matWorld, fAngle);					// Y���� ȸ�������� ȸ�� ����� ����
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);		// ������ ȸ������� ���� ��ķ� ����̽��� ����

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


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;

	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);			//ȭ�� �� ������ �ʱ�ȭ

	SetupMatrices();																		//����, ��, �������� ��� ����

	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//������ �׸��ٰ� �˸���
	{


		// �ﰢ�� �׸��� ������
		//1. ���������� ����ִ� �������۸� ��� ��Ʈ������ �Ҵ�									�������� ����̽��� ����
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//2. D3D���� ���� ���̴� ���� ���� - ��κ� FVF�� ����										���� ������ ����̽��� ����
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//3. �������� ����ϴ� DrawPrimitive() �Լ� ȣ��											���� ���� �������� DrawIndexedPrimitive()								���������� ������ �׸�
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

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
	//case WM_PAINT:
	//	Render();
	//	ValidateRect(hWnd, NULL);
	//	return 0;
	//}

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
		WS_OVERLAPPEDWINDOW, 100, 100, 256, 256,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//���� ���� �ʱ�ȭ
		if (SUCCEEDED(InitVB()))
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



