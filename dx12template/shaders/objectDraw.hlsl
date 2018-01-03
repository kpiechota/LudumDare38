cbuffer objectBuffer : register(b0)
{
	float4x4 ObjectToScreen;
	float4x3 ObjectToWorld;
}

Texture2D DiffTex : register(t0);
Texture2D NormTex : register(t1);
Texture2D EmissiveTex : register(t2);
Texture2D SpecularTex : register(t3);
SamplerState Sampler : register(s0);

struct VSInput
{
	float3 m_position : POSITION;
	float3 m_normal : NORMAL;
	float3 m_tangent : TANGENT;
	float2 m_uv : TEXCOORD;
};

struct PSInput
{
	float3x3 m_tbn		: TANGENT;
	float4 m_position	: SV_POSITION;
	float2 m_uv			: TEXCOORD;
};

struct PSOutput
{
	float4 m_diffuse		: SV_TARGET0;
	float4 m_normalWS		: SV_TARGET1;
	float4 m_emissiveSpec	: SV_TARGET2;
};

PSInput vsMain( VSInput input )
{
	float3 bitangent = cross( input.m_tangent, input.m_normal );

	PSInput output;
	output.m_tbn[ 0 ] = mul( float4( input.m_tangent, 0.f ), ObjectToWorld ).xyz;
	output.m_tbn[ 1 ] = mul( float4( bitangent, 0.f ), ObjectToWorld ).xyz;
	output.m_tbn[ 2 ] = mul( float4( input.m_normal, 0.f ), ObjectToWorld ).xyz;
	output.m_position = mul( float4( input.m_position, 1.f ), ObjectToScreen );
	output.m_uv = input.m_uv;

	return output;
}

PSOutput psMain(PSInput input)
{
	PSOutput output;
	output.m_normalWS = NormTex.Sample( Sampler, float2(input.m_uv.x, 1.f - input.m_uv.y ) );
	output.m_diffuse = DiffTex.Sample( Sampler, float2(input.m_uv.x, 1.f - input.m_uv.y ) );

	output.m_normalWS.xyz = mad( output.m_normalWS, 2.f, -1.f );
	output.m_normalWS.xyz = mul( output.m_normalWS, input.m_tbn );
	output.m_normalWS.xyz = normalize( output.m_normalWS );
	output.m_normalWS.xyz = mad( output.m_normalWS, 0.5f, 0.5f );
	output.m_emissiveSpec.rgb = EmissiveTex.Sample( Sampler, float2(input.m_uv.x, 1.f - input.m_uv.y ) ).rgb;
	output.m_emissiveSpec.a = SpecularTex.Sample( Sampler, float2(input.m_uv.x, 1.f - input.m_uv.y ) ).r;

	return output;
}