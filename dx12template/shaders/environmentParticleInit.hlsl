#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b1)
{
	uint Seed;
}
RWStructuredBuffer< SEnvironmentParticle > Particles : register( u0 );

float RandomFloat(uint seed)
{
	seed ^= 105316U;
	seed &= 204101U;
	seed ^= 915826U;
	return frac( float( seed ) * .1913f );
}

float3 RandomFloat3(uint seed)
{
	return float3( RandomFloat( seed ), RandomFloat( seed + 1 ), RandomFloat( seed + 2 ) );
}

[numthreads(1, 1, 1)]
void csMain( uint3 dispatchID : SV_DispatchThreadID )
{
	uint seedOffset = Seed + 5 * dispatchID.x;
	uint2 coord = uint2( dispatchID.x % 5, dispatchID.x / 5 );
	Particles[ dispatchID.x ].m_position = RandomFloat3( seedOffset );
	seedOffset += 3;
	Particles[ dispatchID.x ].m_size = 0.01f + 0.01f * RandomFloat( seedOffset );
	++seedOffset;
	Particles[ dispatchID.x ].m_velocity = normalize( float3( 1.f, 3.f, 1.f ) );
	Particles[ dispatchID.x ].m_speed = 15.f + 25.f * RandomFloat( seedOffset );
}