#include "SceneGame.h"

SceneGame::SceneGame()
{
	m_bRenderFrustum = false;
	m_bBallCam = true;
	m_bWireRender = false;
	m_bSpaceBar = false;
	m_bSelectMeshRender = false;
	m_bEndGame = false;
	m_bBallImpact = false;
	m_bGoriInit = false;
	m_sT = m_eT = 0;
	m_BallEnergy = 0;
	m_hitCount = 0;
}
SceneGame::~SceneGame()
{
	if (m_pCamMain != NULL)
		delete m_pCamMain;
	if (m_pCamBall != NULL)
		delete m_pCamBall;
	if (m_pCamMap != NULL)
		delete m_pCamMap;
	if (m_pFrustum != NULL)
		delete m_pFrustum;
	if (m_pTerrain != NULL)
		delete m_pTerrain;
	if (m_pBall != NULL)
		delete m_pBall;
	if (m_pBar != NULL)
		delete m_pBar;
	if (m_pSkybox != NULL)
		delete m_pSkybox;
	if (m_pHole != NULL)
		delete m_pHole;
	if (m_pTreeTex != NULL)
		delete m_pTreeTex;
	for (int i = 0; i < 4; i++)
	{
		if (m_pTreeObj[i] != NULL)
			delete m_pTreeObj[i];
	}
	if (m_pTri != NULL)
		delete m_pTri;
	if (m_pDust != NULL)
		delete m_pDust;
	if (m_pGori != NULL)
		delete m_pGori;
	if (m_pDir != NULL)
		delete m_pDir;
}

HRESULT SceneGame::Create(LPDIRECT3DDEVICE9 device, HWND hWnd)
{
	Scene::Create(device, hWnd);

	_InitObj();
	_InitCam();


	return S_OK;
}


HRESULT SceneGame::_InitCam()
{
	// 뷰 행렬을 설정

	m_pCamMain = new CamMain();
	m_pCamMain->Create(m_device);
	D3DXVECTOR3 vEyePt(0.0f, 30.0f, -50.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	m_pCamMain->SetView(&vEyePt, &vLookatPt);
	m_pCamMain->SetViewport(D3DVIEWPORT9{ 0, 0, m_winSizeX, m_winSizeY, 0, 1 });

	m_pCamBall = new CamBall();
	m_pCamBall->Create(m_device);
	m_pCamBall->SetBallPosP(m_pBall->GetCenter());
	m_pCamBall->SetView();
	m_pCamBall->SetViewport(D3DVIEWPORT9{ 0, 0, m_winSizeX, m_winSizeY, 0, 1 });

	if (m_bBallCam)
		m_pNowCam = m_pCamBall;
	else
		m_pNowCam = m_pCamMain;

	m_pCamMap = new CamMiniMap(m_device);
	D3DXVECTOR3 vMapEyePt(0.0f, 160.f, 0.0f);
	D3DXVECTOR3 vMapLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vMapUpVec(0.0f, 0.0f, 1.0f);
	m_pCamMap->SetView(&vMapEyePt, &vMapLookatPt);
	m_pCamMap->SetViewport(D3DVIEWPORT9{ (WORD)(m_winSizeX - 170), 0, 170, 170, 0, 1 });



	return S_OK;
}
HRESULT SceneGame::_InitLight()
{
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 0.9f;
	light.Diffuse.g = 0.9f;
	light.Diffuse.b = 0.9f;
	light.Diffuse.a = 0.9f;
	vecDir = D3DXVECTOR3(1, -1, 1);
	//vecDir = D3DXVECTOR3(cosf(GetTickCount() / 350.0f),
	//	1.0f,
	//	sinf(GetTickCount() / 350.0f));
	D3DXVec3Normalize((D3DXVECTOR3*)& light.Direction, &vecDir);
	light.Range = 1000.0f;
	m_device->SetLight(0, &light);
	m_device->LightEnable(0, TRUE);
	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);			// 광원설정을 켠다

	m_device->SetRenderState(D3DRS_AMBIENT, 0x00909090);		// 환경광원(ambient light)의 값 설정

	return S_OK;
}
HRESULT SceneGame::_InitObj()
{
	m_pFrustum = new ObjFrustum();
	m_pFrustum->Create(m_device);

	string map_path = "src/map129.bmp";
	vector<string> tex_path;
	tex_path.push_back("src/golf/field.bmp");
	tex_path.push_back("src/lightmap.tga");
	m_pTerrain = new ObjTerrain();
	m_pTerrain->Create(m_device, m_pFrustum, &D3DXVECTOR3(1.0f, 0.05f, 1.0f), map_path, tex_path);

	m_pBall = new ObjBall();
	m_pBall->Create(m_device, m_pTerrain, D3DXVECTOR3(44.0f, 0.5f, -50.0f), 0.05f);

	m_pBar = new ObjGauge();
	m_pBar->Create(m_device);

	m_pSkybox = new ObjSkyBox();
	m_pSkybox->Create(m_device);

	m_pHole = new ObjHole();
	//m_pHole->Create(m_device, D3DXVECTOR3(40.0f, -0.1f, -50.f), 0.15f);
	m_pHole->Create(m_device, D3DXVECTOR3(-39.0f, 2.7f, 14.f), 0.15f);

	m_pTreeTex = new ObjTreeTexture();
	m_pTreeTex->Create(m_device, D3DXVECTOR3(30.0f, 1.0f, -45.0f), "src/golf/tree1.dds");

	D3DXVECTOR3 treePos[4] =
	{ D3DXVECTOR3(35.0f, 0.0f, -60.0f), D3DXVECTOR3(-50.0f, 0.0f, 35.0f),
	D3DXVECTOR3(-50.0f, 0.0f, -50.0f), D3DXVECTOR3(35.0f, 0.0f, 35.0f) };

	for (int i = 0; i < 4; i++)
	{
		m_pTreeObj[i] = new ObjTreeObjFile();
		m_pTreeObj[i]->Create(m_device, treePos[i], "src/golf/obj/low_poly_tree.obj");
	}
	m_pTri = new ObjTriangle(m_device);

	m_pDust = new ObjDust(100);
	m_pDust->Create(m_device, "src/golf/desert.bmp");

	m_pGori = new ObjBallGori();
	m_pGori->Create(m_device, m_pBall->GetCenter());

	m_pDir = new ObjTriangle(m_device);
	m_pDir->Create();

	return S_OK;
}

