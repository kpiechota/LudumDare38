#include "enemy.h"
#include "timer.h"
#include "bullet.h"
extern CTimer GTimer;

inline void CEnemyObject::CollisionTest()
{
	std::vector< CGameObject* > const& currentGameObjectArray = GGameObjects[GGameObjectArray];
	unsigned int const gameObjectsNum = currentGameObjectArray.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = currentGameObjectArray[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY))
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_renderObject.m_positionWS;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_renderObject.m_size + pGameObject->GetSize();
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

inline Vec2 CEnemyObject::FindNearestObject()
{
	Vec2 nearest;
	float nearestMagnitude2 = -1.f;

	std::vector< CGameObject* > const& currentGameObjectArray = GGameObjects[GGameObjectArray];
	unsigned int const gameObjectsNum = currentGameObjectArray.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = currentGameObjectArray[gameObjectID];

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
	, m_shootRadius2( 100.f * 100.f)
	, m_health( 1.f )
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
			bulletObject.m_size = 2.f;
			bulletObject.m_texutreID = T_BULLET1;

			CBullet* bullet = new CBullet(bulletObject, 0.2f, CF_ENEMY_BULLET);
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
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
	GRenderObjects[RL_FOREGROUND].push_back(m_renderObject);
}

Vec2 CEnemyObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

float CEnemyObject::GetSize() const
{
	return m_renderObject.m_size;
}

bool CEnemyObject::NeedDelete() const
{
	return m_health < 0.f;
}

void CEnemyObject::TakeDamage(float const damage)
{
	m_health -= damage;
}
