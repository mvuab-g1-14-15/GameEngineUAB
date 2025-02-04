#define __DONT_INCLUDE_MEM_LEAKS__
#include "Actor\PhysicActor.h"
#include "PhysicsManager.h"
#include "Utils\PhysicUserData.h"
#include "Math\Matrix44.h"
#include "PhysicUtils.h"

//----PhysX Includes-------------
#undef min
#undef max
#include "NxPhysics.h"
#include "NxActor.h"
//-------------------------------

using namespace Math;

// -----------------------------------------
//      CONSTRUCTORS / DESTRUCTOR
// -----------------------------------------

CPhysicActor::CPhysicActor( CPhysicUserData* _pUserData )
  : m_pUserData( _pUserData )
  , m_pPhXActor( NULL )
  , m_pPhXActorDesc( NULL )
  , m_pPhXBodyDesc( NULL )
  , m_MaterialIndex( 0 )
{
  assert( m_pUserData );
  _pUserData->SetActor( this );
  m_uCollisionGroups  = _pUserData->GetGroup();
  m_pPhXActorDesc     = new NxActorDesc();
  m_pPhXBodyDesc      = new NxBodyDesc();
}

CPhysicActor::~CPhysicActor( void )
{
  Destroy();
}

// -----------------------------------------
//        M�TODOS PRINCIPALES
// -----------------------------------------
void CPhysicActor::Destroy( void )
{
  CHECKED_DELETE( m_pPhXActorDesc );
  CHECKED_DELETE( m_pPhXBodyDesc );

  if ( m_vBoxDesc.size() > 0 )
  {
    std::vector<NxBoxShapeDesc*>::iterator  it( m_vBoxDesc.begin() ),
                                            itEnd( m_vBoxDesc.end() );

    while ( it != itEnd )
    {
      NxBoxShapeDesc* shapeDesc = *it;
      CHECKED_DELETE( shapeDesc );
      it++;
    }

    m_vBoxDesc.clear();
  }

  if ( m_vMeshDesc.size() > 0 )
  {
    std::vector<NxTriangleMeshShapeDesc*>::iterator it( m_vMeshDesc.begin() ),
                                                    itEnd( m_vMeshDesc.end() );

    while ( it != itEnd )
    {
      NxTriangleMeshShapeDesc* shapeDesc = *it;
      CHECKED_DELETE( shapeDesc );
      it++;
    }

    m_vMeshDesc.clear();
  }

  if ( m_vCapsuleDesc.size() > 0 )
  {
    std::vector<NxCapsuleShapeDesc*>::iterator  it( m_vCapsuleDesc.begin() ),
                                                itEnd( m_vCapsuleDesc.end() );

    while ( it != itEnd )
    {
      NxCapsuleShapeDesc* shapeDesc = *it;
      CHECKED_DELETE( shapeDesc );
      it++;
    }

    m_vCapsuleDesc.clear();
  }

  if ( m_vSphereDesc.size() > 0 )
  {
    std::vector<NxSphereShapeDesc*>::iterator it( m_vSphereDesc.begin() ),
                                              itEnd( m_vSphereDesc.end() );

    while ( it != itEnd )
    {
      NxSphereShapeDesc* shapeDesc = *it;
      CHECKED_DELETE( shapeDesc );
      it++;
    }

    m_vSphereDesc.clear();
  }

  if ( m_vPlaneDesc.size() > 0 )
  {
    std::vector<NxPlaneShapeDesc*>::iterator  it( m_vPlaneDesc.begin() ),
                                              itEnd( m_vPlaneDesc.end() );

    while ( it != itEnd )
    {
      NxPlaneShapeDesc* shapeDesc = *it;
      CHECKED_DELETE( shapeDesc );
      it++;
    }

    m_vPlaneDesc.clear();
  }
}

// -----------------------------------------
//        M�TODOS PRINCIPALES
// -----------------------------------------

void CPhysicActor::CreateActor( NxActor* _pActor )
{
  assert( _pActor );
  m_pPhXActor = _pActor;
  Destroy();
}

