#pragma once
#include "headers.h"

class CPlayerObject : public CGameObject
{
private:
	SRenderObject m_renderObject;

	SRenderObject m_renderObjectTurret;
	SRenderObject m_renderObjectHealth;

	class CStaticObject* m_initScreen;

	float m_speed;
	float m_shootSpeed;
	float m_lastShoot;
	float m_energyValue;

	float m_health;
	float m_maxHealth;

private:
	inline void CollisionTest();
	inline void DrawHealthBar() const;

public:
	CPlayerObject();

	void AddEnergy();
	float GetHealth() const { return m_health; }

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override { return false; }
	virtual void TakeDamage(Vec2 const rotation, float const damage) override;
};
