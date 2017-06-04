#include "render.h"
#include "healthEffectObject.h"
#include "timer.h"
extern CTimer GTimer;
extern Matrix3x3 GScreenMatrix;

CHealthEffectObject::CHealthEffectObject( float const lifeTime)
	: m_maxLifeTime( lifeTime )
	, m_lifeTime( lifeTime )
	, m_shaderID( 0 )
	, m_textureID( 0 )
{
	m_collisionMask = 0;
}

void CHealthEffectObject::Start()
{
	m_maxSize = m_scale.x;
	m_scale = 0.f;
}

void CHealthEffectObject::Update()
{
	m_lifeTime -= GTimer.GameDelta();
	float const lifeFactor = (m_lifeTime / m_maxLifeTime);
	m_scale = m_maxSize * (1.f - lifeFactor);
	m_material.m_color.w = lifeFactor;
}

void CHealthEffectObject::FillRenderData() const
{
	ASSERT( m_shaderID < EShaderType::ST_MAX );
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_shaderID = m_shaderID;
	renderData.m_textureID = m_textureID;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, m_rotation, m_scale));
	m_material.FillConstBuffer( constBuffer );

	GRenderObjects[RL_FOREGROUND1].push_back(renderData);
}

void CHealthEffectObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CHealthEffectObject::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
}

void CHealthEffectObject::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CHealthEffectObject::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CHealthEffectObject::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CHealthEffectObject::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}

