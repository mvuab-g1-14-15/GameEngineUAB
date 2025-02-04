#include "Effect.h"

#include "EffectManager.h"
#include "EffectTechnique.h"
#include "GraphicsManager.h"
#include "Lights/DirectionalLight.h"
#include "Lights/LightManager.h"
#include "Lights/SpotLight.h"
#include "Logger\Logger.h"
#include "Utils/BaseUtils.h"
#include "Utils/StringUtils.h"
#include "EngineManagers.h"
#include "Effects\Defines.h"

CEffect::CEffect(const std::string& EffectName)
    : CName(EffectName)
    , m_FileName("")
    , m_Effect(0)
    , CTOR_EFFECT_PARAMETER(ViewToLightProjectionMatrixParameter)
    , CTOR_EFFECT_PARAMETER(LightEnabledParameter)
    , CTOR_EFFECT_PARAMETER(LightsTypeParameter)
    , CTOR_EFFECT_PARAMETER(LightsPositionParameter)
    , CTOR_EFFECT_PARAMETER(LightsDirectionParameter)
    , CTOR_EFFECT_PARAMETER(LightsAngleParameter)
    , CTOR_EFFECT_PARAMETER(LightsColorParameter)
    , CTOR_EFFECT_PARAMETER(LightsFallOffParameter)
    , CTOR_EFFECT_PARAMETER(LightsStartRangeAttenuationParameter)
    , CTOR_EFFECT_PARAMETER(LightsEndRangeAttenuationParameter)
    , CTOR_EFFECT_PARAMETER(BonesParameter)
    , CTOR_EFFECT_PARAMETER(LightProbesParameter)
    , CTOR_EFFECT_PARAMETER(DebugColor)
    , CTOR_EFFECT_PARAMETER(UseDebugColor)
    , CTOR_EFFECT_PARAMETER(Size)
    , CTOR_EFFECT_PARAMETER(Angle)
    , CTOR_EFFECT_PARAMETER(Alpha)
    , CTOR_EFFECT_PARAMETER(Color)
    , CTOR_EFFECT_PARAMETER(InverseWorldMatrix)
    , CTOR_EFFECT_PARAMETER(WorldMatrix)
    , CTOR_EFFECT_PARAMETER(WVMatrix)
    , CTOR_EFFECT_PARAMETER(WVPMatrix)
    , CTOR_EFFECT_PARAMETER(Direction)
    , CTOR_EFFECT_PARAMETER(Time)
    , CTOR_EFFECT_PARAMETER(ViewMatrix)
    , CTOR_EFFECT_PARAMETER(ProjectionMatrix)
    , CTOR_EFFECT_PARAMETER(VPMatrix)
    , CTOR_EFFECT_PARAMETER(InverseViewMatrix)
    , CTOR_EFFECT_PARAMETER(InverseProjectionMatrix)
    , CTOR_EFFECT_PARAMETER(CameraPosition)
    , CTOR_EFFECT_PARAMETER(AmbientLight)
    , CTOR_EFFECT_PARAMETER(FBWidth)
    , CTOR_EFFECT_PARAMETER(FBHeight)
    , CTOR_EFFECT_PARAMETER(DeltaTime)
    , CTOR_EFFECT_PARAMETER(FlipUVVertical)
    , CTOR_EFFECT_PARAMETER(FlipUVHorizontal)
    // Percentages of lights
    , CTOR_EFFECT_PARAMETER(PercentageTexture1)
    , CTOR_EFFECT_PARAMETER(PercentageTexture2)
    , CTOR_EFFECT_PARAMETER(SelfIlumAmount)
    , CTOR_EFFECT_PARAMETER(Life)
    // Rays Of God
    , CTOR_EFFECT_PARAMETER(RaysWeight)
    // ZBlur
    , CTOR_EFFECT_PARAMETER(FocalStart)
    , CTOR_EFFECT_PARAMETER(FocalEnd)
    , CTOR_EFFECT_PARAMETER(BlurConstant)
    , CTOR_EFFECT_PARAMETER(BlurEnd)
{
    ResetLightsHandle();
}

