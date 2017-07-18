#pragma once

enum EShaderType
{
	ST_OBJECT_DRAW,
	ST_SDF_DRAW,

	ST_MAX
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

	T_MAX
};

enum ERenderLayer
{
	RL_OPAQUE,
	RL_OVERLAY,

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

struct SGeometryInfo
{
	UINT m_indicesNum;
	Byte m_geometryID;
};

struct SRenderData
{
	enum
	{
		MAX_TEXTURES_NUM = 2
	};

	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;

	UINT m_verticesStart;
	UINT m_indicesStart;
	UINT m_dataNum;

	D3D_PRIMITIVE_TOPOLOGY m_topology;

	Byte m_geometryID;

	Byte m_textureID[ MAX_TEXTURES_NUM ];
	Byte m_shaderID;

	SRenderData()
		: m_cbOffset( 0 )
		, m_verticesStart( 0 )
		, m_indicesStart( 0 )
		, m_dataNum( 0 )
		, m_topology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP )
		, m_geometryID( UINT8_MAX )
		, m_shaderID( 0 )
	{
		memset( m_textureID, UINT8_MAX, sizeof( m_textureID ) );
	}
};

class CGameObject
{
protected:
	Quaternion m_rotation;
	Vec3 m_position;
	Vec3 m_scale;
	Vec2 m_colliderSize;

	Byte m_collisionMask;

public:
	CGameObject()
	{
		m_position.Set( 0.f, 0.f, 0.f );
		m_rotation.Set( 0.f, 0.f, 0.f, 1.f );
		m_scale.Set( 1.f, 1.f, 1.f );
		m_colliderSize.Set( 1.f, 1.f );
	}

	bool CollideWith( ECollisionFlag const collisionType ) const { return m_collisionMask & collisionType; }

	virtual void SetPosition( Vec3 const position ) { m_position = position; }
	virtual void SetRotation( Quaternion const rotation ) { m_rotation = rotation; }
	virtual void SetScale( Vec3 const scale ) { m_scale = scale; }
	virtual void SetScale( float const scale ) { m_scale = scale; }

	virtual void SetColliderSize( Vec2 const size ) { m_colliderSize = size; }

	virtual void Start(){}
	virtual void Update() = 0;
	virtual void FillRenderData() const = 0;
	virtual Vec3 GetPosition() const { return m_position; }
	virtual Vec3 GetScale() const { return m_scale; }
	virtual Vec2 GetColliderSize() const { return m_colliderSize; }
	virtual bool NeedDelete() const = 0;
	virtual void TakeDamage(Vec2 const rotation, float const damage) = 0;
};

extern std::vector< CGameObject* > GGameObjects;
extern std::vector< CGameObject* > GGameObjectsToSpawn;
extern std::vector< SRenderData > GRenderObjects[RL_MAX];
extern SGeometryInfo GGeometryInfo[ G_MAX ];
