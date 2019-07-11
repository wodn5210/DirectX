#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

struct MYINDEX
{
	WORD _0, _1, _2;
};
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL;			//D3D 디바이스 생성할 D3D객체 변수
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL;	//렌더링에 사용될 D3D디바이스
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;		//정점 버퍼
LPDIRECT3DINDEXBUFFER9 g_pIB = NULL;			//인덱스 버퍼


//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	// D3D객체 생성
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))					//IDriect3D 객체생성(COM) ->IDiret3Ddevice9생성하면 필요없음
		return E_FAIL;


	D3DPRESENT_PARAMETERS d3dpp;													//디바이스 생성을 위한 구조체
	ZeroMemory(&d3dpp, sizeof(d3dpp));											//구조체 비우기 - 반드시해야함
	d3dpp.Windowed = TRUE;															//창모드
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//복잡한 오브젝트 그릴거라 Z버퍼 필요함 - 깊이처리부분
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,		//모니터번호, 출력디바이스(하드웨어 가속 지원 디바이스), 포커스 윈도우			
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,			//정점 셰이더 SW가속
		&d3dpp, &g_pd3dDevice)))					//구조체 포인터, IDirect3DDevice9 받음
	{
		return E_FAIL;
	}
	// Turn off culling, so we see the front and back of the triangle
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Z버퍼 기능 ON
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	//광원기능 off
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);

	return S_OK;
}


/*
정점 버퍼 생성 -> 정점 값 채워 넣기
*/
HRESULT InitVB()
{
	//큐브 정점 선언
	CUSTOMVERTEX vertices[] =
	{
		{-1, 1, 1, 0xffff0000},		//v0
		{1, 1, 1, 0xff00ff00},		//v1
		{1, 1, -1, 0xff0000ff},		//v2
		{-1, 1, -1, 0xffffff00},	//v3

		{-1, -1, 1, 0xff00ffff},	//v4
		{1, -1, 1, 0xffff00ff},		//v5
		{1, -1, -1, 0xff000000},	//v6
		{-1, -1, -1, 0xffffffff},	//v7
	};

	//정점 버퍼 생성 - 8개의 정점 보관할 메모리 생성 - FVF로 데이터 형식 지정
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX), 
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//정점 버퍼를 값으로 채운다 - Lock호출해서 포인터 얻어온다
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)& pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVB->Unlock();


	//정점 버퍼 생성 - 8개의 정점을 보관할 메모리 할당 - FVF로 보관할 데이터 형식 지정
	return S_OK;
}


HRESULT InitIB()
{
	MYINDEX indices[] =
	{
	{0, 1, 2}, {0, 2, 3},	//위
	{4, 6, 5}, {4, 7, 6},	//아래
	{0, 3, 7}, {0, 7, 4},	//왼
	{1, 5, 6}, {1, 6, 2},	//오
	{3, 2, 6}, {3, 6, 7},	//앞
	{0, 4, 5}, {0, 5, 1}	//뒤
	};

	//인덱스 버퍼 생성													WORD형 선언이기때문에 INDEX16
	if (FAILED(g_pd3dDevice->CreateIndexBuffer(12 * sizeof(MYINDEX), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
	{
		return E_FAIL;
	}

	//인덱스 버퍼를 값으로 채우자
	//인덱스 버퍼의 LOCK 함수 호출하여 포인터 얻어옴
	VOID* pIndices;
	if (FAILED(g_pIB->Lock(0, sizeof(indices), (void**)& pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, indices, sizeof(indices));
	g_pIB->Unlock();

	return S_OK;
}
//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pIB != NULL)
		g_pIB->Release();

	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

VOID SetupMatrices()
{
	//월드행렬
	D3DXMATRIXA16 matWorld;
	//D3DXMatrixIdentity(&matWorld);								//갑자기 이거 왜? 안해도 잘되던데
	D3DXMatrixRotationY(&matWorld, GetTickCount() / 500.f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


	D3DXVECTOR3 vEyePt(5.0f, 5.0f, -5.0f);						// 눈위치
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);					// 눈이 바라보는 위치
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);						// 천장 방향을 나타내는 벡터
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);		// 1, 2, 3의 값으로 뷰 행렬 생성
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);				// 생성한 뷰 행렬을 디바이스에 설정

	//프로젝션 행렬 정의: 시야각(Field Of View), 종횡비(aspect ratio), 클리핑 평면 값
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);		//(입력할 프로젝션 행렬, 시야각(45도), 종횡비, 근접 ~ 원거리 클리핑 평면
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);					//프로젝션 행렬을 디바이스에 설정
}

VOID SetupLights()
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.f;
	g_pd3dDevice->SetMaterial(&mtrl);

	//방향성 광원 설정
	D3DXVECTOR3 vecDir;													//빛의 방향
	D3DLIGHT9 light;													//광원 구조체
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;									//광원의 종류 (점, 방향성, 점적 선택가능)
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
	vecDir = D3DXVECTOR3(cosf(timeGetTime() / 350.f),
		1.f, sinf(timeGetTime() / 350.f));		//광선방향
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);		//광선 방향 단위벡터

	light.Range = 1000.f;												//광원 다다를 수 있는 최대길이
	g_pd3dDevice->SetLight(0, &light);									//디바이스에 0번광원 설정
	g_pd3dDevice->LightEnable(0, TRUE);									//0번 광원 사용
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);					//광원설정 ON

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);			//환경광 설정
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
		D3DCOLOR_XRGB(0, 0, 128), 1.0f, 0);			//화면 한 색으로 초기화

	//SetupLights();
	SetupMatrices();																		//월드, 뷰, 프로젝션 행렬 설정


	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//폴리곤 그린다고 알리기
	{
		//정점 버퍼의 삼각형 그리기

		//1. 정점정보가 담겨있는 정점 버퍼를 출력 스트림으로 할당
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));

		//2. D3D에게 정점 셰이더 정보를 지정, 대부분의 경우에는 FVF만 지정
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

		//3. 인덱스 버퍼 지정
		g_pd3dDevice->SetIndices(g_pIB);

		//4. DrawIndexedPrimitive() 호출
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);

		g_pd3dDevice->EndScene();														//폴리곤 다 그림
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);												//화면에 나타내기 (후면버퍼의 내용을 전면버퍼로 전송)
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
		WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//정점 버퍼 초기화
		if (SUCCEEDED(InitVB()) && SUCCEEDED(InitIB()))
		{
			// Show the window 윈도우 출력
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			// Enter the message loop
			MSG msg;							//메세지 루프는 OS가 메세지를 보냈을 경우 사용자 윈도우에 보내는것
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)			//종료 메세지 아니면 계속돈다
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{	//메세지 발견하면 메세지처리
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