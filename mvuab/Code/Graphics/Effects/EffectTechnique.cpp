#include "EffectTechnique.h"
#include "EffectManager.h"
#include "Effect.h"
#include "Lights\LightManager.h"
#include "Cameras\CameraManager.h"
#include "Core.h"
#include "Timer\Timer.h"
#include "Logger\Logger.h"

CEffectTechnique::CEffectTechnique( CXMLTreeNode& node )
  : m_TechniqueName( node.GetPszProperty( "name", "" ) ),
    m_EffectName( node.GetPszProperty( "effect", "" ) ),
    m_UseCameraPosition( node.GetBoolProperty( "use_camera_position", false ) ),
    m_UseInverseProjMatrix( node.GetBoolProperty( "use_inverse_projection_matrix", false ) ),
    m_UseInverseViewMatrix( node.GetBoolProperty( "use_inverse_view_matrix", false ) ),
    m_UseInverseWorldMatrix( node.GetBoolProperty( "use_inverse_world_matrix", false ) ),
    m_UseLights( node.GetBoolProperty( "use_lights", false ) ),
    m_UseLightAmbientColor( node.GetBoolProperty( "use_light_ambient_color", false ) ),
    m_UseProjMatrix( node.GetBoolProperty( "use_projection_matrix", false ) ),
    m_UseViewMatrix( node.GetBoolProperty( "use_view_matrix", false ) ),
    m_UseWorldMatrix( node.GetBoolProperty( "use_world_matrix", false ) ),
    m_UseWorldViewMatrix( node.GetBoolProperty( "use_world_view_matrix", false ) ),
    m_UseWorldViewProjectionMatrix( node.GetBoolProperty( "use_world_view_projection_matrix", false ) ),
    m_UseViewProjectionMatrix( node.GetBoolProperty( "use_view_projection_matrix", false ) ),
    m_UseViewToLightProjectionMatrix( node.GetBoolProperty( "use_view_to_light_projection_matrix",
                                      false ) ),
    m_UseTime( node.GetBoolProperty( "use_time", false ) ),
    m_UseDebugColor( node.GetBoolProperty( "use_debug_color", false ) ),
    m_NumOfLights( node.GetIntProperty( "num_of_lights", 0 ) ),
    m_FogStart( node.GetFloatProperty( "fog_start", 0 ) ),
    m_FogEnd( node.GetFloatProperty( "fog_end", 0 ) ),
    m_FogExp( node.GetFloatProperty( "fog_exp", 0 ) ),
    m_FogFun( node.GetIntProperty( "fog_fun", 1 ) ),
    m_DebugColor( Math::colWHITE )
{
  Math::Vect3f l_Weight3 = node.GetVect3fProperty( "weight3", Math::Vect3f( 0, 0, 0 ) );
  Math::Vect2f l_Weight2 = node.GetVect2fProperty( "weight2", Math::Vect2f( 0, 0 ) );
  Math::Vect3f l_Offset3 = node.GetVect3fProperty( "offset3", Math::Vect3f( 0, 0, 0 ) );
  Math::Vect2f l_Offset2 = node.GetVect2fProperty( "offset2", Math::Vect2f( 0, 0 ) );
  m_Weight[0] = l_Weight3.x;
  m_Weight[1] = l_Weight3.y;
  m_Weight[2] = l_Weight3.z;
  m_Weight[3] = l_Weight2.x;
  m_Weight[4] = l_Weight2.y;
  m_Offset[0] = l_Offset3.x;
  m_Offset[1] = l_Offset3.y;
  m_Offset[2] = l_Offset3.z;
  m_Offset[3] = l_Offset2.x;
  m_Offset[4] = l_Offset2.y;
  m_Effect = CEffectManager::GetSingletonPtr()->GetEffect( m_EffectName );
  m_D3DTechnique = ( m_Effect ) ? m_Effect->GetTechniqueByName( m_TechniqueName ) : 0;
}

CEffectTechnique::~CEffectTechnique()
{
  m_Effect = 0;
}

