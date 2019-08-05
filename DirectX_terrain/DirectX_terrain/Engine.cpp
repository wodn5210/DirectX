#include "Engine.h"


Engine::Engine()
{
	m_bSelectTriOn = FALSE;
}
Engine::~Engine()
{
	delete m_pCamMain;
	delete m_pFrustum;
	delete m_pTerrain;
	delete m_pCamMap;
	delete m_pTri;
	delete m_pBall;
	delete m_pHole;
	delete m_pTree1;
	delete m_pSkybox;
	delete m_pTree2;

	if (m_device != NULL)
		m_device->Release();

	if (m_pD3D != NULL)
		m_pD3D->Release();
}

HRESULT Engine::InitD3D(HWND hWnd)
{
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	m_hwnd = hWnd;
	RECT	rc;
	GetClientRect(m_hwnd, &rc);
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

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_device)))
	{
		return E_FAIL;
	}

	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Z���۱���� �Ҵ�.
	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);


	return S_OK;
}

HRESULT Engine::InitCam()
{
	// �� ����� ����
	m_pCamMain = new CamMain(m_device);	
	D3DXVECTOR3 vEyePt(0.0f, 30.0f, -50.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 vEyePt(0.0f, 0.0f, -1.0f);
	//D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	m_pCamMain->SetView(&vEyePt, &vLookatPt, &vUpVec);	
	m_pCamMain->SetViewport(D3DVIEWPORT9{ 0, 0, m_winSizeX, m_winSizeY, 0, 1 });
	
	m_pCamMap = new CamMiniMap(m_device);
	D3DXVECTOR3 vMapEyePt(0.0f, 160.f, 0.0f);
	D3DXVECTOR3 vMapLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vMapUpVec(0.0f, 0.0f, 1.0f);
	m_pCamMap->SetView(&vMapEyePt, &vMapLookatPt, &vMapUpVec);
	m_pCamMap->SetViewport(D3DVIEWPORT9{ 100, 100, 170, 170, 0, 1 });

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
	m_device->SetLight(0, &light);
	m_device->LightEnable(0, TRUE);
	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);			// ���������� �Ҵ�

	m_device->SetRenderState(D3DRS_AMBIENT, 0x00909090);		// ȯ�汤��(ambient light)�� �� ����

	return S_OK;
}

HRESULT Engine::InitObj()
{
	vector<string> tex_file_dir;
	tex_file_dir.push_back("src/golf/field.bmp");
	tex_file_dir.push_back("src/lightmap.tga");
	m_pTerrain = new ObjTerrain;
	m_pTerrain->Create(m_device, &D3DXVECTOR3(1.0f, 0.1f, 1.0f),
		0.1f, "src/map129.bmp", tex_file_dir);


	m_pFrustum = new ObjFrustum(m_device);
	m_pFrustum->Init();

	m_pTri = new ObjTriangle(m_device);

	m_pBall = new ObjBall(m_device, m_pTerrain);
	m_pBall->Create(D3DXVECTOR3(44, 1, -50), 0.05f);
	
	m_pSkybox = new ObjSkyBox(m_device);
	m_pSkybox->Create();

	m_pHole = new ObjHole(m_device);
	m_pHole->Create(D3DXVECTOR3(40, -0.1, -50), 0.15f);

	m_pTree1 = new ObjTree1(m_device);
	m_pTree1->Create(D3DXVECTOR3(30, 2, -50), "src/golf/tree1.dds");

	m_pTree2 = new ObjTree2(m_device);
	m_pTree2->Create(D3DXVECTOR3(35, 0, -50), "src/golf/obj/low_poly_tree.obj");

	m_pBar = new ObjProgressbar(m_device);
	m_pBar->Create();

	return TRUE;
}


VOID Engine::SetCameraMoveZ(float dist) 
{

	m_pCamMain->MoveLocalZ(dist);
};
VOID Engine::SetCameraMoveX(float dist) 
{
	m_pCamMain->MoveLocalX(dist);
};
VOID Engine::MouseMove(WORD x, WORD y)
{
	float	fDelta = 0.001f;	// ���콺�� �ΰ���, �� ���� Ŀ������ ���� �����δ�.
	POINT	pt;
	pt.x = m_winSizeX / 2;
	pt.y = m_winSizeY / 2;

	int dx = x - pt.x;	// ���콺�� ��ȭ��
	int dy = y - pt.y;	// ���콺�� ��ȭ��

	m_pCamMain->RotateLocalX(dy * fDelta);	// ���콺�� Y�� ȸ������ 3D world��  X�� ȸ����
	m_pCamMain->RotateLocalY(dx * fDelta);	// ���콺�� X�� ȸ������ 3D world��  Y�� ȸ����

	m_pCamMain->ResetView();

	// ���콺�� �������� �߾����� �ʱ�ȭ
	SetCursor( NULL );	// ���콺�� ��Ÿ���� �ʰ� �ʴ�.
	ClientToScreen(m_hwnd, &pt);
	SetCursorPos(pt.x, pt.y);


}

D3DXMATRIXA16 Engine::_CalcBillBoard()
{
	D3DXMATRIXA16 matBillBoard;
	D3DXMatrixIdentity(&matBillBoard);

	matBillBoard._11 = m_pCamMain->GetViewMatrix()->_11;
	matBillBoard._13 = m_pCamMain->GetViewMatrix()->_13;
	matBillBoard._31 = m_pCamMain->GetViewMatrix()->_31;
	matBillBoard._33 = m_pCamMain->GetViewMatrix()->_33;
	D3DXMatrixInverse(&matBillBoard, NULL, &matBillBoard);
	
	return matBillBoard;
}

