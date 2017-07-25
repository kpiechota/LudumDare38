#include "render.h"
#include "lightObject.h"

CLightObject::CLightObject( Byte const type )
	: m_color( 1.f, 1.f, 1.f )
	, m_radius( 1.f )
	, m_fade( -1.f )
{
	switch ( type )
	{
		case LT_POINT:
			m_lighShader = Byte(LF_POINT);
			break;
	}
}

void CLightObject::FillRenderData() const
{
	SLightData lightData;
	lightData.m_lightShader = m_lighShader;

	CConstBufferCtx const cbCtx = GRender.GetLightConstBufferCtx( lightData.m_cbOffset, m_lighShader );
	SView const& mainCamera = GRender.GetView();

	Matrix4x4 tViewToWorld = mainCamera.m_viewToWorld;
	tViewToWorld.Transpose();
	Vec4 const perspectiveValues(1.f / mainCamera.m_viewToScreen.m_a00, 1.f / mainCamera.m_viewToScreen.m_a11, mainCamera.m_viewToScreen.m_a32, -mainCamera.m_viewToScreen.m_a22 );
	Vec2 const attenuation( 1.f / m_radius, m_fade );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &tViewToWorld ),			sizeof( tViewToWorld ),			EShaderParameters::ViewToWorld );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &perspectiveValues ),	sizeof( perspectiveValues ),	EShaderParameters::PerspectiveValues );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &m_position ),			sizeof( m_position ),			EShaderParameters::LightPos );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &m_color ),				sizeof( m_color ),				EShaderParameters::LightColor );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &attenuation ),			sizeof( attenuation ),			EShaderParameters::Attenuation );

	GViewObject.m_lightData.push_back( lightData );
}