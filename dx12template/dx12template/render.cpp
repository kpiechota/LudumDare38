#include "render.h"
#include "timer.h"
#include <math.h>

extern std::vector< SRenderData > GRenderObjects[RL_MAX];
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
	descResource.Width = 256 * MAX_OBJECTS;

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
	descSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
	ResizeDescriptorHeap( m_renderTargetDH, FRAME_NUM + GBB_MAX );
	UINT rtvID = 0;
	for (; rtvID < FRAME_NUM; ++rtvID)
	{
		CheckResult(m_swapChain->GetBuffer(rtvID, IID_PPV_ARGS(&m_rederTarget[rtvID])));
		m_device->CreateRenderTargetView(m_rederTarget[rtvID], nullptr, m_renderTargetDH.GetCPUDescriptor(rtvID));
		std::wstring const rtvName = L"RenderTarget" + std::to_wstring(rtvID);
		m_rederTarget[rtvID]->SetName(rtvName.c_str());
	}
	m_frameID = 0;

	D3D12_RESOURCE_DESC gbufferDesc = {};
	gbufferDesc.DepthOrArraySize = 1;
	gbufferDesc.SampleDesc.Count = 1;
	gbufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	gbufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	gbufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	gbufferDesc.MipLevels = 1;
	gbufferDesc.Width = m_wndWidth;
	gbufferDesc.Height = m_wndHeight;

	D3D12_RENDER_TARGET_VIEW_DESC gbufferViewDesc = {};
	gbufferViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE gbufferClearValue;
	gbufferClearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gbufferClearValue.Color[ 0 ] = 0.f;
	gbufferClearValue.Color[ 1 ] = 0.f;
	gbufferClearValue.Color[ 2 ] = 0.f;
	gbufferClearValue.Color[ 3 ] = 0.f;

	gbufferViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gbufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_gbufferDescriptorsOffsets[ GBB_DIFFUSE ].m_rtvOffset = rtvID;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &gbufferDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &gbufferClearValue, IID_PPV_ARGS( &m_gbufferBuffers[ GBB_DIFFUSE ] ) ) );
	m_device->CreateRenderTargetView( m_gbufferBuffers[ GBB_DIFFUSE ], &gbufferViewDesc, m_renderTargetDH.GetCPUDescriptor(rtvID) );
	++rtvID;

	gbufferClearValue.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	gbufferViewDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	gbufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	m_gbufferDescriptorsOffsets[ GBB_NORMAL ].m_rtvOffset = rtvID;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &gbufferDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &gbufferClearValue, IID_PPV_ARGS( &m_gbufferBuffers[ GBB_NORMAL ] ) ) );
	m_device->CreateRenderTargetView( m_gbufferBuffers[ GBB_NORMAL ], &gbufferViewDesc, m_renderTargetDH.GetCPUDescriptor(rtvID) );
	++rtvID;

	ResizeDescriptorHeap( m_depthBuffertDH, 1 );

	D3D12_RESOURCE_DESC depthDesc = {};
	depthDesc.DepthOrArraySize = 1;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthDesc.MipLevels = 1;
	depthDesc.Width = m_wndWidth;
	depthDesc.Height = m_wndHeight;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	D3D12_CLEAR_VALUE depthClearValue;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthClearValue.DepthStencil.Depth = 1.f;
	depthClearValue.DepthStencil.Stencil = 0;
	m_gbufferDescriptorsOffsets[ GBB_DEPTH ].m_rtvOffset = 0;
	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &depthDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue, IID_PPV_ARGS( &m_gbufferBuffers[ GBB_DEPTH ] ) ) );

	D3D12_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
	depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Flags = D3D12_DSV_FLAG_NONE;
	depthViewDesc.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView( m_gbufferBuffers[ GBB_DEPTH ], &depthViewDesc, m_depthBuffertDH.GetCPUDescriptor( 0 ) );
}

