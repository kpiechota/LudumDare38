#pragma once

struct SComponentCamera
{
	Matrix4x4 m_projectionMatrix;
	Quaternion m_rotation;
	Vec3 m_position;
};
POD_TYPE( SComponentCamera )

class CComponentCameraManager : public TComponentContainer< SComponentCamera, EComponentType::CT_Camera>
{
private:
	UINT m_mainCameraID;

public:
	void SetMainCamera( UINT const cameraID )
	{
		m_mainCameraID = cameraID;
	}

	void MainCameraTick();
	void PrepareView();
	void SetMainProjection( Matrix4x4 const& projection );
};