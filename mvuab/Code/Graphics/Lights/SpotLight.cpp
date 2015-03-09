#include "SpotLight.h"
#include "GraphicsManager.h"
#include "Effects\EffectManager.h"

#define D3DFVF_CUSTOMVERTEXLIGHT (D3DFVF_XYZ|D3DFVF_DIFFUSE)
typedef struct CUSTOMVERTEXLIGHT
{
  float x, y, z;
  DWORD color;

  static unsigned int getFlags()
  {
    return ( D3DFVF_CUSTOMVERTEXLIGHT );
  }
} CUSTOMVERTEXLIGHT;

CSpotLight::CSpotLight( CXMLTreeNode node )
  : CDirectionalLight( node )
  , m_Angle( node.GetFloatProperty( "angle", 0 ) )
  , m_FallOff( node.GetFloatProperty( "fall_off", 0 ) )
{
  SetType( CLight::SPOT );
}

void CSpotLight::SetFallOff( const float FallOff )
{
  m_FallOff = FallOff;
}

void CSpotLight::SetAngle( const float Angle )
{
  m_Angle = Angle;
}

float CSpotLight::GetAngle() const
{
  return m_Angle;
}

float CSpotLight::GetFallOff() const
{
  return m_FallOff;
}

void CSpotLight::Render()
{
  LPDIRECT3DDEVICE9 l_Device = GraphicsInstance->GetDevice();
  D3DXMATRIX matrix = GetTransform().GetD3DXMatrix();
  D3DXMATRIX translation;
  D3DXVECTOR3 eye( m_Position.x, m_Position.y, m_Position.z );
  D3DXMatrixTranslation( &translation, eye.x , eye.y , eye.z );
  GraphicsInstance->SetTransform( matrix );
  GraphicsInstance->DrawSphere( 0.5f, Math::colMAGENTA );
  D3DXMatrixTranslation( &matrix, 0, 0, 0 );
  GraphicsInstance->SetTransform( matrix );
  GraphicsInstance->DrawLine( m_Position, m_Position + m_Direction.GetNormalized() * 5.0f,
                              Math::colMAGENTA );
}

void CSpotLight::SetShadowMap( CGraphicsManager* GM )
{
    D3DXMATRIX l_View;
    D3DXMATRIX l_Projection;

    D3DXVECTOR3 l_Eye(m_Position.x, m_Position.y, m_Position.z);
    m_Direction.Normalize();
    Vect3f l_LookAtV3f = m_Position + m_Direction;
        
    D3DXVECTOR3 l_LookAt(l_LookAtV3f.x, l_LookAtV3f.y, l_LookAtV3f.z);
    D3DXVECTOR3 l_VUP;

    float l_Value = Math::v3fY*l_LookAtV3f;
    if ((Math::v3fY*m_Direction)> 0.995f || (Math::v3fNEGY*m_Direction)>0.995f)
    {
        Math::Vect3f l_Cross = v3fX ^ m_Direction;
        Math::Vect3f l_VUpV3f = m_Direction ^ l_Cross;
        l_VUP = D3DXVECTOR3(l_VUpV3f.x, l_VUpV3f.y, l_VUpV3f.z);
    }
    else
    {
        Math::Vect3f l_Cross = Math::v3fY ^ m_Direction;
        Math::Vect3f l_VUpV3f = m_Direction ^ l_Cross;
        l_VUP = D3DXVECTOR3(l_VUpV3f.x, l_VUpV3f.y, l_VUpV3f.z);
    }

    D3DXMatrixLookAtLH(&l_View, &l_Eye, &l_LookAt, &l_VUP);
    m_ViewShadowMap = Math::Mat44f(l_View);
    D3DXMatrixPerspectiveFovLH(&l_Projection, m_FallOff, 1.0f, 1.0f, m_EndRangeAttenuation);
    m_ProjectionShadowMap = Math::Mat44f(l_Projection);

    CEffectManager *l_EffectManager = CEffectManager::GetSingletonPtr();
    l_EffectManager->ActivateCamera(m_ViewShadowMap, m_ProjectionShadowMap, m_Position);
}