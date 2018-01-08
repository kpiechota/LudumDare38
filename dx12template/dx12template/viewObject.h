#pragma once

struct SCameraMatrices
{
	Matrix4x4 m_viewToScreen;
	Matrix4x4 m_screenToView;
	Matrix4x4 m_worldToView;
	Matrix4x4 m_viewToWorld;
	Matrix4x4 m_worldToScreen;
	Matrix4x4 m_screenToWorld;
};

struct SRenderData
{
	enum
	{
		MAX_TEXTURES_NUM = 4
	};

	enum class EDrawType : Byte
	{
		DrawInstanced,
		DrawIndexedInstanced,

		DrawInvalid
	};

	D3D12_GPU_VIRTUAL_ADDRESS m_cbOffset;
	D3D_PRIMITIVE_TOPOLOGY m_topology;

	UINT16 m_verticesStart;
	UINT16 m_indicesStart;
	UINT16 m_indicesNum;
	UINT16 m_instancesNum;

	UINT8 m_texturesOffset;
	UINT8 m_texturesNum;

	Byte m_geometryID;

	Byte m_shaderID;
	EDrawType m_drawType;
};
POD_TYPE( SRenderData )

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
	RL_TRANSLUCENT,
	RL_OVERLAY,

	RL_MAX
};

struct SViewObject
{
	SCameraMatrices m_camera;
	TArray< SRenderData > m_renderData[ RL_MAX ];
	TArray< SLightData > m_lightData;
};