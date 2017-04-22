#pragma once
#include "headers.h"

class CTurretObject : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_shootSpeed;
	float m_lastShoot;
	float m_shootRadius2;
	float m_health;

private:
	inline Vec2 FindNearestObject() const;

public:
	CTurretObject( SRenderObject const& renderObject );

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual Vec2 GetPosition() const override;
	virtual float GetSize() const override;
	virtual bool NeedDelete() const override;
	virtual void TakeDamage(float const damage) override;
};
