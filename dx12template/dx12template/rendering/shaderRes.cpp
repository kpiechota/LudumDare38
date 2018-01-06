#include "../headers.h"
#include "render.h"
#include "shaderRes.h"

LPCSTR ShaderParamsNames[] =
{
	"ObjectToScreen",
	"ObjectToWorld",
	"WorldToScreen",
	"ViewToWorld",
	"PerspectiveValues",
	"LightPos",
	"Attenuation",
	"Color",
	"Cutoff",
	"AmbientColor",
	"LightDirWS",
	"CameraPositionWS",
	"Fade",
};
CT_ASSERT( (ARRAYSIZE( ShaderParamsNames ) == ( UINT(EShaderParameters::SP_MAX) )) );

inline void CShaderRes::LoadShader(LPCWSTR pFileName, D3D_SHADER_MACRO const* pDefines, LPCSTR pEmtryPoint, LPCSTR pTarget, ID3DBlob** ppCode) const
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

void CShaderRes::InitShader( LPCWSTR pFileName, D3D12_INPUT_ELEMENT_DESC const* vertexElements, UINT const vertexElementsNum, UINT const renderTargetNum, DXGI_FORMAT const* renderTargetFormats, ERenderTargetBlendStates const* renderTargetBlendStates, EDepthStencilStates const depthStencilState, ERasterizerStates const rasterizationState, D3D_SHADER_MACRO const* pDefines )
{
	m_bufferSize = 0;
	memset( m_paramOffsets, 0xFF, sizeof( m_paramOffsets ) );

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
	descPSO.pRootSignature = GRender.GetMainRS();

	for ( UINT renderTargetID = 0; renderTargetID < renderTargetNum; ++renderTargetID )
	{
		descPSO.BlendState.RenderTarget[ renderTargetID ] = GRenderTargetBlendStates[ renderTargetBlendStates[ renderTargetID ] ];
		descPSO.RTVFormats[ renderTargetID ] = renderTargetFormats[ renderTargetID ];
	}
	ID3D12ShaderReflection* shaderRefl;
	ID3D12ShaderReflectionConstantBuffer* shaderCBRefl = nullptr;
	ID3D12ShaderReflectionVariable*	shaderVaribleRefl = nullptr;
	D3D12_SHADER_BUFFER_DESC bufferDesc;
	D3D12_SHADER_VARIABLE_DESC variableDesc;

	ID3DBlob* vsShader;
	LoadShader(pFileName, pDefines, "vsMain", "vs_5_1", &vsShader);
	descPSO.VS = { vsShader->GetBufferPointer(), vsShader->GetBufferSize() };
	D3DReflect( vsShader->GetBufferPointer(), vsShader->GetBufferSize(), IID_PPV_ARGS( &shaderRefl ) );
	shaderCBRefl = shaderRefl->GetConstantBufferByIndex( 0 );
	shaderCBRefl->GetDesc( &bufferDesc );
	if ( shaderCBRefl->GetDesc( &bufferDesc ) == S_OK )
	{
		m_bufferSize = UINT16( bufferDesc.Size );

		for ( UINT i = 0; i < ARRAYSIZE( ShaderParamsNames ); ++i )
		{
			shaderVaribleRefl = shaderCBRefl->GetVariableByName( ShaderParamsNames[ i ] );
			if ( shaderVaribleRefl->GetDesc( &variableDesc ) == S_OK )
			{
				m_paramOffsets[ i ] = UINT16(variableDesc.StartOffset);
			}
		}
	}

	ID3DBlob* psShader;
	LoadShader(pFileName, pDefines, "psMain", "ps_5_1", &psShader);
	descPSO.PS = { psShader->GetBufferPointer(), psShader->GetBufferSize() };
	D3DReflect( psShader->GetBufferPointer(), psShader->GetBufferSize(), IID_PPV_ARGS( &shaderRefl ) );
	shaderCBRefl = shaderRefl->GetConstantBufferByIndex( 0 );
	if ( shaderCBRefl->GetDesc( &bufferDesc ) == S_OK )
	{
		m_bufferSize = max( m_bufferSize, UINT16( bufferDesc.Size ) );

		for ( UINT i = 0; i < ARRAYSIZE( ShaderParamsNames ); ++i )
		{
			shaderVaribleRefl = shaderCBRefl->GetVariableByName( ShaderParamsNames[ i ] );
			if ( shaderVaribleRefl->GetDesc( &variableDesc ) == S_OK )
			{
				m_paramOffsets[ i ] = UINT16(variableDesc.StartOffset);
			}
		}
	}

	m_bufferSize = (m_bufferSize + 255) & ~255;
	CheckResult(GRender.GetDevice()->CreateGraphicsPipelineState(&descPSO, IID_PPV_ARGS(&m_pso)));

	vsShader->Release();
	psShader->Release();
}

void CShaderRes::InitComputeShader( LPCWSTR pFileName, ID3D12RootSignature* pRootSignature, D3D_SHADER_MACRO const* pDefines )
{
	m_bufferSize = 0;
	memset( m_paramOffsets, 0xFF, sizeof( m_paramOffsets ) );

	D3D12_COMPUTE_PIPELINE_STATE_DESC descPSO = {};
	descPSO.pRootSignature = pRootSignature;

	ID3D12ShaderReflection* shaderRefl;
	ID3D12ShaderReflectionConstantBuffer* shaderCBRefl = nullptr;
	ID3D12ShaderReflectionVariable*	shaderVaribleRefl = nullptr;
	D3D12_SHADER_BUFFER_DESC bufferDesc;
	D3D12_SHADER_VARIABLE_DESC variableDesc;

	ID3DBlob* csShader;
	LoadShader(pFileName, pDefines, "csMain", "cs_5_1", &csShader);
	descPSO.CS = { csShader->GetBufferPointer(), csShader->GetBufferSize() };
	D3DReflect( csShader->GetBufferPointer(), csShader->GetBufferSize(), IID_PPV_ARGS( &shaderRefl ) );
	shaderCBRefl = shaderRefl->GetConstantBufferByIndex( 0 );
	shaderCBRefl->GetDesc( &bufferDesc );
	if ( shaderCBRefl->GetDesc( &bufferDesc ) == S_OK )
	{
		m_bufferSize = UINT16( bufferDesc.Size );

		for ( UINT i = 0; i < ARRAYSIZE( ShaderParamsNames ); ++i )
		{
			shaderVaribleRefl = shaderCBRefl->GetVariableByName( ShaderParamsNames[ i ] );
			if ( shaderVaribleRefl->GetDesc( &variableDesc ) == S_OK )
			{
				m_paramOffsets[ i ] = UINT16(variableDesc.StartOffset);
			}
		}
	}

	m_bufferSize = (m_bufferSize + 255) & ~255;
	CheckResult(GRender.GetDevice()->CreateComputePipelineState(&descPSO, IID_PPV_ARGS(&m_pso)));

	csShader->Release();
}