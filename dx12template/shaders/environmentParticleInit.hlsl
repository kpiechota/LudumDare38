#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b0)
{
	uint Seed;
	uint ParticleNum;
}
RWStructuredBuffer< SEnvironmentParticle > Particles : register( u0 );

float FloatRandStatic( uint seed, float minVal, float maxVal )
{
	seed = (seed << 13) ^ seed;
	float v = (1.0f - ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
	return  (maxVal * ( v + 1.f ) + minVal * ( 1.f - v ) ) * 0.5f;
}

float3 Float3RandStatic(uint seed, float minVal, float maxVal)
{
	return float3( FloatRandStatic( seed, minVal, maxVal ), FloatRandStatic( seed + 1, minVal, maxVal ), FloatRandStatic( seed + 2, minVal, maxVal ) );
}

[numthreads(64, 1, 1)]
void csMain( uint3 dispatchID : SV_DispatchThreadID )
{
	if ( dispatchID.x < ParticleNum )
	{
		uint seedOffset = Seed + 6 * dispatchID.x;
		uint2 coord = uint2( dispatchID.x % 5, dispatchID.x / 5 );
		Particles[ dispatchID.x ].m_position = Float3RandStatic( seedOffset, 0.f, 1.f );
		seedOffset += 3;
		Particles[ dispatchID.x ].m_size = FloatRandStatic( seedOffset, 0.005f, .01f );
		++seedOffset;
		Particles[ dispatchID.x ].m_velocity = normalize( float3( 1.f, FloatRandStatic( seedOffset, 1.f, 3.f ), 1.f ) );
		++seedOffset;
		Particles[ dispatchID.x ].m_speed = FloatRandStatic( seedOffset, 1.f, 5.f );
	}
}