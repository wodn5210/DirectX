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
	FLOAT tu, tv;				//texture 좌표
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
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice	IDriect3D 객체생성(COM)
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device	IDirect3DDevice9
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;								//vertex 버퍼
LPDIRECT3DTEXTURE9 g_pTexture = NULL;								//texture 버퍼


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

	//복잡한 오브젝트 그릴거라 Z버퍼 필요함 - 아래서 설정하는데 이건뭔지 몰겠
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

	// 정점에 있는 색값을 사용할때는 광원기능 끈다
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Z버퍼 기능 ON
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

HRESULT InitGeometry()
{
	//banana.bmp를 이용해서 텍스쳐 생성한다
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"bananaaa.bmp", &g_pTexture)))
	{
		MessageBox(NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK);
		return E_FAIL;
	}

	
	//CreateVertexBuffer(사이즈, 버퍼처리방식-SW/HW, FVF플래그, 정점버퍼 메모리 위치, 반환될 버퍼의 인터페이스) -> 이 코드는 GPU메모리에 보관
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//새로생성된 정점버퍼는 쓰레기값 -> Lock으로 포인터 가져와서 데이터 채운후 Unlock 하자
	CUSTOMVERTEX* pVertices;

	//Lock (Lock할 버퍼의 시작점, Lock할 버퍼 크기, 읽고 쓸수있는 메모리영역 포인터, Lock할때 함께 사용하는 플래그) -> 그럼 여기서 pVertices는 GPU메모리인가?
	//시작과 크기 모두 0 이면 버퍼전체
	if (FAILED(g_pVB->Lock(0, 0, (void**)& pVertices, 0)))
		return E_FAIL;

	for (DWORD i = 0; i < 50; i++)
	{
		FLOAT theta = (2 * D3DX_PI * i) / (50 - 1);
		pVertices[2 * i + 0].position = D3DXVECTOR3(sinf(theta), -1.f, cosf(theta));	//아래쪽 원통좌표
		pVertices[2 * i + 0].color = 0xffff0000;

#ifndef SHOW_HOW_TO_USE_TCI																//SHOW_HOW_TO_USE_TCI가 선언되어 있으면 텍스쳐좌표 생성 X -> 텍스쳐 사용 X
		pVertices[2 * i + 0].tu = ((FLOAT)i) / (50 - 1);								//텍스쳐의 u좌표
		pVertices[2 * i + 0].tv = 1.f;													//텍스쳐의 v좌표
#endif

		pVertices[2 * i + 1].position = D3DXVECTOR3(sinf(theta), 1.f, cosf(theta));		//위쪽 원통좌표
		pVertices[2 * i + 1].color = 0xffffffff;

#ifndef SHOW_HOW_TO_USE_TCI
		pVertices[2 * i + 1].tu = ((FLOAT)i) / (50 - 1);								//텍스쳐의 u좌표
		pVertices[2 * i + 1].tv = 0.f;													//텍스쳐의 v좌표
#endif
	}

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
	//월드행렬
	D3DXMATRIXA16 matWorld;
	//D3DXMatrixIdentity(&matWorld);								//갑자기 이거 왜? 안해도 잘되던데
	D3DXMatrixRotationX(&matWorld, timeGetTime() / 1000.f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	//UINT iTime = timeGetTime() % 1000;						// float연산 정밀도 위해서 나머지연산
	//FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;		// 1초마다 한바퀴씩 회전하는 행렬
	//D3DXMatrixRotationY(&matWorld, fAngle);					// Y축을 회전축으로 회전 행렬을 생성
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);		// 생성한 회전행렬을 월드 행렬로 디바이스에 설정


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

VOID SetupLights()
{
	//재질(material) 설정 (확산광 + 주변광)- 디바이스에 하나만 설정가능
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.f;
	g_pd3dDevice->SetMaterial(&mtrl);

	/*
	재질은 DrawPrimitive() 호출 시에 하나만 지정가능
	메시가 여러개의 재질로 이루어져 있다면 메시를 재질별로 분리해야함 - 2-6에서 메시다룰때 다시보자
	*/


	//방향성 광원 설정
	D3DXVECTOR3 vecDir;													//빛의 방향
	D3DLIGHT9 light;													//광원 구조체
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;									//광원의 종류 (점, 방향성, 점적 선택가능)
	light.Diffuse.r = 1.f;
	light.Diffuse.g = 1.f;
	light.Diffuse.b = 1.f;
	vecDir = D3DXVECTOR3(cosf(timeGetTime() / 350.f),
		1.f, sinf(timeGetTime() / 350.f));		//광선방향
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);		//광선 방향 단위벡터

	light.Range = 1000.f;												//광원 다다를 수 있는 최대길이
	g_pd3dDevice->SetLight(0, &light);									//디바이스에 0번광원 설정
	g_pd3dDevice->LightEnable(0, TRUE);									//0번 광원 사용
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);					//광원설정 ON

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x000f0f0f);			//환경광 설정
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
		D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);			//화면 한 색으로 초기화

	//SetupLights();
	SetupMatrices();																		//월드, 뷰, 프로젝션 행렬 설정


	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//폴리곤 그린다고 알리기
	{
		g_pd3dDevice->SetTexture(0, g_pTexture);												
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);


		//TCI(Texture Coord Index) 텍스쳐 좌표 인덱스를 사용하여 x, y, z 좌표를 u, v 텍스처좌표로 변환한다.
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

		// 삼각형 그리기 시작함
		//1. 정점정보가 담겨있는 정정버퍼를 출력 스트림으로 할당									정점버퍼 디바이스에 결함
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//2. D3D에게 정점 셰이더 정보 지정 - 대부분 FVF만 지정										정점 포맷을 디바이스에 지정
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//3. 기하정보 출력하는 DrawPrimitive() 함수 호출											가장 성능 좋은것은 DrawIndexedPrimitive()								정점버퍼의 폴리곤 그림
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);

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
		//정점 버퍼 초기화
		if (SUCCEEDED(InitGeometry()))
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
