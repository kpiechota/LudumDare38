#include "render.h"
#include "staticObject.h"
#include "timer.h"

extern CTimer GTimer;
extern Matrix3x3 GScreenMatrix;

CStaticObject::CStaticObject( float const lifeTime, Byte const collisionMask, Byte const layer)
	: m_lifeTime( lifeTime )
	, m_layer( layer )
	, m_shaderID( 0 )
	, m_textureID( 0 )
{
	m_collisionMask = collisionMask;
	m_allowDestroy = 0.f < m_lifeTime;
}

void CStaticObject::FillRenderData() const
{
	ASSERT( m_shaderID < EShaderType::ST_MAX );
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = 4;
	renderData.m_shaderID = m_shaderID;
	renderData.m_textureID = m_textureID;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, m_rotation, m_scale));
	m_material.FillConstBuffer( constBuffer );

	GRenderObjects[m_layer].push_back(renderData);
}

void CStaticObject::Update()
{
	m_lifeTime -= GTimer.GameDelta();
}

void CStaticObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CStaticObject::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
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
