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

extern std::vector< SRenderObject > GRenderObjects[RL_MAX];
extern Matrix3x3 GScreenMatrix;
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
	CheckResult(m_mainCL->Close());

	descCQ.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	descCQ.Type = D3D12_COMMAND_LIST_TYPE_COPY;
	CheckResult(m_device->CreateCommandQueue(&descCQ, IID_PPV_ARGS(&m_copyCQ)));
	m_copyCQ->SetName(L"CopyCommandQueue");

	CheckResult(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_copyCA)));
	m_copyCA->SetName(L"CopyCommandAllocator");

	CheckResult(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_copyCA, nullptr, IID_PPV_ARGS(&m_copyCL)));
	m_copyCL->SetName(L"CopyCommandList");
	CheckResult(m_copyCL->Close());
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
	D3D12_DESCRIPTOR_RANGE descriptorRange[] =
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND }
	};

	D3D12_ROOT_PARAMETER rootParameters[2];

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = { 1, descriptorRange };
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC samplers[] =
	{
		{
		/*Filter*/				D3D12_FILTER_MIN_MAG_MIP_POINT
		/*AddressU*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressV*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressW*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*MipLODBias*/			,0
		/*MaxAnisotropy*/		,0
		/*ComparisonFunc*/		,D3D12_COMPARISON_FUNC_NEVER
		/*BorderColor*/			,D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK
		/*MinLOD*/				,0.f
		/*MaxLOD*/				,D3D12_FLOAT32_MAX
		/*ShaderRegister*/		,0
		/*RegisterSpace*/		,0
		/*ShaderVisibility*/	,D3D12_SHADER_VISIBILITY_PIXEL
		}
	};

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = 2;
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = 1;
	descRootSignature.pStaticSamplers = samplers;
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

	commandList->SetGraphicsRootSignature(m_mainRS);
	commandList->SetDescriptorHeaps(1, &m_texturesDH);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	D3D12_GPU_DESCRIPTOR_HANDLE texturesHandle = m_texturesDH->GetGPUDescriptorHandleForHeapStart();

	unsigned int renderObjectID = 0;
	for (unsigned int layerID = 0; layerID < RL_MAX; ++layerID)
	{
		unsigned int const objectsNum = GRenderObjects[layerID].size();
		for (unsigned int objectID = 0; objectID < objectsNum; ++objectID)
		{
			CBObject& cbObject = m_frameData[m_frameID].m_pResourceData[renderObjectID];
			SRenderObject const& gameObject = GRenderObjects[layerID][objectID];
			cbObject.m_objectToScreen = Mul(GScreenMatrix, Matrix3x3::GetTranslateRotationSize(gameObject.m_positionWS, gameObject.m_rotation, gameObject.m_size));
			cbObject.m_colorScale = gameObject.m_colorScale;
			cbObject.m_offset = gameObject.m_offset;

			commandList->SetGraphicsRootConstantBufferView(0, m_frameData[m_frameID].m_frameResource->GetGPUVirtualAddress() + (D3D12_GPU_VIRTUAL_ADDRESS)(renderObjectID * sizeof(CBObject)));
			D3D12_GPU_DESCRIPTOR_HANDLE texture = texturesHandle;
			texture.ptr += m_srvDescriptorHandleIncrementSize * gameObject.m_texutreID;

			commandList->SetGraphicsRootDescriptorTable(1, texture);
			commandList->DrawInstanced(4, 1, 0, 0);
			++renderObjectID;
		}
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

	m_copyCQ->Release();
	m_copyCA->Release();
	m_copyCL->Release();

	m_mainCQ->Release();
	m_mainCA->Release();
	m_mainCL->Release();

	m_mainPSO->Release();
	m_mainRS->Release();
	m_renderTargetDH->Release();
	m_texturesDH->Release();
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

	unsigned int const texutreNum = m_texturesResources.size();
	for (unsigned int texutreID = 0; texutreID < texutreNum; ++texutreID)
	{
		m_texturesResources[texutreID]->Release();
	}

	m_swapChain->Release();
	m_fence->Release();
	m_factor->Release();
	m_device->Release();

#ifdef _DEBUG
	m_debugController->Release();
#endif
}

