#include "healthObject.h"
#include "healthEffectObject.h"
#include "staticObject.h"
#include "timer.h"
#include "soundEngine.h"

extern CTimer GTimer;

void CHealthObject::DrawHealthBar() const
{
	SRenderObject healthBarBackground;
	healthBarBackground.m_colorScale.Set(0.f, 0.f, 0.f, 1.f);
	healthBarBackground.m_positionWS = m_renderObject.m_positionWS + Vec2(0.f, 12.f + 3.f);
	healthBarBackground.m_size.Set(12.f, 2.f);
	healthBarBackground.m_texutreID = T_BLANK;

	SRenderObject healthBar;
	healthBar.m_colorScale.Set(0.f, 1.f, 0.f, 1.f);
	healthBar.m_positionWS = m_renderObject.m_positionWS + Vec2(-12.f, 12.f + 3.f);
	healthBar.m_size.Set(12.f * (m_health / m_maxHealth), 2.f);
	healthBar.m_offset.Set(1.f, 0.f);
	healthBar.m_texutreID = T_BLANK;

	GRenderObjects[RL_OVERLAY0].push_back(healthBarBackground);
	GRenderObjects[RL_OVERLAY0].push_back(healthBar);
}

CHealthObject::CHealthObject(SRenderObject const& renderObject)
	: m_renderObject(renderObject)
	, m_healSpeed(2.f)
	, m_lastHeal(m_healSpeed)
	, m_healRadius2(100.f * 100.f)
	, m_maxHealth(2.f)
	, m_health(m_maxHealth)
	, m_veinsSpawnTime( 0.2f )
	, m_lastVeinsSpawnTime(0.f)
	, m_hitTime(.1f)
	, m_lastHitTime(0.f)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);

	m_renderObject.m_size = 12.f;
	m_renderObject.m_texutreID = T_HEALTH;

	m_healthEffectObject.m_positionWS = m_renderObject.m_positionWS;
	m_healthEffectObject.m_size = 100.f;
	m_healthEffectObject.m_texutreID = T_HEALTH_EFFECT;
	m_healthEffectObject.m_shaderID = ST_OBJECT_DRAW_BLEND;

	SRenderObject bakedGround;
	bakedGround.m_positionWS = m_renderObject.m_positionWS;
	bakedGround.m_size = m_renderObject.m_size * 1.75f;
	bakedGround.m_texutreID = T_GROUND;
	bakedGround.m_rotation = Vec2::GetRandomOnCircle();

	GBakeObjects.push_back(bakedGround);
}

void CHealthObject::Update()
{
	m_lastHeal -= GTimer.GameDelta();
	m_lastVeinsSpawnTime -= GTimer.GameDelta();
	m_lastHitTime -= GTimer.GameDelta();

	if (m_lastHeal < 0.f)
	{
		CHealthEffectObject* healthEffect = new CHealthEffectObject(m_healthEffectObject, .5f);
		GGameObjectsToSpawn.push_back(healthEffect);

		GSoundEngine.Play2DSound(GSounds[SET_HEAL]);
		m_lastHeal = m_healSpeed;
		std::vector< CGameObject* > const& currentGameObjectArray = GGameObjects[GGameObjectArray];
		unsigned int const gameObjectsNum = currentGameObjectArray.size();
		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			CGameObject* const pGameObject = currentGameObjectArray[gameObjectID];

			if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY_BULLET) && !pGameObject->NeedDelete() && (pGameObject->GetPosition() - m_renderObject.m_positionWS).Magnitude2() < m_healRadius2 )
			{
				pGameObject->TakeDamage(m_renderObject.m_rotation,-0.5f);
			}
		}
	}

	if (m_lastVeinsSpawnTime < 0.f)
	{
		m_lastVeinsSpawnTime = m_veinsSpawnTime;

		Vec2 veinOffset = Vec2::GetRandomOnCircle();

		SRenderObject veinObject;
		veinObject.m_positionWS = m_renderObject.m_positionWS + veinOffset * 2.f * m_renderObject.m_size.x;

		veinObject.m_colorScale.Set(0.f, 1.f, 0.f, 1.f);
		veinObject.m_rotation.Set(veinOffset.y, -veinOffset.x);
		veinObject.m_size = 12.f;
		veinObject.m_uvTile.x = (rand() % 2) ? 1.f : -1.f;
		veinObject.m_texutreID = T_VEINS;
		veinObject.m_shaderID = ST_OBJECT_DRAW_BLEND;

		CStaticObject* pVein = new CStaticObject(veinObject, 0.3f, 0, RL_FOREGROUND1);
		GGameObjectsToSpawn.push_back(pVein);
	}
}

void CHealthObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);
	if (0.f < m_lastHitTime)
	{
		GRenderObjects[RL_FOREGROUND0].back().m_colorScale.Set(1.f, 0.f, 0.f, 1.f);
	}

	DrawHealthBar();
}

Vec2 CHealthObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CHealthObject::GetSize() const
{
	return m_renderObject.m_size;
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
