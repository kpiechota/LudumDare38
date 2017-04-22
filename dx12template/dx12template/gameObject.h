#pragma once

enum ETextures
{
	T_PLAYER,
	T_BACKGROUND,
	T_ISLAND,
	T_GENERATOR,
};

enum ERenderLayer
{
	RL_BACKGROUND_STATIC,
	RL_FOREGROUND,
	RL_OVERLAY,

	RL_MAX
};

enum ECollisionFlag
{
	CF_PLAYER			= 1 << 0,
	CF_PLAYER_BULLET	= 1 << 1,
	CF_ENEMY			= 1 << 2,
	CF_ENEMY_BULLET		= 1 << 3,
};

struct SRenderObject
{
	Vec2 m_positionWS;
	Vec2 m_rotation;
	float m_size;

	Byte m_texutreID;

	SRenderObject()
		: m_positionWS( 0.f, 0.f )
		, m_rotation( 1.f, 0.f )
		, m_size( 1.f )
		, m_texutreID( 0 )
	{}
};

class CGameObject
{
protected:
	Byte m_collisionMask;

public:

	bool CollideWith( ECollisionFlag const collisionType ) const { return m_collisionMask & collisionType; }

	virtual void Update() = 0;
	virtual void FillRenderData() const = 0;
	virtual Vec2 GetPosition() const = 0;
	virtual float GetSize() const = 0;
};

extern std::vector< CGameObject* > GGameObjects;
extern std::vector< SRenderObject > GRenderObjects[RL_MAX];
