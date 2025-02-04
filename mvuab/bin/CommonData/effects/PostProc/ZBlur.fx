#include "../samplers.fxh"
#include "../globals.fxh"

float4 ZBlurPS(in float2 UV : TEXCOORD0) : COLOR
{
    float4 l_Color = float4(0.0, 0.0, 0.0, 0.0);
    float4 l_DepthMap = tex2D(S1LinearClampSampler, UV);

    float3 l_CameraPosition = g_ViewInverseMatrix[3].xyz;
    float3 l_WorldPosition = GetPositionFromZDepthView(l_DepthMap, float2(0.0, 0.0), g_ViewInverseMatrix, g_ProjectionInverseMatrix);

    float l_Distance = length(l_WorldPosition - l_CameraPosition);
    float l_Blur = 1.0;

    if(l_Distance < g_ZBlurFocalStart)
    {
        l_Blur = max(l_Distance / g_ZBlurFocalStart, g_ConstantBlur);
    }
    else if(l_Distance > g_ZBlurFocalEnd)
    {
        l_Blur = max(1.0 - (l_Distance - g_ZBlurFocalEnd) / g_ZBlurEnd, g_ConstantBlur);
    }

    const float2 delta[8] =
    {
        float2(-1, 1), float2(1, -1), float2(-1, 1),float2(1, 1),
        float2(-1, 0), float2(1,  0), float2(0, -1),float2(0, 1)
    };

    float2 l_PixelInc = 2 * float2(1.0/1280, 1.0 / 720); //width and height hardcoded
    float4 l_AlbedoColor = tex2D(S0LinearClampSampler, UV);

    for(int i = 0; i < 8; i++)
    {
        l_Color += tex2D(S0LinearClampSampler, UV + delta[i] * l_PixelInc) * (1.0 - l_Blur) + l_Blur * l_AlbedoColor;
    }

    l_Color = l_Color * (1.0 / 8.0);
    return l_Color;
}

technique ZBlurTechnique
{
    pass p0
    {
        AlphaBlendEnable = false;
        CullMode = CCW;

        PixelShader = compile ps_3_0 ZBlurPS();
    }
}