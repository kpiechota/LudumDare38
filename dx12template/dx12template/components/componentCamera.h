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
	UINT m_graphicsCAmeraID;

public:
	void SetMainCamera( UINT const cameraID )
	{
		m_graphicsCAmeraID = cameraID;
	}

	Vec3 GetMainCameraPosition() const;
	Vec3 GetMainCameraForward() const;
	void MainCameraTick();
	void PrepareView();
	void SetMainProjection( Matrix4x4 const& projection );
};