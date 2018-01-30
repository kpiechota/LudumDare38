#include "../headers.h"
#include "../rendering/render.h"

void CComponentLightManager::FillRenderData() const
{
	SLightRenderData lightRenderData;

	Matrix4x4 const viewToWorld = GViewObject.m_camera.m_viewToWorld;
	Matrix4x4 const viewToScreen = GViewObject.m_camera.m_viewToScreen;

	UINT const renderComponentsNum = m_renderComponents.Size();
	GRender.LightRenderDataReserveNext( renderComponentsNum );

	Matrix4x4 tViewToWorld = viewToWorld;
	tViewToWorld.Transpose();
	Vec4 const perspectiveValues(1.f / viewToScreen.m_a00, 1.f / viewToScreen.m_a11, viewToScreen.m_a32, -viewToScreen.m_a22 );

	for ( UINT i = 0; i < renderComponentsNum; ++i )
	{
		SComponentTransform const transform = GComponentTransformManager.GetComponentNoCheck( m_renderComponents[ i ].m_transformID );
		SComponentLight const light = GetComponentNoCheck( m_renderComponents[ i ].m_lightID );

		lightRenderData.m_lightShader = light.m_lighShader;

		CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx( lightRenderData.m_cbOffset, light.m_lighShader );

		Vec2 const attenuation( 1.f / light.m_radius, light.m_fade );
		cbCtx.SetParam( &tViewToWorld,				3 * sizeof( Vec4 ),						EShaderParameters::ViewToWorld );
		cbCtx.SetParam( &perspectiveValues,			sizeof( perspectiveValues ),			EShaderParameters::PerspectiveValues );
		cbCtx.SetParam( &transform.m_position,		sizeof( transform.m_position ),			EShaderParameters::LightPos );
		cbCtx.SetParam( &light.m_color,				sizeof( light.m_color ),				EShaderParameters::Color );
		cbCtx.SetParam( &attenuation,				sizeof( attenuation ),					EShaderParameters::Attenuation );

		GRender.AddLightRenderData( lightRenderData );
	}
}