#include "globals.fxh"
#include "vertex_types.fxh"
#include "samplers.fxh"

struct VertexPS
{
    float4 HPosition		: POSITION;
    float3 Normal 			: TEXCOORD0;
	float3 WorldPosition 	: TEXCOORD1;
	float2 UV				: TEXCOORD2;
};

VertexPS RenderVS(TNORMAL_T1_VERTEX IN)
{
    VertexPS OUT=(VertexPS)0;
    OUT.HPosition=mul(float4(IN.Position, 1.0), g_WorldViewProj);
    OUT.Normal=mul(IN.Normal, (float3x3)g_WorldMatrix);
    OUT.WorldPosition=mul(float4(IN.Position,1.0), g_WorldMatrix);
	OUT.UV = IN.UV;
    return OUT;
}

float4 RenderPS(VertexPS IN) : COLOR
{
	// Obtain the normal of the face and the position
	float3 l_Normal= normalize(IN.Normal);
			
	float3 l_Position = IN.WorldPosition;

	float4 l_DiffuseColor = float4(0.2, 0.2, 0.4, 1.0);
	
	float4 l_LightsContrib = float4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < MAX_LIGHTS_BY_SHADER; i++)
    {
        if(g_LightsEnabled[i] == 1) 
        {
			float3 l_LightDirection = normalize(l_Position-g_LightsPosition[i]);
			float l_Attenuation = DistanceAttenuation(i, l_LightDirection );
            if(OMNI_LIGHT == g_LightsType[i])
            {
            }
            if(DIR_LIGHT == g_LightsType[i])
            {
				l_LightDirection = normalize(g_LightsDirection[i]);
            }
            else if(SPOT_LIGHT == g_LightsType[i])
            {	
				// Modify the distance attenuation with the spot attenuation
				l_Attenuation = l_Attenuation * SpotAttenuation(i, l_LightDirection);
            }

			float3 l_Hn=normalize(normalize(g_CameraPosition-l_Position)-l_LightDirection);
			float3 l_DiffuseContrib = saturate(dot(l_Normal,-l_LightDirection)) * l_Attenuation * g_LightsColor[i];
			float l_SpecularContrib = pow(saturate(dot(l_Normal,l_Hn)),g_SpecularExponent) * l_Attenuation * g_LightsColor[i];
			l_LightsContrib = l_LightsContrib + float4( l_DiffuseContrib + l_SpecularContrib, 1.0);
        }
    }
	
	float3 l_CameraPosition = g_ViewInverseMatrix[3].xyz;
	float3 l_CameraToPixel = normalize(l_CameraPosition - l_Position);
	
	float3 l_ReflectVector = reflect(l_CameraToPixel, l_Normal);
	float4 l_EnvironmentColor = texCUBE(S1CubeLinearSampler, l_ReflectVector);
	
	float4 l_PixelColor = l_LightsContrib * l_DiffuseColor + l_EnvironmentColor;
	
	return l_PixelColor;
}

technique CubeReflectionTechnique
{
    pass p0
    {
        VertexShader =compile vs_3_0 RenderVS();
        PixelShader = compile ps_3_0 RenderPS();
    }
}