VOID SceneGame::_ReadyRender()
{

	_InitLight();

	m_pDir->Update(*(m_pBall->GetCenter()), *(m_pNowCam->GetvView()), *(m_pNowCam->GetvCross()));

	m_ballState = m_pBall->MovePhysical(m_bBallImpact);
	if (m_bBallImpact)
	{
		m_pDust->Reset(*(m_pBall->GetCenter()));
		m_bBallImpact = false;
	}

	if (m_ballState == ObjBall::STOP && m_bGoriInit == false)
	{
		m_pGori->Init(m_pBall->GetCenter());
		m_bGoriInit = true;
	}
	//렌더링 중일때
	else if(m_bGoriInit == false)
	{	
		m_pGori->ReadyRender(m_pBall->GetCenter(), m_pBall->GetDir());
	}
	m_pDust->Update();


	
	

	m_pTreeTex->SetBillBoard(m_pNowCam->GetBillBoard());

	D3DXMATRIXA16 matViewProj = (*m_pNowCam->GetViewMatrix()) * (*m_pNowCam->GetProjMatrix());

	if (m_bSpaceBar)
	{
		m_eT = clock();
		m_BallEnergy = (float)(m_eT - m_sT) / (CLOCKS_PER_SEC * 5);
		if (m_BallEnergy >= 0.7f)
			m_BallEnergy = 0.7f;
		m_pBar->ReadyRender(&matViewProj, m_BallEnergy);
	}

	if (!m_bRenderFrustum)
		m_pFrustum->Update(&matViewProj, m_pNowCam->GetEye());
}
BOOL SceneGame::_ISBallInHole()
{
	//원기둥 센터에서 대각선 길이 + 공 반지름
	float in_size = m_pHole->GetR() * 2 + m_pBall->GetR();

	float dist = D3DXVec3Length(&(*m_pHole->GetCenter() - *m_pBall->GetCenter()));

	if (dist <= in_size)
	{
		return true;
	}
	return false;
}


