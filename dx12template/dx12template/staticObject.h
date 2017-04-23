#pragma once
#include "headers.h"

class CStaticObject : public CGameObject
{
private:
	SRenderObject m_renderObject;

public:
	CStaticObject(SRenderObject const& renderObject);

	virtual void FillRenderData() const override;
	virtual void Update() override {}
	virtual Vec2 GetPosition() const override;
	virtual Vec2 GetSize() const override;
	virtual bool NeedDelete() const override { return false; }
	virtual void TakeDamage(float const damage) override {}
};