void CRender::InitDescriptorHeaps()
{
	for ( UINT dhIncSize = 0; dhIncSize < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++dhIncSize )
	{
		GDescriptorHandleIncrementSize[ dhIncSize ] = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE(dhIncSize));
	}

	m_renderTargetDH.m_pDescriptorHeap = nullptr;
	m_renderTargetDH.m_descriptorsNum = 0;
	m_renderTargetDH.m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_renderTargetDH.m_type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	m_depthBuffertDH.m_pDescriptorHeap = nullptr;
	m_depthBuffertDH.m_descriptorsNum = 0;
	m_depthBuffertDH.m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_depthBuffertDH.m_type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	m_texturesDH.m_pDescriptorHeap = nullptr;
	m_texturesDH.m_descriptorsNum = 0;
	m_texturesDH.m_flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	m_texturesDH.m_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
}

void CRender::InitRootSignatures()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[] =
	{
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
		{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND },
	};
	D3D12_ROOT_PARAMETER rootParameters[4];

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = {0, 0};
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = { 1, &descriptorRange[ 0 ] };
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].DescriptorTable = { 1, &descriptorRange[ 1 ] };
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].DescriptorTable = { 1, &descriptorRange[ 2 ] };
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC const samplers[] =
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
		},
		{
		/*Filter*/				D3D12_FILTER_MIN_MAG_MIP_LINEAR
		/*AddressU*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressV*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*AddressW*/			,D3D12_TEXTURE_ADDRESS_MODE_WRAP
		/*MipLODBias*/			,0
		/*MaxAnisotropy*/		,0
		/*ComparisonFunc*/		,D3D12_COMPARISON_FUNC_NEVER
		/*BorderColor*/			,D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK
		/*MinLOD*/				,0.f
		/*MaxLOD*/				,D3D12_FLOAT32_MAX
		/*ShaderRegister*/		,1
		/*RegisterSpace*/		,0
		/*ShaderVisibility*/	,D3D12_SHADER_VISIBILITY_PIXEL
		}
	};

	D3D12_ROOT_SIGNATURE_DESC descRootSignature;
	descRootSignature.NumParameters = 4;
	descRootSignature.pParameters = rootParameters;
	descRootSignature.NumStaticSamplers = ARRAYSIZE(samplers);
	descRootSignature.pStaticSamplers = samplers;
	descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	ID3DBlob* signature;
	ID3DBlob* error = nullptr;
	CheckResult(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error), error);
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
			error->Release();
			ASSERT( false );
		}
	}
}

void CRender::InitShader( LPCWSTR pFileName, ID3D12PipelineState*& pso, D3D12_INPUT_ELEMENT_DESC const* vertexElements, UINT const vertexElementsNum, UINT const renderTargetNum, DXGI_FORMAT const * renderTargetFormats, ERenderTargetBlendStates const * renderTargetBlendStates, EDepthStencilStates const depthStencilState, ERasterizerStates const rasterizationState )
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC descPSO = {};
	descPSO.BlendState.AlphaToCoverageEnable = FALSE;
	descPSO.BlendState.IndependentBlendEnable = FALSE;
	descPSO.DepthStencilState = GDepthStencilStates[ depthStencilState ];
	descPSO.RasterizerState = GRasterizerStates[ rasterizationState ];
	descPSO.InputLayout = { vertexElements, vertexElementsNum };
	descPSO.SampleDesc.Count = 1;
	descPSO.SampleMask = UINT_MAX;
	descPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	descPSO.NumRenderTargets = renderTargetNum;
	descPSO.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descPSO.pRootSignature = m_mainRS;

	for ( UINT renderTargetID = 0; renderTargetID < renderTargetNum; ++renderTargetID )
	{
		descPSO.BlendState.RenderTarget[ renderTargetID ] = GRenderTargetBlendStates[ renderTargetBlendStates[ renderTargetID ] ];
		descPSO.RTVFormats[ renderTargetID ] = renderTargetFormats[ renderTargetID ];
	}

	ID3DBlob* vsShader;
	LoadShader(pFileName, nullptr, "vsMain", "vs_5_1", &vsShader);
	descPSO.VS = { vsShader->GetBufferPointer(), vsShader->GetBufferSize() };

	ID3DBlob* psShader;
	LoadShader(pFileName, nullptr, "psMain", "ps_5_1", &psShader);
	descPSO.PS = { psShader->GetBufferPointer(), psShader->GetBufferSize() };

	CheckResult(m_device->CreateGraphicsPipelineState(&descPSO, IID_PPV_ARGS(&pso)));

	vsShader->Release();
	psShader->Release();
}

