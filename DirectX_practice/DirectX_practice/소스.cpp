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
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice	IDriect3D 객체생성(COM)
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device	IDirect3DDevice9
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;								//vertex 버퍼

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
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))					//IDriect3D 객체생성(COM) ->IDiret3Ddevice9생성하면 필요없음
		return E_FAIL;


	D3DPRESENT_PARAMETERS d3dpp;													//디바이스 생성을 위한 구조체
	ZeroMemory(&d3dpp, sizeof(d3dpp));											//구조체 비우기 - 반드시해야함
	d3dpp.Windowed = TRUE;															//창모드
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,		//모니터번호, 출력디바이스(하드웨어 가속 지원 디바이스), 포커스 윈도우			
		D3DCREATE_HARDWARE_VERTEXPROCESSING,			//정점 셰이더 SW가속
		&d3dpp, &g_pd3dDevice)))					//구조체 포인터, IDirect3DDevice9 받음
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

	//정점 버퍼 생성
	//CreateVertexBuffer(사이즈, 버퍼처리방식-SW/HW, FVF플래그, 정점버퍼 메모리 위치, 반환될 버퍼의 인터페이스) -> 이 코드는 GPU메모리에 보관
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(OBC * sizeof(CUSTOMVERTEX), 
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//새로생성된 정점버퍼는 쓰레기값 -> Lock으로 포인터 가져와서 데이터 채운후 Unlock 하자
	VOID* pVertices;

	//Lock (Lock할 버퍼의 시작점, Lock할 버퍼 크기, 읽고 쓸수있는 메모리영역 포인터, Lock할때 함께 사용하는 플래그) -> 그럼 여기서 pVertices는 GPU메모리인가?
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)& pVertices, 0)))
		return E_FAIL;

	//정점정보 복사
	memcpy(pVertices, vertices, sizeof(vertices));

	//Lock과 Unlock은 쌍으로 존재해야함
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
	//월드행렬
	D3DXMATRIXA16 matWorld;

	UINT iTime = timeGetTime() % 1000;						// float연산 정밀도 위해서 나머지연산
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;		// 1초마다 한바퀴씩 회전하는 행렬
	D3DXMatrixRotationY(&matWorld, fAngle);					// Y축을 회전축으로 회전 행렬을 생성
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);		// 생성한 회전행렬을 월드 행렬로 디바이스에 설정

	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);						// 눈위치
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


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;

	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);			//화면 한 색으로 초기화

	SetupMatrices();																		//월드, 뷰, 프로젝션 행렬 설정

	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//폴리곤 그린다고 알리기
	{


		// 삼각형 그리기 시작함
		//1. 정점정보가 담겨있는 정정버퍼를 출력 스트림으로 할당									정점버퍼 디바이스에 결함
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//2. D3D에게 정점 셰이더 정보 지정 - 대부분 FVF만 지정										정점 포맷을 디바이스에 지정
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//3. 기하정보 출력하는 DrawPrimitive() 함수 호출											가장 성능 좋은것은 DrawIndexedPrimitive()								정점버퍼의 폴리곤 그림
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		// End the scene
		g_pd3dDevice->EndScene();																	//폴리곤 다 그림
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
		//정점 버퍼 초기화
		if (SUCCEEDED(InitVB()))
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



