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
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
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
	D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	g_pCamera->SetView(&vEyePt, &vLookatPt, &vUpVec);

	return S_OK;
}
HRESULT Engine::InitLight()
{
	// ����(material)����
// ������ ����̽��� �� �ϳ��� ������ �� �ִ�.
	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial(&mtrl);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);			// ���������� �Ҵ�

	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);		// ȯ�汤��(ambient light)�� �� ����

	return S_OK;
}
HRESULT Engine::InitObj()
{
	CUSTOMVERTEX g_Vertices[] =
	{
		{ D3DXVECTOR3(-1.0f, 20.0f, 20.0f),  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
	  
		{  D3DXVECTOR3(0.0f, 21.7f, 20.0f),  D3DXVECTOR3(-1.0f,-1.0f, 0.0f), },
		{	D3DXVECTOR3(1.0f, 20.0f, 20.0f),  D3DXVECTOR3(-1.0f,-1.0f, 0.0f),},
	};

	/// �������� ����
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	/// �������۸� ������ ä���. 
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(g_Vertices), (void**)& pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	g_pVB->Unlock();

	//������ �׽�Ʈ
	//�Ʒ��� ��¥
	vector<string> tex_file_dir;
	tex_file_dir.push_back("src/tile2.tga");
	g_pTerrain = new Terrain;
	g_pTerrain->Create(g_pd3dDevice, &D3DXVECTOR3(1.0f, 0.1f, 1.0f), 0.1f, 
		"src/map129.bmp", tex_file_dir);


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
	SetCursor( NULL );	// ���콺�� ��Ÿ���� �ʰ� �ʴ�.
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
	if (GetAsyncKeyState('W')) g_pCamera->MoveLocalZ(0.5f);	// ī�޶� ����!
	if (GetAsyncKeyState('S')) g_pCamera->MoveLocalZ(-0.5f);	// ī�޶� ����!
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
}

VOID Engine::Rendering()
{
	if (NULL == g_pd3dDevice)
		return;


	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

	RenderReady();
	

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{

		g_pTerrain->Draw();

		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		_SetBillBoard();
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);

		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}