#include "Core.h"
#include "Actor\PhysicController.h"

//---PhysX Includes---//
#undef min
#undef max
#include "NxPhysics.h"
#include "NxController.h"
#include "NxCapsuleController.h"
#include "NxBoxController.h"

#include "Utils\PhysicUserData.h"
#include "Actor\PhysicController.h"
#include "PhysicsManager.h"
#include "Reports\PhysicsControllerHitReport.h"

CPhysicController::CPhysicController( float _fRadius, float _fHeight, float _fSlope,
                                      float _fSkinwidth, float _fStepOffset
                                      , ECollisionGroup _uiCollisionGroups, CPhysicUserData* _pUserData, const Math::Vect3f& _vPos,
                                      float _fGravity )
  : m_pPhXController( NULL )
  , m_pPhXCapsuleControllerDesc( NULL )
  , m_pPhXBoxControllerDesc( NULL )
  , m_pUserData( _pUserData )
  , m_fRadiusControler( _fRadius )
  , m_fHeightControler( _fHeight )
  , m_fSlopeLimitControler( _fSlope )
  , m_fSkinWidthControler( _fSkinwidth )
  , m_fStepOffsetControler( _fStepOffset )
  , m_fGravity( _fGravity )
  , m_uCollisionGroups( _uiCollisionGroups )
  , m_bUseGravity( true )
  , m_Type( ::CAPSULE )
  , m_bRotado( false )

{
  assert( _pUserData );
  m_pUserData->SetController( this );
  //---- Crear un nuevo NxController----
  m_pPhXCapsuleControllerDesc       = new NxCapsuleControllerDesc();
  CPhysicsControllerHitReport* l_Report = new CPhysicsControllerHitReport();
  m_pReport = l_Report;
  m_pPhXCapsuleControllerDesc->position.x     = _vPos.x;
  m_pPhXCapsuleControllerDesc->position.y     = _vPos.y;
  m_pPhXCapsuleControllerDesc->position.z     = _vPos.z;
  m_pPhXCapsuleControllerDesc->radius       = m_fRadiusControler;
  m_pPhXCapsuleControllerDesc->height       = m_fHeightControler;
  m_pUserData->SetRadius( m_fRadiusControler );
  // La pendiente m�xima que puede subir. 0 lo desactiva. En funci�n del coseno �ngulo. Por defecto 0707
  m_pPhXCapsuleControllerDesc->slopeLimit     = cosf( NxMath::degToRad( m_fSlopeLimitControler ) );
  m_pPhXCapsuleControllerDesc->skinWidth      =
    m_fSkinWidthControler;  // La anchura de la piel. Permite dar un marche para evitar c�lculos excesivos. Defecto: 0.1
  m_pPhXCapsuleControllerDesc->stepOffset     =
    m_fStepOffsetControler; // Altura m�xima que el controler puede subir. Demasiado peque�o + costa de subir obst�culos. Defecto: 0.5
  m_pPhXCapsuleControllerDesc->upDirection    = NX_Y;           // Direcci�n hacia el cielo
  m_pPhXCapsuleControllerDesc->callback     =
    l_Report;         // Se llama cuando el controler colisiona
  m_pPhXCapsuleControllerDesc->interactionFlag  =
    NXIF_INTERACTION_USE_FILTER;  // Dice si el controler colisiona con otros controlers
}

CPhysicController::CPhysicController( Math::Vect3f _Dim, float _fSlope, float _fSkinwidth,
                                      float _fStepOffset
                                      , ECollisionGroup _uiCollisionGroups, CPhysicUserData* _pUserData, const Math::Vect3f& _vPos,
                                      float _fGravity )
  : m_pPhXController( NULL )
  , m_pPhXCapsuleControllerDesc( NULL )
  , m_pPhXBoxControllerDesc( NULL )
  , m_pUserData( _pUserData )
  , m_vExtensio( _Dim )
  , m_fSlopeLimitControler( _fSlope )
  , m_fSkinWidthControler( _fSkinwidth )
  , m_fStepOffsetControler( _fStepOffset )
  , m_fGravity( _fGravity )
  , m_uCollisionGroups( _uiCollisionGroups )
  , m_bUseGravity( true )
  , m_Type( ::BOX )

