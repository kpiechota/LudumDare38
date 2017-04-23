#pragma once
#include "headers.h"

class CEnemyObject : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_speed;
	float m_shootSpeed;
	float m_lastShoot;
	float m_shootRadius2;
	float m_maxHealth;
	float m_health;

private:
	inline void DrawHealthBar() const;
	inline void CollisionTest();
	inline Vec2 FindNearestObject() const;

public:
	CEnemyObject();

	void SetPosition(Vec2 const position) { m_renderObject.m_positionWS = position; }

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override;
	virtual void TakeDamage(Vec2 const rotation, float const damage) override;
};
