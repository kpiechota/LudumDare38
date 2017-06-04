#pragma once
#include "headers.h"

class CStaticObject : public CGameObject
{
private:
	SObjectMaterial m_material;

	float m_lifeTime;

	Byte m_shaderID;
	Byte m_textureID;

	Byte m_layer;
	bool m_allowDestroy;

public:
	CStaticObject( float const lifeTime, Byte const collisionMask, Byte const layer);

	void ForceDelete() { m_allowDestroy = true; m_lifeTime = -1.f; }

	void SetShaderID( Byte const shaderID );
	void SetTextureID( Byte const textureID );

	void SetColor( Vec4 const& color );
	void SetPositionOffset( Vec2 const offset );
	void SetUvTile( Vec2 const tile );
	void SetUvOffset( Vec2 const offset );

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual bool NeedDelete() const override { return m_allowDestroy && m_lifeTime < 0.f; }
	virtual void TakeDamage(Vec2 const rotation, float const damage) override {}
};