{
  assert( _pUserData );
  //---- Crear un nuevo NxController----
  m_pPhXBoxControllerDesc = new NxBoxControllerDesc();
  CPhysicsControllerHitReport* l_Report  = new CPhysicsControllerHitReport();
  m_pReport = l_Report;
  m_pPhXBoxControllerDesc->position.x     = _vPos.x;
  m_pPhXBoxControllerDesc->position.y     = _vPos.y;
  m_pPhXBoxControllerDesc->position.z     = _vPos.z;
  m_pPhXBoxControllerDesc->extents.x      = _Dim.x;
  m_pPhXBoxControllerDesc->extents.y      = _Dim.y;
  m_pPhXBoxControllerDesc->extents.z      = _Dim.z;
  m_pPhXBoxControllerDesc->slopeLimit     = cosf( NxMath::degToRad( m_fSlopeLimitControler ) );
  m_pPhXBoxControllerDesc->skinWidth      = m_fSkinWidthControler;
  m_pPhXBoxControllerDesc->stepOffset     = m_fStepOffsetControler;
  m_pPhXBoxControllerDesc->upDirection    = NX_Y;
  m_pPhXBoxControllerDesc->callback     = l_Report;
  m_pPhXBoxControllerDesc->interactionFlag  = NXIF_INTERACTION_USE_FILTER;
}

CPhysicController::~CPhysicController()
{
  //delete m_pPhXControllerDesc->callback;
  CHECKED_DELETE( m_pPhXCapsuleControllerDesc );
  CHECKED_DELETE( m_pPhXBoxControllerDesc );
  CHECKED_DELETE( m_pReport );
  // TODO RAUL CHECK THIS AFTER DELETING TEST PROCESS
  // CHECKED_DELETE ( m_pUserData );
}

// -----------------------------------------
//        M�TODES PRINCIPALS
// -----------------------------------------
void CPhysicController::CreateController( NxController* _pController, NxScene* _pScene )
{
  m_pPhXScene     = _pScene;
  m_pPhXController  = _pController;
  assert( m_pPhXScene );
  assert( m_pPhXController );
  CHECKED_DELETE( m_pPhXBoxControllerDesc );
  CHECKED_DELETE( m_pPhXCapsuleControllerDesc );
}

void CPhysicController::SetPosition( const Math::Vect3f& pos )
{
  if ( m_pPhXController != NULL )
  {
    NxExtendedVec3 position = m_pPhXController->getPosition();
    position.x = pos.x;
    position.z = pos.z;    
    m_pPhXController->setPosition( position );
  }
  else if ( m_pPhXBoxControllerDesc != NULL )
  {
    if ( GetType() == ::BOX )
    {
      m_pPhXBoxControllerDesc->position.x = pos.x;
      m_pPhXBoxControllerDesc->position.y = pos.y;
      m_pPhXBoxControllerDesc->position.z = pos.z;
    }
    else
    {
      m_pPhXCapsuleControllerDesc->position.x = pos.x;
      m_pPhXCapsuleControllerDesc->position.y = pos.y;
      m_pPhXCapsuleControllerDesc->position.z = pos.z;
    }
  }
  else
    return;

  //CObject3D::m_vPosition = pos;
  //CObject3D::InitMat44();
  CObject3D::SetPosition( pos );
}

Math::Vect3f CPhysicController::GetPosition()
{
  Math::Vect3f vec( 0, 0, 0 );

  if ( m_pPhXController != NULL )
  {
    // Parche Jordi!!
    //NxExtendedVec3 tmp1 = m_pPhXController->getPosition();
    NxExtendedVec3 tmp = m_pPhXController->getDebugPosition();
    vec.x = ( float )tmp.x;
    vec.y = ( float )tmp.y;
    vec.z = ( float )tmp.z;
  }
  else if ( m_pPhXBoxControllerDesc != NULL )
  {
    if ( GetType() == ::BOX )
    {
      vec.x = ( float )m_pPhXBoxControllerDesc->position.x;
      vec.y = ( float )m_pPhXBoxControllerDesc->position.y;
      vec.z = ( float )m_pPhXBoxControllerDesc->position.z;
    }
    else
    {
      vec.x = ( float )m_pPhXCapsuleControllerDesc->position.x;
      vec.y = ( float )m_pPhXCapsuleControllerDesc->position.y;
      vec.z = ( float )m_pPhXCapsuleControllerDesc->position.z;
    }
  }

  return vec;
}

