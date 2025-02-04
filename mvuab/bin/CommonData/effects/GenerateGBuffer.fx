#include "vertex_types.fxh"
#include "samplers.fxh"
#include "globals.fxh"

UBER_VERTEX_PS mainVS(UBER_VERTEX_VS IN)
{
	UBER_VERTEX_PS OUT=(UBER_VERTEX_PS)0;
    
#if defined( USE_CAL3D_HW )
	float3 l_Normal  = normalize(IN.Normal).xyz;
	float3 l_Tangent = IN.Tangent.xyz;
	
	CalcAnimatedNormalTangent(IN.Normal.xyz, IN.Tangent.xyz, IN.Indices, IN.Weight, l_Normal, l_Tangent);
	float3 l_Position = CalcAnimtedPos(float4(IN.Position.xyz,1.0), IN.Indices, IN.Weight);
	
	// Modify the x coordinate ( symmetry )
	l_Position.x = -l_Position.x;
	float4 l_WorldPosition = float4(l_Position, 1.0);
	
	OUT.WorldPosition = mul(l_WorldPosition, g_WorldMatrix);
	
	OUT.Normal = normalize(mul(l_Normal, g_WorldMatrix));
	OUT.WorldNormal = OUT.Normal;
	
	l_Normal = l_Normal* -1;
	
	OUT.WorldTangent = float4(normalize(mul(l_Tangent, (float3x3)g_WorldMatrix)), 0.0);
	OUT.WorldBinormal = float4(mul(cross(l_Tangent,l_Normal), (float3x3)g_WorldMatrix), 0.0);
	
	OUT.HPosition = mul(l_WorldPosition, g_WorldViewProj );
	
	float u = IN.UV.x;
	//u = 1.0 - u;
	
	float v = IN.UV.y;
	v = 1.0 - v;
	
	OUT.UV = float2( u, v);
	
#else
	OUT.HPosition=mul(float4(IN.Position, 1.0), g_WorldViewProj);
    OUT.Normal=mul(IN.Normal, (float3x3)g_WorldMatrix);
    OUT.WorldPosition=OUT.HPosition;
	OUT.UV=IN.UV;
#endif

#if defined( USE_NORMAL )
	OUT.WorldTangent = float4( mul(IN.Tangent.xyz, (float3x3)g_WorldMatrix), 1.0);
	OUT.WorldBinormal = float4(mul(IN.Binormal.xyz, (float3x3)g_WorldMatrix),1.0);
#endif

#if defined( USE_SELF_ILUM )
	OUT.UV2 = IN.UV2;
#endif

#if defined( USE_DIFFUSE_COLOR )
	OUT.Color = IN.Color;
#endif

    return OUT;
}

//--------------------------------------------------------------------------------
float4 PackAlbedo( UBER_VERTEX_PS IN )
{
#if defined( USE_DIFFUSE_COLOR )
	return IN.Color;
#elif defined ( USE_DEBUG )
	return g_DebugColor;
#else
	//#if defined( USE_CAL3D_HW )
		//return float4( 1, 0, 0, 1 );
	//#endif
	
	return tex2D(S0LinearSampler,IN.UV);
#endif
}

//--------------------------------------------------------------------------------
float3 PackNormal( UBER_VERTEX_PS IN )
{
#if defined( USE_NORMAL )
	#if defined( USE_SELF_ILUM )
		float4 l_TexNormal = tex2D(S2LinearSampler,IN.UV);
	#else
		float4 l_TexNormal = tex2D(S1LinearSampler,IN.UV);
	#endif
	return CalcNewNormal( normalize(IN.WorldTangent), normalize(IN.WorldBinormal), l_TexNormal, normalize(IN.Normal));
#else
	return normalize(IN.Normal);
#endif
}

