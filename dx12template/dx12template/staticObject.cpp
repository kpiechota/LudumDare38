#include "render.h"
#include "staticObject.h"
#include "timer.h"

extern CTimer GTimer;

CStaticObject::CStaticObject( Byte const collisionMask, Byte const layer)
	: m_layer( layer )
	, m_shaderID( 0 )
{
	memset( m_textureID, 0, sizeof( m_textureID ) );
	m_collisionMask = collisionMask;
}

void CStaticObject::FillRenderData() const
{
	ASSERT( m_shaderID < EShaderType::ST_MAX );
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = GGeometryInfo[ m_geometryInfoID ].m_indicesNum;
	renderData.m_geometryID = GGeometryInfo[ m_geometryInfoID ].m_geometryID;
	renderData.m_shaderID = m_shaderID;

	for ( UINT texture = 0; texture < ARRAYSIZE( m_textureID ); ++texture )
	{
		renderData.m_textureID[ texture ] = m_textureID[ texture ];
	}

	renderData.m_topology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToWorld = Matrix4x4::GetTranslateRotationSize(m_position, m_rotation, m_scale);
	constBuffer->m_objectToScreen = Mul(constBuffer->m_objectToWorld, GRender.GetView().m_worldToScreen );
	constBuffer->m_objectToWorld.Transpose();
	constBuffer->m_objectToScreen.Transpose();
	m_material.FillConstBuffer( constBuffer );

	GRenderObjects[m_layer].push_back(renderData);
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

void CStaticObject::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CStaticObject::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CStaticObject::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CStaticObject::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}
