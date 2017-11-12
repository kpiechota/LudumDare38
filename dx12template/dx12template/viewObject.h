#pragma once

struct SRenderData
{
	enum
	{
		MAX_TEXTURES_NUM = 4
	};

	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;

	UINT m_verticesStart;
	UINT m_indicesStart;
	UINT m_dataNum;

	D3D_PRIMITIVE_TOPOLOGY m_topology;

	Byte m_geometryID;

	Byte m_textureID[ MAX_TEXTURES_NUM ];
	Byte m_shaderID;

	SRenderData()
		: m_cbOffset( 0 )
		, m_verticesStart( 0 )
		, m_indicesStart( 0 )
		, m_dataNum( 0 )
		, m_topology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP )
		, m_geometryID( UINT8_MAX )
		, m_shaderID( 0 )
	{
		memset( m_textureID, UINT8_MAX, sizeof( m_textureID ) );
	}
};

enum ELightFlags
{
	LF_NONE		= 0,
	LF_DIRECT	= FLAG( 0 ),
	LF_POINT	= FLAG( 1 ),
	LF_AMBIENT	= FLAG( 2 ),

	LF_MAX		= FLAG( 3 )
};

struct SLightData
{
	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;
	Byte m_lightShader;
};
POD_TYPE(SLightData)

enum ERenderLayer
{
	RL_OPAQUE,
	RL_OVERLAY,

	RL_MAX
};

struct SViewObject
{
	TArray< SRenderData > m_renderData[ RL_MAX ];
	TArray< SLightData > m_lightData;
};