#include <d3d9.h>
#include "Engine.h"

#define HEIGHT 768
#define WIDTH 1024
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
		case VK_F1:
			engine.SetFrustum();
			break;
		case VK_F2:
			engine.SetWire();
			break;
		}
	
	case WM_LBUTTONDOWN:
		engine.MeshPickingStart(LOWORD(lParam), HIWORD(lParam));
		//printf("x: %d y: %d\n", LOWORD(lParam), HIWORD(lParam));
		break;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
					  GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
					  "D3D Tutorial", NULL };
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Terrain",
		WS_OVERLAPPEDWINDOW, -0, 0, WIDTH, HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	START_CONSOLE();  //// 디버그 콘솔 시작
	
	if (SUCCEEDED(engine.InitD3D(hWnd)) &&
		SUCCEEDED(engine.InitObj()) &&
		SUCCEEDED(engine.InitLight())&&
		SUCCEEDED(engine.InitView()))
	{

		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);


		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			// 메시자가 있으면 가져 온다. 
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
				engine.Rendering();
		}
	}


	UnregisterClass("D3D Tutorial", wc.hInstance);
	return 0;
}