TMultiRenderTargetPixel mainPS(UBER_VERTEX_PS IN) : COLOR
{
	TMultiRenderTargetPixel OUT=(TMultiRenderTargetPixel)0;
	
	float4 l_DiffuseColor = PackAlbedo( IN );
	if(l_DiffuseColor.a<0.5)
		clip(-1);
		
	float3 l_Normal = PackNormal( IN );
		
	float3 l_AmbientColor = float3(0,0,0);
#if defined( USE_SELF_ILUM )
	l_AmbientColor = l_DiffuseColor*tex2D(S1LinearSampler, IN.UV2)*2; //Darle m�s potencia para que se acerque m�s a lo que se ve en el MAX
#elif defined( USE_RNM )
	float3 l_LightMap = GetRadiosityNormalMapBySamplers(FaceNn, FaceNormal, FaceTangent, FaceBinormal, IN.UV2, S2LinearSampler, S3LinearSampler, S4LinearSampler);
	l_AmbientColor = l_DiffuseColor * l_LightMap * 2; //Darle m�s potencia para que se acerque m�s a lo que se ve en el MAX
#else
	l_AmbientColor = l_DiffuseColor.xyz * g_AmbientLight;
#endif

#if defined( USE_CAL3D_HW )

	//OUT.Ambient=float4( 1, 0, 0, 1);
	
	// Light probes
	float3 l_LightProbeColorInit = float3(0.0, 0.0, 0.0);
	float3 lFixedNormal = l_Normal;
	for ( int i = 0; i<52; i+=13)
	{
		float3 l_LightProbeX = (lFixedNormal.x > 0) ? tex2D(LightProbeSampler, float2(g_LightProbes[1+i], g_LightProbes[2+i])) : tex2D(LightProbeSampler, float2(g_LightProbes[7+i], g_LightProbes[8+i]));
		l_LightProbeX *= lFixedNormal.x;
		
		float3 l_LightProbeY = (lFixedNormal.y > 0) ? tex2D(LightProbeSampler, float2(g_LightProbes[3+i], g_LightProbes[4+i])) : tex2D(LightProbeSampler, float2(g_LightProbes[9+i], g_LightProbes[10+i]));
		l_LightProbeY *= lFixedNormal.y;
		
		float3 l_LightProbeZ = (lFixedNormal.z > 0) ? tex2D(LightProbeSampler, float2(g_LightProbes[5+i], g_LightProbes[6+i])) : tex2D(LightProbeSampler, float2(g_LightProbes[11+i], g_LightProbes[12+i]));
		l_LightProbeZ *= lFixedNormal.z;
		
		l_LightProbeColorInit += g_LightProbes[i] * ( l_LightProbeX + l_LightProbeY + l_LightProbeZ );
	}
	float3 l_LightProbeColorFinal = float3(0.0, 0.0, 0.0);
	for ( int i = 52; i<104; i+=13)
	{
		float3 l_LightProbeX = (lFixedNormal.x > 0) ? tex2D(LightProbeSampler, float2(g_LightProbes[1+i], g_LightProbes[2+i])) : tex2D(LightProbeSampler, float2(g_LightProbes[7+i], g_LightProbes[8+i]));
		l_LightProbeX *= lFixedNormal.x;
		
		float3 l_LightProbeY = (lFixedNormal.y > 0) ? tex2D(LightProbeSampler, float2(g_LightProbes[3+i], g_LightProbes[4+i])) : tex2D(LightProbeSampler, float2(g_LightProbes[9+i], g_LightProbes[10+i]));
		l_LightProbeY *= lFixedNormal.y;
		
		float3 l_LightProbeZ = (lFixedNormal.z > 0) ? tex2D(LightProbeSampler, float2(g_LightProbes[5+i], g_LightProbes[6+i])) : tex2D(LightProbeSampler, float2(g_LightProbes[11+i], g_LightProbes[12+i]));
		l_LightProbeZ *= lFixedNormal.z;
		
		l_LightProbeColorFinal += g_LightProbes[i] * ( l_LightProbeX + l_LightProbeY + l_LightProbeZ );
	}
	float lPercentage = g_LightProbes[104];
	float3 l_LightProbeColor = l_LightProbeColorInit * (1 - lPercentage) + l_LightProbeColorFinal * lPercentage;
	l_AmbientColor = saturate(l_LightProbeColor + g_AmbientLight);
	
	l_AmbientColor *= l_DiffuseColor;
	
	OUT.Ambient=float4( l_AmbientColor, g_SpecularExponent );

#else
	OUT.Ambient=float4( l_AmbientColor, g_SpecularExponent );
#endif
	
	OUT.Albedo = float4(l_DiffuseColor.xyz, g_SpecularFactor);
	OUT.Normal = float4(Normal2Texture(l_Normal), g_SelfIlumAmount);
	OUT.Depth  = IN.WorldPosition.z/IN.WorldPosition.w;
	
	return OUT;
}

technique TECHNIQUE_NAME
{
	pass p0
	{
#if defined( USE_CAL3D_HW )
		CullMode = None;
#endif
	
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}
