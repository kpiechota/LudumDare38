#pragma once

struct SComponentLight
{
	Vec3 m_color;
	float m_radius;
	float m_fade;

	Byte m_lighShader;
};

POD_TYPE( SComponentLight )

class CComponentLightManager : public TComponentContainer< SComponentLight, EComponentType::CT_Light>
{
private:
	struct SRenderComponents
	{
		UINT32 m_transformID;
		UINT32 m_lightID;
	};
	TArray< SRenderComponents > m_renderComponents;

public:
	void RegisterRenderComponents( UINT32 const transformID, UINT32 const lightID )
	{
		m_renderComponents.Add( { transformID, lightID } );
	}

	void FillRenderData() const;
};