CEffect::~CEffect()
{
    CHECKED_RELEASE(m_Effect);
}

void CEffect::SetNullParameters()
{
    // Reset the parameters
    RESET_EFFECT_PARAMETER(SelfIlumAmount);
    RESET_EFFECT_PARAMETER(Time);
    RESET_EFFECT_PARAMETER(InverseWorldMatrix);
    RESET_EFFECT_PARAMETER(WorldMatrix);
    RESET_EFFECT_PARAMETER(WVMatrix);
    RESET_EFFECT_PARAMETER(WVPMatrix);
    RESET_EFFECT_PARAMETER(Direction);
    RESET_EFFECT_PARAMETER(VPMatrix);
    RESET_EFFECT_PARAMETER(FlipUVVertical);
    RESET_EFFECT_PARAMETER(FlipUVHorizontal);
    RESET_EFFECT_PARAMETER(ViewMatrix);
    RESET_EFFECT_PARAMETER(ProjectionMatrix);
    RESET_EFFECT_PARAMETER(InverseViewMatrix);
    RESET_EFFECT_PARAMETER(InverseProjectionMatrix);
    RESET_EFFECT_PARAMETER(CameraPosition);
    RESET_EFFECT_PARAMETER(AmbientLight);
    RESET_EFFECT_PARAMETER(FBWidth);
    RESET_EFFECT_PARAMETER(FBHeight);
    RESET_EFFECT_PARAMETER(DeltaTime);
    RESET_EFFECT_PARAMETER(Size)
    RESET_EFFECT_PARAMETER(Angle)
    RESET_EFFECT_PARAMETER(Alpha)
    RESET_EFFECT_PARAMETER(Color)
    RESET_EFFECT_PARAMETER(ViewToLightProjectionMatrixParameter)
    RESET_EFFECT_PARAMETER(LightEnabledParameter)
    RESET_EFFECT_PARAMETER(LightsTypeParameter)
    RESET_EFFECT_PARAMETER(LightsPositionParameter)
    RESET_EFFECT_PARAMETER(LightsDirectionParameter)
    RESET_EFFECT_PARAMETER(LightsAngleParameter)
    RESET_EFFECT_PARAMETER(LightsColorParameter)
    RESET_EFFECT_PARAMETER(LightsFallOffParameter)
    RESET_EFFECT_PARAMETER(LightsStartRangeAttenuationParameter)
    RESET_EFFECT_PARAMETER(LightsEndRangeAttenuationParameter)
    RESET_EFFECT_PARAMETER(BonesParameter)
    RESET_EFFECT_PARAMETER(LightProbesParameter)
    RESET_EFFECT_PARAMETER(PercentageTexture1);
    RESET_EFFECT_PARAMETER(PercentageTexture2);
    RESET_EFFECT_PARAMETER(Life);

    // Rays Of God
    RESET_EFFECT_PARAMETER(RaysWeight);

    // ZBlur
    RESET_EFFECT_PARAMETER(FocalStart);
    RESET_EFFECT_PARAMETER(FocalEnd);
    RESET_EFFECT_PARAMETER(BlurConstant);
    RESET_EFFECT_PARAMETER(BlurEnd);

    ResetLightsHandle();
}

