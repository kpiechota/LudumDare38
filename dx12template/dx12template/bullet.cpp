#include "render.h"
#include "bullet.h"
#include "timer.h"
extern CTimer GTimer;
extern Matrix3x3 GScreenMatrix;

void CBullet::CollisionTest()
{
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject* pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(m_bulletType) && !pGameObject->NeedDelete())
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_position;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_colliderSize.x + pGameObject->GetColliderSize().x;
			float const radius2 = radius * radius;

			if (magnitude2 < radius2 && 0.f < magnitude2)
			{
				pGameObject->TakeDamage(m_rotation, m_damage);
				m_lifeTime = -1.f;
			}
		}
	}
}

CBullet::CBullet(float const damage, ECollisionFlag const bulletType)
	: m_lifeTime( 5.f )
	, m_speed( 800.f )
	, m_damage( damage )
	, m_bulletType( bulletType )
	, m_shaderID( 0 )
	, m_textureID( 0 )
{
	m_collisionMask = 0;
}

void CBullet::Update()
{
	m_position += m_rotation * m_speed * GTimer.GameDelta();
	m_lifeTime -= GTimer.GameDelta();

	CollisionTest();
}

void CBullet::FillRenderData() const
{
	ASSERT( m_shaderID < EShaderType::ST_MAX );
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_shaderID = m_shaderID;
	renderData.m_textureID = m_textureID;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, m_rotation, m_scale));
	m_material.FillConstBuffer( constBuffer );

	GRenderObjects[RL_FOREGROUND0].push_back(renderData);
}

bool CBullet::NeedDelete() const
{
	return m_lifeTime < 0.f;
}

void CBullet::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CBullet::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
}

void CBullet::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CBullet::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CBullet::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CBullet::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}