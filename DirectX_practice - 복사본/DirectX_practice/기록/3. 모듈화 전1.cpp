#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)

D3DXVECTOR3 eye = { 100.0f, 250.0f, -400.0f };

struct CUSTOMVERTEX
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct MYINDEX
{
WORD _0, _1, _2;
};

LPDIRECT3D9             g_pD3D = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;
LPDIRECT3DINDEXBUFFER9	g_pIB = NULL;


HRESULT InitD3D(HWND hWnd)
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


HRESULT InitGeometry()
{
	CUSTOMVERTEX g_Vertices[] =
	{
		{  D3DXVECTOR3(0.0f, 50.0f, 0.0f),		D3DXVECTOR3(0.0f, 50.0f, 0.0f), },	//첫번째 삼각형
		{  D3DXVECTOR3(-50.0f,  0.0f, 0.0f),	D3DXVECTOR3(0, 0.33f, 0), },
		{  D3DXVECTOR3(50.0f,  0.0f, 0.0f),		D3DXVECTOR3(0, 0, 0.33f), },

		{	D3DXVECTOR3(100.f, 0, 0),		D3DXVECTOR3(0.33f, 0, 0)},			//두번째 삼각형
		{	D3DXVECTOR3(-120.f, 0, 0),		D3DXVECTOR3(0, 0.33f, 0)},
		{	D3DXVECTOR3(0, 120.f, 0),		D3DXVECTOR3(0, 0, 0.33f)},

		//큐브 만들기 위한 삼각형들
		{	D3DXVECTOR3 (-1, 1, 1),			D3DXVECTOR3(-1, 1, 1)},		//v0
		{	D3DXVECTOR3(1, 1, 1),			D3DXVECTOR3(1, 1, 1)},		//v1
		{	D3DXVECTOR3(1, 1, -1),			D3DXVECTOR3(1, 1, -1)},		//v2
		{	D3DXVECTOR3 (-1, 1, -1),		D3DXVECTOR3(-1, 1, -1)},	//v3

		{	D3DXVECTOR3 (-1, -1, 1),		D3DXVECTOR3(-1, -1, 1)},	//v4
		{	D3DXVECTOR3(1, -1, 1),			D3DXVECTOR3(1, -1, 1)},		//v5
		{	D3DXVECTOR3(1, -1, -1),			D3DXVECTOR3(1, -1, -1)},	//v6
		{	D3DXVECTOR3 (-1, -1, -1),		D3DXVECTOR3(-1, -1, -1)},	//v7

	};

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(14 * sizeof(CUSTOMVERTEX), 
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

HRESULT InitIB()
{
	//14
	MYINDEX indices[] =
	{
		//첫번째 삼각형 인덱스
		{0, 1, 2},

		//두번째 삼각형 인덱스
		{3, 4, 5},

		//큐브 인덱스
		{6, 7, 8},		{6, 8, 9},		//위
		{10, 12, 11},	{10, 13, 12},	//아래
		{6, 9, 13},		{6, 13, 10},	//왼
		{7, 11, 12},	{7, 12, 8},		//오른
		{9, 8, 12},		{9, 12, 13},	//앞
		{6, 10, 11},	{6, 11, 7}		//뒤

	};

	if (FAILED(g_pd3dDevice->CreateIndexBuffer(14 * sizeof(MYINDEX), 0,
		D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pIndices;
	if (FAILED(g_pIB->Lock(0, sizeof(indices), (void**)&pIndices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pIndices, indices, sizeof(indices));
	g_pIB->Unlock();

	return S_OK;
}

VOID SetupMatrices()
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



HRESULT InitMaterial(int n) 
{
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	if (n == 0) 
	{
		mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
		mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
		mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
		mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	}
	else if (n == 1) 
	{
		mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
		mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
		mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
		mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	}
	else if (n == 2)
	{
		mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
		mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
		mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
		mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	}
	else
	{
		mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
		mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
		mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
		mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	}
	g_pd3dDevice->SetMaterial(&mtrl);



	return TRUE;
}



HRESULT InitLight() {
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







VOID Render()
{
	D3DXMATRIXA16 matWorld, translation, rotation, scale;
	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;

    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB( 128, 128, 128 ), 1.0f, 0 );

	InitLight();
	SetupMatrices();


    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
		//정점버퍼 스트림 설정
        g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( CUSTOMVERTEX ) );
        g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->SetIndices(g_pIB);

  
		//0번 삼각형
		InitMaterial(0);
		D3DXMatrixRotationX(&rotation, fAngle);
		D3DXMatrixTranslation(&translation, -50, -50, -50);
		matWorld = rotation * translation;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
		
		
		//1번 삼각형
		InitMaterial(1);
		D3DXMatrixRotationY(&rotation, fAngle);
		D3DXMatrixTranslation(&translation, -100, -100, -100);
		matWorld = translation * rotation;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 3, 3, 3, 1);

		//큐브
		InitMaterial(2);
		D3DXMatrixTranslation(&translation, 0, 50, 0);
		D3DXMatrixRotationX(&rotation, fAngle);
		D3DXMatrixScaling(&scale, 40, 40, 40);
		matWorld = rotation * scale * translation;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 6, 8, 6, 12);


		//2번째 큐브
		InitMaterial(3);
		D3DXMatrixTranslation(&translation, 0, -50, 0);
		D3DXMatrixRotationY(&rotation, fAngle);
		D3DXMatrixScaling(&scale, 10, 10, 10);
		matWorld = rotation * scale * translation;
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 6, 8, 6, 12);


        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


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

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
		case WM_LBUTTONDOWN:
			eye.x += 50;
			return 0;
		case WM_RBUTTONDOWN:
			eye.x -= 50;
			return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}



INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );


    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 03: Matrices",
                              WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitGeometry() ) && SUCCEEDED( InitIB())  )
        {
			
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