VOID Engine::RenderReady()
{
	//�� ����
	InitLight();

	//�� ������ 
	m_BallState = m_pBall->MovePhysical();
	if (m_BallState != 0)
	{
		m_bBallJump = FALSE;
	}

	ISBallInHole();

	//�������� ȿ�� ����ȭ�ϱ����Ѱ�.
	//������ ���������� �����Ǿ��� ��������ȿ���� �����ش�
	if (!m_bLockFrustum)
	{
		D3DXMATRIXA16	m;
		D3DXMATRIXA16* pView = m_pCamMain->GetViewMatrix();	// ī�޶� Ŭ�����κ��� ��������� ��´�.
		D3DXMATRIXA16* pProj = m_pCamMain->GetProjMatrix();
		m = (*pView) * (*pProj);				// World��ǥ�� ������ؼ� View * Proj����� ����Ѵ�.
		m_pFrustum->Make(&m, m_pCamMain->GetEye());	// View*Proj��ķ� Frustum�� �����.
	}
}

VOID Engine::Rendering()
{
	if (NULL == m_device)
		return;
	
	m_device->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	//m_device->SetRenderState(D3DRS_ZENABLE, TRUE);


	RenderReady();

	//���� ������
	if(m_bBallCamera)
		m_pCamMain->ResetBallView();
	else
		m_pCamMain->ResetView();
	
	D3DXMATRIXA16	m;
	D3DXMATRIXA16* pView = m_pCamMain->GetViewMatrix();	// ī�޶� Ŭ�����κ��� ��������� ��´�.
	D3DXMATRIXA16* pProj = m_pCamMain->GetProjMatrix();


	m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	if (SUCCEEDED(m_device->BeginScene()))
	{
		m_pTerrain->DrawMain(m_pFrustum);
		m_pTree2->DrawMain();
		m_pBall->DrawMain();
		m_pHole->DrawMain();
		m_pSkybox->DrawMain();
	
		m_pTree1->DrawMain(&_CalcBillBoard());

		if (m_bBallEnergyView || m_bBallJump)
		{
			m = (*pView) * (*pProj);
			m_pBar->DrawMain(&m, m_BallEnergy);
			
		}

		if (m_bSelectTriOn) 
		{
			m_pTri->DrawMain();
		}


		if (!m_bHideFrustum)
			m_pFrustum->Draw();
		

		m_device->EndScene();
	}


	//�� ������
	m_pCamMap->ResetView();

	m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

	if (SUCCEEDED(m_device->BeginScene()))
	{
		

		m_pTerrain->DrawMap();
		m_pBall->DrawMap();
		m_pHole->DrawMap();
		if (m_bSelectTriOn) {
			m_pTri->DrawMap();


		}
		
	
		m_device->EndScene();
	}


	//������ �������� Picking ����� �۵�
	if (m_bBallCamera)
		m_pCamMain->ResetBallView();
	else
		m_pCamMain->ResetView();
	
	

	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);

	m_device->Present(NULL, NULL, NULL, NULL);
}


VOID Engine::MeshPickingStart(int x, int y)
{
	Ray ray;
	ray.Create(m_device, x, y);
	temp[0] = *(ray.GetPos());
	temp[1] = *(ray.GetPos()) + 200* *(ray.GetDir());
	//printf("temp1 = %f %f %f\n", temp[0].x, temp[0].y, temp[0].z);
	//printf("temp2 = %f %f %f\n\n", temp[1].x, temp[1].y, temp[1].z);

	D3DXVECTOR3 pos[3];
	float dist = FLT_MAX;

	//Object��ü���� Search��������
	//0<buf_dist<dist pos��� �����ϸ�� - ���ο��� �����Ұ���
	m_pTerrain->MeshPicking(ray, dist, pos);
	
	//�� ���� �Ǿ����� � ��ġ ã������
	if (dist != FLT_MAX)
	{
		printf("click %.3f %.3f %.3f\n", pos[0].x, pos[0].y, pos[0].z);
		//�޽� ã����� ���������� ǥ���� �� �ְ� ������ - Pos ���� 3���� �ﰢ������ ����������
		m_pTri->Create(pos);
		
		m_bSelectTriOn = TRUE;

		

		//���е� �׸���
	}

}


VOID Engine::SetBallCamRotateY(float degree)
{
	D3DXVECTOR3* ballPos = m_pBall->GetCenter();
	m_pCamMain->SetBallViewRotateY(degree);
}
VOID Engine::SetBallCamRotateX(float degree)
{
	D3DXVECTOR3* ballPos = m_pBall->GetCenter();
	m_pCamMain->SetBallViewRotateX(degree);
}

BOOL Engine::ISBallInHole()
{
	//����� ���Ϳ��� �밢�� ���� + �� ������
	float in_size = m_pHole->GetR() * sqrtf(2.0f) + m_pBall->GetR();

	float dist = D3DXVec3Length(&(*m_pHole->GetCenter() - *m_pBall->GetCenter()));

	if (dist <= in_size)
	{
		printf("�浹 !");
		return true;
	}
	return false;
}

VOID Engine::SetBallJump(float energy)
{
	if (m_bBallJump)
		return;
	m_bBallJump = true;
	m_pBall->SetBallJump(energy, m_pCamMain->GetvView()); 
	m_bBallEnergyView = false;

}