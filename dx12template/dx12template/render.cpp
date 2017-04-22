#include "render.h"

D3D12_HEAP_PROPERTIES const GHeapPropertiesGPUOnly =
{
	/*Type*/					D3D12_HEAP_TYPE_CUSTOM
	/*CPUPageProperty*/			,D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE
	/*MemoryPoolPreference*/	,D3D12_MEMORY_POOL_L1
	/*CreationNodeMask*/		,1
	/*VisibleNodeMask*/			,1
};

D3D12_HEAP_PROPERTIES const GHeapPropertiesDefault =
{
	/*Type*/					D3D12_HEAP_TYPE_DEFAULT
	/*CPUPageProperty*/			,D3D12_CPU_PAGE_PROPERTY_UNKNOWN
	/*MemoryPoolPreference*/	,D3D12_MEMORY_POOL_UNKNOWN
	/*CreationNodeMask*/		,1
	/*VisibleNodeMask*/			,1
};
D3D12_HEAP_PROPERTIES const GHeapPropertiesUpload =
{
	/*Type*/					D3D12_HEAP_TYPE_UPLOAD
	/*CPUPageProperty*/			,D3D12_CPU_PAGE_PROPERTY_UNKNOWN
	/*MemoryPoolPreference*/	,D3D12_MEMORY_POOL_UNKNOWN
	/*CreationNodeMask*/		,1
	/*VisibleNodeMask*/			,1
};

extern std::vector< SGameObject > GGameObjects;
extern Matrix3x3 GScreenMatrix;
CRender GRender;

void CRender::InitCommands()
{
	D3D12_COMMAND_QUEUE_DESC descCQ = {};
	descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	descCQ.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	CheckResult(m_device->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_mainCQ)));
	m_mainCQ->SetName(L"MainCommandQueue");
}

void CRender::InitFrameData()
{
	D3D12_RESOURCE_DESC descResource = {};
	descResource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descResource.Alignment = 0;
	descResource.Height = 1;
	descResource.DepthOrArraySize = 1;
	descResource.MipLevels = 1;
	descResource.Format = DXGI_FORMAT_UNKNOWN;
	descResource.SampleDesc.Count = 1;
	descResource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descResource.Flags = D3D12_RESOURCE_FLAG_NONE;
	descResource.Width = sizeof(CBObject) * MAX_OBJECTS;

	for (UINT frameID = 0; frameID < FRAME_NUM; ++frameID)
	{
		SRenderFrameData& frameData = m_frameData[frameID];

		CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frameData.m_frameCA)));
		frameData.m_frameCA->SetName(L"FrameCommandAllocator");

		CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameData.m_frameCA, nullptr, IID_PPV_ARGS(&frameData.m_frameCL)));
		frameData.m_frameCL->SetName(L"FrameCommandList");
		CheckResult(frameData.m_frameCL->Close());

		CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descResource, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&frameData.m_frameResource)));
		frameData.m_frameResource->SetName(L"FrameResource");

		CheckResult(frameData.m_frameResource->Map(0, nullptr, (void**)(&frameData.m_pResourceData)));
	}
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
	m_frameID = 0;
}

void CRender::InitRootSignatures()
{
	D3D12_ROOT_PARAMETER rootParameters[1];

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = 1;
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = 0;
	descRootSignature.pStaticSamplers = nullptr;
	descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	ID3DBlob* signature;
	CheckResult(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr));
	CheckResult(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_mainRS)));
	signature->Release();
}

inline void CRender::LoadShader(LPCWSTR pFileName, D3D_SHADER_MACRO const* pDefines, LPCSTR pEmtryPoint, LPCSTR pTarget, ID3DBlob** ppCode) const
{
	ID3DBlob* error;
	HRESULT result = D3DCompileFromFile(pFileName, pDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEmtryPoint, pTarget, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, ppCode, &error);
	if (FAILED(result))
	{
		if (error != nullptr)
		{
			OutputDebugStringA((char*)error->GetBufferPointer());
			__debugbreak();
			error->Release();
		}
	}
}

