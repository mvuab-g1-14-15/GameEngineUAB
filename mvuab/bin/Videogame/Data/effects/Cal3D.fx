#include "globals.fxh"

struct CAL3D_HW_VERTEX_VS
{
	float3 Position : POSITION0;
	float4 Weight   : BLENDWEIGHT0;
	float4 Indices  : BLENDINDICES0;
	
	float4 Normal   : NORMAL0;
	float4 Tangent  : TANGENT0;
	float4 BiNormal : BINORMAL0;
	float2 TexCoord : TEXCOORD0;
};

struct CAL3D_HW_VERTEX_PS
{
	float4 HPosition : POSITION0;
	float2 UV        : TEXCOORD0;
	
	float3 WorldNormal   : TEXCOORD1;
	float3 WorldPosition : TEXCOORD2;
	float3 WorldTangent  : TEXCOORD3;
	float3 WorldBinormal : TEXCOORD4;
};

sampler DiffuseTextureSampler : register( s0 ) = sampler_state
{
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

float3 CalcAnimtedPos(float4 Position, float4 Indices, float4 Weight)
{
	float3 l_Position = 0;
	
	l_Position  = mul(g_Bones[Indices.x], Position) * Weight.x;
	l_Position += mul(g_Bones[Indices.y], Position) * Weight.y;
	l_Position += mul(g_Bones[Indices.z], Position) * Weight.z;
	l_Position += mul(g_Bones[Indices.w], Position) * Weight.w;
	
	return l_Position;
}

void CalcAnimatedNormalTangent(float3 Normal, float3 Tangent, float4 Indices, float4 Weight, out float3 OutNormal, out float3 OutTangent)
{
	OutNormal  = 0;
	OutTangent = 0;
	float3x3 m;
	
	m[0].xyz = g_Bones[Indices.x][0].xyz;
	m[1].xyz = g_Bones[Indices.x][1].xyz;
	m[2].xyz = g_Bones[Indices.x][2].xyz;
	
	OutNormal  += mul(m, Normal.xyz)* Weight.x;
	OutTangent += mul(m, Tangent.xyz)* Weight.x;
	
	m[0].xyz = g_Bones[Indices.y][0].xyz;
	m[1].xyz = g_Bones[Indices.y][1].xyz;
	m[2].xyz = g_Bones[Indices.y][2].xyz;
	
	OutNormal  += mul(m, Normal.xyz)* Weight.y;
	OutTangent += mul(m, Tangent.xyz)* Weight.y;
	
	m[0].xyz = g_Bones[Indices.z][0].xyz;
	m[1].xyz = g_Bones[Indices.z][1].xyz;
	m[2].xyz = g_Bones[Indices.z][2].xyz;
	
	OutNormal += mul(m, Normal.xyz)* Weight.z;
	OutTangent += mul(m, Tangent.xyz)* Weight.z;
	
	m[0].xyz = g_Bones[Indices.w][0].xyz;
	m[1].xyz = g_Bones[Indices.w][1].xyz;
	m[2].xyz = g_Bones[Indices.w][2].xyz;
	
	OutNormal  += mul(m, Normal.xyz)* Weight.w;
	OutTangent += mul(m, Tangent.xyz)* Weight.w;
	
	OutNormal  = normalize(OutNormal);
	OutTangent = normalize(OutTangent);
}

CAL3D_HW_VERTEX_PS RenderCal3DHWVS(CAL3D_HW_VERTEX_VS IN)
{
	CAL3D_HW_VERTEX_PS OUT =(CAL3D_HW_VERTEX_PS) 0;
	float3 l_Normal= 0;
	float3 l_Tangent=0;
	
	//CalcAnimatedNormalTangent(IN.Normal.xyz, IN.Tangent.xyz, IN.Indices, IN.Weight, l_Normal, l_Tangent);
	float3 l_Position = CalcAnimtedPos(float4(IN.Position.xyz,1.0), IN.Indices, IN.Weight);
	float4 l_WorldPosition = float4(l_Position, 1.0);
	
	OUT.WorldPosition = mul(l_WorldPosition,g_WorldMatrix);
	//OUT.WorldNormal = normalize(mul(l_Normal,g_WorldMatrix));
	
	//OUT.WorldTangent = normalize(mul(l_Tangent,g_WorldMatrix));
	//OUT.WorldBinormal = mul(cross(l_Tangent,l_Normal),(float3x3)g_WorldMatrix);
	
	//OUT.UV = IN.TexCoord.xy;
	OUT.HPosition = mul(l_WorldPosition, g_WorldViewProj );
	
	return OUT;
}

float4 RenderCal3DHWPS(CAL3D_HW_VERTEX_PS IN) : COLOR
{
	return float4(1.0, 1.0, 1.0, 1.0);
}

technique Cal3DTechnique
{
	pass p0
	{
		VertexShader = compile vs_3_0 RenderCal3DHWVS();
		PixelShader = compile ps_3_0 RenderCal3DHWPS();
	}
}
