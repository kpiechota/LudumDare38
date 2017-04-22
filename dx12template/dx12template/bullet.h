#pragma once
#include "headers.h"

class CBullet : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_lifeTime;
	float m_speed;

public:
	CBullet(SRenderObject const& renderObject);

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual Vec2 GetPosition() const override;
	virtual float GetSize() const override;
	virtual bool NeedDelete() const override;
};