void CRender::InitShaders()
{
	DXGI_FORMAT const objectDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R10G10B10A2_UNORM };
	ERenderTargetBlendStates const objectDrawRenderTargetsBlend[] = { ERTBS_Disabled, ERTBS_Disabled };
	InitShader( L"../shaders/objectDraw.hlsl", m_shaders[ ST_OBJECT_DRAW ], SSimpleObjectVertexFormat::desc, SSimpleObjectVertexFormat::descNum, 2, objectDrawRenderTargets, objectDrawRenderTargetsBlend, EDSS_DepthEnable );

	DXGI_FORMAT const sdfDrawRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const sdfDrawRenderTargetsBlend[] = { ERTBS_AlphaBlend };
	InitShader( L"../shaders/sdfDraw.hlsl", m_shaders[ ST_SDF_DRAW ], SPosUvVertexFormat::desc, SPosUvVertexFormat::descNum, 1, sdfDrawRenderTargets, sdfDrawRenderTargetsBlend );

	DXGI_FORMAT const simpleLightRenderTargets[] = { DXGI_FORMAT_R8G8B8A8_UNORM };
	ERenderTargetBlendStates const simpleLightRenderTargetsBlend[] = { ERTBS_Disabled };
	InitShader( L"../shaders/simpleLight.hlsl", m_shaderSimpleLight, SPosVertexFormat::desc, SPosVertexFormat::descNum, 1, simpleLightRenderTargets, simpleLightRenderTargetsBlend );
}

void CRender::Init()
{
	m_constBufferOffset = 0;

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

	InitDescriptorHeaps();
	InitCommands();
	InitFrameData();
	InitSwapChain();
	InitRenderTargets();
	InitRootSignatures();
	InitShaders();

	GTextRenderManager.Init();
}

void CRender::DrawLayer( ID3D12GraphicsCommandList* commandList, ERenderLayer const layerID )
{
	D3D_PRIMITIVE_TOPOLOGY currentTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	Byte currentShader = ST_MAX;
	Byte currentGeometry = UINT8_MAX;
	Byte currentTexture[ SRenderData::MAX_TEXTURES_NUM ];
	bool geometryUseIndices = false;
	memset( currentTexture, UINT8_MAX, sizeof( currentTexture ) );

	D3D12_GPU_VIRTUAL_ADDRESS const constBufferStart = m_frameData[ m_frameID ].m_frameResource->GetGPUVirtualAddress();
	unsigned int const objectsNum = GRenderObjects[layerID].size();
	for (unsigned int objectID = 0; objectID < objectsNum; ++objectID)
	{
		SRenderData const& gameObject = GRenderObjects[layerID][objectID];
		commandList->SetGraphicsRootConstantBufferView(0, constBufferStart + gameObject.m_cbOffset );

		for ( UINT texture = 0; texture < ARRAYSIZE( currentTexture ); ++texture )
		{
			if ( currentTexture[ texture ] != gameObject.m_textureID[ texture ] )
			{
				if ( gameObject.m_textureID[ texture ] != UINT8_MAX )
				{
					commandList->SetGraphicsRootDescriptorTable( texture + 1, m_texturesDH.GetGPUDescriptor( gameObject.m_textureID[ texture ] ) );
				}
				currentTexture[ texture ] = gameObject.m_textureID[ texture ];
			}
		}

		if ( currentTopology != gameObject.m_topology )
		{
			commandList->IASetPrimitiveTopology( gameObject.m_topology );
			currentTopology = gameObject.m_topology;
		}

		if (currentShader != gameObject.m_shaderID)
		{
			commandList->SetPipelineState(m_shaders[gameObject.m_shaderID]);
			currentShader = gameObject.m_shaderID;
		}

		if ( currentGeometry != gameObject.m_geometryID )
		{
			currentGeometry = gameObject.m_geometryID;
			SGeometry const& geometry = m_geometryResources[ currentGeometry ];

			geometryUseIndices = false;
			if ( geometry.m_indicesRes )
			{
				commandList->IASetIndexBuffer( &geometry.m_indexBufferView );
				geometryUseIndices = true;
			}

			if ( geometry.m_vertexRes )
			{
				commandList->IASetVertexBuffers( 0, 1, &geometry.m_vertexBufferView );
			}
		}


		if ( geometryUseIndices )
		{
			commandList->DrawIndexedInstanced( gameObject.m_dataNum, 1, gameObject.m_indicesStart, gameObject.m_verticesStart, 0 );
		}
		else
		{
			commandList->DrawInstanced(gameObject.m_dataNum, 1, gameObject.m_verticesStart, 0);
		}
	}
}

