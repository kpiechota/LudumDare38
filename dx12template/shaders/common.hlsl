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

float FloatRandStatic( uint seed, float minVal, float maxVal )
{
	seed = (seed << 13) ^ seed;
	float v = (1.0f - ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
	return  (maxVal * ( v + 1.f ) + minVal * ( 1.f - v ) ) * 0.5f;
}

uint UIntRandStatic( uint seed, uint minVal, uint maxVal )
{
	return minVal + ( maxVal - minVal ) * FloatRandStatic( seed, 0.f, 1.f );
}

float3 Float3RandStatic(uint seed, float minVal, float maxVal)
{
	return float3( FloatRandStatic( seed, minVal, maxVal ), FloatRandStatic( seed + 1, minVal, maxVal ), FloatRandStatic( seed + 2, minVal, maxVal ) );
}