void CRender::BeginLoadResources(unsigned int const textureNum)
{
	m_srvDescriptorHandleIncrementSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC textureHeapDesc = {};
	textureHeapDesc.NumDescriptors = textureNum;
	textureHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	textureHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	CheckResult(m_device->CreateDescriptorHeap(&textureHeapDesc, IID_PPV_ARGS(&m_texturesDH)));

	m_copyCA->Reset();
	m_copyCL->Reset(m_copyCA, nullptr);
}

void CRender::LoadResource(STexture const& texture)
{
	D3D12_RESOURCE_DESC descTexture = {};
	descTexture.DepthOrArraySize = 1;
	descTexture.SampleDesc.Count = 1;
	descTexture.Flags = D3D12_RESOURCE_FLAG_NONE;
	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	descTexture.MipLevels = 1;
	descTexture.Width = texture.m_width;
	descTexture.Height = texture.m_height;
	descTexture.Format = texture.m_format;

	ID3D12Resource* textureRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textureRes)));
	m_texturesResources.push_back(textureRes);


	UINT64 bufferSize;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprints;
	UINT numRows = 0;
	UINT64 rowPitches = 0;
	m_device->GetCopyableFootprints(&descTexture, 0, 1, 0, &footprints, &numRows, &rowPitches, &bufferSize);

	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descTexture.Format = DXGI_FORMAT_UNKNOWN;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descTexture.MipLevels = 1;
	descTexture.Height = 1;
	descTexture.Width = bufferSize;

	ID3D12Resource* textureUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadRes)));
	m_texturesUploadResources.push_back(textureUploadRes);

	void* pGPU;
	textureUploadRes->Map(0, nullptr, &pGPU);

	for (UINT rowID = 0; rowID < numRows; ++rowID)
	{
		memcpy((BYTE*)pGPU + footprints.Offset + rowID * footprints.Footprint.RowPitch, texture.m_data + rowID * rowPitches, rowPitches);
	}

	D3D12_TEXTURE_COPY_LOCATION dst;
	dst.pResource = textureRes;
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION src;
	src.pResource = textureUploadRes;
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = footprints;

	m_copyCL->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	textureUploadRes->Unmap(0, nullptr);
}

void CRender::EndLoadResources()
{
	m_copyCL->Close();
	m_copyCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&m_copyCL));
}

void CRender::WaitForResourcesLoad()
{
	std::vector< D3D12_RESOURCE_BARRIER > resourcesBarruers;

	m_mainCA->Reset();
	m_mainCL->Reset(m_mainCA, nullptr);

	unsigned int const texutreNum = m_texturesResources.size();
	for (unsigned int texutreID = 0; texutreID < texutreNum; ++texutreID)
	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_texturesResources[texutreID];
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		resourcesBarruers.push_back(barrier);
	}

	m_mainCL->ResourceBarrier(resourcesBarruers.size(), resourcesBarruers.data());
	m_mainCL->Close();

	CheckResult(m_copyCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	m_mainCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&m_mainCL));

	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	D3D12_CPU_DESCRIPTOR_HANDLE textureDH = m_texturesDH->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	for (unsigned int texutreID = 0; texutreID < texutreNum; ++texutreID)
	{
		srvDesc.Format = m_texturesResources[texutreID]->GetDesc().Format;
		m_device->CreateShaderResourceView(m_texturesResources[texutreID], &srvDesc, textureDH);

		textureDH.ptr += m_srvDescriptorHandleIncrementSize;

		m_texturesUploadResources[texutreID]->Release();
	}

	m_texturesUploadResources.clear();
	m_texturesUploadResources.shrink_to_fit();
}
