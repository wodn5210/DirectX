#include "Engine.h"


Engine::Engine()
{
	POINT	pt;
	GetCursorPos(&pt);
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;
}
Engine::~Engine()
{
	delete g_pCamera;
	delete g_pFrustum;
	delete g_pTerrain;
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

HRESULT Engine::InitD3D(HWND hWnd)
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	g_hwnd = hWnd;


	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 기본컬링, CCW
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Z버퍼기능을 켠다.
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	return S_OK;
}

HRESULT Engine::InitView()
{
	g_pCamera = new Camera(g_pd3dDevice);

	// 뷰 행렬을 설정
	D3DXVECTOR3 vEyePt(0.0f, 100.f, -70.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 5.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	g_pCamera->SetView(&vEyePt, &vLookatPt, &vUpVec);


	RECT	rc;
	GetClientRect(g_hwnd, &rc);
	int width = (rc.right - rc.left);
	int height = (rc.bottom - rc.top);
	g_pCamera->SetViewport(width, height);

	return S_OK;
}
HRESULT Engine::InitLight()
{


	D3DXVECTOR3 vecDir;									
	D3DLIGHT9 light;									
	ZeroMemory(&light, sizeof(D3DLIGHT9));			
	light.Type = D3DLIGHT_DIRECTIONAL;			
	light.Diffuse.r = 1.0f;						
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 0.0f;
	vecDir = D3DXVECTOR3(1, 1, 1);				
	vecDir = D3DXVECTOR3(cosf(GetTickCount() / 350.0f),
		1.0f,
		sinf(GetTickCount() / 350.0f));
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);	
	light.Range = 1000.0f;									
	g_pd3dDevice->SetLight(0, &light);							
	g_pd3dDevice->LightEnable(0, TRUE);							
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);			// 광원설정을 켠다

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);		// 환경광원(ambient light)의 값 설정

	return S_OK;
}
HRESULT Engine::InitObj()
{
	
	g_pFrustum = new Frustum(g_pd3dDevice);
	vector<string> tex_file_dir;
	tex_file_dir.push_back("src/tile2.tga");
	tex_file_dir.push_back("src/lightmap.tga");
	g_pTerrain = new Terrain;
	g_pTerrain->Create(g_pd3dDevice, &D3DXVECTOR3(1.0f, 0.1f, 1.0f), 0.1f, 
		"src/map129.bmp", tex_file_dir);

	return TRUE;
}

VOID Engine::_MouseEvent()
{
	POINT	pt;
	float	fDelta = 0.001f;	// 마우스의 민감도, 이 값이 커질수록 많이 움직인다.

	GetCursorPos(&pt);
	int dx = pt.x - g_dwMouseX;	// 마우스의 변화값
	int dy = pt.y - g_dwMouseY;	// 마우스의 변화값

	g_pCamera->RotateLocalX(dy * fDelta);	// 마우스의 Y축 회전값은 3D world의  X축 회전값
	g_pCamera->RotateLocalY(dx * fDelta);	// 마우스의 X축 회전값은 3D world의  Y축 회전값
	D3DXMATRIXA16* pmatView = g_pCamera->GetViewMatrix();		// 카메라 행렬을 얻는다.
	g_pd3dDevice->SetTransform(D3DTS_VIEW, pmatView);			// 카메라 행렬 셋팅


	// 마우스를 윈도우의 중앙으로 초기화
	//SetCursor( NULL );	// 마우스를 나타나지 않게 않다.
	RECT	rc;
	GetClientRect(g_hwnd, &rc);

	pt.x = (rc.right - rc.left) / 2;
	pt.y = (rc.bottom - rc.top) / 2;
	ClientToScreen(g_hwnd, &pt);
	SetCursorPos(pt.x, pt.y);
	g_dwMouseX = pt.x;
	g_dwMouseY = pt.y;

	
}
VOID Engine::_KeyEvent()
{
	if (GetAsyncKeyState('W')) g_pCamera->MoveLocalZ(0.5f);	
	if (GetAsyncKeyState('S')) g_pCamera->MoveLocalZ(-0.5f);	
	if (GetAsyncKeyState('A')) g_pCamera->MoveLocalX(-0.5f);
	if (GetAsyncKeyState('D')) g_pCamera->MoveLocalX(0.5f);
}
VOID Engine::_SetBillBoard()
{
	D3DXMATRIXA16 matBillBoard;
	D3DXMatrixIdentity(&matBillBoard);

	matBillBoard._11 = g_pCamera->GetBillMatrix()->_11;
	matBillBoard._13 = g_pCamera->GetBillMatrix()->_13;
	matBillBoard._31 = g_pCamera->GetBillMatrix()->_31;
	matBillBoard._33 = g_pCamera->GetBillMatrix()->_33;
	/*
	//위치 보정 필요할 수도 있음. Obj의 중심을 넣을까? - 넣어야한다면 SetBillBoard 인자로 Obj중심 넣자
	matBillBoard._41 = ;
	matBillBoard._42 = ;
	matBillBoard._43 = ;
	*/
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matBillBoard);
}

