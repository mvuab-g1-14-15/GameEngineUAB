#include "vertex_types.fxh"
#include "samplers.fxh"
#include "globals.fxh"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TNORMAL_T1_VERTEX_PS mainTNORMAL_T1_VERTEX(TNORMAL_T1_VERTEX IN)
{
	TNORMAL_T1_VERTEX_PS OUT=(TNORMAL_T1_VERTEX_PS)0;
    OUT.HPosition = mul(float4(IN.Position, 1.0), g_WorldViewProj);
	OUT.WorldPosition = OUT.HPosition;
    OUT.Normal = mul(IN.Normal, (float3x3)g_WorldMatrix);
	OUT.WorldNormal = OUT.Normal;
	OUT.UV = IN.UV;
    return OUT;
}
TMultiRenderTargetPixel mainTNORMAL_T1_VERTEX_PS(TNORMAL_T1_VERTEX_PS IN) : COLOR
{
	TMultiRenderTargetPixel OUT=(TMultiRenderTargetPixel)0;
	float4 l_DiffuseColor = tex2D(S0LinearSampler,IN.UV);
	if(l_DiffuseColor.a<0.5)
		clip(-1);

	OUT.Albedo=float4(l_DiffuseColor.xyz, g_SpecularFactor);
	OUT.Ambient=float4(l_DiffuseColor.xyz*g_AmbientLight, g_SpecularExponent );
	OUT.Normal=float4(Normal2Texture(normalize(IN.WorldNormal)), 1);
	OUT.Depth=IN.WorldPosition.z/IN.WorldPosition.w;
	
	return OUT;
}
technique GenerateGBufferTechnique
{
	pass p0
	{
		VertexShader = compile vs_3_0 mainTNORMAL_T1_VERTEX();
		PixelShader = compile ps_3_0 mainTNORMAL_T1_VERTEX_PS();
	}
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TNORMAL_TAN_BI_T1_VERTEX_PS mainTNORMAL_TAN_BI_T1_VERTEX(TNORMAL_TAN_BI_T1_VERTEX IN)
{
	TNORMAL_TAN_BI_T1_VERTEX_PS OUT=(TNORMAL_TAN_BI_T1_VERTEX_PS)0;
    OUT.HPosition=mul(float4(IN.Position, 1.0), g_WorldViewProj);
    OUT.UV=IN.UV;
    OUT.Normal=mul(IN.Normal, (float3x3)g_WorldMatrix);
    OUT.WorldPosition=OUT.HPosition;
	OUT.WorldTangent = float4( mul(IN.Tangent.xyz, (float3x3)g_WorldMatrix), 1.0);
	OUT.WorldBinormal = float4(mul(IN.Binormal.xyz, (float3x3)g_WorldMatrix),1.0);
    return OUT;
}
TMultiRenderTargetPixel mainTNORMAL_TAN_BI_T1_VERTEX_PS(TNORMAL_TAN_BI_T1_VERTEX_PS IN) : COLOR
{
	TMultiRenderTargetPixel OUT=(TMultiRenderTargetPixel)0;
	float4 l_DiffuseColor = tex2D(S0LinearSampler,IN.UV);
	if(l_DiffuseColor.a<0.5)
		clip(-1);

	OUT.Albedo=float4(l_DiffuseColor.xyz, g_SpecularFactor);
	OUT.Ambient=float4(l_DiffuseColor.xyz*g_AmbientLight, g_SpecularExponent );
	
	// Obtain the normal of the face and the position
	float3 l_Normal= normalize(IN.Normal);
	float3 Tn=normalize(IN.WorldTangent);
	float3 Bn=normalize(IN.WorldBinormal);
	float3 bump=g_Bump*(tex2D(S1LinearSampler,IN.UV).rgb - float3(0.5,0.5,0.5));
	l_Normal = l_Normal + bump.x*Tn + bump.y*Bn;
	l_Normal = normalize(l_Normal);
	
	OUT.Normal=float4(Normal2Texture(l_Normal), 1);
	OUT.Depth=IN.WorldPosition.z/IN.WorldPosition.w;
	
	return OUT;
}
technique GenerateGBufferNormalTechnique
{
	pass p0
	{
		VertexShader = compile vs_3_0 mainTNORMAL_TAN_BI_T1_VERTEX();
		PixelShader = compile ps_3_0 mainTNORMAL_TAN_BI_T1_VERTEX_PS();
	}
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