void CPhysicActor::CreateBoxTrigger( const Math::Vect3f& _vGlobalPos, const Math::Vect3f& _vSize,
                                     uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  NxBoxShapeDesc* boxDesc = new NxBoxShapeDesc();
  assert( boxDesc );
  boxDesc->group      = _uiGroup;
  m_vBoxDesc.push_back( boxDesc );
  boxDesc->dimensions = NxVec3( _vSize.x, _vSize.y, _vSize.z );
  boxDesc->shapeFlags |= NX_TRIGGER_ENABLE;
  m_pPhXActorDesc->globalPose.t = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );
  m_pPhXActorDesc->shapes.pushBack( boxDesc );
}

void CPhysicActor::CreateSphereTrigger( const Math::Vect3f& _vGlobalPos, const float _fRadius,
                                        uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  NxSphereShapeDesc* sphereDesc = new NxSphereShapeDesc();
  assert( sphereDesc );
  sphereDesc->group  = _uiGroup;
  m_vSphereDesc.push_back( sphereDesc );
  sphereDesc->radius = _fRadius;
  sphereDesc->shapeFlags       |= NX_TRIGGER_ENABLE;
  m_pPhXActorDesc->globalPose.t = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );
  m_pPhXActorDesc->shapes.pushBack( sphereDesc );
}

// -----------------------------------------
//          M�TODOS
// -----------------------------------------

void CPhysicActor::AddTorque( const Math::Vect3f _vTorque )
{
  if ( m_pPhXActor )
  {
    if ( _vTorque != v3fZERO )
      m_pPhXActor->addLocalTorque( NxVec3( _vTorque.x, _vTorque.y, _vTorque.z ) );
  }
  else
  {
    //TODO log de error...
  }
}

void CPhysicActor::AddSphereShape( float radius, const Math::Vect3f& _vGlobalPos,
                                   const Math::Vect3f& localPos, NxCCDSkeleton* skeleton, uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  // Add a sphere shape to the actor descriptor
  NxSphereShapeDesc* sphereDesc = new NxSphereShapeDesc();
  assert( sphereDesc );
  sphereDesc->group             = _uiGroup;
  sphereDesc->materialIndex     = m_MaterialIndex;
  m_vSphereDesc.push_back( sphereDesc );
  sphereDesc->radius            = radius;
  sphereDesc->localPose.t       = NxVec3( localPos.x, localPos.y, localPos.z );
  m_pPhXActorDesc->globalPose.t = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );

  if ( skeleton != NULL )
  {
    sphereDesc->ccdSkeleton = skeleton;
    sphereDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
  }

  m_pPhXActorDesc->shapes.pushBack( sphereDesc );
}

void CPhysicActor::AddBoxShape( const Math::Vect3f& _vSize, const Math::Vect3f& _vGlobalPos,
                                const Math::Vect3f& localPos, const Math::Vect3f& rotation, NxCCDSkeleton* skeleton,
                                uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  NxBoxShapeDesc* l_BoxDesc = new NxBoxShapeDesc();
  assert( l_BoxDesc );
  l_BoxDesc->group          = _uiGroup;
  l_BoxDesc->materialIndex  = m_MaterialIndex;
  m_vBoxDesc.push_back( l_BoxDesc );
  // Add a box shape to the actor descriptor
  l_BoxDesc->dimensions     = NxVec3( _vSize.x, _vSize.y, _vSize.z );
  //l_BoxDesc->localPose.M.rotX(NxReal(rotation.x));
  //l_BoxDesc->localPose.M.rotY(NxReal(rotation.y));
  //l_BoxDesc->localPose.M.rotZ(NxReal(rotation.z));
  l_BoxDesc->localPose.t    = NxVec3( localPos.x, localPos.y, localPos.z );
  NxMat33 rotX              = m_pPhXActorDesc->globalPose.M;
  NxMat33 rotY              = m_pPhXActorDesc->globalPose.M;
  NxMat33 rotZ              = m_pPhXActorDesc->globalPose.M;
  rotX.rotX( NxReal( rotation.x ) );
  rotY.rotY( NxReal( rotation.y ) );
  rotZ.rotZ( NxReal( rotation.z ) );
  NxMat33 rotAll            = rotY;
  rotAll.multiply( rotY, rotX );
  rotAll.multiply( rotAll, rotZ );
  m_pPhXActorDesc->globalPose.M = rotAll;
  m_pPhXActorDesc->globalPose.t = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );

  if ( skeleton != NULL )
  {
    l_BoxDesc->ccdSkeleton = skeleton;
    l_BoxDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
  }

  m_pPhXActorDesc->shapes.pushBack( l_BoxDesc );
}

