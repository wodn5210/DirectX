
#include <d3dx9.h>
#include "Engine.h"

#define HEIGHT 800
#define WIDTH 800

//�ܼ�â ����׿� �ڵ�
#pragma warning(disable:4996)
#define START_CONSOLE() {AllocConsole();  freopen("CONOUT$", "w", stdout); freopen("CONIN$", "r", stdin);}
#define STOP_CONSOLE()  {FreeConsole();}


Engine engine;



LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostMessage(hWnd, WM_DESTROY, 0, 0L);
			break;
		}
		break;
	}

	//�ƹ��͵� �ȴ������� return -1�Ǽ� ��
	engine.MsgProc(hWnd, msg, wParam, lParam);
	


	

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  "DirectX Golf", NULL };
	RegisterClassEx(&wc);


	//ũ������ �Ұ�
	HWND hWnd = CreateWindow("DirectX Golf", "DirectX Golf",
		WS_MINIMIZEBOX | WS_SYSMENU, 200, 20, WIDTH, HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	START_CONSOLE();  //// ����� �ܼ� ����

	if (SUCCEEDED(engine.Init(hWnd)))
	{

		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);


		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				engine.Rendering();
		}
	}

	UnregisterClass("DirectX Golf", wc.hInstance);
	return 0;
}



