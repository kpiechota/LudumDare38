#include "environmentParticleCommon.hlsl"

cbuffer objectBuffer : register(b0)
{
	float DeltaTime;
	uint ParticleNum;
}
RWStructuredBuffer< SEnvironmentParticle > Particles : register( u0 );

[numthreads(64, 1, 1)]
void csMain( uint3 dispatchID : SV_DispatchThreadID )
{
	if ( dispatchID.x < ParticleNum )
	{
		Particles[ dispatchID.x ].m_position = frac( Particles[ dispatchID.x ].m_position + DeltaTime * Particles[ dispatchID.x ].m_speed * Particles[ dispatchID.x ].m_velocity );
	}
}