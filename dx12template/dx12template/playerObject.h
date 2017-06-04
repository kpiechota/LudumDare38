#pragma once
#include "headers.h"

class CPlayerObject : public CGameObject
{
private:
	SObjectMaterial m_material;

	class CStaticObject* m_initScreen;

	float m_speed;
	float m_shootSpeed;
	float m_lastShoot;
	float m_energyValue;

	float m_health;
	float m_maxHealth;

	Byte m_shaderID;
	Byte m_textureID;

private:
	inline void CollisionTest();
	inline void DrawHealthBar() const;
	inline void DrawIcons() const;

public:
	CPlayerObject();

	void AddEnergy();
	float GetHealth() const { return m_health; }

	void SetShaderID( Byte const shaderID );
	void SetTextureID( Byte const textureID );

	void SetColor( Vec4 const& color );
	void SetPositionOffset( Vec2 const offset );
	void SetUvTile( Vec2 const tile );
	void SetUvOffset( Vec2 const offset );

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual bool NeedDelete() const override { return false; }
	virtual void TakeDamage(Vec2 const rotation, float const damage) override;
};
