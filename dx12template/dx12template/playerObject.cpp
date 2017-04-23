#include "playerObject.h"
#include "timer.h"
#include "input.h"
#include "bullet.h"
#include "turretObject.h"
#include "healthObject.h"
#include "staticObject.h"

extern CTimer GTimer;

inline void CPlayerObject::CollisionTest()
{
	float const magnitudeFromCenter2 = m_renderObject.m_positionWS.x * m_renderObject.m_positionWS.x + m_renderObject.m_positionWS.y * m_renderObject.m_positionWS.y;
	float const centerRadius = GIslandSize - m_renderObject.m_size.x;
	float const centerRadius2 = centerRadius * centerRadius;

	if (centerRadius2 < magnitudeFromCenter2)
	{
		float const magnitudeFromCenter = sqrt(magnitudeFromCenter2);
		float const invMagnitude = 1.f / magnitudeFromCenter2;
		Vec2 vectorTo = m_renderObject.m_positionWS * invMagnitude;
		Vec2 const offset = vectorTo * (centerRadius2 - magnitudeFromCenter2);
		m_renderObject.m_positionWS += offset;
	}

	std::vector< CGameObject* > const& currentGameObjectArray = GGameObjects[GGameObjectArray];
	unsigned int const gameObjectsNum = currentGameObjectArray.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const * const pGameObject = currentGameObjectArray[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_PLAYER))
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_renderObject.m_positionWS;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_renderObject.m_size.x + pGameObject->GetSize().x;
			float const radius2 = radius * radius;

			if (magnitude2 < radius2 && 0.f < magnitude2 )
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

void CPlayerObject::DrawHealthBar() const
{
	SRenderObject healthBarBackground;
	healthBarBackground.m_colorScale.Set(0.f, 0.f, 0.f, 1.f);
	healthBarBackground.m_positionWS = m_renderObject.m_positionWS + Vec2(0.f, 12.f + 3.f);
	healthBarBackground.m_size.Set(12.f, 2.f);
	healthBarBackground.m_texutreID = T_BLANK;

	SRenderObject healthBar;
	healthBar.m_colorScale.Set(0.f, 1.f, 0.f, 1.f);
	healthBar.m_positionWS = m_renderObject.m_positionWS + Vec2(-12.f, 12.f + 3.f);
	healthBar.m_size.Set(12.f * (m_health/m_maxHealth), 2.f);
	healthBar.m_offset.Set(1.f, 0.f);
	healthBar.m_texutreID = T_BLANK;

	GRenderObjects[RL_OVERLAY0].push_back(healthBarBackground);
	GRenderObjects[RL_OVERLAY0].push_back(healthBar);

	healthBarBackground.m_positionWS = m_renderObject.m_positionWS + Vec2(0.f, 12.f + 3.f + 6.f);

	healthBar.m_colorScale.Set(0.2f, .8f, 1.f, 1.f);
	healthBar.m_positionWS = m_renderObject.m_positionWS + Vec2(-12.f, 12.f + 3.f + 6.f);
	healthBar.m_size.Set(12.f * m_energyValue, 2.f);

	GRenderObjects[RL_OVERLAY0].push_back(healthBarBackground);
	GRenderObjects[RL_OVERLAY0].push_back(healthBar);
}

CPlayerObject::CPlayerObject()
	: m_speed( 100.f )
	, m_shootSpeed( 0.1f )
	, m_lastShoot( 0.f )
	, m_energyValue( 1.f )
	, m_maxHealth(10.f)
	, m_health(10.f)
	, m_initScreen( nullptr )
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET);

	m_renderObject.m_positionWS.Set(0.f, 16.f + 12.f);
	m_renderObject.m_size = 12.f;
	m_renderObject.m_texutreID = T_PLAYER;

	float const iconY = -0.5f * ((float)GHeight) + 35.f;

	m_renderObjectTurret.m_positionWS.Set(-35.f, iconY);
	m_renderObjectTurret.m_size = 32.f;
	m_renderObjectTurret.m_texutreID = T_TURRET_ICON;

	m_renderObjectHealth.m_positionWS.Set(35.f, iconY);
	m_renderObjectHealth.m_size = 32.f;
	m_renderObjectHealth.m_texutreID = T_HEALTH_ICON;

	SRenderObject initScreenObject;
	initScreenObject.m_shaderID = ST_OBJECT_DRAW_BLEND;
	initScreenObject.m_texutreID = T_INIT_SCREEN;
	initScreenObject.m_size = 400.f;
	
	m_initScreen = new CStaticObject(initScreenObject, -1.f, 0, RL_OVERLAY2);
	GGameObjectsToSpawn.push_back(m_initScreen);

	GTimer.SetGameScale(0.f);
}

