
#include <d3d9.h>

#include "Engine.h"

#define HEIGHT 800
#define WIDTH 800
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
		case VK_F3:
			engine.SetSelectOff();
			break;
		case VK_F4:
			engine.SetBallCamera();
			break;
		case VK_LEFT:

			break;

		case VK_RIGHT:

			break;
		case 'A':
			engine.SetCameraMoveX(-0.5f);
			//engine.SetBallAccAdd(D3DXVECTOR3(-0.005f, 0, 0));
			//engine.SetBallSpdAdd(D3DXVECTOR3(-0.05f, 0, 0));
			break;                     
		case 'D':
			engine.SetCameraMoveX(0.5f);
		//	engine.SetBallAccAdd(D3DXVECTOR3(0.005f, 0, 0));
			//engine.SetBallSpdAdd(D3DXVECTOR3(0.05f, 0, 0));
			break;
		case 'W':
			engine.SetCameraMoveZ(0.5f);
			//engine.SetBallAccAdd(D3DXVECTOR3(0, 0, 0.005f));
			//engine.SetBallSpdAdd(D3DXVECTOR3(0, 0, 0.05f));
			break;
		case 'S':
			engine.SetCameraMoveZ(-0.5f);
			//engine.SetBallAccAdd(D3DXVECTOR3(0, 0, -0.005f));
			//engine.SetBallSpdAdd(D3DXVECTOR3(0, 0, -0.05f));
			break;
		case VK_SPACE:
			//engine.SetCameraMoveZ(-0.5f);
			//engine.SetBallAccAdd(D3DXVECTOR3(0, 0.005f, 0));
			//engine.SetBallSpdAdd(D3DXVECTOR3(0, 0.1f, 0));
			break;
		}
		break;

	case WM_MOUSEMOVE:
		//��� ���콺 ��� ����
		//engine.MouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
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

	//ũ������ ����
	/*HWND hWnd = CreateWindow("D3D Tutorial", "D3D Terrain",
		WS_OVERLAPPEDWINDOW, -0, 0, WIDTH, HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);*/

	//ũ������ �Ұ�
	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Terrain",
		WS_MINIMIZEBOX | WS_SYSMENU, 200, 20, WIDTH, HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	START_CONSOLE();  //// ����� �ܼ� ����
	
	if (SUCCEEDED(engine.InitD3D(hWnd)) &&
		SUCCEEDED(engine.InitCam()) &&
		SUCCEEDED(engine.InitLight())&&
		SUCCEEDED(engine.InitObj()))
	{

		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);


		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			// �޽��ڰ� ������ ���� �´�. 
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



