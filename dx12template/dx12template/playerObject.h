#pragma once
#include "headers.h"

class CPlayerObject : public CGameObject
{
private:
	SRenderObject m_renderObject;
	float m_speed;

private:
	inline void CollisionTest();

public:
	CPlayerObject();

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual Vec2 GetPosition() const override;
	virtual float GetSize() const override;
};
