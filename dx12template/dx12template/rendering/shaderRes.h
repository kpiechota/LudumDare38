#pragma once

enum class EShaderParameters : Byte
{
	ObjectToScreen,
	ObjectToWorld,
	WorldToScreen,
	ViewToWorld,
	PerspectiveValues,
	LightPos,
	Attenuation,
	Color,
	Cutoff,
	AmbientColor,
	LightDirWS,
	CameraPositionWS,
	Fade,

	SP_MAX
};

class CShaderRes
{
private:
	UINT16 m_paramOffsets[UINT(EShaderParameters::SP_MAX)];
	UINT16 m_bufferSize;
	ID3D12PipelineState* m_pso;

private:
	inline void LoadShader(LPCWSTR pFileName, D3D_SHADER_MACRO const* pDefines, LPCSTR pEmtryPoint, LPCSTR pTarget, ID3DBlob** ppCode) const;

public:
	void InitShader( LPCWSTR pFileName, D3D12_INPUT_ELEMENT_DESC const* vertexElements, UINT const vertexElementsNum, UINT const renderTargetNum, DXGI_FORMAT const* renderTargetFormats, ERenderTargetBlendStates const* renderTargetBlendStates, EDepthStencilStates const depthStencilState = EDepthStencilStates::EDSS_Disabled, ERasterizerStates const rasterizationState = ERasterizerStates::ERS_Default, D3D_SHADER_MACRO const* pDefines = nullptr );
	void InitComputeShader( LPCWSTR pFileName, ID3D12RootSignature* pRootSignature, D3D_SHADER_MACRO const* pDefines = nullptr );
	ID3D12PipelineState* GetPSO() { return m_pso; }
	void Release() { m_pso->Release(); }
	UINT16 GetOffset( EShaderParameters const param ) const { return m_paramOffsets[ UINT(param) ]; }
	UINT16 GetBufferSize() const { return m_bufferSize; }
};
