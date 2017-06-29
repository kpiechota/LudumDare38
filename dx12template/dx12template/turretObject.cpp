#include "render.h"
#include "turretObject.h"
#include "timer.h"
#include "soundEngine.h"
#include "bullet.h"

extern CTimer GTimer;
extern Matrix3x3 GScreenMatrix;

void CTurretObject::DrawHealthBar() const
{
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = 4;
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

Vec2 CTurretObject::FindNearestObject() const
{
	Vec2 nearest;
	float nearestMagnitude2 = -1.f;

	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const* const pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_PLAYER_BULLET) && !pGameObject->NeedDelete())
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_position;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;

			if (-1.f < nearestMagnitude2)
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

CTurretObject::CTurretObject()
	: m_shootSpeed(0.3f)
	, m_lastShoot(0.f)
	, m_shootRadius2(150.f * 150.f)
	, m_maxHealth(1.f)
	, m_health(m_maxHealth)
	, m_collisionSize(12.f)
	, m_hitTime(.1f)
	, m_lastHitTime(0.f)
	, m_shaderID( 0 )
	, m_textureID( T_TURRET )
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);

	m_scale = 20.f;
}

void CTurretObject::Start()
{
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = 4;
	renderData.m_textureID = T_GROUND;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, Vec2::GetRandomOnCircle(), m_scale * 1.75f));
	constBuffer->m_colorScale.Set( 1.f, 1.f, 1.f, 1.f );
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );

	GBakeObjects.push_back(renderData);
}

void CTurretObject::Update()
{
	Vec2 const nearestPoint = FindNearestObject();

	m_lastShoot -= GTimer.GameDelta();
	m_lastHitTime -= GTimer.GameDelta();
	float const nearestMagnitude2 = nearestPoint.Magnitude2();
	if (0.f < nearestMagnitude2 && nearestMagnitude2 < m_shootRadius2)
	{
		m_rotation = nearestPoint.GetNormalized();
		if (m_lastShoot < 0.f)
		{
			CBullet* bullet = new CBullet(.5f, CF_PLAYER_BULLET);
			bullet->SetPosition( m_position + m_rotation * m_scale );
			bullet->SetRotation( m_rotation );
			bullet->SetScale( 4.f );
			bullet->SetColliderSize( 4.f );
			bullet->SetTextureID( T_BULLET0 );

			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;
			GSoundEngine.Play2DSound(GSounds[SET_SHOOT1]);
		}
	}
}

void CTurretObject::FillRenderData() const
{
	ASSERT( m_shaderID < EShaderType::ST_MAX );
	CBObject* constBuffer;
	SRenderData renderData;
	renderData.m_dataNum = 4;
	renderData.m_shaderID = m_shaderID;
	renderData.m_textureID = m_textureID;

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );

	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position, m_rotation, m_scale));
	m_material.FillConstBuffer( constBuffer );

	if (0.f < m_lastHitTime)
	{
		constBuffer->m_colorScale.Set(1.f, 0.f, 0.f, 1.f);
	}

	GRenderObjects[RL_FOREGROUND0].push_back(renderData);

	DrawHealthBar();
}

bool CTurretObject::NeedDelete() const
{
	return m_health <= 0.f;
}

void CTurretObject::TakeDamage(Vec2 const rotation, float const damage)
{
	m_health = max(0.f, min(m_maxHealth, m_health - damage));
	if (0.f < damage && m_lastHitTime < -m_hitTime)
	{
		m_lastHitTime = m_hitTime;
	}
	if (m_health <= 0.f)
	{
		GSoundEngine.Play2DSound(GSounds[SET_EXPLOSION]);
	}
}

void CTurretObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CTurretObject::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
}

void CTurretObject::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CTurretObject::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CTurretObject::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CTurretObject::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}