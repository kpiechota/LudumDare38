#pragma once
#include "headers.h"

class CBullet : public CGameObject
{
private:
	SObjectMaterial m_material;

	ECollisionFlag m_bulletType;
	float m_lifeTime;
	float m_speed;
	float m_damage;

	Byte m_shaderID;
	Byte m_textureID;

private:
	inline void CollisionTest();

public:
	CBullet( float const damage, ECollisionFlag const bulletType);

	void SetShaderID( Byte const shaderID );
	void SetTextureID( Byte const textureID );

	void SetColor( Vec4 const& color );
	void SetPositionOffset( Vec2 const offset );
	void SetUvTile( Vec2 const tile );
	void SetUvOffset( Vec2 const offset );

	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual bool NeedDelete() const override;
	virtual void TakeDamage(Vec2 const rotation, float const damage) override {}
};
