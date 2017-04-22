#include "bullet.h"
#include "timer.h"
extern CTimer GTimer;

CBullet::CBullet(SRenderObject const& renderObject) : m_renderObject( renderObject )
	, m_lifeTime( 5.f )
	, m_speed( 800.f )
{
	m_collisionMask = 0;
}

void CBullet::Update()
{
	m_renderObject.m_positionWS += m_renderObject.m_rotation * m_speed * GTimer.GameDelta();
}

void CBullet::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND].push_back(m_renderObject);
}

Vec2 CBullet::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

float CBullet::GetSize() const
{
	return m_renderObject.m_size;
}

bool CBullet::NeedDelete() const
{
	return m_lifeTime < 0.f;
}