void CPhysicController::Jump( float ammount )
{
  m_Jump.StartJump( ammount );
}

void CPhysicController::Move( const Math::Vect3f& _vDirection, float _ElapsedTime )
{
  assert( m_pPhXController != NULL );
  float l_fDirectionY = _vDirection.y;

  if ( m_bUseGravity )
    l_fDirectionY += ( m_fGravity * _ElapsedTime );

  //l_fDirectionY = 0;

  NxVec3 l_Direction( _vDirection.x, l_fDirectionY, _vDirection.z );
  NxF32 sharpness = 1.0f;
  NxU32 collisionFlags = 0;
  //NxU32 Collision = 0;
  float heightDelta = m_Jump.GetHeight( _ElapsedTime );

  if ( heightDelta != 0.f )
  {
    l_Direction.y += heightDelta;
    /*l_Direction.x *= 0.3f;
    l_Direction.z *= 0.3f;*/
  }

  int mask = 1 << ECG_PLAYER;
  mask |= 1 << ECG_DYNAMIC_OBJECTS;
  mask |= 1 << ECG_STATIC_OBJECTS;
  mask |= 1 << ECG_ESCENE;
  mask |= 1 << ECG_ENEMY;
  mask |= 1 << ECG_LIMITS;
  m_pPhXController->move( l_Direction , mask, 0.000001f, collisionFlags, sharpness );

  if ( ( collisionFlags & NXCC_COLLISION_DOWN ) || ( collisionFlags & NXCC_COLLISION_UP ) )
    m_Jump.StopJump();

  NxExtendedVec3 tmp;
  tmp = m_pPhXController->getDebugPosition();
  SetPosition( Math::Vect3f( ( float ) tmp.x, ( float ) tmp.y, ( float ) tmp.z ) );
  //CObject3D::InitMat44();
}

void CPhysicController::SetCollision( bool _bFlag )
{
  assert( m_pPhXController );
  m_pPhXController->setCollision( _bFlag );
}

bool CPhysicController::UpdateCharacterExtents( bool bent, float ammount )
{
  NxF32 height = m_fHeightControler;
  NxF32 radius = m_fRadiusControler;
  NxExtendedVec3 pos = m_pPhXController->getPosition();

  if ( bent )
  {
    //Ponerse de pie
    height += ammount;
    pos.y += ammount * 0.5f;
  }
  else
  {
    //Agacharse
    height -= ammount;
    pos.y -= ammount * 0.5f;
  }

  NxCapsule worldCapsule;
  worldCapsule.p0.x = worldCapsule.p1.x = ( NxReal )pos.x;
  worldCapsule.p0.y = worldCapsule.p1.y = ( NxReal )pos.y;
  worldCapsule.p0.z = worldCapsule.p1.z = ( NxReal )pos.z;
  worldCapsule.p0.y -= height * 0.5f;
  worldCapsule.p1.y += height * 0.5f;
  worldCapsule.radius = radius;
  m_pPhXController->setCollision( false ); // Avoid checking overlap with ourself
  bool Status = m_pPhXScene->checkOverlapCapsule( worldCapsule );
  m_pPhXController->setCollision( true );

  if ( Status )
    return false;

  NxExtendedVec3 position( pos.x, pos.y, pos.z );
  m_pPhXController->setPosition( position );
  SetPosition( Math::Vect3f( ( float )pos.x, ( float )pos.y, ( float )pos.z ) );
  NxCapsuleController* c = static_cast<NxCapsuleController*>( m_pPhXController );
  c->setHeight( height );
  m_fHeightControler = height;
  //CObject3D::InitMat44();
  return true;
}

