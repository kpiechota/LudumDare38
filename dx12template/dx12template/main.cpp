#include "headers.h"
#include "rendering/render.h"
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
SGeometryInfo GGeometryInfo[ G_MAX ];
CStaticSound GSounds[SET_MAX];

int GWidth = 800;
int GHeight = 800;

SComponentHandle testObjectTransformHandle;

SComponentHandle testLightRTransformHandle;
SComponentHandle testLightGTransformHandle;
SComponentHandle testLightBTransformHandle;
SComponentHandle testLightRHandle;
SComponentHandle testLightGHandle;
SComponentHandle testLightBHandle;

void InitGame()
{
	for (UINT layerID = 0; layerID < RL_MAX; ++layerID)
	{
		GViewObject.m_renderData[layerID].Clear();
		GViewObject.m_lightData.Clear();
	}

	GEntityManager.Clear();

	UINT testEntityID = GEntityManager.CreateEntity();
	CEntity* testEntity = GEntityManager.GetEntity( testEntityID );
	SComponentHandle testCameraHandle = testEntity->AddComponentCamera();
	SComponentCamera* testCamera = &GComponentCameraManager.GetComponent( testCameraHandle );
	GComponentCameraManager.SetMainCamera( testCameraHandle.m_index );
	GComponentCameraManager.SetMainProjection( Matrix4x4::Projection( 40.f, 1.f, 0.0001f, 100000.f ) );
	testCamera->m_position = Vec3::ZERO;
	testCamera->m_rotation = Quaternion::IDENTITY;

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	testLightRTransformHandle = testEntity->AddComponentTransform();
	testLightRHandle = testEntity->AddComponentLight();

	SComponentTransform* testObjectTransform = &GComponentTransformManager.GetComponent( testLightRTransformHandle );
	SComponentLight* testLight = &GComponentLightManager.GetComponent( testLightRHandle );

	testObjectTransform->m_position.Set( 0.f, 0.f, 0.f );
	testLight->m_radius = 3.f;
	testLight->m_color.Set( 2.f, 0.f, 0.f );
	testLight->m_lighShader = LF_POINT;
	testLight->m_fade = -1.f;

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	testLightGTransformHandle = testEntity->AddComponentTransform();
	testLightGHandle = testEntity->AddComponentLight();

	testObjectTransform = &GComponentTransformManager.GetComponent( testLightGTransformHandle );
	testLight = &GComponentLightManager.GetComponent( testLightGHandle );

	testObjectTransform->m_position.Set( 0.f, 0.f, 0.f );
	testLight->m_radius = 3.f;
	testLight->m_color.Set( 0.f, 2.f, 0.f );
	testLight->m_lighShader = LF_POINT;
	testLight->m_fade = -1.f;

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	testLightBTransformHandle = testEntity->AddComponentTransform();
	testLightBHandle = testEntity->AddComponentLight();

	testObjectTransform = &GComponentTransformManager.GetComponent( testLightBTransformHandle );
	testLight = &GComponentLightManager.GetComponent( testLightBHandle );

	testObjectTransform->m_position.Set( 0.f, 0.f, 0.f );
	testLight->m_radius = 3.f;
	testLight->m_color.Set( 0.f, 0.f, 2.f );
	testLight->m_lighShader = LF_POINT;
	testLight->m_fade = -1.f;

	Quaternion const q1 = Quaternion::FromAngleAxis( 90.f * MathConsts::DegToRad, Vec3::LEFT.data );
	Quaternion const q2 = Quaternion::FromAngleAxis( 15.f * MathConsts::DegToRad, Vec3::DOWN.data );

	testEntityID = GEntityManager.CreateEntity();
	testEntity = GEntityManager.GetEntity( testEntityID );
	testObjectTransformHandle = testEntity->AddComponentTransform();
	SComponentHandle testObjectStaticMeshHandle = testEntity->AddComponentStaticMesh();

	testObjectTransform = &GComponentTransformManager.GetComponent( testObjectTransformHandle );
	SComponentStaticMesh& testObjectStaticMesh = GComponentStaticMeshManager.GetComponent( testObjectStaticMeshHandle );

	testObjectTransform->m_position.Set( 0.f, -2.f, 10.f );
	testObjectTransform->m_scale.Set( .25f, .25f, .25f );
	testObjectTransform->m_rotation = q1 * q2;

	testObjectStaticMesh.m_geometryInfoID = G_SPACESHIP;
	testObjectStaticMesh.m_layer = RL_OPAQUE;
	testObjectStaticMesh.m_shaderID = 0;
	testObjectStaticMesh.m_textureID[ 0 ] = T_SPACESHIP;
	testObjectStaticMesh.m_textureID[ 1 ] = T_SPACESHIP_N;
	testObjectStaticMesh.m_textureID[ 2 ] = T_SPACESHIP_E;
	testObjectStaticMesh.m_textureID[ 3 ] = T_SPACESHIP_S;

	GComponentStaticMeshManager.RegisterRenderComponents( testObjectTransformHandle.m_index, testObjectStaticMeshHandle.m_index );
	GComponentLightManager.RegisterRenderComponents( testLightRTransformHandle.m_index, testLightRHandle.m_index );
	GComponentLightManager.RegisterRenderComponents( testLightGTransformHandle.m_index, testLightGHandle.m_index );
	GComponentLightManager.RegisterRenderComponents( testLightBTransformHandle.m_index, testLightBHandle.m_index );
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
		L"../content/rainDrop.png",
		L"../content/snow.png",
	};
	CT_ASSERT( ARRAYSIZE( textures ) == T_MAX );

	GRender.Init();
	GRender.SetDirectLightColor( Vec3( 1.f, 1.f, 1.f ) );
	GRender.SetDirectLightDir( Vec3( 0.f, 1.f, 1.f ).GetNormalized() );
	GRender.SetAmbientLightColor( Vec3( 0.1f, 0.1f, 0.1f ) );

	GRender.BeginLoadResources(ARRAYSIZE(textures));

	for ( UINT meshID = 0; meshID < ARRAYSIZE( meshes ); ++meshID )
	{
		SGeometryData geometryData;
		GGeometryLoader.LoadMesh( geometryData, meshes[ meshID ] );
		GGeometryInfo[ meshID ].m_geometryID = GRender.LoadResource( geometryData );
		GGeometryInfo[ meshID ].m_indicesNum = geometryData.m_indices.Size();
	}

	for (unsigned int texutreID = 0; texutreID < ARRAYSIZE(textures); ++texutreID)
	{
		STexture texture;
		DirectX::TexMetadata texMeta;
		DirectX::ScratchImage image;
		CheckResult( DirectX::LoadFromWICFile( textures[ texutreID ], DirectX::WIC_FLAGS_NONE, &texMeta, image ) );

		texture.m_data = image.GetPixels();
		texture.m_width = UINT(texMeta.width);
		texture.m_height = UINT(texMeta.height);
		texture.m_format = texMeta.format;
		texture.m_mipLevels = Byte(texMeta.mipLevels);

		ASSERT( texture.m_format != DXGI_FORMAT_UNKNOWN );
		GRender.LoadResource(texture);
	}

	GRender.EndLoadResources();

	GInputManager.SetHWND(hwnd);

	GInputManager.Init();
	GInputManager.AddObserver(&GSystemInput);

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

		Quaternion const q0 = Quaternion::FromAngleAxis( GTimer.GetSeconds( GTimer.TimeFromStart() ) * 30.f * MathConsts::DegToRad, Vec3::DOWN.data );
		Quaternion const q1 = Quaternion::FromAngleAxis( 90.f * MathConsts::DegToRad, Vec3::LEFT.data );
		Quaternion const q2 = Quaternion::FromAngleAxis( 15.f * MathConsts::DegToRad, Vec3::DOWN.data );

		SComponentTransform& testObjectTransform = GComponentTransformManager.GetComponent( testObjectTransformHandle );
		//testObjectTransform.m_rotation = q0 * q1 * q2;

		SComponentTransform* testLightTransform = &GComponentTransformManager.GetComponent( testLightRTransformHandle );
		SComponentLight* testLight = &GComponentLightManager.GetComponent( testLightRHandle );
		testLightTransform->m_position.Set( -2.5f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) ), -2.f + 2.5f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ), 10.f );
		testLight->m_fade = sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ) * ( -0.5f ) - 0.5f;

		testLightTransform = &GComponentTransformManager.GetComponent( testLightGTransformHandle );
		testLight = &GComponentLightManager.GetComponent( testLightGHandle );
		testLightTransform->m_position.Set( -2.5f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 2.f * MathConsts::PI / 3.f ), -2.f + 2.5f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 2.f * MathConsts::PI / 3.f ), 10.f );
		testLight->m_fade = sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ) * ( -0.5f ) - 0.5f;

		testLightTransform = &GComponentTransformManager.GetComponent( testLightBTransformHandle );
		testLight = &GComponentLightManager.GetComponent( testLightBHandle );
		testLightTransform->m_position.Set( -2.5f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 4.f * MathConsts::PI / 3.f ), -2.f + 2.5f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) + 4.f * MathConsts::PI / 3.f ), 10.f );
		testLight->m_fade = sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ) * ( -0.5f ) - 0.5f;

		GComponentCameraManager.MainCameraTick();
		GInputManager.Tick();

		GRender.PreDrawFrame();

		DrawDebugInfo();

		GRender.DrawFrame();

		for (UINT layerID = 0; layerID < RL_MAX; ++layerID)
		{
			GViewObject.m_renderData[layerID].Clear();
			GViewObject.m_lightData.Clear();
		}

		GEntityManager.Tick();
	}

	GRender.Release();
	GSoundEngine.Release();
	GGeometryLoader.Release();
	return 0;
}