void CPhysicActor::AddBoxShapeHardcoded( const Math::Vect3f& _vSize,
    const Math::Vect3f& _vGlobalPos, const Math::Vect3f& localPos, const Math::Vect3f& rotation )
{
  assert( m_pPhXActorDesc );
  NxCCDSkeleton* skeleton   = 0;
  uint32 _uiGroup           = 0;
  NxBoxShapeDesc* l_BoxDesc = new NxBoxShapeDesc();
  assert( l_BoxDesc );
  l_BoxDesc->group          = _uiGroup;
  l_BoxDesc->materialIndex  = m_MaterialIndex;
  m_vBoxDesc.push_back( l_BoxDesc );
  // Add a box shape to the actor descriptor
  l_BoxDesc->dimensions     = NxVec3( _vSize.x, _vSize.y, _vSize.z );
  //l_BoxDesc->localPose.M.rotX(NxReal(rotation.x));
  //l_BoxDesc->localPose.M.rotY(NxReal(rotation.y));
  //l_BoxDesc->localPose.M.rotZ(NxReal(rotation.z));
  l_BoxDesc->localPose.t		= NxVec3( localPos.x, localPos.y, localPos.z );
  NxMat33 rotX					= m_pPhXActorDesc->globalPose.M;
  NxMat33 rotY					= m_pPhXActorDesc->globalPose.M;
  NxMat33 rotZ					= m_pPhXActorDesc->globalPose.M;
  rotX.rotX( NxReal( rotation.x ) );
  rotY.rotY( NxReal( rotation.y ) );
  rotZ.rotZ( NxReal( rotation.z ) );
  NxMat33 rotAll				= rotY;
  rotAll.multiply( rotY, rotX );
  rotAll.multiply( rotAll, rotZ );
  m_pPhXActorDesc->globalPose.M = rotAll;
  m_pPhXActorDesc->globalPose.t = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );

  if ( skeleton != NULL )
  {
    l_BoxDesc->ccdSkeleton = skeleton;
    l_BoxDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
  }

  m_pPhXActorDesc->shapes.pushBack( l_BoxDesc );
}

void CPhysicActor::AddCapsuleShape( float radius, float height, const Math::Vect3f& _vGlobalPos,
                                    const Math::Vect3f& localPos, NxCCDSkeleton* skeleton, uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  // Add a capsule shape to the actor descriptor
  NxCapsuleShapeDesc* capsuleDesc = new NxCapsuleShapeDesc();
  assert( capsuleDesc );
  capsuleDesc->group              = _uiGroup;
  capsuleDesc->materialIndex      = m_MaterialIndex;
  m_vCapsuleDesc.push_back( capsuleDesc );
  capsuleDesc->height             = height;
  capsuleDesc->radius             = radius;
  capsuleDesc->localPose.t        = NxVec3( localPos.x, localPos.y, localPos.z );
  m_pPhXActorDesc->globalPose.t   = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );

  if ( skeleton != NULL )
  {
    capsuleDesc->ccdSkeleton = skeleton;
    capsuleDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
  }

  m_pPhXActorDesc->shapes.pushBack( capsuleDesc );
}

void CPhysicActor::AddMeshShape( NxTriangleMesh* mesh, const Math::Mat44f& aTransform , NxCCDSkeleton* _pSkeleton , uint32 _uiGroup )
{
	ASSERT(m_pPhXActorDesc, "Null actor descriptor");
	ASSERT( mesh, "Null mesh" );

	// Add a mesh shape to the actor descriptor
	  NxTriangleMeshShapeDesc* triangleMeshDesc = new NxTriangleMeshShapeDesc();
	  triangleMeshDesc->group                   = _uiGroup;
	  triangleMeshDesc->materialIndex           = m_MaterialIndex;
	  m_vMeshDesc.push_back( triangleMeshDesc );
	  // The actor has mesh shape
	  triangleMeshDesc->meshData                = mesh;
	  PhysicUtils::Convert<Math::Mat44f, NxMat34>(aTransform, m_pPhXActorDesc->globalPose );

	  if ( _pSkeleton )
	  {
		triangleMeshDesc->ccdSkeleton = _pSkeleton;
		triangleMeshDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
		triangleMeshDesc->shapeFlags |= NX_MESH_SMOOTH_SPHERE_COLLISIONS;
	  }

	  m_pPhXActorDesc->shapes.pushBack( triangleMeshDesc );
}

