#include "../headers.h"
#include "../rendering/render.h"

void CComponentLightManager::FillRenderData() const
{
	SLightRenderData lightRenderData;

	UINT const renderComponentsNum = m_renderComponents.Size();
	GRender.LightRenderDataReserveNext( renderComponentsNum );

	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentTransform const transform = GComponentTransformManager.GetComponentNoCheck( m_renderComponents[ i ].m_transformID );
		SComponentLight const light = GetComponentNoCheck( m_renderComponents[ i ].m_lightID );

		lightRenderData.m_lightShader = light.m_lighShader;

		CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx( lightRenderData.m_cbOffset, light.m_lighShader );

		Vec2 const attenuation( 1.f / light.m_radius, light.m_fade );
		cbCtx.SetParam( &transform.m_position,		sizeof( transform.m_position ),			EShaderParameters::LightPos );
		cbCtx.SetParam( &light.m_color,				sizeof( light.m_color ),				EShaderParameters::Color );
		cbCtx.SetParam( &attenuation,				sizeof( attenuation ),					EShaderParameters::Attenuation );

		GRender.AddLightRenderData( lightRenderData );
	}
}