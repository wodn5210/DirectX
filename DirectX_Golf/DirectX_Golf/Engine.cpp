#include "Engine.h"


Engine::Engine()
{

}
Engine::~Engine()
{
	if (m_device != NULL)
		m_device->Release();

	if (m_pD3D != NULL)
		m_pD3D->Release();

	
}
HRESULT Engine::Init(HWND hWnd)
{
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	m_hwnd = hWnd;
	RECT	rc;
	GetClientRect(m_hwnd, &rc);
	m_winSizeX = (WORD)(rc.right - rc.left);
	m_winSizeY = (WORD)(rc.bottom - rc.top);
	SetCursorPos(m_winSizeX / 2, m_winSizeY / 2);


	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = m_winSizeX;               // 버퍼 해상도 넓이 설정
	d3dpp.BackBufferHeight = m_winSizeY;               // 버퍼 해상도 높이 설정 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;   // 버퍼 포맷 설정 
	d3dpp.BackBufferCount = 1;                 // 백버퍼 수 
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

	// Z버퍼기능을 켠다.
	m_device->SetRenderState(D3DRS_ZENABLE, TRUE);



	//Scene 초기화
	_InitScene();

	return S_OK;
}

VOID Engine::_InitScene()
{
	m_pSceneStart = new SceneStart();
	m_pSceneStart->Create(m_device, m_hwnd);
	m_nowPhase = START;


	m_pSceneGame = NULL;
	m_pSceneEnd = NULL;
	

	m_pNowScene = m_pSceneStart;
}


VOID Engine::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//어떤 이벤트 들어올땐 화면이 켜져있어야함
	if (m_pNowScene != NULL)
	{
		int select = m_pNowScene->MsgProcess(hWnd, msg, wParam, lParam);

		switch (select)
		{
		case 0:
			//이미 비어있다면 다른 함수에서 진행중임
			if (m_pNowScene == NULL)
				break;
			delete m_pNowScene;
			if (m_nowPhase == START || m_nowPhase == END)
			{
				/*if(m_pSceneStart != NULL)
					delete m_pSceneStart;
				else
					delete m_pSceneEnd;*/
				m_pSceneGame = new SceneGame();
				m_pSceneGame->Create(m_device, m_hwnd);
				m_pNowScene = m_pSceneGame;
				m_nowPhase = GAME;
			}
			//여기아래 게임 넣으면 변경해야함
			else if (m_nowPhase == GAME)
			{
				printf("hitcount = %d\n", msg);
				//delete m_pSceneGame;
				m_pSceneEnd = new SceneEnd();
				m_pSceneEnd->Create(m_device, m_hwnd);
				m_pNowScene = m_pSceneEnd;
				m_nowPhase = END;
			}
			
			break;
		case 1:
			PostMessage(hWnd, WM_DESTROY, 0, 0L);
			break;
		}

	}

}

VOID Engine::Rendering()
{
	m_pNowScene->Rendering();

}