#include "render.h"

CRender GRender;

void CRender::InitCommands()
{
	D3D12_COMMAND_QUEUE_DESC descCQ = {};
	descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	descCQ.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CheckResult(m_device->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_mainCQ)));
	m_mainCQ->SetName(L"MainCommandQueue");

	CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_mainCA)));
	m_mainCA->SetName(L"MainCommandAllocator");

	CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_mainCA, nullptr, IID_PPV_ARGS(&m_mainCL)));
	m_mainCL->SetName(L"MainCommandList");
	m_mainCL->Close();
}

void CRender::InitSwapChain()
{
	DXGI_SWAP_CHAIN_DESC descSwapChain = {};
	descSwapChain.BufferCount = FRAME_NUM;
	descSwapChain.BufferDesc.Width = m_wndWidth;
	descSwapChain.BufferDesc.Height = m_wndHeight;
	descSwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	descSwapChain.OutputWindow = m_hwnd;
	descSwapChain.SampleDesc.Count = 1;
	descSwapChain.Windowed = TRUE;

	IDXGISwapChain* swapChain;
	CheckResult(m_factor->CreateSwapChain(m_mainCQ, &descSwapChain, &swapChain));
	m_swapChain = (IDXGISwapChain3*)swapChain;
}

void CRender::InitRenderTargets()
{
	m_rtvDescriptorHandleIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_DESCRIPTOR_HEAP_DESC descDescHeap = {};
	descDescHeap.NumDescriptors = FRAME_NUM;
	descDescHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descDescHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	CheckResult(m_device->CreateDescriptorHeap(&descDescHeap, IID_PPV_ARGS(&m_renderTargetDH)));
	m_renderTargetDH->SetName(L"RenderTargetsDescriptorHeaps");

	D3D12_CPU_DESCRIPTOR_HANDLE descHandle = m_renderTargetDH->GetCPUDescriptorHandleForHeapStart();
	for (UINT rtvID = 0; rtvID < FRAME_NUM; ++rtvID)
	{
		CheckResult(m_swapChain->GetBuffer(rtvID, IID_PPV_ARGS(&m_rederTarget[rtvID])));
		m_device->CreateRenderTargetView(m_rederTarget[rtvID], nullptr, descHandle);
		std::wstring const rtvName = L"RenderTarget" + std::to_wstring(rtvID);
		m_rederTarget[rtvID]->SetName(rtvName.c_str());
		descHandle.ptr += m_rtvDescriptorHandleIncrementSize;
	}
	m_renderTargetID = 0;
}

void CRender::Init()
{
	m_viewport.MaxDepth = 1.f;
	m_viewport.MinDepth = 0.f;
	m_viewport.Width = m_wndWidth;
	m_viewport.Height = m_wndHeight;
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;

	m_scissorRect.right = (long)m_wndWidth;
	m_scissorRect.bottom = (long)m_wndHeight;
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;

	m_fenceValue = 0;

#ifdef _DEBUG
	CheckResult(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
	m_debugController->EnableDebugLayer();
#endif

	CheckResult(CreateDXGIFactory1(IID_PPV_ARGS(&m_factor)));
	CheckResult(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

	CheckResult(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	++m_fenceValue;
	m_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (!m_fenceEvent)
	{
		throw;
	}

	InitCommands();
	InitSwapChain();
	InitRenderTargets();
}

void CRender::DrawFrame()
{
	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	ID3D12GraphicsCommandList* commandList = m_mainCL;

	m_mainCA->Reset();
	commandList->Reset(m_mainCA, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDH = m_renderTargetDH->GetCPUDescriptorHandleForHeapStart();
	renderTargetDH.ptr += m_renderTargetID * m_rtvDescriptorHandleIncrementSize;

	D3D12_RESOURCE_BARRIER renderTargetBarrier = {};
	renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	renderTargetBarrier.Transition.pResource = m_rederTarget[m_renderTargetID];
	renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	renderTargetBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &renderTargetBarrier);

	commandList->OMSetRenderTargets(1, &renderTargetDH, true, nullptr);

	commandList->RSSetScissorRects(1, &m_scissorRect);
	commandList->RSSetViewports(1, &m_viewport);

	float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	commandList->ClearRenderTargetView(renderTargetDH, clearColor, 0, nullptr);

	renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

	commandList->ResourceBarrier(1, &renderTargetBarrier);

	CheckResult(commandList->Close());

	m_mainCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&commandList));

	CheckResult(m_swapChain->Present(0, 0));
	m_renderTargetID = (m_renderTargetID + 1) % FRAME_NUM;
}

void CRender::Release()
{
	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	m_mainCQ->Release();
	m_mainCA->Release();
	m_mainCL->Release();
	m_renderTargetDH->Release();
	for (UINT renderID = 0; renderID < FRAME_NUM; ++renderID)
	{
		m_rederTarget[renderID]->Release();
	}

	m_swapChain->Release();
	m_fence->Release();
	m_factor->Release();
	m_device->Release();

#ifdef _DEBUG
	m_debugController->Release();
#endif
}
