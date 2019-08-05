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
	delete m_hole;
	delete m_tree1;
	delete m_skybox;
	delete m_tree2;

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
	d3dpp.BackBufferWidth = m_winSizeX;               // ���� �ػ� ���� ����
	d3dpp.BackBufferHeight = m_winSizeY;               // ���� �ػ� ���� ���� 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;   // ���� ���� ���� 
	d3dpp.BackBufferCount = 1;                 // ����� �� 
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Z���۱���� �Ҵ�.
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	return S_OK;
}

HRESULT Engine::InitCam()
{
	// �� ����� ����
	m_CamMain = new CamMain(g_pd3dDevice);	
	D3DXVECTOR3 vEyePt(0.0f, 30.0f, -50.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 vEyePt(0.0f, 0.0f, -1.0f);
	//D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 1.0f);
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
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);			// ���������� �Ҵ�

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);		// ȯ�汤��(ambient light)�� �� ����

	return S_OK;
}

HRESULT Engine::InitObj()
{
	vector<string> tex_file_dir;
	tex_file_dir.push_back("src/golf/field.bmp");
	tex_file_dir.push_back("src/lightmap.tga");
	g_pTerrain = new Terrain;
	g_pTerrain->Create(g_pd3dDevice, &D3DXVECTOR3(1.0f, 0.1f, 1.0f),
		0.1f, "src/map129.bmp", tex_file_dir);


	g_pFrustum = new Frustum(g_pd3dDevice);
	g_pFrustum->Init();

	m_tri = new ObjTriangle(g_pd3dDevice);

	m_ball = new ObjBall(g_pd3dDevice, g_pTerrain);
	m_ball->Create(D3DXVECTOR3(44, 1, -50), 0.05f);
	
	m_skybox = new ObjSkyBox(g_pd3dDevice);
	m_skybox->Create();

	m_hole = new ObjHole(g_pd3dDevice);
	m_hole->Create(D3DXVECTOR3(40, -0.1, -50), 0.15f);

	m_tree1 = new ObjTree1(g_pd3dDevice);
	m_tree1->Create(D3DXVECTOR3(30, 2, -50), "src/golf/tree1.dds");

	m_tree2 = new ObjTree2(g_pd3dDevice);
	m_tree2->Create(D3DXVECTOR3(35, 0, -50), "src/golf/obj/low_poly_tree.obj");

	m_bar = new ObjProgressbar(g_pd3dDevice);
	m_bar->Create();

	return TRUE;
}


VOID Engine::SetCameraMoveZ(float dist) 
{

	m_CamMain->MoveLocalZ(dist);
};
VOID Engine::SetCameraMoveX(float dist) 
{
	m_CamMain->MoveLocalX(dist);
};
VOID Engine::MouseMove(WORD x, WORD y)
{
	float	fDelta = 0.001f;	// ���콺�� �ΰ���, �� ���� Ŀ������ ���� �����δ�.
	POINT	pt;
	pt.x = m_winSizeX / 2;
	pt.y = m_winSizeY / 2;

	int dx = x - pt.x;	// ���콺�� ��ȭ��
	int dy = y - pt.y;	// ���콺�� ��ȭ��

	m_CamMain->RotateLocalX(dy * fDelta);	// ���콺�� Y�� ȸ������ 3D world��  X�� ȸ����
	m_CamMain->RotateLocalY(dx * fDelta);	// ���콺�� X�� ȸ������ 3D world��  Y�� ȸ����

	m_CamMain->ResetView();

	// ���콺�� �������� �߾����� �ʱ�ȭ
	SetCursor( NULL );	// ���콺�� ��Ÿ���� �ʰ� �ʴ�.
	ClientToScreen(g_hwnd, &pt);
	SetCursorPos(pt.x, pt.y);


}

D3DXMATRIXA16 Engine::_CalcBillBoard()
{
	D3DXMATRIXA16 matBillBoard;
	D3DXMatrixIdentity(&matBillBoard);

	matBillBoard._11 = m_CamMain->GetViewMatrix()->_11;
	matBillBoard._13 = m_CamMain->GetViewMatrix()->_13;
	matBillBoard._31 = m_CamMain->GetViewMatrix()->_31;
	matBillBoard._33 = m_CamMain->GetViewMatrix()->_33;
	D3DXMatrixInverse(&matBillBoard, NULL, &matBillBoard);
	
	return matBillBoard;
}

