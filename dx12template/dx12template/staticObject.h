#pragma once

class CStaticObject : public CGameObject
{
private:
	Byte m_geometryInfoID;
	Byte m_shaderID;
	Byte m_textureID[ SRenderData::MAX_TEXTURES_NUM ];

	Byte m_layer;

public:
	CStaticObject( Byte const collisionMask, Byte const layer);

	void SetShaderID( Byte const shaderID );
	void SetTextureID( UINT const texture, Byte const textureID );
	void SetGeomtryInfoID( Byte const geometryInfoID );

	virtual void FillRenderData() const override;
	virtual void Update() override;
	virtual bool NeedDelete() const override { return false; }
};
