#pragma once

class CLightObject : public CGameObject
{
private:
	Vec3 m_color;
	float m_radius;
	float m_fade;

	Byte m_lighShader;

public:
	CLightObject( Byte const type );

	void SetColor( Vec3 const& color ) { m_color = color; }
	void SetRadius( float const radius ) { m_radius = radius; }
	void SetFade( float const fade ) { m_fade = fade; }

	virtual void FillRenderData() const override;
	virtual void Update() override{}
	virtual bool NeedDelete() const override { return false; }
};
