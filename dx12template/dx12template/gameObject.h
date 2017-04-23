#pragma once

enum ETextures
{
	T_BLANK,
	T_PLAYER,
	T_BACKGROUND,
	T_ISLAND,
	T_GENERATOR,
	T_BULLET0,
	T_BULLET1,
	T_ENEMY,
	T_TURRET,
	T_HEALTH,
};

enum ERenderLayer
{
	RL_BACKGROUND_STATIC,
	RL_FOREGROUND,
	RL_OVERLAY0,
	RL_OVERLAY1,

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
	Vec4 m_colorScale;
	Vec2 m_positionWS;
	Vec2 m_rotation;
	Vec2 m_size;
	Vec2 m_offset;

	Byte m_texutreID;

	SRenderObject()
		: m_colorScale( 1.f, 1.f, 1.f, 1.f )
		, m_positionWS( 0.f, 0.f )
		, m_rotation( 1.f, 0.f )
		, m_size(1.f, 1.f)
		, m_offset(0.f, 0.f)
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
	virtual Vec2 GetSize() const = 0;
	virtual bool NeedDelete() const = 0;
	virtual void TakeDamage(float const damage) = 0;
};

extern unsigned int GGameObjectArray;
extern std::vector< CGameObject* > GGameObjects[2];
extern std::vector< CGameObject* > GGameObjectsToSpawn;
extern std::vector< SRenderObject > GRenderObjects[RL_MAX];
