#include "headers.h"
#include "render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"
#include "utility/FreeImage.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;

std::vector< SGameObject > GGameObjects;

int const width = 800;
int const height = 800;
Matrix3x3 GScreenMatrix = Matrix3x3::GetOrthogonalMatrix(-((float)width), (float)width);

DXGI_FORMAT GFreeImageToDXGI[] =
{
	DXGI_FORMAT_R8_UNORM,			
	DXGI_FORMAT_R8_UNORM,			
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_UNKNOWN,			
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_UNKNOWN				
};

enum ETextures
{
	T_PLAYER,
	T_BACKGROUND,
	T_ISLAND,
};

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

	char const* textures[] =
	{
		"../content/player.png",
		"../content/background.png",
		"../content/island.png"
	};

	GRender.Init();

	GRender.BeginLoadResources(ARRAYSIZE(textures));

	for (unsigned int texutreID = 0; texutreID < ARRAYSIZE(textures); ++texutreID)
	{
		STexture texutre;

		FREE_IMAGE_FORMAT const format = FreeImage_GetFileType(textures[texutreID]);
		if (format == FIF_UNKNOWN)
		{
			__debugbreak();
		}

		FIBITMAP* bitmap = FreeImage_Load(format, textures[texutreID]);
		if (!bitmap)
		{
			__debugbreak();
		}

		texutre.m_data = FreeImage_GetBits(bitmap);
		texutre.m_width = FreeImage_GetWidth(bitmap);
		texutre.m_height = FreeImage_GetHeight(bitmap);
		FREE_IMAGE_COLOR_TYPE const colorType = FreeImage_GetColorType(bitmap);
		texutre.m_format = GFreeImageToDXGI[colorType];

		if (texutre.m_format == DXGI_FORMAT_UNKNOWN)
		{
			__debugbreak();
		}

		GRender.LoadResource(texutre);

		FreeImage_Unload(bitmap);
	}


	GRender.EndLoadResources();

	GInputManager.SetHWND(hwnd);

	GInputManager.Init();
	GInputManager.AddObserver(&GSystemInput);

	float timeToRender = 0.f;

	SGameObject gameObject;
	gameObject.m_size.Set(800.f, 800.f);
	gameObject.m_texutreID = T_BACKGROUND;
	GGameObjects.push_back(gameObject);

	gameObject.m_size.Set(700.f, 700.f);
	gameObject.m_texutreID = T_ISLAND;
	GGameObjects.push_back(gameObject);

	gameObject.m_size.Set(32.f, 32.f);
	gameObject.m_texutreID = T_PLAYER;
	GGameObjects.push_back(gameObject);

	GRender.WaitForResourcesLoad();

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