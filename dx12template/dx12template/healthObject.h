#pragma once
#include "headers.h"

class CHealthObject : public CGameObject
{
private:
	SObjectMaterial m_material;

	float m_healSpeed;
	float m_lastHeal;
	float m_healRadius2;
	float m_maxHealth;
	float m_health;
	float m_veinsSpawnTime;
	float m_lastVeinsSpawnTime;
	float m_hitTime;
	float m_lastHitTime;

	Byte m_shaderID;
	Byte m_textureID;

private:
	inline void DrawHealthBar() const;

public:
	CHealthObject();

	void SetShaderID( Byte const shaderID );
	void SetTextureID( Byte const textureID );

	void SetColor( Vec4 const& color );
	void SetPositionOffset( Vec2 const offset );
	void SetUvTile( Vec2 const tile );
	void SetUvOffset( Vec2 const offset );

	virtual void Start() override;
	virtual void Update() override;
	virtual void FillRenderData() const override;
	virtual bool NeedDelete() const override;
	virtual void TakeDamage(Vec2 const rotation, float const damage) override;
};