void CPlayerObject::AddEnergy()
{
	m_energyValue = min(1.f, m_energyValue + 0.1f);
}

void CPlayerObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);

	DrawHealthBar();

	float const iconColor = (m_energyValue < 1.f) ? 0.5f : 1.f;
	GRenderObjects[RL_OVERLAY1].push_back(m_renderObjectTurret);
	GRenderObjects[RL_OVERLAY1].back().m_colorScale.Set(iconColor, iconColor, iconColor, 1.f);
	GRenderObjects[RL_OVERLAY1].push_back(m_renderObjectHealth);
	GRenderObjects[RL_OVERLAY1].back().m_colorScale.Set(iconColor, iconColor, iconColor, 1.f);
}

void CPlayerObject::Update()
{
	if ( !m_initScreen && 0.f < m_health )
	{
		Vec2 moveDir;

		if (GInputManager.IsKeyDown('W'))
		{
			moveDir.y += 1.f;
		}

		if (GInputManager.IsKeyDown('S'))
		{
			moveDir.y -= 1.f;
		}

		if (GInputManager.IsKeyDown('A'))
		{
			moveDir.x -= 1.f;
		}

		if (GInputManager.IsKeyDown('D'))
		{
			moveDir.x += 1.f;
		}

		moveDir.Normalize();
		moveDir *= m_speed * GTimer.GameDelta();

		m_renderObject.m_positionWS += moveDir;
		CollisionTest();

		Vec2i mouseScreenPos;
		GInputManager.GetMousePosition(mouseScreenPos);

		Vec2 mouseWorldPos = Vec2(mouseScreenPos) - Vec2((float)(GWidth) * 0.5f, (float)(GHeight) * 0.5f);
		mouseWorldPos.y = -mouseWorldPos.y;

		m_renderObject.m_rotation = mouseWorldPos - m_renderObject.m_positionWS;
		m_renderObject.m_rotation.Normalize();

		m_lastShoot -= GTimer.GameDelta();
		if (m_lastShoot < 0.f && GInputManager.IsKeyDown(K_LEFTM))
		{
			SRenderObject bulletObject;
			bulletObject.m_positionWS = m_renderObject.m_positionWS + m_renderObject.m_rotation * m_renderObject.m_size;
			bulletObject.m_rotation = m_renderObject.m_rotation;
			bulletObject.m_size = 2.f;
			bulletObject.m_texutreID = T_BULLET0;

			CBullet* bullet = new CBullet(bulletObject, 0.3f, CF_PLAYER_BULLET);
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
		}

		if (1.f == m_energyValue)
		{
			if (GInputManager.IsKeyDown('Q'))
			{
				m_energyValue = 0.f;

				SRenderObject turretObject;
				turretObject.m_positionWS = m_renderObject.m_positionWS + m_renderObject.m_rotation * m_renderObject.m_size;
				turretObject.m_rotation = m_renderObject.m_rotation;
				turretObject.m_size = 12.f;
				turretObject.m_texutreID = T_TURRET;

				CTurretObject* turret = new CTurretObject(turretObject);
				GGameObjectsToSpawn.push_back(turret);
			}
			else if (GInputManager.IsKeyDown('E'))
			{
				m_energyValue = 0.f;

				SRenderObject healthObject;
				healthObject.m_positionWS = m_renderObject.m_positionWS + m_renderObject.m_rotation * m_renderObject.m_size;
				healthObject.m_size = 12.f;
				healthObject.m_texutreID = T_HEALTH;

				CHealthObject* health = new CHealthObject(healthObject);
				GGameObjectsToSpawn.push_back(health);
			}
		}
	}
	else if (m_initScreen && GInputManager.IsKeyDown(' '))
	{
		m_initScreen->ForceDelete();
		m_initScreen = nullptr;
		GTimer.SetGameScale(1.f);
	}
}

Vec2 CPlayerObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CPlayerObject::GetSize() const
{
	return m_renderObject.m_size;
}

void CPlayerObject::TakeDamage(float const damage)
{
	m_health = max( 0.f, min(m_maxHealth, m_health - damage) );
}
