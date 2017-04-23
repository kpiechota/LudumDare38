#pragma once
#include "headers.h"

class CBullet : public CGameObject
{
private:
	SRenderObject m_renderObject;
	ECollisionFlag m_bulletType;
	float m_lifeTime;
	float m_speed;
	float m_damage;

private:
	inline void CollisionTest();

public:
	CBullet(SRenderObject const& renderObject, float const damage, ECollisionFlag const bulletType);

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override;
	virtual void TakeDamage(float const damage) override {}
};