void CPhysicActor::AddMeshShape( NxTriangleMesh* mesh, const Math::Vect3f& _vGlobalPos,
                                 const Math::Vect3f& localPos, NxCCDSkeleton* skeleton, uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  assert( mesh );
  // Add a mesh shape to the actor descriptor
  NxTriangleMeshShapeDesc* triangleMeshDesc = new NxTriangleMeshShapeDesc();
  triangleMeshDesc->group                   = _uiGroup;
  triangleMeshDesc->materialIndex           = m_MaterialIndex;
  m_vMeshDesc.push_back( triangleMeshDesc );
  // The actor has mesh shape
  triangleMeshDesc->meshData                = mesh;
  m_pPhXActorDesc->globalPose.t             = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );

  if ( skeleton != NULL )
  {
    triangleMeshDesc->ccdSkeleton = skeleton;
    triangleMeshDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
    triangleMeshDesc->shapeFlags |= NX_MESH_SMOOTH_SPHERE_COLLISIONS;
  }

  m_pPhXActorDesc->shapes.pushBack( triangleMeshDesc );
}

void CPhysicActor::AddPlaneShape( const Math::Vect3f& _vNormal, float distance, uint32 _uiGroup )
{
  assert( m_pPhXActorDesc );
  // Add a plane shape to the actor descriptor
  NxPlaneShapeDesc* planeDesc = new NxPlaneShapeDesc();
  assert( planeDesc );
  planeDesc->group			  = _uiGroup;
  planeDesc->materialIndex    = m_MaterialIndex;
  m_vPlaneDesc.push_back( planeDesc );
  planeDesc->normal           = NxVec3( _vNormal.x, _vNormal.y, _vNormal.z );
  planeDesc->d                = distance;
  m_pPhXActorDesc->shapes.pushBack( planeDesc );
}

void CPhysicActor::ActivateAllTriggers( void )
{
  uint32 size = m_pPhXActorDesc->shapes.size();

  for ( uint32 i = 0; i < size; ++i )
    m_pPhXActorDesc->shapes[i]->shapeFlags |= NX_TRIGGER_ENABLE;
}

void CPhysicActor::Activate( bool _bActivate )
{
  if ( m_pPhXActor )
  {
    if ( _bActivate )
      m_pPhXActor->clearActorFlag( NX_AF_DISABLE_COLLISION );
    else
      m_pPhXActor->raiseActorFlag( NX_AF_DISABLE_COLLISION );
  }
}

void CPhysicActor::AddImpulseAtPos( const Math::Vect3f& _vDirection, const Math::Vect3f& _vPos, float _fPower, bool _bLocal )
{
  AddForceAtPos( _vDirection, _vPos, _fPower, NX_IMPULSE, _bLocal );
}

void CPhysicActor::AddVelocityAtPos( const Math::Vect3f& _vDirection, const Math::Vect3f& _vPos, float _fPower, bool _bLocal )
{
  AddForceAtPos( _vDirection, _vPos, _fPower, NX_VELOCITY_CHANGE, _bLocal );
}

void CPhysicActor::AddAcelerationAtPos( const Math::Vect3f& _vDirection, const Math::Vect3f& _vPos, float _fPower, bool _bLocal )
{
  AddForceAtPos( _vDirection, _vPos, _fPower, NX_ACCELERATION, _bLocal );
}

void CPhysicActor::AddForceAtPos( const Math::Vect3f& _vDirection, const Math::Vect3f& _vPos, float _fPower, bool _bLocal )
{
  AddForceAtPos( _vDirection, _vPos, _fPower, NX_FORCE, _bLocal );
}

void CPhysicActor::AddForceAtPos( const Math::Vect3f& _vDirection, const Math::Vect3f& _vPos, float _fPower, NxForceMode _sForceMode, bool _bLocal )
{
  if ( m_pPhXActor )
  {
    NxVec3 l_vDirection( _vDirection.x, _vDirection.y, _vDirection.z );
    NxVec3 l_vPos( _vPos.x, _vPos.y, _vPos.z );

    if ( l_vDirection.isFinite() && l_vPos.isFinite() && NxMath::isFinite( _fPower ) )
    {
      if ( _bLocal )
        m_pPhXActor->addForceAtLocalPos( l_vDirection * _fPower, l_vPos, _sForceMode, true );
      else
        m_pPhXActor->addForceAtPos( l_vDirection * _fPower, l_vPos, _sForceMode, true );
    }
  }
}

