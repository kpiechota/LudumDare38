#pragma once

struct SGameObject
{
	Vec2 m_positionWS;
	Vec2 m_rotation;
	Vec2 m_size;

	SGameObject()
		: m_positionWS( 0.f, 0.f )
		, m_rotation( 1.f, 0.f )
		, m_size( 1.f, 1.f )
	{}
};
