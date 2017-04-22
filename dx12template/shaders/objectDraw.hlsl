cbuffer objectBuffer : register(b0)
{
	float4x4 ObjectToScreen;
}

float4 vsMain( uint vertexID : SV_VertexID ) : SV_POSITION
{
	const float3 positionOS = float3(mad((float)(vertexID & 1), 2.f, -1.f), mad((float)((vertexID >> 1) & 1), -2.f, 1.f), 1.f);
	const float3x3 objectToScreen3x3 = (float3x3)ObjectToScreen;

	const float3 positionSS = mul(positionOS, objectToScreen3x3);

	return float4( positionSS, 1.f );
}

float4 psMain( float4 position : SV_POSITION ) : SV_TARGET0
{
	return 1.f;
}