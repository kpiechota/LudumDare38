#include "render.h"
#include "staticObject.h"

CStaticObject::CStaticObject( Byte const collisionMask, Byte const layer)
	: m_layer( layer )
	, m_shaderID( 0 )
{
	memset( m_textureID, 0, sizeof( m_textureID ) );
}

void CStaticObject::FillRenderData() const
{
	ASSERT( m_shaderID < EShaderType::ST_MAX );
	SRenderData renderData;
	renderData.m_dataNum = GGeometryInfo[ m_geometryInfoID ].m_indicesNum;
	renderData.m_geometryID = GGeometryInfo[ m_geometryInfoID ].m_geometryID;
	renderData.m_shaderID = m_shaderID;

	for ( UINT texture = 0; texture < ARRAYSIZE( m_textureID ); ++texture )
	{
		renderData.m_textureID[ texture ] = m_textureID[ texture ];
	}

	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Matrix4x4 tObjectToWorld = Matrix4x4::GetTranslateRotationSize(m_position, m_rotation, m_scale);
	Matrix4x4 tObjectToScreen = Mul(tObjectToWorld, GRender.GetView().m_worldToScreen );
	tObjectToWorld.Transpose();
	tObjectToScreen.Transpose();

	CConstBufferCtx const cbCtx = GRender.GetConstBufferCtx( renderData.m_cbOffset, m_shaderID );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &tObjectToWorld ), sizeof( tObjectToWorld ), EShaderParameters::ObjectToWorld );
	cbCtx.SetParam( reinterpret_cast<Byte const*>( &tObjectToScreen ), sizeof( tObjectToScreen ), EShaderParameters::ObjectToScreen );

	GViewObject.m_renderData[m_layer].push_back(renderData);
}

void CStaticObject::Update()
{
}

void CStaticObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CStaticObject::SetTextureID( UINT const texture, Byte const textureID )
{
	m_textureID[ texture ] = textureID;
}

void CStaticObject::SetGeomtryInfoID( Byte const geometryInfoID )
{
	m_geometryInfoID = geometryInfoID;
}