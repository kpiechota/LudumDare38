#pragma once

struct STextVertexFormat
{
	static D3D12_INPUT_ELEMENT_DESC const desc[];

	Vec2 m_position;
	Vec2 m_uv;
};
