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
	// �⺻�ø�, CCW
	//g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Z���۱���� �Ҵ�.
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	return S_OK;
}

HRESULT Engine::InitView()
{
	g_pCamera = new Camera(g_pd3dDevice);

	// �� ����� ����
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
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);			// ���������� �Ҵ�

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00909090);		// ȯ�汤��(ambient light)�� �� ����

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
	float	fDelta = 0.001f;	// ���콺�� �ΰ���, �� ���� Ŀ������ ���� �����δ�.

	GetCursorPos(&pt);
	int dx = pt.x - g_dwMouseX;	// ���콺�� ��ȭ��
	int dy = pt.y - g_dwMouseY;	// ���콺�� ��ȭ��

	g_pCamera->RotateLocalX(dy * fDelta);	// ���콺�� Y�� ȸ������ 3D world��  X�� ȸ����
	g_pCamera->RotateLocalY(dx * fDelta);	// ���콺�� X�� ȸ������ 3D world��  Y�� ȸ����
	D3DXMATRIXA16* pmatView = g_pCamera->GetViewMatrix();		// ī�޶� ����� ��´�.
	g_pd3dDevice->SetTransform(D3DTS_VIEW, pmatView);			// ī�޶� ��� ����


	// ���콺�� �������� �߾����� �ʱ�ȭ
	//SetCursor( NULL );	// ���콺�� ��Ÿ���� �ʰ� �ʴ�.
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
	//��ġ ���� �ʿ��� ���� ����. Obj�� �߽��� ������? - �־���Ѵٸ� SetBillBoard ���ڷ� Obj�߽� ����
	matBillBoard._41 = ;
	matBillBoard._42 = ;
	matBillBoard._43 = ;
	*/
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matBillBoard);
}

VOID Engine::RenderReady()
{
	//�� ����
	InitLight();

	//���콺 �̺�Ʈ
	_MouseEvent();

	//Ű���� �̺�Ʈ
	_KeyEvent();


	//�������� ȿ�� ����ȭ�ϱ����Ѱ�.
	//������ ���������� �����Ǿ��� ��������ȿ���� �����ش�
	if (!g_bLockFrustum)
	{
		D3DXMATRIXA16	m;
		D3DXMATRIXA16* pView = g_pCamera->GetViewMatrix();	// ī�޶� Ŭ�����κ��� ��������� ��´�.
		D3DXMATRIXA16* pProj = g_pCamera->GetProjMatrix();

		m = (*pView) * (*pProj);				// World��ǥ�� ������ؼ� View * Proj����� ����Ѵ�.
		g_pFrustum->Make(&m, g_pCamera->GetEye());	// View*Proj��ķ� Frustum�� �����.
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
		
		//��
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
	
	//Object��ü���� Search��������
	//dist �ִ밪�϶� pos��� �����ϸ�� - ���ο��� �����Ұ���
	g_pTerrain->MeshPicking(ray, dist, pos);

	//�� ���� �Ǿ����� � ��ġ ã������
	if(dist != FLT_MAX)
	{
		//�޽� ã����� ���������� ǥ���� �� �ְ� ������ - Pos ���� 3���� �ﰢ������ ����������
		
		//���� �ﰢ�� ��� �׽�Ʈ �ڵ�
		//����ȭ�� ���ؼ� 1 dir��ŭ ȭ������ ��ܿ´� - ���� ��ġ�� ����ȭ�� �߾ȵ�
		CUSTOMVERTEX g_Vertices[] =
		{
			{ pos[0] ,  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
			{ pos[1],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
			{ pos[2],  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		};

		/// �������� ����
		if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
			0, D3DFVF_CUSTOMVERTEX,
			D3DPOOL_DEFAULT, &g_pVB, NULL)))
		{
			return;
		}

		/// �������۸� ������ ä���. 
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