void CRender::DrawFrame()
{
	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	

	WaitForSingleObject(m_fenceEvent, INFINITE);

	GDynamicGeometryManager.PreDraw();
	ID3D12GraphicsCommandList* commandList = m_frameData[m_frameID].m_frameCL;

	m_frameData[m_frameID].m_frameCA->Reset();
	commandList->Reset( m_frameData[ m_frameID ].m_frameCA, nullptr );

	commandList->RSSetScissorRects(1, &m_scissorRect);
	commandList->RSSetViewports(1, &m_viewport);
	commandList->SetDescriptorHeaps(1, &m_texturesDH.m_pDescriptorHeap);
	commandList->SetGraphicsRootSignature(m_mainRS);

	D3D12_RESOURCE_BARRIER barriers[ 2 ];
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[0].Transition.pResource = m_rederTarget[m_frameID];
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barriers[1].Transition.pResource = m_gbufferBuffers[ GBB_DIFFUSE ];
	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	float const clearRT[] = { 0.f, 0.f, 0.f, 0.f };
	
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetDH = m_renderTargetDH.GetCPUDescriptor( m_gbufferDescriptorsOffsets[ GBB_DIFFUSE ].m_rtvOffset );
	D3D12_CPU_DESCRIPTOR_HANDLE depthBufferDH = m_depthBuffertDH.GetCPUDescriptor( 0 );
	commandList->OMSetRenderTargets(2, &renderTargetDH, true, &depthBufferDH);

	commandList->ResourceBarrier(2, barriers);
	commandList->ClearDepthStencilView( depthBufferDH, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr );
	commandList->ClearRenderTargetView( renderTargetDH, clearRT, 0, nullptr );
	commandList->ClearRenderTargetView( m_renderTargetDH.GetCPUDescriptor( m_gbufferDescriptorsOffsets[ GBB_NORMAL ].m_rtvOffset), clearRT, 0, nullptr );
	DrawLayer( commandList, RL_OPAQUE );


	renderTargetDH = m_renderTargetDH.GetCPUDescriptor( m_frameID );
	commandList->OMSetRenderTargets(1, &renderTargetDH, true, nullptr);
	commandList->ClearRenderTargetView( renderTargetDH, clearRT, 0, nullptr );

	barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList->ResourceBarrier(1, &barriers[1]);

	commandList->SetPipelineState( m_shaderSimpleLight );
	commandList->SetGraphicsRootDescriptorTable( 1, m_texturesDH.GetGPUDescriptor( m_gbufferDescriptorsOffsets[ GBB_DIFFUSE ].m_srvOffset ) );
	commandList->SetGraphicsRootDescriptorTable( 2, m_texturesDH.GetGPUDescriptor( m_gbufferDescriptorsOffsets[ GBB_NORMAL ].m_srvOffset ) );
	commandList->SetGraphicsRootDescriptorTable( 3, m_texturesDH.GetGPUDescriptor( m_gbufferDescriptorsOffsets[ GBB_DEPTH ].m_srvOffset ) );

	extern CTimer GTimer;

	CBSimpleLight* simpleLightCB;
	D3D12_GPU_VIRTUAL_ADDRESS simpleLightConstBufferOffset;
	GetRenderData( sizeof( CBSimpleLight ), simpleLightConstBufferOffset, reinterpret_cast< void*& >( simpleLightCB ) );
	simpleLightCB->m_viewToWorld = m_mainCamera.m_viewToWorld;
	simpleLightCB->m_perspectiveValues.Set( 1.f / m_mainCamera.m_viewToScreen.m_a00, 1.f / m_mainCamera.m_viewToScreen.m_a11, m_mainCamera.m_viewToScreen.m_a32, -m_mainCamera.m_viewToScreen.m_a22 );
	simpleLightCB->m_lightPos = Vec4( 5.f * cos( GTimer.GetSeconds( GTimer.TimeFromStart() ) ), -2.f + 5.f * sin( GTimer.GetSeconds( GTimer.TimeFromStart() ) ), 10.f, 0.f );

	D3D12_GPU_VIRTUAL_ADDRESS const constBufferStart = m_frameData[ m_frameID ].m_frameResource->GetGPUVirtualAddress();
	commandList->SetGraphicsRootConstantBufferView(0, constBufferStart + simpleLightConstBufferOffset );

	DrawFullscreenTriangle( commandList );

	DrawLayer( commandList, RL_OVERLAY );

	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

	commandList->ResourceBarrier(1, barriers);

	CheckResult(commandList->Close());

	m_mainCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&commandList));

	CheckResult(m_swapChain->Present(0, 0));
	m_frameID = (m_frameID + 1) % FRAME_NUM;

	m_constBufferOffset = 0;
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

	for (unsigned int shaderID = 0; shaderID < ST_MAX; ++shaderID)
	{
		m_shaders[shaderID]->Release();
	}
	m_shaderSimpleLight->Release();

	m_mainRS->Release();
	m_renderTargetDH.Release();
	m_texturesDH.Release();
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

	for ( UINT gbufferID = 0; gbufferID < GBB_MAX; ++gbufferID )
	{
		m_gbufferBuffers[ gbufferID ]->Release();
	}

	m_depthBuffertDH.Release();
	m_fullscreenTriangleRes->Release();

	unsigned int const geometryNum = m_geometryResources.size();
	for (unsigned int geometryID = 0; geometryID < geometryNum; ++geometryID)
	{
		m_geometryResources[geometryID].Release();
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

void CRender::PrepareView()
{
	m_mainCamera.m_worldToScreen = Mul( m_mainCamera.m_worldToView, m_mainCamera.m_viewToScreen );
	m_mainCamera.m_worldToScreen.Inverse( m_mainCamera.m_screenToWorld );
	m_mainCamera.m_worldToView.Inverse( m_mainCamera.m_viewToWorld );
}

Byte CRender::AddGeometry( SGeometry const& geometry )
{
	Byte const geometryID = Byte( m_geometryResources.size() );
	m_geometryResources.push_back( geometry );

	return geometryID;
}

SGeometry& CRender::GetGeometry( Byte const geometryID )
{
	return m_geometryResources[ geometryID ];
}

void CRender::ReleaseGeometry( Byte const geometryID )
{
	m_geometryResources[ geometryID ].Release();
}

void CRender::DrawFullscreenTriangle( ID3D12GraphicsCommandList* commandList )
{
	commandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	commandList->IASetVertexBuffers( 0, 1, &m_fullscreenTriangleView );
	commandList->DrawInstanced( 3, 1, 0, 0 );
}

void CRender::ResizeDescriptorHeap( SDescriptorHeap& descriptorHeap, UINT const size )
{
	ASSERT( size > 0 );
	ID3D12DescriptorHeap* oldHeap = descriptorHeap.m_pDescriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = size;
	heapDesc.Type = descriptorHeap.m_type;
	heapDesc.Flags = descriptorHeap.m_flags;
	CheckResult(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap.m_pDescriptorHeap)));

	if ( 0 < descriptorHeap.m_descriptorsNum )
	{
		m_device->CopyDescriptorsSimple( min( size, descriptorHeap.m_descriptorsNum ), descriptorHeap.m_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), oldHeap->GetCPUDescriptorHandleForHeapStart(), descriptorHeap.m_type );
		oldHeap->Release();
	}

	descriptorHeap.m_descriptorsNum = size;
}

