#include "bullet.h"
#include "timer.h"
extern CTimer GTimer;

void CBullet::CollisionTest()
{
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject* pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(m_bulletType) && !pGameObject->NeedDelete())
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_renderObject.m_positionWS;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_renderObject.m_size.x + pGameObject->GetSize().x;
			float const radius2 = radius * radius;

			if (magnitude2 < radius2 && 0.f < magnitude2)
			{
				pGameObject->TakeDamage(m_renderObject.m_rotation, m_damage);
				m_lifeTime = -1.f;
			}
		}
	}
}

CBullet::CBullet(SRenderObject const& renderObject, float const damage, ECollisionFlag const bulletType)
	: m_renderObject( renderObject )
	, m_lifeTime( 5.f )
	, m_speed( 800.f )
	, m_damage( damage )
	, m_bulletType( bulletType )
{
	m_collisionMask = 0;
}

void CBullet::Update()
{
	m_renderObject.m_positionWS += m_renderObject.m_rotation * m_speed * GTimer.GameDelta();
	m_lifeTime -= GTimer.GameDelta();

	CollisionTest();
}

void CBullet::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);
}

Vec2 CBullet::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CBullet::GetSize() const
{
	return m_renderObject.m_size;
}

bool CBullet::NeedDelete() const
{
	return m_lifeTime < 0.f;
}