void CPhysicController::SetGroup( int _iGroup )
{
  //m_pPhXController->getActor()->setGroup(_iGroup);
  NxShape* const* shapes = m_pPhXController->getActor()->getShapes();
  int l_iNumShapes = m_pPhXController->getActor()->getNbShapes();

  for ( int i = 0; i < l_iNumShapes; ++i )
    shapes[i]->setGroup( _iGroup );
}

void CPhysicController::SetHeight( float _fHeight )
{
  NxCapsuleController* l_CC = dynamic_cast<NxCapsuleController*>( m_pPhXController );

  if ( l_CC )
    l_CC->setHeight( _fHeight );
}

void CPhysicController::SetRadius( float _fRadius )
{
  NxCapsuleController* l_CC = dynamic_cast<NxCapsuleController*>( m_pPhXController );

  if ( l_CC )
    l_CC->setRadius( _fRadius );
}

void CPhysicController::SetActive( bool _bActive )
{
  m_pPhXController->setCollision( _bActive );
}

NxControllerDesc* CPhysicController::GetPhXControllerDesc( void )
{
  NxControllerDesc* l_Controler = NULL;

  switch ( m_Type )
  {
  case NX_CONTROLLER_BOX:
    l_Controler = m_pPhXBoxControllerDesc;

  case NX_CONTROLLER_CAPSULE:
    l_Controler = m_pPhXCapsuleControllerDesc;
  };

  return l_Controler;
}

void   CPhysicController::SetRotation( float _fAngle )
{
  NxActor* l_Actor = m_pPhXController->getActor();
  NxShape* const* shapes;
  shapes = l_Actor->getShapes();
  NxShape* shape = shapes[0];
  NxF32 m_aux[16];
  shape->getGlobalPose().getColumnMajor44( m_aux );

  //Guardo la posici�n original porque si rotas dos angulos, pierdes la rotaci�n original
  if ( m_bRotado == false )
    for ( int i = 0; i < 15; ++i )
      m_RotacionOriginal[i] = m_aux[i];

  //Paso toda la informaci�n de la shape a una matriz para trabajar con ella
  Mat44f m( m_aux[0], m_aux[4], m_aux[8], m_aux[12],
            m_aux[1], m_aux[5], m_aux[9], m_aux[13],
            m_aux[2], m_aux[6], m_aux[10], m_aux[14],
            m_aux[3], m_aux[7], m_aux[11], m_aux[15] );

  //Calcular la direcci�n a la que est� mirando el controlador y rotarlo 90 grados
  float l_Yaw = GetYaw();
  Math::Vect3f l_Direction( Math::Utils::Cos( l_Yaw ) , 0.0f, Math::Utils::Sin( l_Yaw ) );
  l_Direction = l_Direction.RotateY( 90 );
  l_Direction = l_Direction.Normalize();
  float n = l_Direction.Length();

  if ( ( l_Direction.x > 0 && l_Direction.z > 0 ) || ( l_Direction.x < 0 && l_Direction.z < 0 ) )
    m.RotByAngleX( _fAngle );
  else
    m.RotByAngleZ( _fAngle );

  NxMat34 l_m;

  m_aux[0] = m.m00;
  m_aux[4] = m.m01;
  m_aux[8] = m.m02;
  m_aux[12] = m.m03;
  m_aux[1] = m.m10;
  m_aux[5] = m.m11;
  m_aux[9] = m.m12;
  m_aux[13] = m.m13;
  m_aux[2] = m.m20;
  m_aux[6] = m.m21;
  m_aux[10] = m.m22;
  m_aux[14] = m.m23;
  m_aux[3] = m.m30;
  m_aux[7] = m.m31;
  m_aux[11] = m.m32;
  m_aux[15] = m.m33;

  if ( m_bRotado )
  {
    m_bRotado = false;
    l_m.setColumnMajor44( m_RotacionOriginal );
  }
  else
  {
    m_bRotado = true;
    l_m.setColumnMajor44( m_aux );
  }

  shape->setGlobalPose( l_m );
}