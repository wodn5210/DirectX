#include "SceneEnd.h"

SceneEnd::~SceneEnd()
{
	if (m_pReStartBnt != NULL)
		delete m_pReStartBnt;
	if (m_pEndBnt != NULL)
		delete m_pEndBnt;
	if (m_pCamMain != NULL)
		delete m_pCamMain;
	if (m_pSprite != NULL)
		m_pSprite->Release();
	if (m_pFont != NULL)
		m_pFont->Release();
}

HRESULT SceneEnd::Create(LPDIRECT3DDEVICE9 device, HWND hWnd, int hitCount)
{
	Scene::Create(device, hWnd);

	m_hitCount = hitCount;
	_InitCam();
	_InitObj();

	return S_OK;
}

HRESULT SceneEnd::_InitCam()
{
	// �� ����� ����
	m_pCamMain = new CamMain();
	m_pCamMain->Create(m_device);
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -1.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);

	m_pCamMain->SetView(&vEyePt, &vLookatPt);
	m_pCamMain->SetViewport(D3DVIEWPORT9{ 0, 0, m_winSizeX, m_winSizeY, 0, 1 });

	return S_OK;
}
HRESULT SceneEnd::_InitLight()
{

	// ���������� �Ҵ�
	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// ȯ�汤��(ambient light)�� �� ����
	m_device->SetRenderState(D3DRS_AMBIENT, 0xffffffff);

	return S_OK;
}
HRESULT SceneEnd::_InitObj()
{
	m_pReStartBnt = new ObjButton();
	m_pReStartBnt->Create(m_device, "src/golf/ReGame.bmp");
	m_pReStartBnt->SetDrawPos(pos_restart[0], pos_restart[1]);


	m_pEndBnt = new ObjButton();
	m_pEndBnt->Create(m_device, "src/golf/GameEnd2.bmp");
	m_pEndBnt->SetDrawPos(pos_end[0], pos_end[1]);


	//Setup�κ�
	D3DXCreateFont(m_device, 25, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "����ü", &m_pFont);
	D3DXCreateSprite(m_device, &m_pSprite);

	return S_OK;
}
VOID SceneEnd::_ReadyRender()
{
	_InitLight();


	D3DXMATRIXA16 matViewProj = (*m_pCamMain->GetViewMatrix()) * (*m_pCamMain->GetProjMatrix());
	m_pReStartBnt->ReadyRender(&matViewProj);
	m_pEndBnt->ReadyRender(&matViewProj);

}
VOID SceneEnd::_RenderText()
{
	if (m_pSprite == NULL)
		return;


	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	RECT rt = { 290,300,0,0 };
	string name = "   Hit Score";
	m_pFont->DrawText(m_pSprite, name.c_str(), -1, &rt, DT_NOCLIP, D3DCOLOR_XRGB(255, 0, 0));


	rt.top = 340;
	rt.left = 375;
	string score = to_string(m_hitCount);
	m_pFont->DrawText(m_pSprite, score.c_str(), -1, &rt, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 0));
	m_pSprite->End();

}
VOID SceneEnd::Rendering()
{
	if (NULL == m_device)
		return;

	m_pCamMain->ResetView();

	m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	_ReadyRender();
	if (SUCCEEDED(m_device->BeginScene()))
	{
		m_pReStartBnt->DrawMain();
		m_pEndBnt->DrawMain();

		_RenderText();

		m_device->EndScene();
	}

	m_device->Present(NULL, NULL, NULL, NULL);
}
int SceneEnd::MsgProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONUP:
		return _BntSearch(LOWORD(lParam), HIWORD(lParam));
	}
	return -1;
}

int SceneEnd::_BntSearch(int x, int y)
{
	float px = (((2.0f * x) / m_winSizeX) - 1.0f);
	float py = (((-2.0f * y) / m_winSizeY) + 1.0f);

	for (int i = 0; i < 2; i++)
	{

		D3DXVECTOR2* pos;
		if (i == 0)
			pos = &pos_restart[0];
		else
			pos = &pos_end[0];

		if (pos[0].x <= px && px <= pos[1].x &&
			pos[1].y <= py && py <= pos[0].y)
		{
			return i;
		}
	}

	return -1;
}