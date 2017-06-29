#include "render.h"
#include "healthObject.h"
#include "healthEffectObject.h"
#include "staticObject.h"
#include "timer.h"
#include "soundEngine.h"

extern CTimer GTimer;
extern Matrix3x3 GScreenMatrix;

void CHealthObject::DrawHealthBar() const
{
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = 4;
	renderData.m_textureID = T_BLANK;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position + Vec2(0.f, 12.f + 3.f), Vec2(1.f, 0.f), Vec2(12.f, 2.f)));
	constBuffer->m_colorScale.Set( 0.f, 0.f, 0.f, 1.f );
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );
	GRenderObjects[RL_OVERLAY0].push_back(renderData);

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position + Vec2(-12.f, 12.f + 3.f), Vec2(1.f, 0.f), Vec2(12.f * (m_health / m_maxHealth), 2.f)));
	constBuffer->m_colorScale.Set( 0.f, 1.f, 0.f, 1.f );
	constBuffer->m_offset.Set( 1.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );
	GRenderObjects[RL_OVERLAY0].push_back(renderData);
}

CHealthObject::CHealthObject()
	: m_healSpeed(2.f)
	, m_lastHeal(m_healSpeed)
	, m_healRadius2(100.f * 100.f)
	, m_maxHealth(2.f)
	, m_health(m_maxHealth)
	, m_veinsSpawnTime( 0.2f )
	, m_lastVeinsSpawnTime(0.f)
	, m_hitTime(.1f)
	, m_lastHitTime(0.f)
	, m_shaderID( 0 )
	, m_textureID( 0 )
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);

	m_scale = 12.f;
	m_textureID = T_HEALTH;
}

void CHealthObject::Start()
{
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = 4;
	renderData.m_textureID = T_GROUND;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, Vec2::GetRandomOnCircle(), m_scale * 1.75f));
	constBuffer->m_colorScale.Set( 1.f, 1.f, 1.f, 1.f );
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );

	GBakeObjects.push_back(renderData);
}

void CHealthObject::Update()
{
	m_lastHeal -= GTimer.GameDelta();
	m_lastVeinsSpawnTime -= GTimer.GameDelta();
	m_lastHitTime -= GTimer.GameDelta();

	if (m_lastHeal < 0.f)
	{
		CHealthEffectObject* healthEffect = new CHealthEffectObject(.5f);
		healthEffect->SetPosition( m_position );
		healthEffect->SetScale( 100.f );
		healthEffect->SetTextureID( T_HEALTH_EFFECT );
		healthEffect->SetShaderID( ST_OBJECT_DRAW_BLEND );

		GGameObjectsToSpawn.push_back(healthEffect);

		GSoundEngine.Play2DSound(GSounds[SET_HEAL]);
		m_lastHeal = m_healSpeed;
		unsigned int const gameObjectsNum = GGameObjects.size();
		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			CGameObject* const pGameObject = GGameObjects[gameObjectID];

			if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY_BULLET) && !pGameObject->NeedDelete() && (pGameObject->GetPosition() - m_position).Magnitude2() < m_healRadius2 )
			{
				pGameObject->TakeDamage(m_rotation,-0.5f);
			}
		}
	}

	if (m_lastVeinsSpawnTime < 0.f)
	{
		m_lastVeinsSpawnTime = m_veinsSpawnTime;

		Vec2 const veinOffset = Vec2::GetRandomOnCircle();
		CStaticObject* pVein = new CStaticObject(0.3f, 0, RL_FOREGROUND1);
		pVein->SetColor( Vec4( 0.f, 1.f, 0.f, 1.f ) );
		pVein->SetPosition( m_position + veinOffset * 2.f * m_scale.x );
		pVein->SetRotation( Vec2( veinOffset.y, -veinOffset.x ) );
		pVein->SetScale( 12.f );
		pVein->SetUvTile( Vec2( ( rand() % 2 ) ? 1.f : -1.f, 1.f ) );
		pVein->SetTextureID( T_VEINS );
		pVein->SetShaderID( ST_OBJECT_DRAW_BLEND );

		GGameObjectsToSpawn.push_back(pVein);
	}
}

void CHealthObject::FillRenderData() const
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
	if (0.f < m_lastHitTime)
	{
		constBuffer->m_colorScale.Set(1.f, 0.f, 0.f, 1.f);
	}

	GRenderObjects[RL_FOREGROUND0].push_back(renderData);

	DrawHealthBar();
}

bool CHealthObject::NeedDelete() const
{
	return m_health <= 0.f;
}

void CHealthObject::TakeDamage(Vec2 const rotation, float const damage)
{
	m_health = max( 0.f, min(m_maxHealth, m_health - damage) );
	if (0.f < damage && m_lastHitTime < -m_hitTime)
	{
		m_lastHitTime = m_hitTime;
	}
	if (m_health <= 0.f)
	{
		GSoundEngine.Play2DSound(GSounds[SET_EXPLOSION]);
	}
}

void CHealthObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CHealthObject::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
}

void CHealthObject::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CHealthObject::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CHealthObject::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CHealthObject::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}
