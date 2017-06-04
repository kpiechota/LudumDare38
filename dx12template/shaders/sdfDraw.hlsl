cbuffer objectBuffer : register(b0)
{
	float4x4 ObjectToScreen;
	float4 SdfColor;
	float2 Cutoff;
}

Texture2D DistTex : register(t0);
SamplerState Sampler : register(s1);

struct PSInput
{
	float4 m_position	: SV_POSITION;
	float2 m_uv			: TEXCOORD;
};

PSInput vsMain( uint vertexID : SV_VertexID )
{
	const float2 position = float2(mad((float)(vertexID & 1), 2.f, -1.f), mad((float)((vertexID >> 1) & 1), -2.f, 1.f));
	const float3 positionOS = float3(position.xy, 1.f);
	const float3x3 objectToScreen3x3 = (float3x3)ObjectToScreen;

	const float3 positionSS = mul(positionOS, objectToScreen3x3);

	PSInput output;
	output.m_position = float4( positionSS, 1.f );
	output.m_uv = mad(position.xy, 0.5f, 0.5f);

	return output;
}

float4 psMain(PSInput input) : SV_TARGET0
{
	float dist = DistTex.Sample(Sampler, input.m_uv).r;
	
	clip( dist - Cutoff.y );

	float alpha = smoothstep( 0.f, 1.f, ( dist - Cutoff.y ) / ( Cutoff.x * Cutoff.y ) );

	return float4(SdfColor.rgb, SdfColor.a * alpha);
}