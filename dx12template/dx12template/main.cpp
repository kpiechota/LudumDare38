#include "headers.h"
#include "render.h"
#include "soundEngine.h"
#include "timer.h"
#include "input.h"
#include "../DirectXTex/DirectXTex.h"

#include "geometryLoader.h"

#include <Windows.h>

CInputManager GInputManager;
CSystemInput GSystemInput;
CTimer GTimer;

SViewObject GViewObject;
std::vector< CGameObject* > GGameObjects;
std::vector< CGameObject* > GGameObjectsToSpawn;
std::vector< CGameObject* > GGameObjectsToDelete;
SGeometryInfo GGeometryInfo[ G_MAX ];
CStaticSound GSounds[SET_MAX];

int GWidth = 800;
int GHeight = 800;
CStaticObject* testObject;
CLightObject* testLightR;
CLightObject* testLightG;
CLightObject* testLightB;

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
		GViewObject.m_renderData[layerID].clear();
		GViewObject.m_lightData.clear();
	}

	GGameObjectsToDelete.clear();
	GGameObjectsToSpawn.clear();
	GGameObjects.clear();

	testObject = new CStaticObject( 0, RL_OPAQUE );
	testObject->SetPosition( Vec3( 0.f, -2.f, 10.f ) );
	testObject->SetScale( .25f );
	testObject->SetGeomtryInfoID( G_SPACESHIP );
	testObject->SetTextureID( 0, T_SPACESHIP );
	testObject->SetTextureID( 1, T_SPACESHIP_N );
	testObject->SetTextureID( 2, T_SPACESHIP_E );
	testObject->SetTextureID( 3, T_SPACESHIP_S );

	GGameObjectsToSpawn.push_back( testObject );

	testLightR = new CLightObject( LT_POINT );
	testLightR->SetColor( Vec3( 2.f, 0.f, 0.f ) );
	testLightR->SetRadius( 3.f );
	GGameObjectsToSpawn.push_back( testLightR );

	testLightG = new CLightObject( LT_POINT );
	testLightG->SetColor( Vec3( 0.f, 2.f, 0.f ) );
	testLightG->SetRadius( 3.f );
	GGameObjectsToSpawn.push_back( testLightG );

	testLightB = new CLightObject( LT_POINT );
	testLightB->SetColor( Vec3( 0.f, 0.f, 2.f ) );
	testLightB->SetRadius( 3.f );
	GGameObjectsToSpawn.push_back( testLightB );
}

void DrawDebugInfo()
{
	char fpsText[ 15 ];
	sprintf_s( fpsText, 15, "FPS:%.2f", 1.f / GTimer.Delta() );
	GTextRenderManager.Print( Vec4( 1.f, 0.5f, 0.f, 1.f ), Vec2( 0.f, 1.f - 0.025f ), 0.05f, fpsText );
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

	GSoundEngine.Init();
	GGeometryLoader.Init();

	GRender.SetWindowWidth(GWidth);
	GRender.SetWindowHeight(GHeight);
	GRender.SetHWND(hwnd);

	char const* meshes[] =
	{
		"../content/spaceship",
	};
	CT_ASSERT( ARRAYSIZE( meshes ) == G_MAX );

	wchar_t const* textures[] =
	{
		L"../content/sdf_font_512.png",
		L"../content/spaceship_d.png",
		L"../content/spaceship_n.png",
		L"../content/spaceship_e.png",
		L"../content/spaceship_s.png",
	};
	CT_ASSERT( ARRAYSIZE( textures ) == T_MAX );

	GRender.Init();
	GRender.SetDirectLightColor( Vec3( 1.f, 1.f, 1.f ) );
	GRender.SetDirectLightDir( Vec3( 0.f, 1.f, 1.f ).GetNormalized() );
	GRender.SetAmbientLightColor( Vec3( 0.1f, 0.1f, 0.1f ) );

	GRender.SetProjectionMatrix( Matrix4x4::Projection( 45.f, 1.f, 0.0001f, 100000.f ) );

	GRender.BeginLoadResources(ARRAYSIZE(textures));

	for ( UINT meshID = 0; meshID < ARRAYSIZE( meshes ); ++meshID )
	{
		SGeometryData geometryData;
		GGeometryLoader.LoadMesh( geometryData, meshes[ meshID ] );
		GGeometryInfo[ meshID ].m_geometryID = GRender.LoadResource( geometryData );
		GGeometryInfo[ meshID ].m_indicesNum = UINT( geometryData.m_indices.size() );
	}

	for (unsigned int texutreID = 0; texutreID < ARRAYSIZE(textures); ++texutreID)
	{
		STexture texture;
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		CheckResult( DirectX::LoadFromWICFile( textures[ texutreID ], DirectX::WIC_FLAGS_NONE, &texMeta, image ) );

		texture.m_data = image.GetPixels();
		texture.m_width = texMeta.width;
		texture.m_height = texMeta.height;
		texture.m_format = texMeta.format;
		texture.m_mipLevels = texMeta.mipLevels;

		ASSERT( texture.m_format != DXGI_FORMAT_UNKNOWN );
		GRender.LoadResource(texture);
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

	MSG msg = { 0 };
	bool run = true;
	while (run)
	{
		unsigned int const gameObjectsNum = GGameObjects.size();
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

		float const axis0[] = { 0.f, -1.f, 0.f };
		Quaternion const q0 = Quaternion::FromAngleAxis( GTimer.GetSeconds( GTimer.TimeFromStart() ) * 30.f * MathConsts::DegToRad, axis0 );
		float const axis1[] = { -1.f, 0.f, 0.f };
		Quaternion const q1 = Quaternion::FromAngleAxis( 90.f * MathConsts::DegToRad, axis1 );
		float const axis2[] = { 0.f, -1.f, 0.f };
		Quaternion const q2 = Quaternion::FromAngleAxis( 15.f * MathConsts::DegToRad, axis2 );
		testObject->SetRotation( q0 * q1 * q2 );

		testLightR->SetPosition( Vec3( -2.5f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) ), -2.f + 2.5f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ), 10.f ) );
		testLightG->SetPosition( Vec3( -2.5f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 2.f * MathConsts::PI / 3.f ), -2.f + 2.5f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 2.f * MathConsts::PI / 3.f ), 10.f ) );
		testLightB->SetPosition( Vec3( -2.5f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 4.f * MathConsts::PI / 3.f ), -2.f + 2.5f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 4.f * MathConsts::PI / 3.f ), 10.f ) );

		testLightR->SetFade( sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ) * ( -0.5f ) - 0.5f );
		testLightG->SetFade( sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ) * ( -0.5f ) - 0.5f );
		testLightB->SetFade( sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ) * ( -0.5f ) - 0.5f );

		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			GGameObjects[gameObjectID]->Update();
		}

		timeToRender -= GTimer.Delta();
		if (timeToRender < 0.f)
		{
			GRender.PrepareView();
			for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
			{
				GGameObjects[gameObjectID]->FillRenderData();
			}
			DrawDebugInfo();
			GRender.DrawFrame();

			for (unsigned int layerID = 0; layerID < RL_MAX; ++layerID)
			{
				GViewObject.m_renderData[layerID].clear();
				GViewObject.m_lightData.clear();
			}
			timeToRender = 1.f / 60.f;
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
	GGeometryLoader.Release();
	return 0;
}