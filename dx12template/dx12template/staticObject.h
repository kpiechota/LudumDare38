#pragma once
#include "headers.h"

class CStaticObject : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_lifeTime;
	Byte m_layer;
	bool m_allowDestroy;

public:
	CStaticObject(SRenderObject const& renderObject, float const lifeTime, Byte const collisionMask, Byte const layer);

	void ForceDelete() { m_allowDestroy = true; m_lifeTime = -1.f; }

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override { return m_allowDestroy && m_lifeTime < 0.f; }
	virtual void TakeDamage(Vec2 const rotation, float const damage) override {}
};
