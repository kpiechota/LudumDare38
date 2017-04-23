#include "generatorObject.h"
#include "staticObject.h"
#include "timer.h"

extern CTimer GTimer;

void CGeneratorObject::DrawHealthBar() const
{
	SRenderObject healthBarBackground;
	healthBarBackground.m_colorScale.Set(0.f, 0.f, 0.f, 1.f);
	healthBarBackground.m_positionWS = m_renderObject.m_positionWS + Vec2(0.f, 32.f + 3.f);
	healthBarBackground.m_size.Set(32.f, 2.f);
	healthBarBackground.m_texutreID = T_BLANK;

	SRenderObject healthBar;
	healthBar.m_colorScale.Set(0.f, 1.f, 0.f, 1.f);
	healthBar.m_positionWS = m_renderObject.m_positionWS + Vec2(-32.f, 32.f + 3.f);
	healthBar.m_size.Set(32.f * (m_health / m_maxHealth), 2.f);
	healthBar.m_offset.Set(1.f, 0.f);
	healthBar.m_texutreID = T_BLANK;

	GRenderObjects[RL_OVERLAY0].push_back(healthBarBackground);
	GRenderObjects[RL_OVERLAY0].push_back(healthBar);
}

CGeneratorObject::CGeneratorObject(SRenderObject const& renderObject)
	: m_renderObject(renderObject)
	, m_maxHealth(100.f)
	, m_health(m_maxHealth)
	, m_veinsSpawnTime(0.1f)
	, m_lastVeinsSpawnTime(0.f)
{
	m_collisionMask = (Byte)(CF_ENEMY | CF_ENEMY_BULLET | CF_PLAYER);
}

void CGeneratorObject::Update()
{
	m_lastVeinsSpawnTime -= GTimer.GameDelta();

	if (m_lastVeinsSpawnTime < 0.f)
	{
		m_lastVeinsSpawnTime = m_veinsSpawnTime;

		Vec2 veinOffset = Vec2::GetRandomOnCircle();

		SRenderObject veinObject;
		veinObject.m_positionWS = m_renderObject.m_positionWS + veinOffset * 1.25f * m_renderObject.m_size.x;

		veinObject.m_colorScale.Set(0.2f, .8f, 1.f, 1.f);
		veinObject.m_rotation.Set(veinOffset.y, -veinOffset.x);
		veinObject.m_size = 16.f;
		veinObject.m_uvTile.x = (rand() % 2) ? 1.f : -1.f;
		veinObject.m_texutreID = T_VEINS;
		veinObject.m_shaderID = ST_OBJECT_DRAW_BLEND;

		CStaticObject* pVein = new CStaticObject(veinObject, 0.2f, 0, RL_FOREGROUND1);
		GGameObjectsToSpawn.push_back(pVein);
	}
}

void CGeneratorObject::FillRenderData() const
{
	GRenderObjects[RL_FOREGROUND0].push_back(m_renderObject);
	DrawHealthBar();
}

Vec2 CGeneratorObject::GetPosition() const
{
	return m_renderObject.m_positionWS;
}

Vec2 CGeneratorObject::GetSize() const
{
	return m_renderObject.m_size;
}
