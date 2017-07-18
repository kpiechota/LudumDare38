#pragma once

#include "headers.h"
#include "renderConstant.h"
#include "vertexFormats.h"
#include "geometry.h"
#include "descriptorHeap.h"

struct SRenderFrameData
{
	ID3D12CommandAllocator*		m_frameCA;
	ID3D12GraphicsCommandList*	m_frameCL;
	ID3D12Resource*				m_frameResource;
	Byte*						m_pResourceData;
};

struct SDescriptorsOffsets
{
	UINT8 m_rtvOffset;
	UINT8 m_srvOffset;
};

struct SView
{
	Matrix4x4 m_viewToScreen;
	Matrix4x4 m_screenToView;
	Matrix4x4 m_worldToView;
	Matrix4x4 m_viewToWorld;
	Matrix4x4 m_worldToScreen;
	Matrix4x4 m_screenToWorld;
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

	SView							m_mainCamera;

	D3D12_VIEWPORT					m_viewport;
	D3D12_RECT						m_scissorRect;

	ID3D12CommandQueue*				m_copyCQ;
	ID3D12CommandAllocator*			m_copyCA;
	ID3D12GraphicsCommandList*		m_copyCL;

	ID3D12CommandQueue*				m_mainCQ;
	ID3D12CommandAllocator*			m_mainCA;
	ID3D12GraphicsCommandList*		m_mainCL;

	IDXGISwapChain3*				m_swapChain;

	SDescriptorHeap					m_renderTargetDH;
	ID3D12Resource*					m_gbufferBuffers[ GBB_MAX ];
	ID3D12Resource*					m_rederTarget[FRAME_NUM];
	SDescriptorHeap					m_depthBuffertDH;

	SRenderFrameData				m_frameData[FRAME_NUM];

	ID3D12RootSignature*			m_mainRS;
	ID3D12PipelineState*			m_shaders[ST_MAX];
	ID3D12PipelineState*			m_shaderSimpleLight;

	SDescriptorHeap					m_texturesDH;

	ID3D12Resource*					m_fullscreenTriangleRes;
	D3D12_VERTEX_BUFFER_VIEW		m_fullscreenTriangleView;

	SDescriptorsOffsets				m_gbufferDescriptorsOffsets[ GBB_MAX ];

	std::vector< SGeometry >				m_geometryResources;
	std::vector< ID3D12Resource* >			m_texturesResources;
	std::vector< ID3D12Resource* >			m_uploadResources;
	std::vector< D3D12_RESOURCE_BARRIER >	m_resourceBarrier;

	int								m_wndWidth;
	int								m_wndHeight;
	HWND							m_hwnd;
	UINT							m_frameID;

	UINT							m_constBufferOffset;

private:
	void InitCommands();
	void InitFrameData();
	void InitSwapChain();
	void InitRenderTargets();
	void InitDescriptorHeaps();

	void InitRootSignatures();

	void ResizeDescriptorHeap( SDescriptorHeap& descriptorHeap, UINT const size );
	void CreateFullscreenTriangleRes();

	inline void LoadShader(LPCWSTR pFileName, D3D_SHADER_MACRO const* pDefines, LPCSTR pEmtryPoint, LPCSTR pTarget, ID3DBlob** ppCode) const;

	void InitShader( LPCWSTR pFileName, ID3D12PipelineState*& pso, D3D12_INPUT_ELEMENT_DESC const* vertexElements, UINT const vertexElementsNum, UINT const renderTargetNum, DXGI_FORMAT const* renderTargetFormats, ERenderTargetBlendStates const* renderTargetBlendStates, EDepthStencilStates const depthStencilState = EDepthStencilStates::EDSS_Disabled, ERasterizerStates const rasterizationState = ERasterizerStates::ERS_Default );
	void InitShaders();
	void DrawFullscreenTriangle( ID3D12GraphicsCommandList* commandList );
	void DrawLayer( ID3D12GraphicsCommandList* commandList, ERenderLayer const layerID );

public:
	void Init();
	void DrawFrame();
	void Release();

	ID3D12Device* GetDevice() { return m_device; }

	void PrepareView();

	Byte AddGeometry( SGeometry const& geometry );
	SGeometry& GetGeometry( Byte const geometryID );
	void ReleaseGeometry( Byte const geometryID );

	void BeginLoadResources(unsigned int const textureNum);
	void LoadResource(STexture const& texture);
	Byte LoadResource(SGeometryData const& geometryData);
	void EndLoadResources();
	void WaitForResourcesLoad();

	void GetRenderData( UINT const cbSize, D3D12_GPU_VIRTUAL_ADDRESS& outConstBufferOffset, void*& outConstBufferPtr );
	void SetProjectionMatrix( Matrix4x4 const& projection ) { m_mainCamera.m_viewToScreen = projection; m_mainCamera.m_viewToScreen.Inverse( m_mainCamera.m_screenToView ); }
	SView const& GetView() const { return m_mainCamera; }

public: //Getters/setters
	void SetWindowWidth(int const wndWidth) { m_wndWidth = wndWidth; }
	int GetWindowWidth() const { return m_wndWidth; }

	void SetWindowHeight(int const wndHeight) { m_wndHeight = wndHeight; }
	int GetWindowHeight() const { return m_wndHeight; }

	void SetHWND(HWND& hwnd) { m_hwnd = hwnd; }
	HWND SetHWND() const { return m_hwnd; }
};

extern CRender GRender;