bool CEffectTechnique::BeginRender()
{
  if ( !m_Effect )
    return false;

  // Obtain the direct x effect
  LPD3DXEFFECT l_Effect = m_Effect->GetEffect();
  CEffectManager* l_pEffectManager = CEffectManager::GetSingletonPtr();
  D3DXHANDLE l_Handle;

  if ( m_UseCameraPosition )
  {
    Math::Vect3f l_CameraEye = CCameraManager::GetSingletonPtr()->GetCurrentCamera()->GetPos();
    m_Effect->SetCameraPosition( l_CameraEye );
  }

  if ( m_UseDebugColor )
  {
    l_Handle = m_Effect->GetDebugColor();
    float32 l_DebugColor[4];
    l_DebugColor[0] = m_DebugColor.GetRed();
    l_DebugColor[1] = m_DebugColor.GetGreen();
    l_DebugColor[2] = m_DebugColor.GetBlue();
    l_DebugColor[3] = m_DebugColor.GetAlpha();
    l_Effect->SetFloatArray( l_Handle, l_DebugColor, sizeof( float ) * 4 );
  }

  if ( m_UseTime )
  {
    l_Handle = m_Effect->GetTimeParameter();
    l_Effect->SetFloat( l_Handle, deltaTime );
  }

  if ( m_FogFun == 2 )
  {
    l_Handle = m_Effect->GetFogStart();
    l_Effect->SetFloat( l_Handle, m_FogStart );
    l_Handle = m_Effect->GetFogEnd();
    l_Effect->SetFloat( l_Handle, m_FogEnd );
    l_Handle = m_Effect->GetFogFun();
    l_Effect->SetInt( l_Handle, m_FogFun );
  }

  if ( m_FogFun == 1 || m_FogFun == 3 )
  {
    l_Handle = m_Effect->GetFogExp();
    l_Effect->SetFloat( l_Handle, m_FogExp );
    l_Handle = m_Effect->GetFogFun();
    l_Effect->SetInt( l_Handle, m_FogFun );
  }

  float l_CheckDefault[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  bool UseWeightOffset = false;

  for ( size_t i = 0; i < 5; ++i )
  {
    if ( m_Weight[i] != l_CheckDefault[i] || m_Offset[i] != l_CheckDefault[i] )
      UseWeightOffset = true;
  }

  if ( UseWeightOffset )
  {
    l_Handle = m_Effect->GetWeights();
    l_Effect->SetFloatArray( l_Handle, m_Weight, 5 );
    l_Handle = m_Effect->GetOffsets();
    l_Effect->SetFloatArray( l_Handle, m_Offset, 5 );
  }

  SetupMatrices();
  SetupLights();
  return true;
}

bool CEffectTechnique::Refresh()
{
  m_Effect = CEffectManager::GetSingletonPtr()->GetEffect( m_EffectName );
  m_D3DTechnique = m_Effect->GetTechniqueByName( m_TechniqueName );
  return ( m_Effect != 0 && m_D3DTechnique != 0 );
}

void CEffectTechnique::SetupMatrices()
{
  CEffectManager* l_pEffectManager = CEffectManager::GetSingletonPtr();

  if ( m_UseProjMatrix )
  {
    if ( !m_Effect->SetProjectionMatrix( l_pEffectManager->GetProjectionMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting Projection Matrix" );
  }

  if ( m_UseViewMatrix )
  {
    if ( !m_Effect->SetViewMatrix( l_pEffectManager->GetViewMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting View Matrix" );
  }

  if ( m_UseWorldMatrix )
  {
    if ( !m_Effect->SetWorldMatrix( l_pEffectManager->GetWorldMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting World Matrix" );
  }

  if ( m_UseWorldViewMatrix )
  {
    if ( !m_Effect->SetWorldViewMatrix( l_pEffectManager->GetViewMatrix()
                                        *l_pEffectManager->GetWorldMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting World-View Matrix" );
  }

  if ( m_UseWorldViewProjectionMatrix )
  {
    if ( !m_Effect->SetWorldViewProjectionMatrix( l_pEffectManager->GetProjectionMatrix()
         *l_pEffectManager->GetViewMatrix()*l_pEffectManager->GetWorldMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting World-View-Projection Matrix" );
  }

  if ( m_UseViewProjectionMatrix )
  {
    if ( !m_Effect->SetViewProjectionMatrix( l_pEffectManager->GetProjectionMatrix()
         *l_pEffectManager->GetViewMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting View-Projection Matrix" );
  }

  if ( m_UseViewToLightProjectionMatrix )
  {
    Math::Mat44f l_ViewToLightProjectionMatrix = l_pEffectManager->GetViewMatrix();
    l_ViewToLightProjectionMatrix.Invert();
    l_ViewToLightProjectionMatrix = l_pEffectManager->GetLightViewMatrix() *
                                    l_ViewToLightProjectionMatrix;
    l_ViewToLightProjectionMatrix = l_pEffectManager->GetShadowProjectionMatrix() *
                                    l_ViewToLightProjectionMatrix;

    if ( !m_Effect->SetViewToLightMatrix( l_ViewToLightProjectionMatrix ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting View To Light Matrix" );
  }

  if ( m_UseInverseProjMatrix )
  {
    if ( !m_Effect->SetInverseProjectionMatrix( l_pEffectManager->GetProjectionMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting Inverse Projection Matrix" );
  }

  if ( m_UseInverseViewMatrix )
  {
    if ( !m_Effect->SetInverseViewMatrix( l_pEffectManager->GetViewMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting Inverse View Matrix" );
  }

  if ( m_UseInverseWorldMatrix )
  {
    if ( !m_Effect->SetInverseWorldMatrix( l_pEffectManager->GetWorldMatrix() ) )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupMatrices->Error Setting Inverse World Matrix" );
  }
}

bool CEffectTechnique::SetupLights()
{
  if ( m_UseLights )
  {
    LPD3DXEFFECT l_Effect = m_Effect->GetEffect();
    CEffectManager* l_pEffectManager = CEffectManager::GetSingletonPtr();

    // Check the number of lights
    // If the technique is rendering a deferred shader the lights has been setted up
    // in the command, therfore check here the light number in order to not reset the values
    if ( m_NumOfLights > 1 )
    {
      if ( !m_Effect->SetLights( m_NumOfLights ) )
      {
        CLogger::GetSingletonPtr()->AddNewLog( ELL_ERROR,
                                               "CEffectTechnique::SetupLights->Setting the num of lights to %d", m_NumOfLights );
        return false;
      }
    }

    if ( l_Effect->SetBoolArray( m_Effect->GetLightEnabledParameter(), &m_Effect->GetLightsEnabled()[0],
                                 MAX_LIGHTS_BY_SHADER ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights enabled" );

    if ( l_Effect->SetIntArray( m_Effect->GetLightsTypeParameter(), m_Effect->GetLightsType(),
                                MAX_LIGHTS_BY_SHADER ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights types" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsAngleParameter(), m_Effect->GetLightsAngle(),
                                  MAX_LIGHTS_BY_SHADER ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights  angle parameters" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsFallOffParameter(), m_Effect->GetLightsFallOff(),
                                  MAX_LIGHTS_BY_SHADER ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights fall of parameters" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsStartRangeAttenuationParameter(),
                                  &m_Effect->GetLightsStartRangeAttenuation()[0], MAX_LIGHTS_BY_SHADER ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting start range" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsEndRangeAttenuationParameter(),
                                  &m_Effect->GetLightsEndRangeAttenuation()[0], MAX_LIGHTS_BY_SHADER ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting end range" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsPositionParameter(),
                                  &m_Effect->GetLightsPosition()[0].x, MAX_LIGHTS_BY_SHADER * 3 ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights position" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsDirectionParameter(),
                                  &m_Effect->GetLightsDirection()[0].x, MAX_LIGHTS_BY_SHADER * 3 ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights position" );

    if ( l_Effect->SetFloatArray( m_Effect->GetLightsColorParameter(), &m_Effect->GetLightsColor()[0].x,
                                  MAX_LIGHTS_BY_SHADER * 3 ) != S_OK )
      CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING,
                                             "CEffectTechnique::SetupLights->Error Setting lights colors" );
  }

  if ( m_UseLightAmbientColor )
  {
    // Ask Jordi
  }

  return true;
}

void CEffectTechnique::SetDebugColor( Math::CColor color )
{
  m_DebugColor = color;
}