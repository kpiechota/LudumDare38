#include "../headers.h"
#include "../render.h"

void CComponentLightManager::FillRenderData() const
{
	SLightData lightData;

	UINT const renderComponentsNum = m_renderComponents.Size();
	GViewObject.m_lightData.Reserve( GViewObject.m_lightData.Size() + renderComponentsNum );

	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentTransform const transform = GComponentTransformManager.GetComponent( m_renderComponents[ i ].m_transformID );
		SComponentLight const light = GetComponent( m_renderComponents[ i ].m_lightID );

		lightData.m_lightShader = light.m_lighShader;

		CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx( lightData.m_cbOffset, light.m_lighShader );
		SView const& mainCamera = GRender.GetView();

		Matrix4x4 tViewToWorld = mainCamera.m_viewToWorld;
		tViewToWorld.Transpose();
		Vec4 const perspectiveValues(1.f / mainCamera.m_viewToScreen.m_a00, 1.f / mainCamera.m_viewToScreen.m_a11, mainCamera.m_viewToScreen.m_a32, -mainCamera.m_viewToScreen.m_a22 );
		Vec2 const attenuation( 1.f / light.m_radius, light.m_fade );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &tViewToWorld ),				sizeof( tViewToWorld ),					EShaderParameters::ViewToWorld );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &perspectiveValues ),		sizeof( perspectiveValues ),			EShaderParameters::PerspectiveValues );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &transform.m_position ),		sizeof( transform.m_position ),			EShaderParameters::LightPos );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &light.m_color ),			sizeof( light.m_color ),				EShaderParameters::LightColor );
		cbCtx.SetParam( reinterpret_cast<Byte const*>( &attenuation ),				sizeof( attenuation ),					EShaderParameters::Attenuation );

		GViewObject.m_lightData.Add( lightData );
	}
}