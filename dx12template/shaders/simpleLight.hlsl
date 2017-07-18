cbuffer objectBuffer : register(b0)
{
	float4x4 ViewToWorld;
	float4 PerspectiveValues;
	float3 LightPos;
}

Texture2D DiffTex : register( t0 );
Texture2D NormalTex : register( t1 );
Texture2D DepthTex : register( t2 );

float GetLinearDepth( float2 uv )
{
	float depthHW = DepthTex.Load( int3( uv.xy, 0 ) ).r;
	return PerspectiveValues.z / ( depthHW + PerspectiveValues.w );
}

float3 GetPositionWS( float2 uv, float2 position )
{
	float linearDepth = GetLinearDepth( position );
	float3 positionWS = float3( uv * PerspectiveValues.xy * linearDepth, linearDepth );
	return mul( float4( positionWS, 1.f ), ViewToWorld ).xyz;
}

struct VSToPS
{
	float4 m_position : SV_POSITION;
	float2 m_uv : TEXCOORD;
};

VSToPS vsMain( float2 position : POSITION )
{
	VSToPS output;
	output.m_position = float4( position, 0.f, 1.f );
	output.m_uv = position;

	return output;
}

float4 psMain( VSToPS input ) : SV_TARGET
{
	float3 positionWS = GetPositionWS( input.m_uv, input.m_position );
	float3 normalWS = NormalTex.Load( int3( input.m_position.xy, 0 ) ) * 2.f - 1.f;
	float3 lightDirWS = LightPos - positionWS;
	float att = 1.f / length( lightDirWS );
	lightDirWS *= att;
	float ndl = saturate( dot( lightDirWS, normalWS ) );
	return float4( DiffTex.Load( int3( input.m_position.xy, 0 ) ).rgb * 5.f * ndl * att, 1.f );
}