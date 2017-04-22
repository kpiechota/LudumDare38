#pragma once
#include "headers.h"

class CPlayerObject : public CGameObject
{
private:
	SRenderObject m_renderObject;

	SRenderObject m_renderObjectTurret;
	SRenderObject m_renderObjectHealth;

	float m_speed;
	float m_shootSpeed;
	float m_lastShoot;
	float m_energyValue;

private:
	inline void CollisionTest();

public:
	CPlayerObject();

	void AddEnergy();

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual Vec2 GetPosition() const override;
	virtual float GetSize() const override;
	virtual bool NeedDelete() const override { return false; }
	virtual void TakeDamage(float const damage) override;
};
