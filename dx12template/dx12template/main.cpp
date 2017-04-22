#include "headers.h"
#include "render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"
#include "utility/FreeImage.h"

#include "playerObject.h"
#include "staticObject.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;

std::vector< SRenderObject > GRenderObjects[ RL_MAX ];
std::vector< CGameObject* > GGameObjects[2];
std::vector< CGameObject* > GGameObjectsToSpawn;

unsigned int GGameObjectArray = 0;
int const GWidth = 800;
int const GHeight = 800;
Matrix3x3 GScreenMatrix = Matrix3x3::GetOrthogonalMatrix(-0.5f * ((float)GWidth), 0.5f * (float)GHeight);

DXGI_FORMAT GFreeImageToDXGI[] =
{
	DXGI_FORMAT_R8_UNORM,			
	DXGI_FORMAT_R8_UNORM,			
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_UNKNOWN,			
	DXGI_FORMAT_B8G8R8A8_UNORM,
	DXGI_FORMAT_UNKNOWN				
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

	GRender.SetWindowWidth(GWidth);
	GRender.SetWindowHeight(GHeight);

	HWND hwnd = CreateWindow(
		L"WindowClass",
		L"Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		GWidth, GHeight,
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
		"../content/island.png",
		"../content/generator.png",
		"../content/bullet_0.png",
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

	SRenderObject gameObject;
	gameObject.m_size = 400.f;
	gameObject.m_texutreID = T_BACKGROUND;
	GRenderObjects[RL_BACKGROUND_STATIC].push_back(gameObject);

	gameObject.m_size = 350.f;
	gameObject.m_texutreID = T_ISLAND;
	GRenderObjects[RL_BACKGROUND_STATIC].push_back(gameObject);

	CPlayerObject* pPlayer = new CPlayerObject();
	GGameObjects[GGameObjectArray].push_back(pPlayer);

	gameObject.m_size = 16.f;
	gameObject.m_texutreID = T_GENERATOR;

	CStaticObject* pStaticObject = new CStaticObject(gameObject);
	GGameObjects[GGameObjectArray].push_back(pStaticObject);

	GRender.WaitForResourcesLoad();

	MSG msg = { 0 };
	bool run = true;
	while (run)
	{
		std::vector< CGameObject* >& currentGameObjectArray = GGameObjects[GGameObjectArray];

		unsigned int const gameObjectsNum = currentGameObjectArray.size();
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


		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			currentGameObjectArray[gameObjectID]->Update();
		}

		timeToRender -= GTimer.Delta();
		if (timeToRender < 0.f)
		{
			for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
			{
				currentGameObjectArray[gameObjectID]->FillRenderData();
			}
			GRender.DrawFrame();
			timeToRender = 1.f / 60.f;
		}

		for (unsigned int layerID = 1; layerID < RL_MAX; ++layerID)
		{
			GRenderObjects[layerID].clear();
		}

		unsigned int const nextGameObjectArray = (GGameObjectArray + 1) % 2;
		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			if (!currentGameObjectArray[gameObjectID]->NeedDelete())
			{
				GGameObjects[nextGameObjectArray].push_back(currentGameObjectArray[gameObjectID]);
			}
			else
			{
				delete currentGameObjectArray[gameObjectID];
			}
		}

		unsigned int const objectToSpawnNum = GGameObjectsToSpawn.size();
		for (unsigned int gameObjectID = 0; gameObjectID < objectToSpawnNum; ++gameObjectID)
		{
			GGameObjects[nextGameObjectArray].push_back(GGameObjectsToSpawn[gameObjectID]);
		}

		GGameObjectsToSpawn.clear();
		currentGameObjectArray.clear();
		GGameObjectArray = nextGameObjectArray;
	}

	unsigned int const gameObjectsNum = GGameObjects[GGameObjectArray].size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		delete GGameObjects[GGameObjectArray][gameObjectID];
	}
	unsigned int const objectToSpawnNum = GGameObjectsToSpawn.size();
	for (unsigned int gameObjectID = 0; gameObjectID < objectToSpawnNum; ++gameObjectID)
	{
		delete GGameObjectsToSpawn[gameObjectID];
	}

	GRender.Release();
	GSoundEngine.Release();
	return 0;
}