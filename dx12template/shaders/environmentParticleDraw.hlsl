#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b0)
{
	float4x4 WorldToScreen;
	float4x3 ObjectToWorld;
	float4 Color;
	float3 CameraPositionWS;
	//float FadeDistanceSq;
}
StructuredBuffer< SEnvironmentParticle > Particles : register( t0 );

Texture2D ColorTex : register( t0 );
Texture2D DepthTex : register( t3 );
SamplerState Sampler : register(s0);

struct VStoPS
{
	float4 m_position : SV_POSITION;
	float2 m_uv : TEXCOORD0;
	float m_fade : TEXCOORD1;
};

VStoPS vsMain(uint vertexID : SV_VertexID ) 
{
	uint instanceID = vertexID / 6;

	SEnvironmentParticle particle = Particles[ instanceID ];
	float3 velocityOS = particle.m_velocity;
	float3 positionWS = mul( float4( particle.m_position, 1.f ), ObjectToWorld ).xyz;
	float3 positionToCameraWS = CameraPositionWS - positionWS;
	float3 velocityWS = normalize( mul( float4( velocityOS, 0.f ), ObjectToWorld ).xyz );
	float3 perpendicularVector = particle.m_size.x * cross( normalize( positionToCameraWS ), velocityWS );
	velocityWS *= particle.m_size.y;

	float3 verticesPositions[] =
	{
		-perpendicularVector - velocityWS,
		-perpendicularVector + velocityWS,
		perpendicularVector - velocityWS,
		perpendicularVector - velocityWS,
		-perpendicularVector + velocityWS,
		perpendicularVector + velocityWS,
	};
	
	float2 verticesUV[] =
	{
		float2( 0.f, 0.f ),
		float2( 0.f, 1.f ),
		float2( 1.f, 0.f ),
		float2( 1.f, 0.f ),
		float2( 0.f, 1.f ),
		float2( 1.f, 1.f ),
	};

	float3 vertexPosition = positionWS + verticesPositions[ vertexID % 6 ];

	VStoPS output;
	output.m_position = mul( float4( vertexPosition, 1.f ), WorldToScreen );
	output.m_uv = verticesUV[ vertexID % 6 ];
	output.m_fade = 1.f;
	return output;
}

float4 psMain(VStoPS input) : SV_TARGET0
{
	clip( input.m_fade - 0.002f );
	return input.m_fade * ColorTex.Sample( Sampler, input.m_uv );
}