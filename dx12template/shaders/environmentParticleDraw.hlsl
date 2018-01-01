#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b0)
{
	float4x4 ObjectToWorld;
	float4x4 WorldToScreen;
	float4 Color;
	float3 CameraPositionWS;
}
StructuredBuffer< SEnvironmentParticle > Particles : register( t0 );
Texture2D DepthTex : register( t3 );

float4 vsMain(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID ) : SV_POSITION
{
	SEnvironmentParticle particle = Particles[ instanceID ];
	float3 velocityOS = particle.m_velocity;
	float3 velocityWS = normalize( mul( float4( velocityOS, 0.f ), ObjectToWorld ).xyz );
	float3 positionWS = mul( float4( particle.m_position, 1.f ), ObjectToWorld ).xyz;
	
	float3 perpendicularVector = cross( normalize( CameraPositionWS - positionWS ), velocityWS );
	
	float3 verticesPositions[ 4 ] =
	{
		particle.m_size * ( -perpendicularVector - velocityWS * particle.m_speed * 0.1f ),
		particle.m_size * ( -perpendicularVector + velocityWS * particle.m_speed * 0.1f ),
		particle.m_size * ( perpendicularVector - velocityWS * particle.m_speed * 0.1f ),
		particle.m_size * ( perpendicularVector + velocityWS * particle.m_speed * 0.1f )
	};
	
	float3 vertexPosition = positionWS + verticesPositions[ vertexID ];
	return  mul( float4( vertexPosition, 1.f ), WorldToScreen );
}

float4 psMain(float4 position : SV_POSITION) : SV_TARGET0
{
	float depthHW = DepthTex.Load( int3( position.xy, 0 ) ).r;
	return Color * smoothstep( position.z, 1.f, depthHW );
}