VOID SceneGame::Rendering()
{
	if (NULL == m_device)
		return;

	_ReadyRender();

	m_pNowCam->ResetView();

	m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	m_device->SetRenderState(D3DRS_FILLMODE, m_bWireRender ? D3DFILL_WIREFRAME : D3DFILL_SOLID);

	if (SUCCEEDED(m_device->BeginScene()))
	{
	
		

		
		m_pDust->Render();
		m_pTerrain->DrawMain();
		
		m_pBall->DrawMain();
		
		if (!m_bWireRender)
		{
			
			for (int i = 0; i < 4; i++)
				m_pTreeObj[i]->DrawMain();
			
			m_pSkybox->DrawMain();

			

			m_pTreeTex->DrawMain();


			if (m_bGoriInit == false)
			{

				m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				m_pGori->DrawMain();
				m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

			}
		}
		m_pHole->DrawMain();

		if (m_bSpaceBar)
			m_pBar->DrawMain();

		
		if (m_bSelectMeshRender && m_bBallCam == false)
		{
			m_pTri->DrawMain();
		}


		if (m_bRenderFrustum)
		{
			m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			m_pFrustum->Draw();
		}

		m_device->EndScene();
	}

	//맵 렌더링
	m_pCamMap->ResetView();
	m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(200, 200, 200), 1.0f, 0);

	if (SUCCEEDED(m_device->BeginScene()))
	{
		m_pTerrain->DrawMap();
		m_pHole->DrawMap();
		m_pBall->DrawMap();
		m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pDir->DrawMap();
		m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		m_device->EndScene();
	}

	m_pNowCam->ResetView();

	m_device->Present(NULL, NULL, NULL, NULL);


	
	//끝났다고 Engine에 알릴거임
	m_bEndGame = _ISBallInHole();
	if (m_bEndGame || ((m_ballState == ObjBall::STOP) && (m_hitCount >= 8)))
	{
		m_bEndGame = true;
		SendMessage(m_hWnd, 0, m_hitCount, 0);		
	}
}



/*
이하 이벤트 관련 함수
*/


int SceneGame::MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (m_bEndGame)
		return 0;

	switch (msg)
	{
	case WM_LBUTTONDOWN:
		_MeshPicking(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEMOVE:
		_MouseMove(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F1:
			m_bRenderFrustum = !m_bRenderFrustum;
			break;
		case VK_F2:
			m_bWireRender = !m_bWireRender;
			break;
		case VK_F3:
			m_bSelectMeshRender = !m_bSelectMeshRender;
		case VK_F4:
			m_bBallCam = !m_bBallCam;
			m_pNowCam = (m_bBallCam) ? m_pCamBall : m_pCamMain;
			break;

		case 'R':
			m_pBall->Reset();
			break;
		case VK_LEFT:
			m_pNowCam->SetPressLeft();
			break;
		case VK_RIGHT:
			m_pNowCam->SetPressRight();
			break;
		case VK_UP:
			m_pNowCam->SetPressUp();
			break;
		case VK_DOWN:
			m_pNowCam->SetPressDown();
			break;
		case VK_SPACE:
			if (m_sT == 0 && m_ballState == 1)
			{
				
				m_sT = clock();
				m_bSpaceBar = true;
			}
			break;
		}
		break;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			if (m_sT == 0)
				break;

			m_eT = clock();
			m_BallEnergy = ((float)(m_eT - m_sT) + EPSILON) / (CLOCKS_PER_SEC * 5);

			if (m_BallEnergy >= 0.7f)
				m_BallEnergy = 0.7f;

			if (m_ballState == ObjBall::STOP)
			{
				m_pBall->SetBallJump(m_BallEnergy, m_pNowCam->GetvView());
				m_bGoriInit = false;
				m_hitCount++;
			}

			m_bSpaceBar = false;
			m_BallEnergy = 0;
			m_sT = m_eT = 0;
			break;
		}
		break;
	}
	return -1;
}
VOID SceneGame::_MeshPicking(int x, int y)
{
	Ray ray;
	ray.Create(m_device, x, y);
	
	D3DXVECTOR3 mesh_vtx[3];
	float dist = FLT_MAX;

	m_pTerrain->MeshPicking(ray, dist, mesh_vtx);
	if (dist != FLT_MAX)
	{
		m_pTri->Create(mesh_vtx);
		m_bSelectMeshRender = TRUE;
	}
}


VOID SceneGame::_MouseMove(WORD x, WORD y)
{
	//마우스 이동 - CamMain으로 렌터딩일때만
	if (m_bBallCam)
		return;


	float	fDelta = 0.001f;	// 마우스의 민감도, 이 값이 커질수록 많이 움직인다.
	POINT	pt;
	pt.x = m_winSizeX / 2;
	pt.y = m_winSizeY / 2;

	int dx = x - pt.x;	// 마우스의 변화값
	int dy = y - pt.y;	// 마우스의 변화값

	m_pCamMain->RotateLocalX(dy * fDelta);	// 마우스의 Y축 회전값은 3D world의  X축 회전값
	m_pCamMain->RotateLocalY(dx * fDelta);	// 마우스의 X축 회전값은 3D world의  Y축 회전값

	m_pCamMain->ResetView();

	// 마우스를 윈도우의 중앙으로 초기화
	SetCursor(NULL);	// 마우스를 나타나지 않게 않다.
	ClientToScreen(m_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
}