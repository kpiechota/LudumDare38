#include "turretObject.h"
#include "timer.h"
#include "bullet.h"

extern CTimer GTimer;

Vec2 CTurretObject::FindNearestObject() const
{
	Vec2 nearest;
	float nearestMagnitude2 = -1.f;

	std::vector< CGameObject* > const& currentGameObjectArray = GGameObjects[GGameObjectArray];
	unsigned int const gameObjectsNum = currentGameObjectArray.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = currentGameObjectArray[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_PLAYER_BULLET) && !pGameObject->NeedDelete())
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_renderObject.m_positionWS;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;

			if (-1.f < nearestMagnitude2)
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

CTurretObject::CTurretObject(SRenderObject const& renderObject)
	: m_renderObject(renderObject)
	, m_shootSpeed(0.3f)
	, m_lastShoot(0.f)
	, m_shootRadius2(150.f * 150.f)
	, m_health(30.f)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);

	m_renderObject.m_size = 12.f;
	m_renderObject.m_texutreID = T_TURRET;
}

void CTurretObject::Update()
{
	Vec2 const nearestPoint = FindNearestObject();

	m_lastShoot -= GTimer.GameDelta();
	float const nearestMagnitude2 = nearestPoint.Magnitude2();
	if (0.f < nearestMagnitude2 && nearestMagnitude2 < m_shootRadius2)
	{
		m_renderObject.m_rotation = nearestPoint.GetNormalized();
		if (m_lastShoot < 0.f)
		{
			SRenderObject bulletObject;
			bulletObject.m_positionWS = m_renderObject.m_positionWS + m_renderObject.m_rotation * m_renderObject.m_size;
			bulletObject.m_rotation = m_renderObject.m_rotation;
			bulletObject.m_size = 2.f;
			bulletObject.m_texutreID = T_BULLET0;

			CBullet* bullet = new CBullet(bulletObject, 1.f, CF_PLAYER_BULLET);
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
		}
	}
}

void CTurretObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND].push_back(m_renderObject);
}

Vec2 CTurretObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

float CTurretObject::GetSize() const
{
	return m_renderObject.m_size;
}

bool CTurretObject::NeedDelete() const
{
	return m_health < 0.f;
}

void CTurretObject::TakeDamage(float const damage)
{
	m_health -= damage;
}