void CRender::CreateFullscreenTriangleRes()
{
	UINT const verticesNum = 3;

	D3D12_RESOURCE_DESC descVertices = {};
	descVertices.DepthOrArraySize = 1;
	descVertices.SampleDesc.Count = 1;
	descVertices.Flags = D3D12_RESOURCE_FLAG_NONE;
	descVertices.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descVertices.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descVertices.MipLevels = 1;
	descVertices.Width = verticesNum * sizeof( SPosVertexFormat );
	descVertices.Height = 1;
	descVertices.Format = DXGI_FORMAT_UNKNOWN;

	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &m_fullscreenTriangleRes ) ) );

	ID3D12Resource* verticesUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&verticesUploadRes)));
	m_uploadResources.push_back(verticesUploadRes);
	void* pGPU;
	verticesUploadRes->Map(0, nullptr, &pGPU);

	SPosVertexFormat const vertices[] =
	{
		Vec2( 3.f, -1.f ),
		Vec2( -1.f, -1.f ),
		Vec2( -1.f, 3.f ),
	};
	memcpy( pGPU, vertices, sizeof( SPosVertexFormat ) * verticesNum );
	verticesUploadRes->Unmap( 0, nullptr );
	m_copyCL->CopyResource( m_fullscreenTriangleRes, verticesUploadRes );

	m_fullscreenTriangleView.BufferLocation = m_fullscreenTriangleRes->GetGPUVirtualAddress();
	m_fullscreenTriangleView.SizeInBytes = verticesNum * sizeof( SPosVertexFormat );
	m_fullscreenTriangleView.StrideInBytes = sizeof( SPosVertexFormat );

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_fullscreenTriangleRes;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_resourceBarrier.push_back( barrier );
}

