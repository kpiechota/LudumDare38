#pragma once
#include "headers.h"

class CRender
{
private:
	enum { FRAME_NUM = 3 };

#ifdef _DEBUG
	ID3D12Debug*				m_debugController;
#endif

	ID3D12Device*				m_device;
	IDXGIFactory4*				m_factor;

	ID3D12Fence*				m_fence;
	HANDLE						m_fenceEvent;
	UINT						m_fenceValue;

	D3D12_VIEWPORT				m_viewport;
	D3D12_RECT					m_scissorRect;

	ID3D12CommandQueue*			m_mainCQ;
	ID3D12CommandAllocator*		m_mainCA;
	ID3D12GraphicsCommandList*	m_mainCL;

	IDXGISwapChain3*			m_swapChain;

	ID3D12DescriptorHeap*		m_renderTargetDH;
	ID3D12Resource*				m_rederTarget[FRAME_NUM];

	int							m_wndWidth;
	int							m_wndHeight;
	HWND						m_hwnd;
	UINT						m_renderTargetID;

	UINT						m_rtvDescriptorHandleIncrementSize;

private:
	void InitCommands();
	void InitSwapChain();
	void InitRenderTargets();
public:
	void Init();
	void DrawFrame();
	void Release();

public: //Getters\setters
	void SetWindowWidth(int const wndWidth) { m_wndWidth = wndWidth; }
	int GetWindowWidth() const { return m_wndWidth; }

	void SetWindowHeight(int const wndHeight) { m_wndHeight = wndHeight; }
	int GetWindowHeight() const { return m_wndHeight; }

	void SetHWND(HWND& hwnd) { m_hwnd = hwnd; }
	HWND SetHWND() const { return m_hwnd; }
};

extern CRender GRender;