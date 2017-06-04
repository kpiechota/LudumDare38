#pragma once
#include "headers.h"

class CHealthEffectObject : public CGameObject
{
private:
	SObjectMaterial m_material;

	float m_maxLifeTime;
	float m_lifeTime;
	float m_maxSize;

	Byte m_shaderID;
	Byte m_textureID;

public:
	CHealthEffectObject( float const lifeTime);

	void SetShaderID( Byte const shaderID );
	void SetTextureID( Byte const textureID );

	void SetColor( Vec4 const& color );
	void SetPositionOffset( Vec2 const offset );
	void SetUvTile( Vec2 const tile );
	void SetUvOffset( Vec2 const offset );

	virtual void Start() override;
	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual bool NeedDelete() const override { return m_lifeTime < 0.f; }
	virtual void TakeDamage(Vec2 const rotation, float const damage) override {}
};