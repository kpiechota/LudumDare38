#include "healthObject.h"
#include "timer.h"

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
	, m_healSpeed(1.f)
	, m_lastHeal(m_healSpeed)
	, m_healRadius2(100.f * 100.f)
	, m_maxHealth(25.f)
	, m_health(15.f)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);

	m_renderObject.m_size = 12.f;
	m_renderObject.m_texutreID = T_HEALTH;
}

void CHealthObject::Update()
{
	m_lastHeal -= GTimer.GameDelta();
	if (m_lastHeal < 0.f)
	{
		m_lastHeal = m_healSpeed;
		std::vector< CGameObject* > const& currentGameObjectArray = GGameObjects[GGameObjectArray];
		unsigned int const gameObjectsNum = currentGameObjectArray.size();
		for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
		{
			CGameObject* const pGameObject = currentGameObjectArray[gameObjectID];

			if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY_BULLET) && !pGameObject->NeedDelete() && (pGameObject->GetPosition() - m_renderObject.m_positionWS).Magnitude2() < m_healRadius2 )
			{
				pGameObject->TakeDamage(-5.f);
			}
		}
	}
}

void CHealthObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND].push_back(m_renderObject);

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
	return m_health < 0.f;
}

void CHealthObject::TakeDamage(float const damage)
{
	m_health = min(m_maxHealth, m_health - damage);
}
