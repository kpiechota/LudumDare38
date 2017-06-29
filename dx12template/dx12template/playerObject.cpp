#include "render.h"
#include "playerObject.h"
#include "timer.h"
#include "soundEngine.h"
#include "input.h"
#include "bullet.h"
#include "turretObject.h"
#include "healthObject.h"
#include "staticObject.h"

extern CTimer GTimer;
extern Matrix3x3 GScreenMatrix;

inline void CPlayerObject::CollisionTest()
{
	float const magnitudeFromCenter2 = m_position.x * m_position.x + m_position.y * m_position.y;
	float const centerRadius = GIslandSize - m_scale.x;
	float const centerRadius2 = centerRadius * centerRadius;

	if (centerRadius2 < magnitudeFromCenter2)
	{
		float const magnitudeFromCenter = sqrt(magnitudeFromCenter2);
		float const invMagnitude = 1.f / magnitudeFromCenter2;
		Vec2 vectorTo = m_position * invMagnitude;
		Vec2 const offset = vectorTo * (centerRadius2 - magnitudeFromCenter2);
		m_position += offset;
	}

	unsigned int const gameObjectsNum = GGameObjects.size();
	for (unsigned int gameObjectID = 0; gameObjectID < gameObjectsNum; ++gameObjectID)
	{
		CGameObject const * const pGameObject = GGameObjects[gameObjectID];

		if (pGameObject != this && pGameObject->CollideWith(CF_PLAYER))
		{
			Vec2 vectorTo = pGameObject->GetPosition() - m_position;
			float const magnitude2 = vectorTo.x * vectorTo.x + vectorTo.y * vectorTo.y;
			float const radius = m_colliderSize.x + pGameObject->GetColliderSize().x;
			float const radius2 = radius * radius;

			if (magnitude2 < radius2 && 0.f < magnitude2 )
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

void CPlayerObject::DrawHealthBar() const
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

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position + Vec2(0.f, 12.f + 3.f + 6.f), Vec2(1.f, 0.f), Vec2(12.f, 2.f)));
	constBuffer->m_colorScale.Set( 0.f, 0.f, 0.f, 1.f );
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );
	GRenderObjects[RL_OVERLAY0].push_back(renderData);

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(m_position + Vec2(-12.f, 12.f + 3.f + 6.f), Vec2(1.f, 0.f), Vec2(12.f * m_energyValue, 2.f)));
	constBuffer->m_colorScale.Set(0.2f, .8f, 1.f, 1.f);
	constBuffer->m_offset.Set( 1.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );
	GRenderObjects[RL_OVERLAY0].push_back(renderData);
}

void CPlayerObject::DrawIcons() const
{
	CBObject* constBuffer;
	SRenderData renderData;
	float const iconY = -0.5f * ((float)GHeight) + 35.f;
	float const iconColor = (m_energyValue < 1.f) ? 0.5f : 1.f;

	renderData.m_dataNum = 4;
	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	renderData.m_textureID = T_TURRET_ICON;
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(Vec2(-35.f, iconY), Vec2(1.f, 0.f), Vec2(32.f, 32.f)));
	constBuffer->m_colorScale.Set(iconColor, iconColor, iconColor, 1.f);
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );

	GRenderObjects[RL_OVERLAY1].push_back(renderData);

	GRender.GetRenderData( sizeof( CBObject ), renderData.m_cbOffset, reinterpret_cast< void*& >( constBuffer ) );
	renderData.m_textureID = T_HEALTH_ICON;
	constBuffer->m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(Vec2(35.f, iconY), Vec2(1.f, 0.f), Vec2(32.f, 32.f)));
	constBuffer->m_colorScale.Set(iconColor, iconColor, iconColor, 1.f);
	constBuffer->m_offset.Set( 0.f, 0.f );
	constBuffer->m_uvOffset.Set( 0.f, 0.f );
	constBuffer->m_uvTile.Set( 1.f, 1.f );

	GRenderObjects[RL_OVERLAY1].push_back(renderData);
}


CPlayerObject::CPlayerObject()
	: m_speed( 100.f )
	, m_shootSpeed( 0.1f )
	, m_lastShoot( 0.f )
	, m_energyValue( 1.f )
	, m_maxHealth(15.f)
	, m_health(15.f)
	, m_initScreen( nullptr )
	, m_shaderID( 0 )
	, m_textureID( T_PLAYER )
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET);

	m_position.Set(0.f, 32.f + 12.f);
	m_scale = 12.f;
	m_colliderSize = 12.f;

	float const iconY = -0.5f * ((float)GHeight) + 35.f;

	m_initScreen = new CStaticObject(-1.f, 0, RL_OVERLAY2);
	m_initScreen->SetScale( 400.f );
	m_initScreen->SetShaderID( ST_OBJECT_DRAW_BLEND );
	m_initScreen->SetTextureID( T_INIT_SCREEN );
	GGameObjectsToSpawn.push_back(m_initScreen);

	GTimer.SetGameScale(0.f);
}

