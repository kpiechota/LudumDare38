#include "headers.h"
#include "render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"
#include "utility/FreeImage.h"

#include "playerObject.h"
#include "generatorObject.h"
#include "staticObject.h"

#include "enemySpawner.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;
CEnemySpawner GEnemySpawner;
CPlayerObject* GPlayer;
CGeneratorObject* GGenerator;

std::vector< SRenderObject > GRenderObjects[ RL_MAX ];
std::vector< SRenderObject > GBakeObjects;
std::vector< CGameObject* > GGameObjects[2];
std::vector< CGameObject* > GGameObjectsToSpawn;
std::vector< CGameObject* > GGameObjectsToDelete;
CStaticSound GSounds[SET_MAX];

unsigned int GGameObjectArray = 0;
int const GWidth = 800;
int const GHeight = 800;
float const GIslandSize = 350.f;
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

void InitGame()
{
	unsigned int const objectToDeleteNum = GGameObjectsToDelete.size();
	for (unsigned int gameObjectID = 0; gameObjectID < objectToDeleteNum; ++gameObjectID)
	{
		delete GGameObjectsToDelete[gameObjectID];
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

	for (unsigned int layerID = 0; layerID < RL_MAX; ++layerID)
	{
		GRenderObjects[layerID].clear();
	}

	GGameObjectsToDelete.clear();
	GGameObjectsToSpawn.clear();
	GGameObjects[GGameObjectArray].clear();
	GBakeObjects.clear();

	GRender.ClearBaked();

	SRenderObject gameObject;
	gameObject.m_size = 400.f;
	gameObject.m_texutreID = T_BACKGROUND;
	GRenderObjects[RL_BACKGROUND_STATIC].push_back(gameObject);

	gameObject.m_size = GIslandSize;
	gameObject.m_texutreID = T_ISLAND;
	GRenderObjects[RL_BACKGROUND_STATIC].push_back(gameObject);

	gameObject.m_size = 400.f;
	gameObject.m_texutreID = T_BAKED;
	gameObject.m_shaderID = ST_OBJECT_DRAW_BLEND;
	gameObject.m_uvTile.y = -1.f;
	GRenderObjects[RL_BACKGROUND_STATIC].push_back(gameObject);

	GPlayer = new CPlayerObject();
	GGameObjects[GGameObjectArray].push_back(GPlayer);

	gameObject.m_size = 32.f;
	gameObject.m_texutreID = T_GENERATOR;

	GGenerator = new CGeneratorObject(gameObject);
	GGameObjects[GGameObjectArray].push_back(GGenerator);

	GEnemySpawner.Init();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, INT nCmdShow)
{
	srand(time(NULL));
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
		L"Small World",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
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
		"../content/blank.png",
		"../content/player.png",
		"../content/background.png",
		"../content/island.png",
		"../content/generator.png",
		"../content/bullet_0.png",
		"../content/bullet_1.png",
		"../content/enemy.png",
		"../content/turret.png",
		"../content/health.png",
		"../content/healthEffect.tif",
		"../content/turretIcon.png",
		"../content/healthIcon.png",
		"../content/veins.tif",
		"../content/initScreen.tif",
		"../content/deathScreen.tif",
		"../content/deadEnemy.tif",
		"../content/ground.png",
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

	InitGame();

	char const* sounds[] =
	{
		"../content/shoot0.wav",
		"../content/shoot1.wav",
		"../content/shoot2.wav",
		"../content/heal.wav",
		"../content/explosion.wav",
		"../content/build.wav",
	};

	for (unsigned int soundID = 0; soundID < SET_MAX; ++soundID)
	{
		GSounds[soundID] = GSoundEngine.CreateStaticSound(sounds[soundID]);
	}

	GRender.WaitForResourcesLoad();

	CStaticObject* pDeathScreen = nullptr;

	MSG msg = { 0 };
	bool run = true;
	while (run)
	{
		std::vector< CGameObject* >& currentGameObjectArray = GGameObjects[GGameObjectArray];

		unsigned int const gameObjectsNum = currentGameObjectArray.size();
		GTimer.Tick();
		GEnemySpawner.Update();

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


		GRenderObjects[RL_BACKGROUND_STATIC][0].m_uvOffset.x += GTimer.GameDelta();

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

		unsigned int const objectToDeleteNum = GGameObjectsToDelete.size();
		for (unsigned int gameObjectID = 0; gameObjectID < objectToDeleteNum; ++gameObjectID)
		{
			delete GGameObjectsToDelete[gameObjectID];
		}
		GGameObjectsToDelete.clear();

		unsigned int const nextGameObjectArray = (GGameObjectArray + 1) % 2;
		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			if (!currentGameObjectArray[gameObjectID]->NeedDelete())
			{
				GGameObjects[nextGameObjectArray].push_back(currentGameObjectArray[gameObjectID]);
			}
			else
			{
				GGameObjectsToDelete.push_back( currentGameObjectArray[gameObjectID] );
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

		if ((GPlayer->GetHealth() * GGenerator->GetHealth()) <= 0.f )
		{
			if (!pDeathScreen)
			{
				SRenderObject initDeathObject;
				initDeathObject.m_shaderID = ST_OBJECT_DRAW_BLEND;
				initDeathObject.m_texutreID = T_DEATH_SCREEN;
				initDeathObject.m_size = 400.f;

				pDeathScreen = new CStaticObject(initDeathObject, -1.f, 0, RL_OVERLAY2);
				GGameObjectsToSpawn.push_back(pDeathScreen);

				GTimer.SetGameScale(0.1f);
			}
			else if (GInputManager.IsKeyDown(' '))
			{
				InitGame();
				pDeathScreen = nullptr;
			}
		}
	}

	unsigned int const objectToDeleteNum = GGameObjectsToDelete.size();
	for (unsigned int gameObjectID = 0; gameObjectID < objectToDeleteNum; ++gameObjectID)
	{
		delete GGameObjectsToDelete[gameObjectID];
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