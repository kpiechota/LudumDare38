#pragma once
#include "headers.h"

class CGeneratorObject : public CGameObject
{
private:
	SRenderObject m_renderObject;

	float m_maxHealth;
	float m_health;
	float m_veinsSpawnTime;
	float m_lastVeinsSpawnTime;

private:
	inline void DrawHealthBar() const;

public:
	CGeneratorObject(SRenderObject const& renderObject);
	float GetHealth() const { return m_health; }

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override { return m_health < 0.f; }
	virtual void TakeDamage(float const damage) override;
};
