#pragma once

enum EShaderType
{
	ST_OBJECT_DRAW,
	ST_SDF_DRAW,

	ST_MAX
};

enum ELightType 
{
	LT_POINT,

	LT_MAX
};

enum EGeometry
{
	G_SPACESHIP,

	G_MAX
};

enum ETextures
{
	T_SDF_FONT_512,
	T_SPACESHIP,
	T_SPACESHIP_N,
	T_SPACESHIP_E,
	T_SPACESHIP_S,

	T_MAX
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

struct SGeometryInfo
{
	UINT m_indicesNum;
	Byte m_geometryID;
};

class CGameObject
{
protected:
	Quaternion m_rotation;
	Vec3 m_position;
	Vec3 m_scale;

public:
	CGameObject()
	{
		m_position.Set( 0.f, 0.f, 0.f );
		m_rotation.Set( 0.f, 0.f, 0.f, 1.f );
		m_scale.Set( 1.f, 1.f, 1.f );
	}

	virtual void SetPosition( Vec3 const position ) { m_position = position; }
	virtual void SetRotation( Quaternion const rotation ) { m_rotation = rotation; }
	virtual void SetScale( Vec3 const scale ) { m_scale = scale; }
	virtual void SetScale( float const scale ) { m_scale = scale; }

	virtual void Start(){}
	virtual void Update() = 0;
	virtual void FillRenderData() const = 0;
	virtual Vec3 GetPosition() const { return m_position; }
	virtual Vec3 GetScale() const { return m_scale; }
	virtual bool NeedDelete() const = 0;
};

extern std::vector< CGameObject* > GGameObjects;
extern std::vector< CGameObject* > GGameObjectsToSpawn;
extern SViewObject GViewObject;
extern SGeometryInfo GGeometryInfo[ G_MAX ];