void CPhysicActor::CreateBody( float density, float angularDamping, float linearDamping )
{
  if ( density != 0 )
  {
    assert( m_pPhXBodyDesc );
    // Create body
    m_pPhXBodyDesc->angularDamping  = angularDamping;
    m_pPhXBodyDesc->linearDamping   = linearDamping;
    m_pPhXActorDesc->body           = m_pPhXBodyDesc;
    m_pPhXActorDesc->density        = density;
  }
}

void CPhysicActor::SetGlobalPosition( const Math::Vect3f& _vPos )
{
  if ( m_pPhXActorDesc )
    m_pPhXActorDesc->globalPose.t  = NxVec3( _vPos.x, _vPos.y, _vPos.z );

  if ( m_pPhXActor )
    m_pPhXActor->setGlobalPosition( NxVec3( _vPos.x, _vPos.y, _vPos.z ) );
}

void CPhysicActor::MoveGlobalPosition( const Math::Vect3f& _vPos )
{
  if ( m_pPhXActor )
  {
    if ( !m_pPhXActor->isDynamic() )
      return;

    m_pPhXActor->moveGlobalPosition( NxVec3( _vPos.x, _vPos.y, _vPos.z ) );
  }
}

void CPhysicActor::MoveGlobalPoseMat44( const Mat44f& _mMatrix )
{
  assert( m_pPhXActor );
  NxF32 m_aux[16];
  m_aux[0]  = _mMatrix.m00;
  m_aux[4]  = _mMatrix.m01;
  m_aux[8]  = _mMatrix.m02;
  m_aux[12] = _mMatrix.m03;
  m_aux[1]  = _mMatrix.m10;
  m_aux[5]  = _mMatrix.m11;
  m_aux[9]  = _mMatrix.m12;
  m_aux[13] = _mMatrix.m13;
  m_aux[2]  = _mMatrix.m20;
  m_aux[6]  = _mMatrix.m21;
  m_aux[10] = _mMatrix.m22;
  m_aux[14] = _mMatrix.m23;
  m_aux[3]  = _mMatrix.m30;
  m_aux[7]  = _mMatrix.m31;
  m_aux[11] = _mMatrix.m32;
  m_aux[15] = _mMatrix.m33;
  NxMat34 mat;
  mat.setColumnMajor44( m_aux );
  m_pPhXActor->moveGlobalPose( mat );
}

// -----------------------------------------
//        PROPIEDADES
// -----------------------------------------

void CPhysicActor::SetActorSolverIterationCount( int _iCount )
{
  if ( m_pPhXActor )
    m_pPhXActor->setSolverIterationCount( _iCount );
}

Math::Vect3f CPhysicActor::GetAngularMomentum( void )
{
  if ( m_pPhXActor )
  {
    NxVec3 l_nxMom = m_pPhXActor->getAngularMomentum();
    return Math::Vect3f( l_nxMom.x, l_nxMom.y, l_nxMom.z );
  }

  return v3fZERO;
}

void CPhysicActor::SetAngularVelocity( const Math::Vect3f _vVelocity )
{
  if ( m_pPhXActor )
  {
    if ( _vVelocity != v3fZERO )
      m_pPhXActor->setAngularVelocity( NxVec3( _vVelocity.x, _vVelocity.y, _vVelocity.z ) );
  }
  else
  {
    //TODO log de error...
  }
}

Math::Vect3f CPhysicActor::GetAngularVelocity( void )
{
  if ( m_pPhXActor )
  {
    NxVec3 l_nxVel = m_pPhXActor->getAngularVelocity();
    return Math::Vect3f( l_nxVel.x, l_nxVel.y, l_nxVel.z );
  }

  return v3fZERO;
}

void CPhysicActor::SetKinematic( bool _bValue )
{
  if ( m_pPhXActor )
  {
    if ( _bValue )
      m_pPhXActor->raiseBodyFlag( NX_BF_KINEMATIC );
    else
      m_pPhXActor->clearBodyFlag( NX_BF_KINEMATIC );
  }
}

