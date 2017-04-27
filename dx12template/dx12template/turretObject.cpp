#include "turretObject.h"
#include "timer.h"
#include "soundEngine.h"
#include "bullet.h"

extern CTimer GTimer;

void CTurretObject::DrawHealthBar() const
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

Vec2 CTurretObject::FindNearestObject() const
{
	Vec2 nearest;
	float nearestMagnitude2 = -1.f;

	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = GGameObjects[gameObjectID];

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
	, m_maxHealth(1.f)
	, m_health(m_maxHealth)
	, m_collisionSize(12.f)
	, m_hitTime(.1f)
	, m_lastHitTime(0.f)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);

	m_renderObject.m_size = 20.f;
	m_renderObject.m_texutreID = T_TURRET;

	SRenderObject bakedGround;
	bakedGround.m_positionWS = m_renderObject.m_positionWS;
	bakedGround.m_size = m_renderObject.m_size * 1.75f;
	bakedGround.m_texutreID = T_GROUND;
	bakedGround.m_rotation = Vec2::GetRandomOnCircle();

	GBakeObjects.push_back(bakedGround);
}

void CTurretObject::Update()
{
	Vec2 const nearestPoint = FindNearestObject();

	m_lastShoot -= GTimer.GameDelta();
	m_lastHitTime -= GTimer.GameDelta();
	float const nearestMagnitude2 = nearestPoint.Magnitude2();
	if (0.f < nearestMagnitude2 && nearestMagnitude2 < m_shootRadius2)
	{
		m_renderObject.m_rotation = nearestPoint.GetNormalized();
		if (m_lastShoot < 0.f)
		{
			SRenderObject bulletObject;
			bulletObject.m_positionWS = m_renderObject.m_positionWS + m_renderObject.m_rotation * m_renderObject.m_size;
			bulletObject.m_rotation = m_renderObject.m_rotation;
			bulletObject.m_size = 4.f;
			bulletObject.m_texutreID = T_BULLET0;

			CBullet* bullet = new CBullet(bulletObject, .5f, CF_PLAYER_BULLET);
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
			GSoundEngine.Play2DSound(GSounds[SET_SHOOT1]);
		}
	}
}

void CTurretObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);
	if (0.f < m_lastHitTime)
	{
		GRenderObjects[RL_FOREGROUND0].back().m_colorScale.Set(1.f, 0.f, 0.f, 1.f);
	}

	DrawHealthBar();
}

Vec2 CTurretObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CTurretObject::GetSize() const
{
	return Vec2(m_collisionSize);
}

bool CTurretObject::NeedDelete() const
{
	return m_health <= 0.f;
}

void CTurretObject::TakeDamage(Vec2 const rotation, float const damage)
{
	m_health = max(0.f, min(m_maxHealth, m_health - damage));
	if (0.f < damage && m_lastHitTime < -m_hitTime)
	{
		m_lastHitTime = m_hitTime;
	}
	if (m_health <= 0.f)
	{
		GSoundEngine.Play2DSound(GSounds[SET_EXPLOSION]);
	}
}
