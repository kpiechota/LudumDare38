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
CStaticObject* GBackgroundStars;

std::vector< SRenderData > GRenderObjects[ RL_MAX ];
std::vector< SRenderData > GBakeObjects;
std::vector< CGameObject* > GGameObjects;
std::vector< CGameObject* > GGameObjectsToSpawn;
std::vector< CGameObject* > GGameObjectsToDelete;
CStaticSound GSounds[SET_MAX];

int GWidth = 800;
int GHeight = 800;
float const GIslandSize = 350.f;
Matrix3x3 GScreenMatrix;

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
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		delete GGameObjects[gameObjectID];
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
	GGameObjects.clear();
	GBakeObjects.clear();

	GRender.ClearBaked();

	GBackgroundStars = new CStaticObject( -1.f, 0, RL_BACKGROUND);
	GBackgroundStars->SetScale( 400.f );
	GBackgroundStars->SetShaderID( ST_OBJECT_DRAW_NO_CLIP );
	GBackgroundStars->SetTextureID( T_BACKGROUND );
	GGameObjectsToSpawn.push_back(GBackgroundStars);

	CStaticObject* pIsland = new CStaticObject( -1.f, 0, RL_BACKGROUND);
	pIsland->SetScale( GIslandSize );
	pIsland->SetShaderID( ST_OBJECT_DRAW );
	pIsland->SetTextureID( T_ISLAND );
	GGameObjectsToSpawn.push_back(pIsland);

	CStaticObject* pBaked = new CStaticObject( -1.f, 0, RL_BACKGROUND);
	pBaked->SetScale( 400.f );
	pBaked->SetShaderID( ST_OBJECT_DRAW_ALPHA_MULT );
	pBaked->SetTextureID( T_BAKED );
	pBaked->SetUvTile( Vec2(1.f, -1.f) );
	GGameObjectsToSpawn.push_back(pBaked);

	GPlayer = new CPlayerObject();
	GGameObjectsToSpawn.push_back(GPlayer);

	GGenerator = new CGeneratorObject();
	GGenerator->SetScale( 32.f );
	GGenerator->SetColliderSize( 32.f );
	GGenerator->SetTextureID( T_GENERATOR );
	GGameObjectsToSpawn.push_back(GGenerator);

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
	
	ShowWindow(hwnd, nCmdShow);

	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	GWidth = clientRect.right - clientRect.left;
	GHeight = clientRect.bottom - clientRect.top;
	GScreenMatrix = Matrix3x3::GetOrthogonalMatrix(-0.5f * ((float)GWidth), 0.5f * ((float)GWidth), -0.5f * ((float)GHeight), 0.5f * ((float)GHeight));

	GSoundEngine.Init();

	GRender.SetWindowWidth(GWidth);
	GRender.SetWindowHeight(GHeight);
	GRender.SetHWND(hwnd);

	char const* textures[] =
	{
		"../content/blank.png",
		"../content/player.png",
		"../content/background.tif",
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
		"../content/sdf_font_512.png",
		"../content/sdf_font_64.png",
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
		unsigned int const gameObjectsNum = GGameObjects.size();
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

		GBackgroundStars->SetUvOffset( Vec2( GTimer.GetSeconds( GTimer.TimeFromStart() ), 0.f ) );

		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			GGameObjects[gameObjectID]->Update();
		}

		timeToRender -= GTimer.Delta();
		if (timeToRender < 0.f)
		{
			for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
			{
				GGameObjects[gameObjectID]->FillRenderData();
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

		for (unsigned int gameObjectID = 0; gameObjectID < GGameObjects.size(); ++gameObjectID)
		{
			if (GGameObjects[gameObjectID]->NeedDelete())
			{
				CGameObject* const pObject = GGameObjects[gameObjectID];
				GGameObjectsToDelete.push_back(pObject);
				GGameObjects[gameObjectID] = GGameObjects.back();
				GGameObjects.pop_back();
				--gameObjectID;
			}
		}

		unsigned int const objectToSpawnNum = GGameObjectsToSpawn.size();
		for (unsigned int gameObjectID = 0; gameObjectID < objectToSpawnNum; ++gameObjectID)
		{
			GGameObjects.push_back(GGameObjectsToSpawn[gameObjectID]);
			GGameObjectsToSpawn[ gameObjectID ]->Start();
		}

		GGameObjectsToSpawn.clear();

		if ((GPlayer->GetHealth() * GGenerator->GetHealth()) <= 0.f )
		{
			if (!pDeathScreen)
			{
				pDeathScreen = new CStaticObject( -1.f, 0, RL_OVERLAY2);
				pDeathScreen->SetScale( 400.f );
				pDeathScreen->SetShaderID( ST_OBJECT_DRAW_BLEND );
				pDeathScreen->SetTextureID( T_DEATH_SCREEN );

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
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		delete GGameObjects[gameObjectID];
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