void CPhysicActor::SetLinearVelocity( const Math::Vect3f& _vVelocity )
{
  if ( m_pPhXActor )
  {
    if ( _vVelocity != v3fZERO )
      m_pPhXActor->setLinearVelocity( NxVec3( _vVelocity.x, _vVelocity.y, _vVelocity.z ) );
  }
  else
  {
    //TODO log de error...
  }
}

Math::Vect3f CPhysicActor::GetLinearVelocity( void )
{
  if ( m_pPhXActor )
  {
    NxVec3 l_nxVel = m_pPhXActor->getLinearVelocity();
    return Math::Vect3f( l_nxVel.x, l_nxVel.y, l_nxVel.z );
  }

  return v3fZERO;
}

void CPhysicActor::SetMat44( const Mat44f& matrix )
{
  assert( m_pPhXActor );
  NxF32 m_aux[16];
  m_aux[0]  = matrix.m00;
  m_aux[4]  = matrix.m01;
  m_aux[8]  = matrix.m02;
  m_aux[12] = matrix.m03;
  m_aux[1]  = matrix.m10;
  m_aux[5]  = matrix.m11;
  m_aux[9]  = matrix.m12;
  m_aux[13] = matrix.m13;
  m_aux[2]  = matrix.m20;
  m_aux[6]  = matrix.m21;
  m_aux[10] = matrix.m22;
  m_aux[14] = matrix.m23;
  m_aux[3]  = matrix.m30;
  m_aux[7]  = matrix.m31;
  m_aux[11] = matrix.m32;
  m_aux[15] = matrix.m33;
  NxMat34 mat;
  mat.setColumnMajor44( m_aux );
  m_pPhXActor->setGlobalPose( mat );
}

void CPhysicActor::GetMat44( Mat44f& matrix ) const
{
  assert( m_pPhXActor );
  NxF32 m_aux[16];
  m_pPhXActor->getGlobalPose().getColumnMajor44( m_aux );
  matrix.m00 = m_aux[0];
  matrix.m01 = m_aux[4];
  matrix.m02 = m_aux[8];
  matrix.m03 = m_aux[12];
  matrix.m10 = m_aux[1];
  matrix.m11 = m_aux[5];
  matrix.m12 = m_aux[9];
  matrix.m13 = m_aux[13];
  matrix.m20 = m_aux[2];
  matrix.m21 = m_aux[6];
  matrix.m22 = m_aux[10];
  matrix.m23 = m_aux[14];
  matrix.m30 = m_aux[3];
  matrix.m31 = m_aux[7];
  matrix.m32 = m_aux[11];
  matrix.m33 = m_aux[15];
}

Mat33f CPhysicActor::GetInertiaTensor( void )
{
  if ( m_pPhXActor )
  {
    NxMat33 l_nxTensor = m_pPhXActor->getGlobalInertiaTensor();
    NxF32 m_aux[9];
    l_nxTensor.getRowMajor( m_aux );
    return Mat33f( m_aux[0], m_aux[1], m_aux[2],
                   m_aux[4], m_aux[5], m_aux[6],
                   m_aux[7], m_aux[8], m_aux[9]
                 );
  }

  return m33fIDENTITY;
}

void CPhysicActor::SetContactReportFlags( unsigned int _uiFlags )
{
  if ( m_pPhXActor )
    m_pPhXActor->setContactReportFlags( _uiFlags );
}

void CPhysicActor::SetContactReportThreshold( float _fThreshold )
{
  if ( m_pPhXActor )
    m_pPhXActor->setContactReportThreshold( _fThreshold );
}