VOID Engine::RenderReady()
{
	//�� ����
	InitLight();

	//�� ������ 
	g_BallState = m_ball->MovePhysical();
	if (g_BallState != 0)
	{
		g_bBallJump = FALSE;
	}

	ISBallInHole();

	//�������� ȿ�� ����ȭ�ϱ����Ѱ�.
	//������ ���������� �����Ǿ��� ��������ȿ���� �����ش�
	if (!g_bLockFrustum)
	{
		D3DXMATRIXA16	m;
		D3DXMATRIXA16* pView = m_CamMain->GetViewMatrix();	// ī�޶� Ŭ�����κ��� ��������� ��´�.
		D3DXMATRIXA16* pProj = m_CamMain->GetProjMatrix();
		m = (*pView) * (*pProj);				// World��ǥ�� ������ؼ� View * Proj����� ����Ѵ�.
		g_pFrustum->Make(&m, m_CamMain->GetEye());	// View*Proj��ķ� Frustum�� �����.
	}
}

VOID Engine::Rendering()
{
	if (NULL == g_pd3dDevice)
		return;
	
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	//g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	RenderReady();

	//���� ������
	if(g_bBallCamera)
		m_CamMain->ResetBallView();
	else
		m_CamMain->ResetView();
	
	D3DXMATRIXA16	m;
	D3DXMATRIXA16* pView = m_CamMain->GetViewMatrix();	// ī�޶� Ŭ�����κ��� ��������� ��´�.
	D3DXMATRIXA16* pProj = m_CamMain->GetProjMatrix();


	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pTerrain->DrawMain(g_pFrustum);
		m_tree2->DrawMain();
		m_ball->DrawMain();
		m_hole->DrawMain();
		m_skybox->DrawMain();
	
		m_tree1->DrawMain(&_CalcBillBoard());

		if (g_bBallEnergyView || g_bBallJump)
		{
			m = (*pView) * (*pProj);
			m_bar->DrawMain(&m, g_BallEnergy);
			
		}

		if (g_bSelectTriOn) 
		{
			m_tri->DrawMain();
		}


		if (!g_bHideFrustum)
			g_pFrustum->Draw();
		

		g_pd3dDevice->EndScene();
	}


	//�� ������
	m_CamMap->ResetView();

	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		

		g_pTerrain->DrawMap();
		m_ball->DrawMap();
		m_hole->DrawMap();
		if (g_bSelectTriOn) {
			m_tri->DrawMap();


		}
		
	
		g_pd3dDevice->EndScene();
	}


	//������ �������� Picking ����� �۵�
	if (g_bBallCamera)
		m_CamMain->ResetBallView();
	else
		m_CamMain->ResetView();
	
	

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

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

	//Object��ü���� Search��������
	//0<buf_dist<dist pos��� �����ϸ�� - ���ο��� �����Ұ���
	g_pTerrain->MeshPicking(ray, dist, pos);
	
	//�� ���� �Ǿ����� � ��ġ ã������
	if (dist != FLT_MAX)
	{
		printf("click %.3f %.3f %.3f\n", pos[0].x, pos[0].y, pos[0].z);
		//�޽� ã����� ���������� ǥ���� �� �ְ� ������ - Pos ���� 3���� �ﰢ������ ����������
		m_tri->Create(pos);
		
		g_bSelectTriOn = TRUE;

		

		//���е� �׸���
	}

}


VOID Engine::SetBallCamRotateY(float degree)
{
	D3DXVECTOR3* ballPos = m_ball->GetCenter();
	m_CamMain->SetBallViewRotateY(degree);
}
VOID Engine::SetBallCamRotateX(float degree)
{
	D3DXVECTOR3* ballPos = m_ball->GetCenter();
	m_CamMain->SetBallViewRotateX(degree);
}

BOOL Engine::ISBallInHole()
{
	//����� ���Ϳ��� �밢�� ���� + �� ������
	float in_size = m_hole->GetR() * sqrtf(2.0f) + m_ball->GetR();

	float dist = D3DXVec3Length(&(*m_hole->GetCenter() - *m_ball->GetCenter()));

	if (dist <= in_size)
	{
		printf("�浹 !");
		return true;
	}
	return false;
}

VOID Engine::SetBallJump(float energy)
{
	if (g_bBallJump)
		return;
	g_bBallJump = true;
	m_ball->SetBallJump(energy, m_CamMain->GetvView()); 
	g_bBallEnergyView = false;

}