#include "playerObject.h"
#include "timer.h"
#include "input.h"

extern CTimer GTimer;

inline void CPlayerObject::CollisionTest()
{
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject* pGameObject = GGameObjects[gameObjectID];

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

	Vec2 mouseWorldPos = Vec2(mouseScreenPos) - Vec2(GWidth >> 1, GHeight >> 1);
	mouseWorldPos.y = -mouseWorldPos.y;

	m_renderObject.m_rotation = mouseWorldPos - m_renderObject.m_positionWS;
	m_renderObject.m_rotation.Normalize();
}

Vec2 CPlayerObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

float CPlayerObject::GetSize() const
{
	return m_renderObject.m_size;
}
