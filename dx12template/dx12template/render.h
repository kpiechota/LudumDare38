#pragma once

#include "headers.h"
#include "renderConstant.h"
#include "vertexFormats.h"
#include "geometry.h"
struct SRenderFrameData
{
	ID3D12CommandAllocator*		m_frameCA;
	ID3D12GraphicsCommandList*	m_frameCL;
	ID3D12Resource*				m_frameResource;
	Byte*						m_pResourceData;
};

class CRender
{
private:
	enum
	{
		FRAME_NUM = 3,
		MAX_OBJECTS = 2048
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

	ID3D12CommandQueue*				m_mainCQ;
	ID3D12CommandAllocator*			m_mainCA;
	ID3D12GraphicsCommandList*		m_mainCL;

	IDXGISwapChain3*				m_swapChain;

	ID3D12DescriptorHeap*			m_renderTargetDH;
	ID3D12Resource*					m_rederTarget[FRAME_NUM];

	SRenderFrameData				m_frameData[FRAME_NUM];

	ID3D12RootSignature*			m_mainRS;
	ID3D12PipelineState*			m_shaders[ST_MAX];

	ID3D12PipelineState*			m_bakeShader;

	ID3D12DescriptorHeap*			m_texturesDH;
	ID3D12Resource*					m_bakeTexture;

	std::vector< SGeometry >		m_geometryResources;
	std::vector< ID3D12Resource* >	m_texturesResources;
	std::vector< ID3D12Resource* >	m_texturesUploadResources;

	int								m_wndWidth;
	int								m_wndHeight;
	HWND							m_hwnd;
	UINT							m_frameID;

	UINT							m_rtvDescriptorHandleIncrementSize;
	UINT							m_srvDescriptorHandleIncrementSize;

	UINT							m_constBufferOffset;

private:
	void InitCommands();
	void InitFrameData();
	void InitSwapChain();
	void InitRenderTargets();

	void InitRootSignatures();

	inline void LoadShader(LPCWSTR pFileName, D3D_SHADER_MACRO const* pDefines, LPCSTR pEmtryPoint, LPCSTR pTarget, ID3DBlob** ppCode) const;
	void InitShaders();

	inline void Bake();

public:
	void Init();
	void DrawFrame();
	void Release();

	ID3D12Device* GetDevice() { return m_device; }

	Byte AddGeometry( SGeometry const& geometry );
	SGeometry& GetGeometry( Byte const geometryID );
	void ReleaseGeometry( Byte const geometryID );

	void BeginLoadResources(unsigned int const textureNum);
	void LoadResource(STexture const& texture);
	void EndLoadResources();
	void WaitForResourcesLoad();

	void ClearBaked();

	void GetRenderData( UINT const cbSize, D3D12_GPU_VIRTUAL_ADDRESS& outConstBufferOffset, void*& outConstBufferPtr );

public: //Getters/setters
	void SetWindowWidth(int const wndWidth) { m_wndWidth = wndWidth; }
	int GetWindowWidth() const { return m_wndWidth; }

	void SetWindowHeight(int const wndHeight) { m_wndHeight = wndHeight; }
	int GetWindowHeight() const { return m_wndHeight; }

	void SetHWND(HWND& hwnd) { m_hwnd = hwnd; }
	HWND SetHWND() const { return m_hwnd; }
};

extern CRender GRender;