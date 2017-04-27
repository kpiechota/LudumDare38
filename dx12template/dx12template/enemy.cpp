#include "enemy.h"
#include "timer.h"
#include "soundEngine.h"
#include "bullet.h"
#include "enemySpawner.h"

extern CTimer GTimer;
extern CEnemySpawner GEnemySpawner;

void CEnemyObject::DrawHealthBar() const
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

inline void CEnemyObject::CollisionTest()
{
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY))
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_renderObject.m_positionWS;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_renderObject.m_size.x + pGameObject->GetSize().x;
			float const radius2 = radius * radius;

			if (magnitude2 < radius2 && 0.f < magnitude2)
			{
				float const magnitude = sqrt(magnitude2);
				float const invMagnitude = 1.f / magnitude;
				vectorTo *= invMagnitude;

				Vec2 const offset = vectorTo * (magnitude - radius);
				m_renderObject.m_positionWS += offset;
			}
		}
	}
}

inline Vec2 CEnemyObject::FindNearestObject() const
{
	Vec2 nearest;
	float nearestMagnitude2 = -1.f;

	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY_BULLET) && !pGameObject->NeedDelete())
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_renderObject.m_positionWS;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;

			if (-1.f < nearestMagnitude2 )
			{
				if (magnitude2 < nearestMagnitude2)
				{
					nearestMagnitude2 = magnitude2;
					nearest = vectorTo;
				}
			}
			else
			{
				nearestMagnitude2 = magnitude2;
				nearest = vectorTo;
			}
		}
	}

	return nearest;
}

CEnemyObject::CEnemyObject()
	: m_speed(70.f)
	, m_shootSpeed(0.2f)
	, m_lastShoot(0.f)
	, m_shootRadius2( 150.f * 150.f)
	, m_maxHealth(1.f)
	, m_health(1.f)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_PLAYER_BULLET | CF_PLAYER);

	m_renderObject.m_size = 12.f;
	m_renderObject.m_texutreID = T_ENEMY;
}

void CEnemyObject::Update()
{
	Vec2 const nearestPoint = FindNearestObject();
	m_renderObject.m_rotation = nearestPoint.GetNormalized();

	m_lastShoot -= GTimer.GameDelta();
	if (nearestPoint.Magnitude2() < m_shootRadius2)
	{
		if (m_lastShoot < 0.f)
		{
			SRenderObject bulletObject;
			bulletObject.m_positionWS = m_renderObject.m_positionWS + m_renderObject.m_rotation * m_renderObject.m_size;
			bulletObject.m_rotation = m_renderObject.m_rotation;
			bulletObject.m_size = 4.f;
			bulletObject.m_texutreID = T_BULLET1;

			CBullet* bullet = new CBullet(bulletObject, 0.2f, CF_ENEMY_BULLET);
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
			GSoundEngine.Play2DSound(GSounds[SET_SHOOT2]);
		}			
	}
	else
	{
		m_renderObject.m_positionWS += m_renderObject.m_rotation * m_speed * GTimer.GameDelta();
	}

	CollisionTest();
}

void CEnemyObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);

	DrawHealthBar();
}

Vec2 CEnemyObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CEnemyObject::GetSize() const
{
	return m_renderObject.m_size;
}

bool CEnemyObject::NeedDelete() const
{
	return m_health <= 0.f;
}

void CEnemyObject::TakeDamage(Vec2 const rotation, float const damage)
{
	m_health -= damage;

	if (m_health <= 0.f)
	{
		GEnemySpawner.EnemyKilled();

		SRenderObject deadObject = m_renderObject;
		deadObject.m_texutreID = T_DEAD_ENEMY;
		deadObject.m_rotation.Set(rotation.y, -rotation.x);
		deadObject.m_offset.Set(0.f, 1.f);
		deadObject.m_size.Set(25.f, 50.f);
		deadObject.m_uvTile.x = (rand() % 2) ? -1.f : 1.f;

		GBakeObjects.push_back(deadObject);
	}
}