void CRender::InitShaders()
{
	D3D12_RASTERIZER_DESC rasterizerState =
	{
		/*FillMode*/					D3D12_FILL_MODE_SOLID
		/*CullMode*/					,D3D12_CULL_MODE_BACK
		/*FrontCounterClockwise*/		,FALSE
		/*DepthBias*/					,D3D12_DEFAULT_DEPTH_BIAS
		/*DepthBiasClamp*/				,D3D12_DEFAULT_DEPTH_BIAS_CLAMP
		/*SlopeScaledDepthBias*/		,D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS
		/*DepthClipEnable*/				,FALSE
		/*MultisampleEnable*/			,FALSE
		/*AntialiasedLineEnable*/		,FALSE
		/*ForcedSampleCount*/			,0
		/*ConservativeRaster*/			,D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};
	static D3D12_BLEND_DESC const blendState =
	{
		/*AlphaToCoverageEnable*/					FALSE
		/*IndependentBlendEnable*/					,FALSE
		/*RenderTarget[0].BlendEnable*/				,FALSE
		/*RenderTarget[0].LogicOpEnable*/			,FALSE
		/*RenderTarget[0].SrcBlend*/				,D3D12_BLEND_ONE
		/*RenderTarget[0].DestBlend*/				,D3D12_BLEND_ZERO
		/*RenderTarget[0].BlendOp*/					,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].SrcBlendAlpha*/			,D3D12_BLEND_ONE
		/*RenderTarget[0].DestBlendAlpha*/			,D3D12_BLEND_ZERO
		/*RenderTarget[0].BlendOpAlpha*/			,D3D12_BLEND_OP_ADD
		/*RenderTarget[0].LogicOp*/					,D3D12_LOGIC_OP_NOOP
		/*RenderTarget[0].RenderTargetWriteMask*/	,D3D12_COLOR_WRITE_ENABLE_ALL
	};
	static D3D12_DEPTH_STENCIL_DESC const depthStencilState =
	{
		/*DepthEnable*/							FALSE
		/*DepthWriteMask*/						,D3D12_DEPTH_WRITE_MASK_ALL
		/*DepthFunc*/							,D3D12_COMPARISON_FUNC_LESS
		/*StencilEnable*/						,FALSE
		/*StencilReadMask*/						,D3D12_DEFAULT_STENCIL_READ_MASK
		/*StencilWriteMask*/					,D3D12_DEFAULT_STENCIL_WRITE_MASK
		/*FrontFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilDepthFailOp*/		,D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
		/*FrontFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
		/*BackFace.StencilFailOp*/				,{ D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilDepthFailOp*/			,D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilPassOp*/				,D3D12_STENCIL_OP_REPLACE
		/*BackFace.StencilFunc*/				,D3D12_COMPARISON_FUNC_ALWAYS }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC descPSO = {};
	descPSO.BlendState = blendState;
	descPSO.DepthStencilState = depthStencilState;
	descPSO.RasterizerState = rasterizerState;
	descPSO.InputLayout = { nullptr, 0 };
	descPSO.SampleDesc.Count = 1;
	descPSO.SampleMask = UINT_MAX;
	descPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPSO.NumRenderTargets = 1;
	descPSO.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
	descPSO.pRootSignature = m_mainRS;

	ID3DBlob* vsShader;
	LoadShader(L"../shaders/objectDraw.hlsl", nullptr, "vsMain", "vs_5_1", &vsShader);
	descPSO.VS = { vsShader->GetBufferPointer(), vsShader->GetBufferSize() };

	ID3DBlob* psShader;
	LoadShader(L"../shaders/objectDraw.hlsl", nullptr, "psMain", "ps_5_1", &psShader);
	descPSO.PS = { psShader->GetBufferPointer(), psShader->GetBufferSize() };

	CheckResult(m_device->CreateGraphicsPipelineState(&descPSO, IID_PPV_ARGS(&m_mainPSO)));

	vsShader->Release();
	psShader->Release();
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

//#ifdef _DEBUG
	CheckResult(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController)));
	m_debugController->EnableDebugLayer();
//#endif

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
	InitFrameData();
	InitSwapChain();
	InitRenderTargets();
	InitRootSignatures();
	InitShaders();
}

void CRender::DrawFrame()
{
	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	ID3D12GraphicsCommandList* commandList = m_frameData[m_frameID].m_frameCL;

	m_frameData[m_frameID].m_frameCA->Reset();
	commandList->Reset(m_frameData[m_frameID].m_frameCA, m_mainPSO);

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDH = m_renderTargetDH->GetCPUDescriptorHandleForHeapStart();
	renderTargetDH.ptr += m_frameID * m_rtvDescriptorHandleIncrementSize;

	D3D12_RESOURCE_BARRIER renderTargetBarrier = {};
	renderTargetBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	renderTargetBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	renderTargetBarrier.Transition.pResource = m_rederTarget[m_frameID];
	renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	renderTargetBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier(1, &renderTargetBarrier);

	commandList->OMSetRenderTargets(1, &renderTargetDH, true, nullptr);

	commandList->RSSetScissorRects(1, &m_scissorRect);
	commandList->RSSetViewports(1, &m_viewport);

	float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	commandList->ClearRenderTargetView(renderTargetDH, clearColor, 0, nullptr);

	commandList->SetGraphicsRootSignature(m_mainRS);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	unsigned int const objectsNum = GGameObjects.size();
	CBObject* currCBObject = m_frameData[m_frameID].m_pResourceData;
	for (unsigned int objectID = 0; objectID < objectsNum; ++objectID)
	{
		SGameObject const& gameObject = GGameObjects[objectID];
		currCBObject->m_objectToScreen = Mul(Matrix3x3::GetTranslateRotationSize(gameObject.m_positionWS, gameObject.m_rotation, gameObject.m_size), GScreenMatrix);

		commandList->SetGraphicsRootConstantBufferView(0, m_frameData[m_frameID].m_frameResource->GetGPUVirtualAddress() + (D3D12_GPU_VIRTUAL_ADDRESS)(objectID * sizeof(CBObject)));
		commandList->DrawInstanced(4, 1, 0, 0);
	}

	renderTargetBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	renderTargetBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

	commandList->ResourceBarrier(1, &renderTargetBarrier);

	CheckResult(commandList->Close());

	m_mainCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&commandList));

	CheckResult(m_swapChain->Present(0, 0));
	m_frameID = (m_frameID + 1) % FRAME_NUM;
}

void CRender::Release()
{
	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	m_mainCQ->Release();
	m_mainPSO->Release();
	m_renderTargetDH->Release();
	for (UINT frameID = 0; frameID < FRAME_NUM; ++frameID)
	{
		m_rederTarget[frameID]->Release();

		SRenderFrameData& frameData = m_frameData[frameID];
		frameData.m_frameCA->Release();
		frameData.m_frameCL->Release();
		frameData.m_frameResource->Unmap(0, nullptr);
		frameData.m_frameResource->Release();
		frameData.m_pResourceData = nullptr;
	}

	m_swapChain->Release();
	m_fence->Release();
	m_factor->Release();
	m_device->Release();

//#ifdef _DEBUG
	m_debugController->Release();
//#endif
}
