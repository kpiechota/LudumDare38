#pragma once
#include "headers.h"

class CPlayerObject : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_speed;
	float m_shootSpeed;
	float m_lastShoot;

private:
	inline void CollisionTest();

public:
	CPlayerObject();

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual Vec2 GetPosition() const override;
	virtual float GetSize() const override;
	virtual bool NeedDelete() const override { return false; }
	virtual void TakeDamage(float const damage) override;
};
