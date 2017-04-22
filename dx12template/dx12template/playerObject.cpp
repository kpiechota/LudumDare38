#include "playerObject.h"
#include "timer.h"
#include "input.h"
#include "bullet.h"
extern CTimer GTimer;

inline void CPlayerObject::CollisionTest()
{
	float const magnitudeFromCenter2 = m_renderObject.m_positionWS.x * m_renderObject.m_positionWS.x + m_renderObject.m_positionWS.y * m_renderObject.m_positionWS.y;
	float const centerRadius = 350.f - m_renderObject.m_size;
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
			float const radius = m_renderObject.m_size + pGameObject->GetSize();
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

CPlayerObject::CPlayerObject()
	: m_speed( 100.f )
	, m_shootSpeed( 0.1f )
	, m_lastShoot( 0.f )
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET);

	m_renderObject.m_positionWS.Set(0.f, 16.f + 12.f);
	m_renderObject.m_size = 12.f;
	m_renderObject.m_texutreID = T_PLAYER;
}

void CPlayerObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND].push_back(m_renderObject);
}

void CPlayerObject::Update()
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
}

Vec2 CPlayerObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

float CPlayerObject::GetSize() const
{
	return m_renderObject.m_size;
}

void CPlayerObject::TakeDamage(float const damage)
{
}