void CEffect::LinkSemantics()
{
    LINK_EFFECT_PARAMETER(Time);
    LINK_EFFECT_PARAMETER(Size);
    LINK_EFFECT_PARAMETER(Angle);
    LINK_EFFECT_PARAMETER(Alpha);
    LINK_EFFECT_PARAMETER(Color);
    LINK_EFFECT_PARAMETER(Direction);
    LINK_EFFECT_PARAMETER(InverseWorldMatrix);
    LINK_EFFECT_PARAMETER(WorldMatrix);
    LINK_EFFECT_PARAMETER(WVMatrix);
    LINK_EFFECT_PARAMETER(WVPMatrix);
    LINK_EFFECT_PARAMETER(VPMatrix);
    LINK_EFFECT_PARAMETER(ViewMatrix);
    LINK_EFFECT_PARAMETER(ProjectionMatrix);
    LINK_EFFECT_PARAMETER(InverseViewMatrix);
    LINK_EFFECT_PARAMETER(InverseProjectionMatrix);
    LINK_EFFECT_PARAMETER(CameraPosition);
    LINK_EFFECT_PARAMETER(AmbientLight);
    LINK_EFFECT_PARAMETER(FBWidth);
    LINK_EFFECT_PARAMETER(FBHeight);
    LINK_EFFECT_PARAMETER(DeltaTime);
    LINK_EFFECT_PARAMETER(FlipUVVertical);
    LINK_EFFECT_PARAMETER(FlipUVHorizontal);
    LINK_EFFECT_PARAMETER(PercentageTexture1);
    LINK_EFFECT_PARAMETER(PercentageTexture2);
    LINK_EFFECT_PARAMETER(SelfIlumAmount);
    LINK_EFFECT_PARAMETER(Life);

    GetParameterBySemantic(ViewToLightProjectionMatrixParameterStr,  m_ViewToLightProjectionMatrixParameter);
    GetParameterBySemantic(LightEnabledParameterStr, m_LightEnabledParameter);
    GetParameterBySemantic(LightsSpecularIntensityParameterStr, m_LightsSpecularIntensityParameter);
    GetParameterBySemantic(LightsIntensityParameterStr, m_LightsIntensityParameter);
    GetParameterBySemantic(LightsTypeParameterStr, m_LightsTypeParameter);
    GetParameterBySemantic(LightsPositionParameterStr, m_LightsPositionParameter);
    GetParameterBySemantic(LightsDirectionParameterStr, m_LightsDirectionParameter);
    GetParameterBySemantic(LightsAngleParameterStr, m_LightsAngleParameter);
    GetParameterBySemantic(LightsColorParameterStr, m_LightsColorParameter);
    GetParameterBySemantic(LightsFallOffParameterStr, m_LightsFallOffParameter);
    GetParameterBySemantic(LightsStartRangeAttenuationParameterStr, m_LightsStartRangeAttenuationParameter);
    GetParameterBySemantic(LightsEndRangeAttenuationParameterStr, m_LightsEndRangeAttenuationParameter);
    GetParameterBySemantic(BonesParameterStr, m_BonesParameter);
    GetParameterBySemantic(LightProbesParameterStr, m_LightProbesParameter);
    GetParameterBySemantic(DebugColorStr, m_DebugColor);
    GetParameterBySemantic(UseDebugColorStr, m_UseDebugColor);
    GetParameterBySemantic(ShadowMapTextureSizeStr, m_ShadowMapTextureSizeParameter);
    GetParameterBySemantic(UseShadowMaskTextureStr, m_UseShadowMaskTextureParameter);
    GetParameterBySemantic(UseShadowStaticStr, m_UseStaticShadowmapParameter);
    GetParameterBySemantic(UseShadowDynamicStr, m_UseDynamicShadowmapParameter);

    // Rays Of God
    GetParameterBySemantic(RaysOfGodWeigth, m_RaysWeight);

    // ZBlur
    GetParameterBySemantic(FocalStartStr, m_FocalStart);
    GetParameterBySemantic(FocalEndStr, m_FocalEnd);
    GetParameterBySemantic(BlurConstantstr, m_BlurConstant);
    GetParameterBySemantic(BlurEndStr, m_BlurEnd);
}

void CEffect::GetParameterBySemantic(const std::string& SemanticName, D3DXHANDLE& a_Handle)
{
    a_Handle = m_Effect->GetParameterBySemantic(0, SemanticName.c_str());
    ASSERT(a_Handle, "Parameter by semantic '%s' wasn't found on effect '%s'", SemanticName.c_str(),
           m_FileName.c_str());
}

void CEffect::GetParameterBySemantic(const char* SemanticName, D3DXHANDLE& a_Handle)
{
    a_Handle = m_Effect->GetParameterBySemantic(0, SemanticName);
    ASSERT(a_Handle, "Parameter by semantic '%s' wasn't found on effect '%s'", SemanticName,
           m_FileName.c_str());
}

