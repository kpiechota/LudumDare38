#pragma once
#include "headers.h"

class CStaticObject : public CGameObject
{
private:
	SObjectMaterial m_material;

	Byte m_geometryInfoID;
	Byte m_shaderID;
	Byte m_textureID[ SRenderData::MAX_TEXTURES_NUM ];

	Byte m_layer;

public:
	CStaticObject( Byte const collisionMask, Byte const layer);

	void SetShaderID( Byte const shaderID );
	void SetTextureID( UINT const texture, Byte const textureID );
	void SetGeomtryInfoID( Byte const geometryInfoID );

	void SetColor( Vec4 const& color );
	void SetPositionOffset( Vec2 const offset );
	void SetUvTile( Vec2 const tile );
	void SetUvOffset( Vec2 const offset );

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual bool NeedDelete() const override { return false; }
	virtual void TakeDamage(Vec2 const rotation, float const damage) override {}
};
