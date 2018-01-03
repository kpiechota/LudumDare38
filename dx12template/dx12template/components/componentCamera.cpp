#include "../headers.h"
#include "../input.h"
#include "../timer.h"

void CComponentCameraManager::MainCameraTick()
{
	static Vec2i const midPos(GWidth / 2, GHeight / 2);
	SComponentCamera& mainCamera = m_components[ m_graphicsCAmeraID ];

	if ( GInputManager.IsKeyDown( EKeys::K_RIGHT_MOUSE_BUTTON ) )
	{
		if ( GInputManager.KeyDownLastFrame( EKeys::K_RIGHT_MOUSE_BUTTON ) )
		{
			GInputManager.SetMousePosition( midPos );
		}

		Vec2i mouseDelta;
		GInputManager.GetMousePosition( mouseDelta );
		mouseDelta -= midPos;

		float const factor = GTimer.Delta() * 45.f * 2.f * MathConsts::DegToRad;
		mainCamera.m_rotation = Quaternion::FromAngleAxis( factor * ( float )( mouseDelta.x ), Vec3::UP.data ) * mainCamera.m_rotation;
		mainCamera.m_rotation = mainCamera.m_rotation * Quaternion::FromAngleAxis(factor * ( float )( mouseDelta.y ), Vec3::RIGHT.data );

		GInputManager.SetMousePosition( midPos );
	}

	Vec3 dir(0.f, 0.f, 0.f);

	if (GInputManager.IsKeyDown('W'))
	{
		dir += mainCamera.m_rotation * Vec3::FORWARD;
	}
	if (GInputManager.IsKeyDown('S'))
	{
		dir += mainCamera.m_rotation * Vec3::BACKWARD;
	}

	if (GInputManager.IsKeyDown('A'))
	{
		dir += mainCamera.m_rotation * Vec3::LEFT;
	}
	if (GInputManager.IsKeyDown('D'))
	{
		dir += mainCamera.m_rotation * Vec3::RIGHT;
	}

	if ( 0.f < dir.GetMagnitudeSq())
	{
		dir.Normalize();
		float const speed = GInputManager.IsKeyDown(K_SPACE) ? 100.f : 2.f;
		mainCamera.m_position += dir * GTimer.Delta() * speed;
	}
}

void CComponentCameraManager::PrepareView()
{
	SComponentCamera& mainCamera = m_components[ m_graphicsCAmeraID ];
	SCameraMatrices& cameraMatrices = GViewObject.m_camera;

	cameraMatrices.m_viewToScreen = mainCamera.m_projectionMatrix;
	cameraMatrices.m_viewToScreen.Inverse( cameraMatrices.m_screenToView );
	
	cameraMatrices.m_viewToWorld = Matrix4x4::GetTranslateRotationSize( mainCamera.m_position, mainCamera.m_rotation, Vec3::ONE );
	cameraMatrices.m_viewToWorld.Inverse( cameraMatrices.m_worldToView );

	cameraMatrices.m_screenToWorld = Math::Mul( cameraMatrices.m_screenToView, cameraMatrices.m_viewToWorld );
	cameraMatrices.m_worldToScreen = Math::Mul( cameraMatrices.m_worldToView, cameraMatrices.m_viewToScreen );
}

void CComponentCameraManager::SetMainProjection( Matrix4x4 const& projection )
{
	SComponentCamera& mainCamera = m_components[ m_graphicsCAmeraID ];
	mainCamera.m_projectionMatrix = projection;
}

Vec3 CComponentCameraManager::GetMainCameraPosition() const
{
	SComponentCamera const& mainCamera = m_components[ m_graphicsCAmeraID ];
	return mainCamera.m_position;
}

Vec3 CComponentCameraManager::GetMainCameraForward() const
{
	SComponentCamera const& mainCamera = m_components[ m_graphicsCAmeraID ];
	return mainCamera.m_rotation * Vec3::FORWARD;
}