void CEffect::Unload()
{
    SetNullParameters();
    CHECKED_RELEASE(m_Effect);
}

bool CEffect::Load(const std::string& aFileName, const std::vector<SDefines>& aDefines)
{
    SetNullParameters();

    bool lOk(true);

    // Store the filename
    m_FileName = aFileName;

    // Obtain the device from the graphics manager and load the effect
    LPDIRECT3DDEVICE9 l_Device = GraphicsInstance->GetDevice();
    DWORD dwShaderFlags = 0;
    dwShaderFlags |= D3DXSHADER_USE_LEGACY_D3DX9_31_DLL;
    //dwShaderFlags |= D3DXFX_LARGEADDRESSAWARE;
    LPD3DXBUFFER l_ErrorBuffer = 0;

#ifdef _DEBUG
    // Transform the defines struct to dx9 struct
    std::vector<D3DXMACRO> lDefines;
    for(uint32 j = 0, lCount = aDefines.size(); j < lCount; ++j)
    {
        D3DXMACRO macro = { aDefines[j].name.c_str(), aDefines[j].description.c_str() };
        lDefines.push_back(macro);
    }

    // Set the end of the vector
    D3DXMACRO null = { NULL, NULL };
    lDefines.push_back(null);

    lOk = D3DXCreateEffectFromFile(l_Device, m_FileName.c_str(), &lDefines[0], 0, dwShaderFlags, 0, &m_Effect, &l_ErrorBuffer) == S_OK;

#else

    lOk = D3DXCreateEffectFromFile(l_Device, m_FileName.c_str(), 0, 0, dwShaderFlags, 0, &m_Effect, &l_ErrorBuffer) == S_OK;

#endif // _DEBUG

    if(l_ErrorBuffer)
    {
        lOk = false;
        ASSERT(lOk, "Error creating effect '%s': \n % s", m_FileName.c_str(), l_ErrorBuffer->GetBufferPointer());
        CHECKED_RELEASE(l_ErrorBuffer);
        CHECKED_RELEASE(m_Effect);
    }

    ASSERT(lOk, "%s", m_FileName.c_str());

    if(lOk)
    {
        LinkSemantics();
    }

    return lOk;
}

D3DXHANDLE CEffect::GetTechniqueByName(const std::string& TechniqueName)
{
    return (m_Effect) ? m_Effect->GetTechniqueByName(TechniqueName.c_str()) : 0;
}

bool CEffect::SetLights(size_t NumOfLights)
{
    ResetLightsHandle();
    CLightManager* l_pLightManager = LightMInstance;

    for(size_t i = 0; i < NumOfLights; ++i)
    {
        CLight* l_pCurrentLight = l_pLightManager->GetLight(i);
        m_LightsEnabled[i] = (BOOL)l_pCurrentLight == NULL ? 0 : 1;

        if(l_pCurrentLight != NULL)
        {
            m_LightsEnabled[i]               = (BOOL) l_pCurrentLight->IsVisible() ? 0 : 1;
            CLight::ELightType l_LightType   = l_pCurrentLight->GetType();
            m_LightsIntensity[i]             = l_pCurrentLight->GetIntensity();
            m_LightsSpecularIntensity[i]     = l_pCurrentLight->GetSpecularIntensity();
            m_LightsType[i]                  = static_cast<int>(l_LightType);
            m_LightsStartRangeAttenuation[i] = l_pCurrentLight->GetStartRangeAttenuation();
            m_LightsEndRangeAttenuation[i]   = l_pCurrentLight->GetEndRangeAttenuation();
            m_LightsPosition[i]              = l_pCurrentLight->GetPosition();
            Math::CColor l_Color             = l_pCurrentLight->GetColor();
            m_LightsColor[i]                 = Math::Vect3f(l_Color.GetRed() , l_Color.GetGreen(), l_Color.GetBlue());

            if(l_LightType == CLight::DIRECTIONAL)
            {
                CDirectionalLight* l_pDirectionalLight = static_cast<CDirectionalLight*>(l_pCurrentLight);
                m_LightsDirection[i]                   = l_pDirectionalLight->GetDirection();
            }
            else if(l_LightType == CLight::SPOT)
            {
                CSpotLight* l_SpotLight = static_cast<CSpotLight*>(l_pCurrentLight);
                m_LightsDirection[i]    = l_SpotLight->GetDirection();
                m_LightsAngle[i]        = l_SpotLight->GetAngle();
                m_LightsFallOff[i]      = l_SpotLight->GetFallOff();
            }

            //Begin the render of the shadow
            l_pCurrentLight->BeginRenderEffectManagerShadowMap(this);
        }
    }

    return true;
}

