#include "headers.h"
#include "render.h"

#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);

		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow)
{
	WNDCLASS windowClass = { 0 };

	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"WindowClass";
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	if (!RegisterClass(&windowClass))
	{
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
	}

	int const width = 720;
	int const height = 480;

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

	MSG msg = { 0 };
	bool run = true;
	while (run)
	{
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

		GRender.DrawFrame();
	}

	GRender.Release();
	return 0;
}