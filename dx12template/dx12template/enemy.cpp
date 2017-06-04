#include "render.h"
#include "enemy.h"
#include "timer.h"
#include "soundEngine.h"
#include "bullet.h"
#include "enemySpawner.h"

extern CTimer GTimer;
extern CEnemySpawner GEnemySpawner;
extern Matrix3x3 GScreenMatrix;

void CEnemyObject::DrawHealthBar() const
{
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_textureID = T_BLANK;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position + Vec2(0.f, 12.f + 3.f), Vec2(1.f, 0.f), Vec2(12.f, 2.f)));
	constBuffer->m_colorScale.Set( 0.f, 0.f, 0.f, 1.f );
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );
	GRenderObjects[RL_OVERLAY0].push_back(renderData);

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position + Vec2(-12.f, 12.f + 3.f), Vec2(1.f, 0.f), Vec2(12.f * (m_health / m_maxHealth), 2.f)));
	constBuffer->m_colorScale.Set( 0.f, 1.f, 0.f, 1.f );
	constBuffer->m_offset.Set( 1.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );
	GRenderObjects[RL_OVERLAY0].push_back(renderData);
}

inline void CEnemyObject::CollisionTest()
{
	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY))
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_position;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_colliderSize.x + pGameObject->GetColliderSize().x;
			float const radius2 = radius * radius;

			if (magnitude2 < radius2 && 0.f < magnitude2)
			{
				float const magnitude = sqrt(magnitude2);
				float const invMagnitude = 1.f / magnitude;
				vectorTo *= invMagnitude;

				Vec2 const offset = vectorTo * (magnitude - radius);
				m_position += offset;
			}
		}
	}
}

inline Vec2 CEnemyObject::FindNearestObject() const
{
	Vec2 nearest;
	float nearestMagnitude2 = -1.f;

	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_ENEMY_BULLET) && !pGameObject->NeedDelete())
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_position;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;

			if (-1.f < nearestMagnitude2 )
			{
				if (magnitude2 < nearestMagnitude2)
				{
					nearestMagnitude2 = magnitude2;
					nearest = vectorTo;
				}
			}
			else
			{
				nearestMagnitude2 = magnitude2;
				nearest = vectorTo;
			}
		}
	}

	return nearest;
}

CEnemyObject::CEnemyObject()
	: m_speed(70.f)
	, m_shootSpeed(0.2f)
	, m_lastShoot(0.f)
	, m_shootRadius2( 150.f * 150.f)
	, m_maxHealth(1.f)
	, m_health(1.f)
	, m_shaderID( 0 )
	, m_textureID( T_ENEMY )
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_PLAYER_BULLET | CF_PLAYER);

	m_scale = 12.f;
	m_colliderSize = 12.f;
}

void CEnemyObject::Update()
{
	Vec2 const nearestPoint = FindNearestObject();
	m_rotation = nearestPoint.GetNormalized();

	m_lastShoot -= GTimer.GameDelta();
	if (nearestPoint.Magnitude2() < m_shootRadius2)
	{
		if (m_lastShoot < 0.f)
		{
			CBullet* bullet = new CBullet(0.2f, CF_ENEMY_BULLET);
			bullet->SetPosition( m_position + m_rotation * m_scale );
			bullet->SetRotation( m_rotation );
			bullet->SetScale( 2.f );
			bullet->SetColliderSize( 2.f );
			bullet->SetTextureID( T_BULLET1 );
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
			GSoundEngine.Play2DSound(GSounds[SET_SHOOT2]);
		}			
	}
	else
	{
		m_position += m_rotation * m_speed * GTimer.GameDelta();
	}

	CollisionTest();
}

void CEnemyObject::FillRenderData() const
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

	DrawHealthBar();
}

bool CEnemyObject::NeedDelete() const
{
	return m_health <= 0.f;
}

void CEnemyObject::TakeDamage(Vec2 const rotation, float const damage)
{
	m_health -= damage;

	if (m_health <= 0.f)
	{
		GEnemySpawner.EnemyKilled();

		CBObject* constBuffer;
		SRenderData renderData;
		renderData.m_textureID = T_DEAD_ENEMY;

		GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

		constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, Vec2(rotation.y, -rotation.x), Vec2(25.f, 50.f) ));
		constBuffer->m_colorScale.Set( 1.f, 1.f, 1.f, 1.f );
		constBuffer->m_offset.Set( 0.f, 1.f );
		constBuffer->m_uvOffset.Set( 0.f, 0.f );
		constBuffer->m_uvTile.Set( (rand() % 2) ? -1.f : 1.f, 1.f );

		GBakeObjects.push_back(renderData);
	}
}

void CEnemyObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CEnemyObject::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
}

void CEnemyObject::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CEnemyObject::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CEnemyObject::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CEnemyObject::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}