void CPhysicActor::SetRotation( const Math::Vect3f& _vRot )
{
  assert( m_pPhXActor );
  //no va bien, s�lo rota de 0 a pi y repite.
  //angles between -pi and pi
  Math::Vect3f l_vRot = _vRot;

  while ( l_vRot.x > pi32 )  l_vRot.x -= 2 * pi32;

  while ( l_vRot.x < -pi32 ) l_vRot.x += 2 * pi32;

  while ( l_vRot.y > pi32 )  l_vRot.y -= 2 * pi32;

  while ( l_vRot.y < -pi32 ) l_vRot.y += 2 * pi32;

  while ( l_vRot.z > pi32 )  l_vRot.z -= 2 * pi32;

  while ( l_vRot.z < -pi32 ) l_vRot.z += 2 * pi32;

  Mat44f l_mat44;
  GetMat44( l_mat44 );

  Mat44f l_rot44;
  l_rot44.SetIdentity();
  l_rot44.RotByAnglesYXZ( l_vRot.y, l_vRot.x, l_vRot.z );

  Math::Vect3f l_rotScaleX( l_rot44.m00, l_rot44.m10, l_rot44.m20 );
  Math::Vect3f l_rotScaleY( l_rot44.m01, l_rot44.m11, l_rot44.m21 );
  Math::Vect3f l_rotScaleZ( l_rot44.m02, l_rot44.m12, l_rot44.m22 );

  assert( l_rotScaleX * l_rotScaleX < 1.01f && l_rotScaleX * l_rotScaleX > 0.99f );
  assert( l_rotScaleY * l_rotScaleY < 1.01f && l_rotScaleY * l_rotScaleY > 0.99f );
  assert( l_rotScaleZ * l_rotScaleZ < 1.01f && l_rotScaleZ * l_rotScaleZ > 0.99f );

  l_rot44.Translate( l_mat44.GetPos() );
  SetMat44( l_rot44 );
}

void CPhysicActor::SetRotation( const Mat33f& _vRot )
{
  Mat44f l_rot44;
  l_rot44.SetIdentity();
  l_rot44.m00 = _vRot.m00;
  l_rot44.m01 = _vRot.m01;
  l_rot44.m02 = _vRot.m02;
  l_rot44.m10 = _vRot.m10;
  l_rot44.m11 = _vRot.m11;
  l_rot44.m12 = _vRot.m12;
  l_rot44.m20 = _vRot.m20;
  l_rot44.m21 = _vRot.m21;
  l_rot44.m22 = _vRot.m22;
  l_rot44.Translate( GetPosition() );
  SetMat44( l_rot44 );
}

Math::Vect3f CPhysicActor::GetPosition( void )
{
  assert( m_pPhXActor );
  NxVec3 pos = m_pPhXActor->getGlobalPosition();
  return Math::Vect3f( pos.x, pos.y, pos.z );
}

void CPhysicActor::SetPosition( Math::Vect3f aPos )
{
  assert( m_pPhXActor );
  m_pPhXActor->setGlobalPosition( NxVec3(aPos.x, aPos.y, aPos.z) );
}

Math::Vect3f CPhysicActor::GetRotation( void )
{
  assert( m_pPhXActor );
  Mat44f l_mat44;
  GetMat44( l_mat44 );
  Math::Vect3f l_vRot;
  l_mat44.GetAnglesYXZ( l_vRot.y, l_vRot.x, l_vRot.z );
  return l_vRot;
}

void CPhysicActor::SetCollisionGroup( uint32 _uiGroup ) //NxCollisionGroup group
{
  NxShape* const* shapes = m_pPhXActor->getShapes();
  NxU32 nShapes          = m_pPhXActor->getNbShapes();

  while ( nShapes-- )
    shapes[nShapes]->setGroup( _uiGroup );
}

void CPhysicActor::AddSphereShapeHardcoded( float radius, const Math::Vect3f& _vGlobalPos,
                                   const Math::Vect3f& localPos )
{
  NxCCDSkeleton* skeleton = 0;
  uint32 _uiGroup         = 0;
  assert( m_pPhXActorDesc );
  // Add a sphere shape to the actor descriptor
  NxSphereShapeDesc* sphereDesc = new NxSphereShapeDesc();
  assert( sphereDesc );
  sphereDesc->group             = _uiGroup;
  sphereDesc->materialIndex     = m_MaterialIndex;
  m_vSphereDesc.push_back( sphereDesc );
  sphereDesc->radius            = radius;
  sphereDesc->localPose.t       = NxVec3( localPos.x, localPos.y, localPos.z );
  m_pPhXActorDesc->globalPose.t = NxVec3( _vGlobalPos.x, _vGlobalPos.y, _vGlobalPos.z );

  if ( skeleton != NULL )
  {
    sphereDesc->ccdSkeleton = skeleton;
    sphereDesc->shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body
  }

  m_pPhXActorDesc->shapes.pushBack( sphereDesc );
}