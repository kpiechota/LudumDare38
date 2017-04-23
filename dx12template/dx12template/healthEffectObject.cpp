#include "healthEffectObject.h"
#include "timer.h"
extern CTimer GTimer;

CHealthEffectObject::CHealthEffectObject(SRenderObject const& renderObject, float const lifeTime)
	: m_renderObject( renderObject )
	, m_maxLifeTime( lifeTime )
	, m_lifeTime( lifeTime )
{
	m_collisionMask = 0;
	m_maxSize = m_renderObject.m_size.x;
	m_renderObject.m_size = 0.f;
}

void CHealthEffectObject::Update()
{
	m_lifeTime -= GTimer.GameDelta();
	float const lifeFactor = (m_lifeTime / m_maxLifeTime);
	m_renderObject.m_size = m_maxSize * (1.f - lifeFactor);
	m_renderObject.m_colorScale.w = lifeFactor;
}

void CHealthEffectObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND1].push_back(m_renderObject);
}

Vec2 CHealthEffectObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CHealthEffectObject::GetSize() const
{
	return m_renderObject.m_size;
}
