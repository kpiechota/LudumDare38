#include "headers.h"
#include "render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;

std::vector< SGameObject > GGameObjects;

int const width = 800;
int const height = 800;
Matrix3x3 GScreenMatrix = Matrix3x3::GetOrthogonalMatrix(-((float)width), (float)width);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow)
{
	WNDCLASS windowClass = { 0 };

	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = CInputManager::WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClass(&windowClass))
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
	}

	GSoundEngine.Init();

	GRender.SetWindowWidth(width);
	GRender.SetWindowHeight(height);

	HWND hwnd = CreateWindow(
		L"WindowClass",
		L"Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL, NULL,
		hInstance,
		NULL);

	if (hwnd == 0)
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
	}

	GRender.SetHWND(hwnd);

	ShowWindow(hwnd, nCmdShow);

	GRender.Init();

	GInputManager.SetHWND(hwnd);

	GInputManager.Init();
	GInputManager.AddObserver(&GSystemInput);

	float timeToRender = 0.f;

	SGameObject gameObject;
	gameObject.m_size.Set(100.f, 100.f);
	GGameObjects.push_back(gameObject);

	MSG msg = { 0 };
	bool run = true;
	while (run)
	{
		GTimer.Tick();

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				run = false;
				break;
			}
		}

		timeToRender -= GTimer.Delta();
		if (timeToRender < 0.f)
		{
			GRender.DrawFrame();
			timeToRender = 1.f / 60.f;
		}
	}

	GRender.Release();
	GSoundEngine.Release();
	return 0;
}