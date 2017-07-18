#pragma once

__declspec(align(256))
struct CBObject
{
	Matrix4x4 m_objectToScreen;
	Matrix4x4 m_objectToWorld;
};
__declspec(align(256))
struct CBSdfDraw
{
	Vec4 m_sdfColor;
	Vec2 m_cutoff;
};

__declspec(align(256))
struct CBSimpleLight
{
	Matrix4x4 m_viewToWorld;
	Vec4 m_perspectiveValues;
	Vec4 m_lightPos;
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
	}
};