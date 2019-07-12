#include "Engine.h"

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	DWORD color;        // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

HRESULT Engine::InitD3D(HWND hWnd)
{
	if (NULL == (_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	if (FAILED(_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &_pd3dDevice)))
	{
		return E_FAIL;
	}
	_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	
	return S_OK;
}

VOID Engine::Render()
{
	D3DXMATRIXA16 matWorld, translation, rotation, scale;
	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;

	_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

	_InitLight();
	_SetupMatrices();


	if (SUCCEEDED(_pd3dDevice->BeginScene()))
	{
		_pd3dDevice->SetStreamSource(0, _pVB, 0, sizeof(CUSTOMVERTEX));
		_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
		// End the scene
		_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	_pd3dDevice->Present(NULL, NULL, NULL, NULL);									//화면에 나타내기 (후면버퍼의 내용을 전면버퍼로 전송)
}

HRESULT Engine::_InitGeometry()
{
	return S_OK;
}
HRESULT Engine::InitVB()
{
	CUSTOMVERTEX vertices[] =
	{
		{ 150.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
		{  50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
	};

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 3 custom vertices. We also
	// specify the FVF, so the vertex buffer knows what data it contains.
	if (FAILED(_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,				//정점버퍼 종류(SW, HW), 정점 구조체에 따른 FVF플래그
		D3DPOOL_DEFAULT, &_pVB, NULL)))	//정점버퍼가 저장될 메모리 위치, 반환 인터페이스
	{
		return E_FAIL;
	}

	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required becuase vertex
	// buffers may be in device memory.
	VOID* pVertices;																	//정점버퍼는 기본적으로 쓰레기값이라 초기화해야함
	if (FAILED(_pVB->Lock(0, sizeof(vertices), (void**)& pVertices, 0)))		//Lock을 이용해서 메모리 포인터 가져오기
		return E_FAIL;																	//
	memcpy(pVertices, vertices, sizeof(vertices));
	_pVB->Unlock();
	return S_OK;
}

HRESULT Engine::InitIB()
{
	return S_OK;
}
VOID Engine::_SetupMatrices()
{

}
HRESULT Engine::_InitLight()
{
	return S_OK;
}