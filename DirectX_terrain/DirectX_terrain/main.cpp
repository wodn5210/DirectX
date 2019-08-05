
#include <d3d9.h>
#include <time.h>
#include "Engine.h"
#include "FIleReadObj.h"

using namespace std;

#define HEIGHT 800
#define WIDTH 800
#pragma warning(disable:4996)
#define START_CONSOLE() {AllocConsole();  freopen("CONOUT$", "w", stdout); freopen("CONIN$", "r", stdin);}
#define STOP_CONSOLE()  {FreeConsole();}


Engine engine;
bool g_bBallCamera = false;
float energy;
time_t sT = 0, eT = 0;

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
			g_bBallCamera = !g_bBallCamera;
			engine.SetBallCamera();
			break;

		case VK_LEFT:			
			if (g_bBallCamera)
				engine.SetBallCamRotateY(-0.1f);
			break;
		case VK_RIGHT:
			if (g_bBallCamera)
				engine.SetBallCamRotateY(0.1f);
			break;
		case VK_UP:
			if (g_bBallCamera)
				engine.SetBallCamRotateX(-0.1f);
			break;
		case VK_DOWN:
			if (g_bBallCamera)
				engine.SetBallCamRotateX(0.1f);
			break;

		case 'A':
			if(g_bBallCamera)
				engine.SetBallSpdAdd(D3DXVECTOR3(-0.05f, 0, 0));
			else
				engine.SetCameraMoveX(-0.5f);

			break;                     
		case 'D':
			if (g_bBallCamera)
				engine.SetBallSpdAdd(D3DXVECTOR3(0.05f, 0, 0));
			else
				engine.SetCameraMoveX(0.5f);
			break;
		case 'W':
			if (g_bBallCamera)
				engine.SetBallSpdAdd(D3DXVECTOR3(0, 0, 0.05f));
			else
				engine.SetCameraMoveZ(0.5f);
			break;
		case 'S':
			if (g_bBallCamera)
				engine.SetBallSpdAdd(D3DXVECTOR3(0, 0, -0.05f));
			else
				engine.SetCameraMoveZ(-0.5f);
			break;
		case VK_SPACE:
			if(sT == 0)
				sT = clock();
			else
			{
				eT = clock();
				energy = (float)(eT - sT) / (CLOCKS_PER_SEC * 5);
				engine.SetEnergyView(energy);
			}
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			eT = clock();
			energy = (float)(eT - sT) / (CLOCKS_PER_SEC*5);
			//printf("energy = %f\n", energy);
			sT = eT = 0;
			if (energy >= 0.7f)
				energy = 0.7f;
			engine.SetBallJump(energy);

			break;
		}
		break;
	case WM_MOUSEMOVE:

		if (!g_bBallCamera)
			engine.MouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		if (g_bBallCamera)
			engine.SetBallCamRotateY(-0.1f);
		else
			engine.MeshPickingStart(LOWORD(lParam), HIWORD(lParam));

		break;
	case WM_RBUTTONDOWN:
		if (g_bBallCamera)
			engine.SetBallCamRotateY(0.1f);
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


	//크기조절 불가
	HWND hWnd = CreateWindow("D3D Tutorial", "D3D Terrain",
		WS_MINIMIZEBOX | WS_SYSMENU, 200, 20, WIDTH, HEIGHT,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	START_CONSOLE();  //// 디버그 콘솔 시작

	
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



