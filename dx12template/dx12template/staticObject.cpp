#include "staticObject.h"

CStaticObject::CStaticObject(SRenderObject const& renderObject)
	: m_renderObject(renderObject)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);
}

void CStaticObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);
}

Vec2 CStaticObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CStaticObject::GetSize() const
{
	return m_renderObject.m_size;
}