bool CEffect::SetLight(CLight * aLight)
{
    ResetLightsHandle();

    if(aLight)
    {
        m_LightsEnabled[0] = (BOOL)aLight == NULL ? 0 : 1;
        m_LightsType[0] = static_cast<int>(aLight->GetType());
        m_LightsIntensity[0]             = aLight->GetIntensity();
        m_LightsSpecularIntensity[0]     = aLight->GetSpecularIntensity();
        m_LightsStartRangeAttenuation[0] = aLight->GetStartRangeAttenuation();
        m_LightsEndRangeAttenuation[0] = aLight->GetEndRangeAttenuation();
        m_LightsPosition[0] = aLight->GetPosition();
        Math::CColor l_Color = aLight->GetColor();
        m_LightsColor[0] = Math::Vect3f(l_Color.GetRed(), l_Color.GetGreen(), l_Color.GetBlue());
        CDirectionalLight* l_pDirectionalLight = dynamic_cast<CDirectionalLight*>(aLight);

        if(l_pDirectionalLight)
        {
            m_LightsDirection[0] = l_pDirectionalLight->GetDirection();
            CSpotLight* lSpotLight = dynamic_cast<CSpotLight*>(aLight);

            if(lSpotLight)
            {
                m_LightsAngle[0] = lSpotLight->GetAngle();
                m_LightsFallOff[0] = lSpotLight->GetFallOff();
            }
        }

        //Begin the render of the shadow
        aLight->BeginRenderEffectManagerShadowMap(this);
    }

    return true;
}

bool CEffect::SetViewToLightMatrix(const Math::Mat44f& Matrix)
{
    return (m_Effect->SetMatrix(m_ViewToLightProjectionMatrixParameter,
                                &Matrix.GetD3DXMatrix()) == S_OK);
}

void CEffect::SetFlipUVs(bool aHorizontal, bool aVertical)
{
    HRESULT lRes = SET_BOOL_PARAMETER(FlipUVHorizontal, aHorizontal);
    ASSERT(lRes == S_OK, "Error setting flip horizontal");
    lRes = SET_BOOL_PARAMETER(FlipUVVertical  , aVertical);
    ASSERT(lRes == S_OK, "Error setting flip vertical");
}

void CEffect::SetShadowMapParameters(bool UseShadowMaskTexture, bool UseStaticShadowmap, bool UseDynamicShadowmap)
{
    HRESULT lRes = SET_BOOL_PARAMETER(UseShadowMaskTextureParameter, UseShadowMaskTexture);
    ASSERT(lRes == S_OK, "Error setting shadow mask");
    lRes = SET_BOOL_PARAMETER(UseStaticShadowmapParameter  , UseStaticShadowmap);
    ASSERT(lRes == S_OK, "Error setting static shadow map");
    lRes = SET_BOOL_PARAMETER(UseDynamicShadowmapParameter , UseDynamicShadowmap);
    ASSERT(lRes == S_OK, "Error setting dynamic shadow map");
}

void CEffect::SetDebugColor(bool aUse, const Math::CColor aColor)
{
    HRESULT lRes = SET_BOOL_PARAMETER(UseDebugColor , aUse);
    ASSERT(lRes == S_OK, "Error setting debug color");

    if(aUse)
    {
        float32 l_DebugColor[4];
        l_DebugColor[0] = aColor.GetRed();
        l_DebugColor[1] = aColor.GetGreen();
        l_DebugColor[2] = aColor.GetBlue();
        l_DebugColor[3] = aColor.GetAlpha();
        lRes = m_Effect->SetFloatArray(m_DebugColor, l_DebugColor, sizeof(float) * 4);
        ASSERT(lRes == S_OK, "Error setting float array");
    }
}