void CRender::BeginLoadResources(unsigned int const textureNum)
{
	ResizeDescriptorHeap( m_texturesDH, textureNum + GBB_MAX );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	m_gbufferDescriptorsOffsets[ GBB_DIFFUSE ].m_srvOffset = textureNum + GBB_DIFFUSE;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_device->CreateShaderResourceView(m_gbufferBuffers[ GBB_DIFFUSE ], &srvDesc, m_texturesDH.GetCPUDescriptor(m_gbufferDescriptorsOffsets[ GBB_DIFFUSE ].m_srvOffset) );

	m_gbufferDescriptorsOffsets[ GBB_NORMAL ].m_srvOffset = textureNum + GBB_NORMAL;
	srvDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
	m_device->CreateShaderResourceView(m_gbufferBuffers[ GBB_NORMAL ], &srvDesc, m_texturesDH.GetCPUDescriptor(m_gbufferDescriptorsOffsets[ GBB_NORMAL ].m_srvOffset) );

	m_gbufferDescriptorsOffsets[ GBB_DEPTH ].m_srvOffset = textureNum + GBB_DEPTH;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	m_device->CreateShaderResourceView(m_gbufferBuffers[ GBB_DEPTH ], &srvDesc, m_texturesDH.GetCPUDescriptor(m_gbufferDescriptorsOffsets[ GBB_DEPTH ].m_srvOffset) );

	m_copyCA->Reset();
	m_copyCL->Reset(m_copyCA, nullptr);
	CreateFullscreenTriangleRes();
}

