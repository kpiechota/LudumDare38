#pragma once

#include "../headers.h"
#include "renderConstant.h"
#include "vertexFormats.h"
#include "geometry.h"
#include "descriptorHeap.h"
#include "shaderRes.h"
#include "texture.h"
#include "textRenderManager.h"
#include "environmentParticleManager.h"

POD_TYPE(D3D12_RESOURCE_BARRIER)

struct SRenderFrameData
{
	ID3D12CommandAllocator*		m_frameCA;
	ID3D12GraphicsCommandList*	m_frameCL;
};

struct SDescriptorsOffsets
{
	UINT8 m_rtvOffset;
	UINT8 m_srvOffset;
};

class CConstBufferCtx
{
private:
	CShaderRes const* m_shader;
	Byte* m_pConstBuffer;

public:
	CConstBufferCtx( CShaderRes const* shader, Byte* pConstBuffer )
		: m_shader( shader )
		, m_pConstBuffer( pConstBuffer )
	{}
	void SetParam( Byte const* pData, UINT16 const size, EShaderParameters const param ) const;
};

class CRender
{
private:
	enum
	{
		FRAME_NUM = 3,
		MAX_OBJECTS = 2048
	};

	enum EGBufferBuffers
	{
		GBB_DIFFUSE,
		GBB_NORMAL,
		GBB_EMISSIVE_SPEC,
		GBB_DEPTH,

		GBB_MAX,
	};

#ifdef _DEBUG
	ID3D12Debug*					m_debugController;
#endif

	ID3D12Device*					m_device;
	IDXGIFactory4*					m_factor;

	ID3D12Fence*					m_fence;
	HANDLE							m_fenceEvent;
	UINT							m_fenceValue;

	D3D12_VIEWPORT					m_viewport;
	D3D12_RECT						m_scissorRect;

	ID3D12CommandQueue*				m_copyCQ;
	ID3D12CommandAllocator*			m_copyCA;
	ID3D12GraphicsCommandList*		m_copyCL;

	ID3D12CommandQueue*				m_graphicsCQ;
	ID3D12CommandAllocator*			m_graphicsCA;
	ID3D12GraphicsCommandList*		m_graphicsCL;

	ID3D12CommandQueue*				m_computeCQ;
	TArray< ID3D12CommandList* >	m_computeCommandLists;

	IDXGISwapChain3*				m_swapChain;

	SDescriptorHeap					m_renderTargetDH;
	ID3D12Resource*					m_gbufferBuffers[ GBB_MAX ];
	ID3D12Resource*					m_rederTarget[FRAME_NUM];
	SDescriptorHeap					m_depthBuffertDH;

	SRenderFrameData				m_frameData[FRAME_NUM];

	ID3D12RootSignature*			m_graphicsRS;
	CShaderRes						m_shaders[ST_MAX];
	CShaderRes						m_shaderLight[LF_MAX];

	SDescriptorHeap					m_texturesDH;

	ID3D12Resource*					m_fullscreenTriangleRes;
	D3D12_VERTEX_BUFFER_VIEW		m_fullscreenTriangleView;

	SDescriptorsOffsets				m_gbufferDescriptorsOffsets[ GBB_MAX ];

	TArray< SGeometry >				m_geometryResources;
	TArray< ID3D12Resource* >		m_texturesResources;
	TArray< ID3D12Resource* >		m_uploadResources;
	TArray< D3D12_RESOURCE_BARRIER >m_resourceBarrier;

	int								m_wndWidth;
	int								m_wndHeight;
	HWND							m_hwnd;
	UINT							m_frameID;

	ID3D12Resource*					m_constBufferResource;
	Byte*							m_pConstBufferData;
	UINT							m_constBufferOffset;

	Vec3							m_directLightDir;
	Vec3							m_directLightColor;
	Vec3							m_ambientLightColor;

private:
	void InitCommands();
	void InitFrameData();
	void InitConstBuffer();
	void InitSwapChain();
	void InitRenderTargets();
	void InitDescriptorHeaps();

	void InitRootSignatures();

	void ResizeDescriptorHeap( SDescriptorHeap& descriptorHeap, UINT const size );
	void CreateFullscreenTriangleRes();

	void InitShaders();
	void DrawFullscreenTriangle( ID3D12GraphicsCommandList* commandList );
	FORCE_INLINE void DrawOpaque( ID3D12GraphicsCommandList* commandList );
	void DrawRenderData( ID3D12GraphicsCommandList* commandList, TArray< SRenderData > const& renderData );
	void DrawLights( ID3D12GraphicsCommandList* commandList, TArray< SLightData > const& lightData );

public:
	void Init();
	void PreDrawFrame();
	void DrawFrame();
	void Release();

	ID3D12RootSignature* GetMainRS() { return m_graphicsRS; }
	ID3D12Device* GetDevice() { return m_device; }

	Byte AddGeometry( SGeometry const& geometry );
	SGeometry& GetGeometry( Byte const geometryID );
	void ReleaseGeometry( Byte const geometryID );

	void BeginLoadResources(unsigned int const textureNum);
	void LoadResource(STexture const& texture);
	Byte LoadResource(SGeometryData const& geometryData);
	void EndLoadResources();
	void WaitForResourcesLoad();

	CConstBufferCtx GetConstBufferCtx( D3D12_GPU_VIRTUAL_ADDRESS& outCbOffset, Byte const shader );
	CConstBufferCtx GetLightConstBufferCtx( D3D12_GPU_VIRTUAL_ADDRESS& outCbOffset, Byte const shader );
	void SetConstBuffer( D3D12_GPU_VIRTUAL_ADDRESS& outConstBufferAddress, Byte* const pData, UINT const size );

	void ExecuteComputeQueue( UINT const commandListNum, ID3D12CommandList* const* pCommandLists );

	void WaitForCopyQueue();
	void WaitForGraphicsQueue();
	void WaitForComputeQueue();

	void AddComputeCommandList( ID3D12CommandList* pCommandList );

public: //Getters/setters
	void SetWindowWidth(int const wndWidth) { m_wndWidth = wndWidth; }
	int GetWindowWidth() const { return m_wndWidth; }

	void SetWindowHeight(int const wndHeight) { m_wndHeight = wndHeight; }
	int GetWindowHeight() const { return m_wndHeight; }

	void SetHWND(HWND& hwnd) { m_hwnd = hwnd; }
	HWND SetHWND() const { return m_hwnd; }

	void SetDirectLightDir( Vec3 const dir ) { m_directLightDir = dir; }
	void SetDirectLightColor( Vec3 const color ) { m_directLightColor = color; }
	void SetAmbientLightColor( Vec3 const color ) { m_ambientLightColor = color; }
};

extern CRender GRender;