VOID Engine::RenderReady()
{
	//빛 설정
	InitLight();

	//마우스 이벤트
	_MouseEvent();

	//키보드 이벤트
	_KeyEvent();


	//프러스텀 효과 가시화하기위한것.
	//막히면 마지막으로 설정되었던 프러스텀효과를 보여준다
	if (!g_bLockFrustum)
	{
		D3DXMATRIXA16	m;
		D3DXMATRIXA16* pView = g_pCamera->GetViewMatrix();	// 카메라 클래스로부터 행렬정보를 얻는다.
		D3DXMATRIXA16* pProj = g_pCamera->GetProjMatrix();

		m = (*pView) * (*pProj);				// World좌표를 얻기위해서 View * Proj행렬을 계산한다.
		g_pFrustum->Make(&m, g_pCamera->GetEye());	// View*Proj행렬로 Frustum을 만든다.
	}
}

VOID Engine::Rendering()
{
	if (NULL == g_pd3dDevice)
		return;


	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);


	RenderReady();
	

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		
		g_pTerrain->Draw(g_pFrustum);
		

		if (!g_bHideFrustum)
			g_pFrustum->Draw();
		if (g_bSelectTriOn)
			_SelectTriDraw();
		
		//끝
		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}


VOID Engine::MeshPickingStart(int x, int y)
{
	Ray ray;
	ray.Create(g_pd3dDevice, x, y);

	D3DXVECTOR3 pos[3];
	float dist = FLT_MAX;
	
	//Object객체들의 Search시작하자
	//dist 최대값일때 pos계속 갱신하면됨 - 내부에서 진행할거임
	g_pTerrain->MeshPicking(ray, dist, pos);

	//값 갱신 되었으면 어떤 위치 찾은거임
	if(dist != FLT_MAX)
	{
		//메시 찾은경우 빨강색으로 표시할 수 있게 만들자 - Pos 정점 3개를 삼각형으로 만들어버리자
		
		//이하 삼각형 출력 테스트 코드
		//가시화를 위해서 1 dir만큼 화면으로 당겨온다 - 완전 겹치면 가시화가 잘안됨
		CUSTOMVERTEX g_Vertices[] =
		{
			{ pos[0] ,  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
			{ pos[1],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
			{ pos[2],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		};

		/// 정점버퍼 생성
		if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
			0, D3DFVF_CUSTOMVERTEX,
			D3DPOOL_DEFAULT, &g_pVB, NULL)))
		{
			return;
		}

		/// 정점버퍼를 값으로 채운다. 
		VOID* pVertices;
		if (FAILED(g_pVB->Lock(0, sizeof(g_Vertices), (void**)& pVertices, 0)))
			return;
		memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
		g_pVB->Unlock();

		g_bSelectTriOn = TRUE;
	}

}

VOID Engine::_SelectTriDraw()
{
	D3DXMATRIXA16 world;
	D3DXMatrixIdentity(&world);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &world);
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	//_SetBillBoard();
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	g_pd3dDevice->SetMaterial(&mtrl);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);
}