void CRender::LoadResource(STexture const& texture)
{
	D3D12_RESOURCE_DESC descTexture = {};
	descTexture.DepthOrArraySize = 1;
	descTexture.SampleDesc.Count = 1;
	descTexture.Flags = D3D12_RESOURCE_FLAG_NONE;
	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	descTexture.MipLevels = texture.m_mipLevels;
	descTexture.Width = texture.m_width;
	descTexture.Height = texture.m_height;
	descTexture.Format = texture.m_format;

	ID3D12Resource* textureRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textureRes)));
	m_texturesResources.push_back(textureRes);

	UINT64 bufferSize;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pFootprints = new D3D12_PLACED_SUBRESOURCE_FOOTPRINT[ texture.m_mipLevels ];
	UINT* pNumRows = new UINT[ texture.m_mipLevels ];
	UINT64* pRowPitches = new UINT64[ texture.m_mipLevels ];
	m_device->GetCopyableFootprints(&descTexture, 0, texture.m_mipLevels, 0, pFootprints, pNumRows, pRowPitches, &bufferSize);

	descTexture.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descTexture.Format = DXGI_FORMAT_UNKNOWN;
	descTexture.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descTexture.MipLevels = 1;
	descTexture.Height = 1;
	descTexture.Width = bufferSize;

	ID3D12Resource* textureUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descTexture, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&textureUploadRes)));
	m_uploadResources.push_back(textureUploadRes);

	void* pGPU;
	textureUploadRes->Map(0, nullptr, &pGPU);

	for ( UINT mipLevel = 0; mipLevel < texture.m_mipLevels; ++mipLevel )
	{
		for ( UINT rowID = 0; rowID < pNumRows[ mipLevel ]; ++rowID )
		{
			memcpy( ( BYTE* )pGPU + pFootprints[ mipLevel ].Offset + rowID * pFootprints[ mipLevel ].Footprint.RowPitch, texture.m_data + rowID * pRowPitches[ mipLevel ], pRowPitches[ mipLevel ] );
		}

		D3D12_TEXTURE_COPY_LOCATION dst;
		dst.pResource = textureRes;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = mipLevel;

		D3D12_TEXTURE_COPY_LOCATION src;
		src.pResource = textureUploadRes;
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint = pFootprints[ mipLevel ];

		m_copyCL->CopyTextureRegion( &dst, 0, 0, 0, &src, nullptr );
	}
	textureUploadRes->Unmap(0, nullptr);

	delete[] pFootprints;
	delete[] pNumRows;
	delete[] pRowPitches;

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureRes;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_resourceBarrier.push_back( barrier );

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = texture.m_mipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Format = texture.m_format;

	m_device->CreateShaderResourceView(textureRes, &srvDesc, m_texturesDH.GetCPUDescriptor(m_texturesResources.size() - 1));
}

