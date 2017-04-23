#pragma once

enum EShaderType
{
	ST_OBJECT_DRAW,
	ST_OBJECT_DRAW_BLEND,

	ST_MAX
};

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
	T_HEALTH_EFFECT,
	T_TURRET_ICON,
	T_HEALTH_ICON,
	T_VEINS,
	T_INIT_SCREEN,
	T_DEATH_SCREEN,
	T_DEAD_ENEMY,
	T_GROUND,
	T_BAKED,
};

enum ERenderLayer
{
	RL_BACKGROUND_STATIC,
	RL_FOREGROUND0,
	RL_FOREGROUND1,
	RL_OVERLAY0,
	RL_OVERLAY1,
	RL_OVERLAY2,

	RL_MAX
};

enum ESoundEffectType
{
	SET_SHOOT0,
	SET_SHOOT1,
	SET_SHOOT2,
	SET_HEAL,
	SET_EXPLOSION,
	SET_BUILD,

	SET_MAX
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
	Vec2 m_uvTile;
	Vec2 m_uvOffset;

	Byte m_texutreID;
	Byte m_shaderID;

	SRenderObject()
		: m_colorScale( 1.f, 1.f, 1.f, 1.f )
		, m_positionWS( 0.f, 0.f )
		, m_rotation( 1.f, 0.f )
		, m_size(1.f, 1.f)
		, m_offset(0.f, 0.f)
		, m_uvTile(1.f, 1.f)
		, m_uvOffset(0.f, 0.f)
		, m_texutreID( 0 )
		, m_shaderID( 0 )
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
	virtual void TakeDamage(Vec2 const rotation, float const damage) = 0;
};

extern unsigned int GGameObjectArray;
extern std::vector< CGameObject* > GGameObjects[2];
extern std::vector< CGameObject* > GGameObjectsToSpawn;
extern std::vector< SRenderObject > GRenderObjects[RL_MAX];
extern std::vector< SRenderObject > GBakeObjects;