void CEffect::ResetLightsHandle()
{
    //Reset all the lights of the effect
    memset(m_LightsEnabled,                  0, sizeof(BOOL) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsType,                     0, sizeof(int32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsIntensity,                0, sizeof(float32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsSpecularIntensity,        0, sizeof(float32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsAngle,                    0, sizeof(float32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsFallOff,                  0, sizeof(float32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsStartRangeAttenuation,    0, sizeof(float32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsEndRangeAttenuation,      0, sizeof(float32) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsPosition,                 0, sizeof(Math::Vect3f) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsDirection,                0, sizeof(Math::Vect3f) * MAX_LIGHTS_BY_SHADER);
    memset(m_LightsColor,                    0, sizeof(Math::Vect3f) * MAX_LIGHTS_BY_SHADER);
}

bool CEffect::SetPercentageTextures(float aPercentageTexture1, float aPercentageTexture2)
{
    HRESULT lRes1 = SET_FLOAT_PARAMETER(PercentageTexture1, aPercentageTexture1);
    ASSERT(lRes1 == S_OK, "Error setting Percentage Texture 1");
    HRESULT lRes2 = SET_FLOAT_PARAMETER(PercentageTexture2, aPercentageTexture2);
    ASSERT(lRes2 == S_OK, "Error setting Percentage Texture 1");
    return lRes1 == S_OK && lRes2 == S_OK;
}

void CEffect::SetSize(float aSize)
{
    HRESULT lRes = SET_FLOAT_PARAMETER(Size, aSize);
    ASSERT(lRes == S_OK, "Error setting size");
}

void CEffect::SetAlpha(float aAlpha)
{
    HRESULT lRes = SET_FLOAT_PARAMETER(Alpha, aAlpha);
    ASSERT(lRes == S_OK, "Error setting alpha");
}

void CEffect::SetDirection(const Math::Vect3f& aDirection)
{
    float32 l_Direction[3];
    l_Direction[0] = aDirection.x;
    l_Direction[1] = aDirection.y;
    l_Direction[2] = aDirection.z;
    HRESULT lRes = m_Effect->SetFloatArray(m_Direction, l_Direction, sizeof(float) * 3);
    ASSERT(lRes == S_OK, "Error setting direction");
}

void CEffect::SetAngle(float aAngle)
{
    HRESULT lRes = SET_FLOAT_PARAMETER(Angle, aAngle);
    ASSERT(lRes == S_OK, "Error setting size");
}

bool CEffect::SetWorldMatrix(const Math::Mat44f& Matrix)
{
    return S_OK == SET_MATRIX_PARAMETER(WorldMatrix, Matrix);
}

bool CEffect::SetInverseWorldMatrix(const Math::Mat44f& Matrix)
{
    return (m_Effect->SetMatrix(m_InverseWorldMatrix, &Matrix.GetInverted().GetD3DXMatrix()) == S_OK);
}

bool CEffect::SetWorldViewMatrix(const Math::Mat44f& Matrix)
{
    return S_OK == SET_MATRIX_PARAMETER(WVMatrix, Matrix);
}

bool CEffect::SetWorldViewProjectionMatrix(const Math::Mat44f& Matrix)
{
    return S_OK == SET_MATRIX_PARAMETER(WVPMatrix, Matrix);
}

bool CEffect::SetCameraPosition(const Math::Vect3f &CameraPosition)
{
    return (m_Effect->SetFloatArray(m_CameraPosition, &CameraPosition.x, 3) == S_OK);
}

bool CEffect::SetViewMatrix(const Math::Mat44f& Matrix)
{
    return S_OK == SET_MATRIX_PARAMETER(ViewMatrix, Matrix);
}

bool CEffect::SetProjectionMatrix(const Math::Mat44f& Matrix)
{
    return S_OK == SET_MATRIX_PARAMETER(ProjectionMatrix, Matrix);
}

bool CEffect::SetInverseViewMatrix(const Math::Mat44f& Matrix)
{
    return (m_Effect->SetMatrix(m_InverseViewMatrix, &Matrix.GetInverted().GetD3DXMatrix()) == S_OK);
}

bool CEffect::SetInverseProjectionMatrix(const Math::Mat44f& Matrix)
{
    return (m_Effect->SetMatrix(m_InverseProjectionMatrix, &Matrix.GetInverted().GetD3DXMatrix()) == S_OK);
}

bool CEffect::SetViewProjectionMatrix(const Math::Mat44f& Matrix)
{
    return S_OK == SET_MATRIX_PARAMETER(VPMatrix, Matrix);
}

bool CEffect::SetAmbientLightColor(const Math::Vect3f &aAmbienLightColor)
{
    float lAmbientColor[3] = {aAmbienLightColor.x, aAmbienLightColor.y, aAmbienLightColor.z};

    HRESULT lRes = m_Effect->SetFloatArray(m_AmbientLight, lAmbientColor, 3);
    ASSERT(lRes == S_OK, "Error setting ambient color");

    return lRes == S_OK;
}

bool CEffect::SetFBSize(const Math::Vect2u aSize)
{
    return (S_OK == SET_INT_PARAMETER(FBWidth, aSize.x)) && (S_OK == SET_INT_PARAMETER(FBHeight, aSize.y));
}

bool CEffect::SetDeltaTime(const float dt)
{
    return S_OK == SET_FLOAT_PARAMETER(DeltaTime, dt);
}

bool CEffect::SetSelfIlumAmount(float aSelfIlumAmount)
{
    HRESULT lRes = SET_FLOAT_PARAMETER(SelfIlumAmount, aSelfIlumAmount);
    ASSERT(lRes == S_OK, "Error setting selfilumamount");
    return lRes == S_OK;
}

void CEffect::SetScatterLights(const std::vector<BOOL> &l_ActiveLights, const std::vector<Math::Vect3f> &l_PosLights, float l_Weight)
{
    ASSERT(l_ActiveLights.size() <= MAX_LIGHTS_BY_SHADER, "Error setting scattering params");

    HRESULT lRes1 = m_Effect->SetBoolArray(m_LightEnabledParameter, &l_ActiveLights[0], l_ActiveLights.size());
    ASSERT(lRes1 == S_OK, "Error setting enabled lights");

    HRESULT lRes2 = m_Effect->SetFloatArray(m_LightsPositionParameter, &l_PosLights[0].x, l_PosLights.size() * 3);
    ASSERT(lRes2 == S_OK, "Error setting lights position");

    HRESULT lRes3 = m_Effect->SetFloat(m_RaysWeight, l_Weight);
    ASSERT(lRes3 == S_OK, "Error setting rays weight");
}

void CEffect::SetBlur(float l_FocalStart, float l_FocalEnd, float l_BlurConstant, float l_BlurEnd)
{
    HRESULT lRes0 = m_Effect->SetFloat(m_FocalStart, l_FocalStart);
    ASSERT(lRes0 == S_OK, "Error setting ZBlur FocalStart");

    HRESULT lRes1 = m_Effect->SetFloat(m_FocalEnd, l_FocalEnd);
    ASSERT(lRes1 == S_OK, "Error setting ZBlur FocalEnd");

    HRESULT lRes2 = m_Effect->SetFloat(m_BlurConstant, l_BlurConstant);
    ASSERT(lRes2 == S_OK, "Error setting ZBlur BlurConstant");

    HRESULT lRes3 = m_Effect->SetFloat(m_BlurEnd, l_BlurEnd);
    ASSERT(lRes3 == S_OK, "Error setting ZBlur BlurEnd");
}

void CEffect::SetLife(float aLife)
{
    HRESULT lRes = SET_FLOAT_PARAMETER(Life, aLife);
    ASSERT(lRes == S_OK, "Error setting life");
}