void CPlayerObject::AddEnergy()
{
	m_energyValue = min(1.f, m_energyValue + 0.2f);
}
void CPlayerObject::FillRenderData() const
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

	GRenderObjects[RL_FOREGROUND0].push_back(renderData);

	DrawHealthBar();
	DrawIcons();

	char const c = char(UINT(GTimer.GetSeconds( GTimer.TimeFromStart() )) % 93 );
}

void CPlayerObject::Update()
{
	if ( !m_initScreen && 0.f < m_health )
	{
		Vec2 moveDir;

		if (GInputManager.IsKeyDown('W'))
		{
			moveDir.y += 1.f;
		}

		if (GInputManager.IsKeyDown('S'))
		{
			moveDir.y -= 1.f;
		}

		if (GInputManager.IsKeyDown('A'))
		{
			moveDir.x -= 1.f;
		}

		if (GInputManager.IsKeyDown('D'))
		{
			moveDir.x += 1.f;
		}

		moveDir.Normalize();
		moveDir *= m_speed * GTimer.GameDelta();

		m_position += moveDir;
		CollisionTest();

		Vec2i mouseScreenPos;
		GInputManager.GetMousePosition(mouseScreenPos);

		Vec2 mouseWorldPos = Vec2(mouseScreenPos) - Vec2((float)(GWidth) * 0.5f, (float)(GHeight) * 0.5f);
		mouseWorldPos.y = -mouseWorldPos.y;

		m_rotation = mouseWorldPos - m_position;
		m_rotation.Normalize();

		m_lastShoot -= GTimer.GameDelta();
		if (m_lastShoot < 0.f && GInputManager.IsKeyDown(K_LEFTM))
		{
			CBullet* bullet = new CBullet(0.2f, CF_PLAYER_BULLET);
			bullet->SetPosition( m_position + m_rotation * m_scale );
			bullet->SetRotation( m_rotation );
			bullet->SetScale( 2.f );
			bullet->SetColliderSize( 2.f );
			bullet->SetTextureID( T_BULLET0 );
			GGameObjectsToSpawn.push_back(bullet);

			m_lastShoot = m_shootSpeed;

			GSoundEngine.Play2DSound(GSounds[SET_SHOOT0]);
		}

		if (1.f == m_energyValue)
		{
			if (GInputManager.IsKeyDown('Q'))
			{
				m_energyValue = 0.f;

				CTurretObject* turret = new CTurretObject();
				turret->SetPosition( m_position + m_rotation * (m_scale + 14.f) );
				turret->SetRotation( m_rotation );
				turret->SetScale( 12.f );
				turret->SetColliderSize( 12.f );
				turret->SetTextureID( T_TURRET );

				GGameObjectsToSpawn.push_back(turret);
				GSoundEngine.Play2DSound(GSounds[SET_BUILD]);
			}
			else if (GInputManager.IsKeyDown('E'))
			{
				m_energyValue = 0.f;

				CHealthObject* health = new CHealthObject();
				health->SetPosition( m_position + m_rotation * (m_scale + 14.f) );
				health->SetRotation( m_rotation );
				health->SetScale( 12.f );
				health->SetColliderSize( 12.f );
				health->SetTextureID( T_HEALTH );

				GGameObjectsToSpawn.push_back(health);
				GSoundEngine.Play2DSound(GSounds[SET_BUILD]);
			}
		}
	}
	else if (m_initScreen && GInputManager.IsKeyDown(' '))
	{
		m_initScreen->ForceDelete();
		m_initScreen = nullptr;
		GTimer.SetGameScale(1.f);
	}
}

void CPlayerObject::TakeDamage(Vec2 const rotation, float const damage)
{
	m_health = max( 0.f, min(m_maxHealth, m_health - damage) );
}

void CPlayerObject::SetShaderID( Byte const shaderID )
{
	m_shaderID = shaderID;
}
void CPlayerObject::SetTextureID( Byte const textureID )
{
	m_textureID = textureID;
}

void CPlayerObject::SetColor( Vec4 const& color )
{
	m_material.m_color = color;
}
void CPlayerObject::SetPositionOffset( Vec2 const offset )
{
	m_material.m_positionOffset = offset;
}
void CPlayerObject::SetUvTile( Vec2 const tile )
{
	m_material.m_uvTile = tile;
}
void CPlayerObject::SetUvOffset( Vec2 const offset )
{
	m_material.m_uvOffset = offset;
}
