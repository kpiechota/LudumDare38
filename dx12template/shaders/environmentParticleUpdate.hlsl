#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b0)
{
	float DeltaTime;
}
RWStructuredBuffer< SEnvironmentParticle > Particles : register( u0 );

[numthreads(1, 1, 1)]
void csMain( uint3 dispatchID : SV_DispatchThreadID )
{
	Particles[ dispatchID.x ].m_position = frac(Particles[ dispatchID.x ].m_position + DeltaTime * Particles[ dispatchID.x ].m_speed * Particles[ dispatchID.x ].m_velocity);
}