Byte CRender::LoadResource( SGeometryData const & geometryData )
{
	SGeometry geometry;

	UINT const verticesNum = UINT( geometryData.m_vertices.size() );
	UINT const indicesNum = UINT( geometryData.m_indices.size() );

	D3D12_RESOURCE_DESC descVertices = {};
	descVertices.DepthOrArraySize = 1;
	descVertices.SampleDesc.Count = 1;
	descVertices.Flags = D3D12_RESOURCE_FLAG_NONE;
	descVertices.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descVertices.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descVertices.MipLevels = 1;
	descVertices.Width = verticesNum * sizeof( SSimpleObjectVertexFormat );
	descVertices.Height = 1;
	descVertices.Format = DXGI_FORMAT_UNKNOWN;

	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &geometry.m_vertexRes ) ) );

	ID3D12Resource* verticesUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descVertices, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&verticesUploadRes)));
	m_uploadResources.push_back(verticesUploadRes);
	void* pGPU;
	verticesUploadRes->Map(0, nullptr, &pGPU);
	memcpy( pGPU, geometryData.m_vertices.data(), sizeof( SSimpleObjectVertexFormat ) * verticesNum );
	verticesUploadRes->Unmap( 0, nullptr );
	m_copyCL->CopyResource( geometry.m_vertexRes, verticesUploadRes );

	D3D12_RESOURCE_DESC descIndices = {};
	descIndices.DepthOrArraySize = 1;
	descIndices.SampleDesc.Count = 1;
	descIndices.Flags = D3D12_RESOURCE_FLAG_NONE;
	descIndices.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	descIndices.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	descIndices.MipLevels = 1;
	descIndices.Width = indicesNum * sizeof( UINT16 );
	descIndices.Height = 1;
	descIndices.Format = DXGI_FORMAT_UNKNOWN;

	CheckResult( m_device->CreateCommittedResource( &GHeapPropertiesGPUOnly, D3D12_HEAP_FLAG_NONE, &descIndices, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS( &geometry.m_indicesRes ) ) );

	ID3D12Resource* indicesUploadRes;
	CheckResult(m_device->CreateCommittedResource(&GHeapPropertiesUpload, D3D12_HEAP_FLAG_NONE, &descIndices, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indicesUploadRes)));
	m_uploadResources.push_back(indicesUploadRes);
	indicesUploadRes->Map(0, nullptr, &pGPU);
	memcpy( pGPU, geometryData.m_indices.data(), sizeof( UINT16 ) * indicesNum );
	indicesUploadRes->Unmap( 0, nullptr );
	m_copyCL->CopyResource( geometry.m_indicesRes, indicesUploadRes );

	geometry.m_vertexBufferView.BufferLocation = geometry.m_vertexRes->GetGPUVirtualAddress();
	geometry.m_vertexBufferView.SizeInBytes = verticesNum * sizeof( SSimpleObjectVertexFormat );
	geometry.m_vertexBufferView.StrideInBytes = sizeof( SSimpleObjectVertexFormat );

	geometry.m_indexBufferView.BufferLocation = geometry.m_indicesRes->GetGPUVirtualAddress();
	geometry.m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	geometry.m_indexBufferView.SizeInBytes = indicesNum * sizeof( UINT16 );

	Byte const geometryID = Byte( m_geometryResources.size() );
	m_geometryResources.push_back( geometry );

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = geometry.m_vertexRes;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_resourceBarrier.push_back( barrier );

	barrier.Transition.pResource = geometry.m_indicesRes;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	m_resourceBarrier.push_back( barrier );

	return geometryID;
}

void CRender::EndLoadResources()
{
	m_copyCL->Close();
	m_copyCQ->ExecuteCommandLists(1, (ID3D12CommandList**)(&m_copyCL));
}

void CRender::WaitForResourcesLoad()
{
	CheckResult(m_mainCQ->Signal(m_fence, m_fenceValue));
	CheckResult(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
	++m_fenceValue;
	WaitForSingleObject(m_fenceEvent, INFINITE);

	m_mainCA->Reset();
	m_mainCL->Reset(m_mainCA, nullptr);

	m_mainCL->ResourceBarrier(m_resourceBarrier.size(), m_resourceBarrier.data());
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

	UINT const uploadResNum = UINT( m_uploadResources.size() );
	for ( UINT uploadResID = 0; uploadResID < uploadResNum; ++uploadResID )
	{
		m_uploadResources[uploadResID]->Release();
	}
	m_uploadResources.clear();
	m_uploadResources.shrink_to_fit();
}

void CRender::GetRenderData( UINT const cbSize, D3D12_GPU_VIRTUAL_ADDRESS& outConstBufferOffset, void*& outConstBufferPtr )
{
	ASSERT_STR( ( cbSize & 0xFF ) == 0, "Const buffer not alignet ty 256B" );
	ASSERT_STR( m_constBufferOffset + cbSize <= 256 * MAX_OBJECTS, "Not enough space for constant buffer" );
	outConstBufferPtr = reinterpret_cast<void*>( &m_frameData[ m_frameID ].m_pResourceData[ m_constBufferOffset ] );
	outConstBufferOffset = m_constBufferOffset;

	m_constBufferOffset += cbSize;
}
