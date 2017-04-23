#pragma once
#include "headers.h"

class CHealthEffectObject : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_maxLifeTime;
	float m_lifeTime;
	float m_maxSize;

public:
	CHealthEffectObject(SRenderObject const& renderObject, float const lifeTime);

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override { return m_lifeTime < 0.f; }
	virtual void TakeDamage(float const damage) override {}
};