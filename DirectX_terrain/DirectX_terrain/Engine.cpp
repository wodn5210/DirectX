#include "Engine.h"


Engine::Engine()
{
	g_bSelectTriOn = FALSE;
}
Engine::~Engine()
{
	delete m_CamMain;
	delete g_pFrustum;
	delete g_pTerrain;
	delete m_CamMap;
	delete m_tri;
	delete m_ball;


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
	RECT	rc;
	GetClientRect(g_hwnd, &rc);
	m_winSizeX = (WORD)(rc.right - rc.left);
	m_winSizeY = (WORD)(rc.bottom - rc.top);
	SetCursorPos(m_winSizeX/2, m_winSizeY/2);


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

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Z버퍼기능을 켠다.
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	return S_OK;
}

HRESULT Engine::InitCam()
{
	// 뷰 행렬을 설정
	m_CamMain = new CamMain(g_pd3dDevice);
	D3DXVECTOR3 vEyePt(0.0f, 50.0f, 10.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 5.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	m_CamMain->SetView(&vEyePt, &vLookatPt, &vUpVec);	
	m_CamMain->SetViewport(D3DVIEWPORT9{ 0, 0, m_winSizeX, m_winSizeY, 0, 1 });
	
	m_CamMap = new CamMiniMap(g_pd3dDevice);
	D3DXVECTOR3 vMapEyePt(0.0f, 160.f, 0.0f);
	D3DXVECTOR3 vMapLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vMapUpVec(0.0f, 0.0f, 1.0f);
	m_CamMap->SetView(&vMapEyePt, &vMapLookatPt, &vMapUpVec);
	m_CamMap->SetViewport(D3DVIEWPORT9{ 100, 100, 170, 170, 0, 1 });

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
	light.Diffuse.b = 1.0f;
	light.Diffuse.a = 1.0f;
	//vecDir = D3DXVECTOR3(1, 1, 1);
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
	vector<string> tex_file_dir;
	tex_file_dir.push_back("src/tile2.tga");
	tex_file_dir.push_back("src/lightmap.tga");
	g_pTerrain = new Terrain;
	g_pTerrain->Create(g_pd3dDevice, &D3DXVECTOR3(1.0f, 0.1f, 1.0f), 0.1f,
		"src/map129.bmp", tex_file_dir);

	g_pFrustum = new Frustum(g_pd3dDevice);
	m_tri = new ObjTriangle(g_pd3dDevice);
	m_ball = new ObjBall(g_pd3dDevice);
	m_ball->Create(D3DXVECTOR3(0, 20, 0), 1);
	return TRUE;
}

//카메라를 움직이기전에  
VOID Engine::_BallCameraSetup()
{
	D3DXVECTOR3 eye = *(m_CamMain->GetEye());			//카메라의 위치
	D3DXVECTOR3 view = *(m_CamMain->GetvView());		//카메라의 방향벡터

	D3DXVECTOR3 ballPos = eye + view * 10;
	m_ball->SetCenter(ballPos);
	
}
VOID Engine::SetCameraMoveZ(float dist) 
{
	//ball camera on이면 미리 이동해서 지형과 충돌판단 해야함
	if (g_bBallCamera)
	{
		D3DXVECTOR3 vNewCenter = *(m_ball->GetCenter());
		D3DXVECTOR3 vMove, vView;
		D3DXVec3Normalize(&vView, m_CamMain->GetvView());
		vMove = dist * vView;
		vNewCenter += vMove;

			
		bool move = g_pTerrain->IsBallCollision(&vNewCenter, m_ball->GetR());

		if (move)
		{
			m_CamMain->MoveLocalZ(dist);
			_BallCameraSetup();
		}
	}
	else
	{
		m_CamMain->MoveLocalZ(dist);
	}
};
VOID Engine::SetCameraMoveX(float dist) 
{
	m_CamMain->MoveLocalX(dist);
	if (g_bBallCamera)
	{
		_BallCameraSetup();
	}
};
VOID Engine::MouseMove(WORD x, WORD y)
{
	float	fDelta = 0.001f;	// 마우스의 민감도, 이 값이 커질수록 많이 움직인다.
	POINT	pt;
	pt.x = m_winSizeX / 2;
	pt.y = m_winSizeY / 2;

	int dx = x - pt.x;	// 마우스의 변화값
	int dy = y - pt.y;	// 마우스의 변화값

	m_CamMain->RotateLocalX(dy * fDelta);	// 마우스의 Y축 회전값은 3D world의  X축 회전값
	m_CamMain->RotateLocalY(dx * fDelta);	// 마우스의 X축 회전값은 3D world의  Y축 회전값

	m_CamMain->ResetView();

	// 마우스를 윈도우의 중앙으로 초기화
	//SetCursor( NULL );	// 마우스를 나타나지 않게 않다.
	ClientToScreen(g_hwnd, &pt);
	SetCursorPos(pt.x, pt.y);


	if (g_bBallCamera)
	{
		_BallCameraSetup();
	}
}

VOID Engine::_SetBillBoard()
{
	D3DXMATRIXA16 matBillBoard;
	D3DXMatrixIdentity(&matBillBoard);

	matBillBoard._11 = m_CamMain->GetBillMatrix()->_11;
	matBillBoard._13 = m_CamMain->GetBillMatrix()->_13;
	matBillBoard._31 = m_CamMain->GetBillMatrix()->_31;
	matBillBoard._33 = m_CamMain->GetBillMatrix()->_33;
	/*
	//위치 보정 필요함 - 나중에 빌보드 쓸때 수정하자
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


	//프러스텀 효과 가시화하기위한것.
	//막히면 마지막으로 설정되었던 프러스텀효과를 보여준다
	if (!g_bLockFrustum)
	{
		D3DXMATRIXA16	m;
		D3DXMATRIXA16* pView = m_CamMain->GetViewMatrix();	// 카메라 클래스로부터 행렬정보를 얻는다.
		D3DXMATRIXA16* pProj = m_CamMain->GetProjMatrix();

		m = (*pView) * (*pProj);				// World좌표를 얻기위해서 View * Proj행렬을 계산한다.
		g_pFrustum->Make(&m, m_CamMain->GetEye());	// View*Proj행렬로 Frustum을 만든다.
	}
}

VOID Engine::Rendering()
{
	if (NULL == g_pd3dDevice)
		return;

	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	RenderReady();	
	

	//메인 렌더링
	m_CamMain->ResetView();

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pTerrain->DrawMain(g_pFrustum);
		m_ball->DrawMain();
		if (g_bSelectTriOn) {

			m_tri->DrawMain();
		}


		if (!g_bHideFrustum)
			g_pFrustum->Draw();
		

		g_pd3dDevice->EndScene();
	}


	//맵 렌더링
	m_CamMap->ResetView();

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pTerrain->DrawMap();
		m_ball->DrawMap();
		if (g_bSelectTriOn) {
			m_tri->DrawMap();


		}
		
	
		g_pd3dDevice->EndScene();
	}


	//원래로 돌려놔야 Picking 제대로 작동
	m_CamMain->ResetView();


	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}


VOID Engine::MeshPickingStart(int x, int y)
{
	Ray ray;
	ray.Create(g_pd3dDevice, x, y);
	temp[0] = *(ray.GetPos());
	temp[1] = *(ray.GetPos()) + 200* *(ray.GetDir());
	//printf("temp1 = %f %f %f\n", temp[0].x, temp[0].y, temp[0].z);
	//printf("temp2 = %f %f %f\n\n", temp[1].x, temp[1].y, temp[1].z);

	D3DXVECTOR3 pos[3];
	float dist = FLT_MAX;

	//Object객체들의 Search시작하자
	//0<buf_dist<dist pos계속 갱신하면됨 - 내부에서 진행할거임
	g_pTerrain->MeshPicking(ray, dist, pos);

	//값 갱신 되었으면 어떤 위치 찾은거임
	if (dist != FLT_MAX)
	{
		//메시 찾은경우 빨강색으로 표시할 수 있게 만들자 - Pos 정점 3개를 삼각형으로 만들어버리자
		m_tri->Create(pos);
		
		g_bSelectTriOn = TRUE;


		//선분도 그리기
	}
	printf("\n\n");

}
