#include "Character.h"
#include "Actor\PhysicController.h"
#include "Utils\PhysicUserData.h"
#include "PhysicsManager.h"
#include "Math\LerpAnimator3D.h"
#include "Timer\Timer.h"
#include "Core.h"
#include "RenderableObject\RenderableObjectsManager.h"
#include "RenderableObject\RenderableObjectsLayersManager.h"
#include "PhysicsManager.h"

CCharacter::CCharacter( const std::string& Name )
  : CName( Name ), CObject3D()
  , m_Speed( 0.06f )
  , m_Life( 0.0f )
  , m_init( false )
{
}

CCharacter::~CCharacter()
{
  Destroy();
}

void CCharacter::Destroy()
{
  if ( m_init )
  {
    CPhysicUserData* l_PUD = mController->GetUserData();
    CHECKED_DELETE( l_PUD );
    CHECKED_DELETE( mController );
  }
}

void CCharacter::ExecuteAI()
{
}

void CCharacter::Update()
{
  CPhysicUserData* l_PUD = CPhysicsManager::GetSingletonPtr()->GetUserData( "CharacterController" );
  CPhysicController* l_CharacterController = l_PUD->GetController();
  Math::Vect3f l_Distance = l_CharacterController->GetPosition() - mController->GetPosition();
  float l_Cantidad = ( Math::Utils::Pow2( l_Distance ) ).x;
  l_Cantidad = Math::Utils::Sqrt( l_Cantidad );

  if ( l_Cantidad < 1.0f )
    SetTargetPosition( l_CharacterController->GetPosition() );
  else
    SetTargetPosition( m_TargetPositionOriginal );

  Math::Vect3f l_Position = mController->GetPosition();
  float l_Yaw = mController->GetYaw();
  Math::Vect3f l_Direction( Math::Utils::Cos( l_Yaw ) , 0.0f, Math::Utils::Sin( l_Yaw ) );
  l_Direction = l_Direction.GetNormalized();
  Math::Vect3f l_DessiredDirection = m_TargetPosition - l_Position;
  l_DessiredDirection.y = 0;

  if ( l_DessiredDirection != Math::Vect3f( 0, 0, 0 ) )
    l_DessiredDirection = l_DessiredDirection.GetNormalized();

  Math::CLerpAnimator3D l_Interpolator3D;
  l_Interpolator3D.SetValues( l_Direction, l_DessiredDirection, 1.0f, Math::FUNC_CONSTANT );
  Math::Vect3f l_LookAt;
  l_Interpolator3D.Update( 1, l_LookAt );
  mController->SetYaw( Math::Utils::ATan2( l_LookAt.z, l_LookAt.x ) );
  SetYaw( Math::Utils::ATan2( l_LookAt.z, l_LookAt.x ) );

  if ( l_LookAt != Math::Vect3f( 0, 0, 0 ) )
    l_LookAt = l_LookAt.GetNormalized();

  float n = l_LookAt.Length();
  mController->Move( l_LookAt * m_Speed, deltaTime );
  SetPosition( mController->GetPosition() );
  /* CRenderableObject* l_Box =
     CCore::GetSingletonPtr()->GetRenderableObjectsLayersManager()->GetResource( "solid" )->GetResource( "Pyramid001" );
   l_Box->SetPosition( mController->GetPosition() + Vect3f( 0, 2, 0 ) );
   l_Box->SetYaw( mController->GetYaw() );*/
}

void CCharacter::Render()
{
 /* CGraphicsManager* l_GM = CCore::GetSingletonPtr()->GetGraphicsManager();
  Mat44f m;
  m.Translate( m_Position );
  m.RotByAnglesYXZ( GetYaw(), GetPitch(), GetRoll() );
  l_GM->SetTransform( m );
  l_GM->DrawCapsule(m_Radius, m_Height);
  l_GM->DrawAxis( 1 );
  m.SetIdentity();
  l_GM->SetTransform( m );
  */
}

void CCharacter::Init()
{
  m_Height = 1.0f;
  m_Radius = 0.2f;
  mController = new CPhysicController( m_Radius,
                                         m_Height,
                                         2.0f,
                                         2.0f,
                                         2.0f,
                                         GetCollisionGroup(),
                                         GetPhysicsUserData(),
                                         Math::Vect3f( -10, 0.0, 10 ),
                                         -10.0 );
  mController->GetUserData()->SetPaint(false);
  CPhysicsManager* l_PM = CPhysicsManager::GetSingletonPtr();
  l_PM->AddPhysicController( mController );
  m_init = true;
}
void CCharacter::Init( CXMLTreeNode& Node )
{
  m_AIPath = Node.GetPszProperty( "lua_path", "no_path" );
  mController = new CPhysicController( Node.GetFloatProperty( "radius", 0.2f ),
                                         Node.GetFloatProperty( "height", 1.0f ),
                                         Node.GetFloatProperty( "slope", 0.2f ),
                                         Node.GetFloatProperty( "skin_width", 0.01f ),
                                         Node.GetFloatProperty( "step", 0.5f ),
                                         GetCollisionGroup(), GetPhysicsUserData(),
                                         Node.GetVect3fProperty( "pos", Math::Vect3f( 0, 4.0, 0 ) ),
                                         Node.GetFloatProperty( "gravity", -10.0f ) );

  CPhysicsManager* l_PM = CPhysicsManager::GetSingletonPtr();
  l_PM->AddPhysicController( mController );
  SetPosition( mController->GetPosition() );
  SetTargetPosition( mController->GetPosition() );  
  SetTargetPositionOriginal( mController->GetPosition() );
  m_init = true;
}

ECollisionGroup CCharacter::GetCollisionGroup()
{
  return ECG_PLAYER;
}

CPhysicUserData* CCharacter::GetPhysicsUserData()
{
  std::string& l_UserDataName = m_Name + std::string( "UserData" );
  CPhysicUserData* l_PUD = new CPhysicUserData( l_UserDataName );
  // TODO CHECK THIS
  l_PUD->SetPaint( true );
  l_PUD->SetColor( Math::CColor(0.3, 0.3, 0.3, 1) );
  // TODO CHECK THIS
  return l_PUD;
}