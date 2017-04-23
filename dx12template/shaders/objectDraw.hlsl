cbuffer objectBuffer : register(b0)
{
	float4x4 ObjectToScreen;
	float4 ColorScale;
	float2 Offset;
}

Texture2D DiffTex : register(t0);
SamplerState Sampler : register(s0);

struct PSInput
{
	float4 m_position	: SV_POSITION;
	float2 m_uv			: TEXCOORD;
};

PSInput vsMain( uint vertexID : SV_VertexID )
{
	const float3 positionOS = float3(mad((float)(vertexID & 1), 2.f, -1.f) + Offset.x, mad((float)((vertexID >> 1) & 1), -2.f, 1.f) + Offset.y, 1.f);
	const float3x3 objectToScreen3x3 = (float3x3)ObjectToScreen;

	const float3 positionSS = mul(positionOS, objectToScreen3x3);

	PSInput output;
	output.m_position = float4( positionSS, 1.f );
	output.m_uv = mad(positionOS, 0.5f, 0.5f);

	return output;
}

float4 psMain(PSInput input) : SV_TARGET0
{
	const float4 color = DiffTex.Sample(Sampler, input.m_uv) * ColorScale;
	
	clip(color.a - 0.002f);
	
	return color;
}