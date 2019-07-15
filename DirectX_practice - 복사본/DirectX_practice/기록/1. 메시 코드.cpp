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
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice	IDriect3D 객체생성(COM)
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
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))					//IDriect3D 객체생성(COM) ->IDiret3Ddevice9생성하면 필요없음
		return E_FAIL;


	D3DPRESENT_PARAMETERS d3dpp;													//디바이스 생성을 위한 구조체
	ZeroMemory(&d3dpp, sizeof(d3dpp));											//구조체 비우기 - 반드시해야함
	d3dpp.Windowed = TRUE;															//창모드
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//복잡한 오브젝트 그릴거라 Z버퍼 필요함 - 깊이처리부분
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;


	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,		//모니터번호, 출력디바이스(하드웨어 가속 지원 디바이스), 포커스 윈도우			
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,			//정점 셰이더 SW가속
		&d3dpp, &g_pd3dDevice)))					//구조체 포인터, IDirect3DDevice9 받음
	{
		return E_FAIL;
	}
	//// Turn off culling, so we see the front and back of the triangle
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Z버퍼 기능 ON
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

HRESULT InitGeometry()
{
	/*
	모든 메시 파일은 여러개의 폴리곤으로 구성되어 있고, 폴리곤은 각각의 재질을 가질수 있음
	재질이 여러개일 경우 메시를 재질별로 부분메시로 분할
	분할된 부분메시는 부분메시별로 따로 그림 -> DrawSubset()
	*/

	//재질 임시 보관 버퍼
	LPD3DXBUFFER pD3DXMtrlBuffer;

	//tiger.x에서 메쉬정보와 재질정보 읽는다
	if (FAILED(D3DXLoadMeshFromX(L"Tiger.x", D3DXMESH_SYSTEMMEM, 
		g_pd3dDevice, NULL, &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, &g_pMesh)))
	{
		MessageBox(NULL, L"Could not find Tiger.x", L"Mesh.exe", MB_OK);
		return E_FAIL;
	}

	//재질정보와 텍스쳐 정보를 따로 뽑아낸다.
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();	
	//재질 개수만큼 재질 구조체 배열 생성
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	//재질 개수만큼 텍스쳐 배열 생성
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	
	for (DWORD i = 0; i < g_dwNumMaterials; i++) 
	{		
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;					//재질 정보 복사
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;		//주변광원 정보를 Diffuse 정보로
		g_pMeshTextures[i] = NULL;										
																 
		if (d3dxMaterials[i].pTextureFilename != NULL && lstrlen((LPCWSTR)d3dxMaterials[i].pTextureFilename) > 0)
		{
			//텍스쳐를 파일에서 로드한다
			if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice,
				L"Tiger.bmp", &g_pMeshTextures[i])))
			{
				MessageBox(NULL, L"Could not find texture map", L"Mesh.exe", MB_OK);
				return E_FAIL;
			}
		}
	}

	//임시생성 재질버퍼 해제
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
	//월드행렬
	D3DXMATRIXA16 matWorld;
	//D3DXMatrixIdentity(&matWorld);								//갑자기 이거 왜? 안해도 잘되던데
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);


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
	////재질(material) 설정 (확산광 + 주변광)- 디바이스에 하나만 설정가능
	//D3DMATERIAL9 mtrl;
	//ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	//mtrl.Diffuse.r = mtrl.Ambient.r = 1.f;
	//mtrl.Diffuse.g = mtrl.Ambient.g = 1.f;
	//mtrl.Diffuse.b = mtrl.Ambient.b = 1.f;
	//g_pd3dDevice->SetMaterial(&mtrl);

	/*
	재질은 DrawPrimitive() 호출 시에 하나만 지정가능
	메시가 여러개의 재질로 이루어져 있다면 메시를 재질별로 분리해야함 - 2-6에서 메시다룰때 다시보자
	x파일에는 재질정보 있으니 사용안해도 될듯?
	*/


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

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xaaaaaaaa);			//환경광 설정
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

	SetupLights();
	SetupMatrices();																		//월드, 뷰, 프로젝션 행렬 설정


	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))													//폴리곤 그린다고 알리기
	{
		/*
		메시를 그리기 직전에 그 메시에 해당하는 텍스처와 재질을 적용한다.
		*/
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			//부분집합 메시의 재질과 텍스쳐 설정
			g_pd3dDevice->SetMaterial(&g_pMeshMaterials[i]);		
			g_pd3dDevice->SetTexture(0, g_pMeshTextures[i]);		
			
			//부분집합 메시 출력
			g_pMesh->DrawSubset(i);
		}
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
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

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