#pragma once

__declspec(align(256))
struct CBObject
{
	Matrix4x4 m_objectToScreen;
	Vec4 m_colorScale;
	Vec2 m_uvTile;
	Vec2 m_uvOffset;
	Vec2 m_offset;
};
__declspec(align(256))
struct CBSdfDraw
{
	Matrix4x4 m_objectToScreen;
	Vec4 m_sdfColor;
	Vec2 m_cutoff;
};

struct SObjectMaterial
{
	Vec4 m_color;
	Vec2 m_uvTile;
	Vec2 m_uvOffset;
	Vec2 m_positionOffset;

	SObjectMaterial()
	{
		m_color.Set( 1.f, 1.f, 1.f, 1.f );
		m_uvTile.Set( 1.f, 1.f );
		m_uvOffset.Set( 0.f, 0.f );
		m_positionOffset.Set( 0.f, 0.f );
	}

	void FillConstBuffer( CBObject* cbuffer ) const
	{
		cbuffer->m_colorScale = m_color;
		cbuffer->m_offset = m_positionOffset;
		cbuffer->m_uvOffset = m_uvOffset;
		cbuffer->m_uvTile = m_uvTile;
	}
};