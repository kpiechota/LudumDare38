cbuffer objectBuffer : register(b0)
{
	float4x4 ObjectToScreen;
	float4 ColorScale;
	float2 UVTile;
	float2 UVOffset;
	float2 Offset;
}

Texture2D MaskTex : register(t0);
Texture2D DiffTex : register(t1);
SamplerState Sampler : register(s0);

struct PSInput
{
	float4 m_position	: SV_POSITION;
	float2 m_uv			: TEXCOORD;
};

PSInput vsMain(uint vertexID : SV_VertexID)
{
	const float2 position = float2(mad((float)(vertexID & 1), 2.f, -1.f), mad((float)((vertexID >> 1) & 1), -2.f, 1.f));
	const float3 positionOS = float3(position + Offset, 1.f);
	const float3x3 objectToScreen3x3 = (float3x3)ObjectToScreen;

	const float3 positionSS = mul(positionOS, objectToScreen3x3);

	PSInput output;
	output.m_position = float4(positionSS, 1.f);
	output.m_uv = mad(position, 0.5f, 0.5f) * UVTile + UVOffset;

	return output;
}

float4 psMain(PSInput input) : SV_TARGET0
{
	const float4 maskAlpha = MaskTex.Sample(Sampler, input.m_uv).a;
	const float4 color = DiffTex.Sample(Sampler, input.m_uv) * ColorScale;
	//clip((color.a < 0.002f || maskAlpha < 0.002f) ? -1.f : 1.f);

	return color;
}