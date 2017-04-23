#include "staticObject.h"
#include "timer.h"

extern CTimer GTimer;

CStaticObject::CStaticObject(SRenderObject const& renderObject, float const lifeTime, Byte const collisionMask, Byte const layer)
	: m_renderObject(renderObject)
	, m_lifeTime( lifeTime )
	, m_layer( layer )
{
	m_collisionMask = collisionMask;
	m_allowDestroy = 0.f < m_lifeTime;
}

void CStaticObject::FillRenderData() const
{
	GRenderObjects[m_layer].push_back(m_renderObject);
}

void CStaticObject::Update()
{
	m_lifeTime -= GTimer.GameDelta();
}

Vec2 CStaticObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CStaticObject::GetSize() const
{
	return m_renderObject.m_size;
}
