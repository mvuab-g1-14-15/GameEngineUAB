#include "PhysXFunctions.h"
#include "Utils\PhysicUserData.h"
#include "Actor\PhysicActor.h"
#include "Actor\PhysicController.h"
#include "PhysicsManager.h"
#include "Utils\Name.h"
#include "PhysicsDefs.h"

#include <set>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/operator.hpp>

using namespace luabind;

bool Add_PhysicController( CPhysicsManager* PhysicManager, CPhysicController* PhysicController )
{
  return PhysicManager->AddPhysicController( PhysicController );
}

void Move_PhysicController( CPhysicController* PhysicController, const Math::Vect3f& Direction, float Dt )
{
  const Math::Vect3f& DirectionConst = Direction;
  PhysicController->Move( DirectionConst, Dt );
}

template<class T>
size_t set_getIdByResource( std::set<T>& vec, T val )
{
  std::set<T>::iterator it = vec.begin()
                             , it_end = vec.end();

  size_t i = 0;

  for ( ; it != it_end ; ++it )
  {
    if ( *it == val )
      return i;

    ++i;
  }

  return 0;
}

void registerPhysX( lua_State* m_LS )
{
  module( m_LS )
  [
    class_<CPhysicUserData, CName>( "CPhysicUserData" )
    .def( constructor<const std::string&>() )
    .def( "SetColor", ( void ( CPhysicUserData::* )( const float, const float, const float,
                        const float ) ) &CPhysicUserData::SetColor )
    .def( "GetColor", &CPhysicUserData::GetColor )
    .def( "SetPaint", &CPhysicUserData::SetPaint )
    .def( "SetName", &CPhysicUserData::SetName )
    .def( "GetActor", &CPhysicUserData::GetActor )
    .def( "GetName", &CPhysicUserData::GetName )
    .def( "GetController", &CPhysicUserData::GetController )
  ];
  module( m_LS )
  [
    class_<CPhysicActor>( "CPhysicActor" )
    .def( constructor<CPhysicUserData*>() )
    //.def( "GetUserData", &CPhysicActor::GetUserData )
    .def( "CreateBody", &CPhysicActor::CreateBody )
    .def( "AddSphereShape", &CPhysicActor::AddSphereShapeHardcoded )
    .def( "AddBoxShape", &CPhysicActor::AddBoxShapeHardcoded )
    .def( "AddPlaneShape", &CPhysicActor::AddPlaneShape )
    .def( "AddMeshShape", &CPhysicActor::AddMeshShape )
    .def( "AddCapsuleShape", &CPhysicActor::AddCapsuleShape )
    .def( "AddImpulseAtPos", &CPhysicActor::AddImpulseAtPos )
    .def( "AddVelocityAtPos", &CPhysicActor::AddVelocityAtPos )
    .def( "AddAcelerationAtPos", &CPhysicActor::AddAcelerationAtPos )
    //.def( "AddForceAtPos", &CPhysicActor::AddForceAtPos )
    .def( "AddTorque", &CPhysicActor::AddTorque )
    .def( "SetGlobalPosition", &CPhysicActor::SetGlobalPosition )
  ];
  module( m_LS ) [
    class_<CPhysicsManager>( "CPhysicsManager" )
    .def( "RaycastClosestActor", &CPhysicsManager::RaycastClosestActor )
    .def( "RaycastClosestActorName", &CPhysicsManager::RaycastClosestActorName )
    //Adds
    .def( "AddPhysicActor", &CPhysicsManager::AddPhysicActor )
    .def( "AddPhysicRevoluteJoint", &CPhysicsManager::AddPhysicRevoluteJoint )
    .def( "AddPhysicSphericalJoint", &CPhysicsManager::AddPhysicSphericalJoint )
    .def( "AddPhysicController", &Add_PhysicController )
    .def( "AddActor", ( bool ( CPhysicsManager::* )( const std::string&, std::string&, const Math::Vect3f&, const Math::CColor&,
                        bool, const Math::Vect3f&, const Math::Vect3f&, const Math::Vect3f&,
                        NxCCDSkeleton*, uint32 ) )&CPhysicsManager::AddActor )
    .def( "AddController", &CPhysicsManager::AddController )
    //Release
    .def( "ReleasePhysicActor", &CPhysicsManager::ReleasePhysicActor )
    .def( "ReleasePhysicController", &CPhysicsManager::ReleasePhysicController )
    .def( "Load", &CPhysicsManager::Load )
    .def( "Init", &CPhysicsManager::Init )
    .def( "Done", &CPhysicsManager::Done )
    .def( "Reload", &CPhysicsManager::Reload )
    .def( "ReloadXML", &CPhysicsManager::ReloadXML )
    //Gets
    .def( "GetActor", &CPhysicsManager::GetActor )
    .def( "GetUserData", ( CPhysicUserData * ( CPhysicsManager::* )( const std::string& ) )
          &CPhysicsManager::GetUserData )
    .def( "OverlapSphere", &CPhysicsManager::OverlapSphereHardcoded )
    .def( "OverlapSphereActor", &CPhysicsManager::OverlapSphereActor )
    .def( "GetController", &CPhysicsManager::CMapManager<CPhysicController>::GetResource )
    .def( "GetActor", &CPhysicsManager::CMapManager<CPhysicActor>::GetResource )

  ];
  module( m_LS ) [
    class_<CPhysicController, CObject3D>( "CPhysicController" )
    .def( constructor<float, float, float, float, float, ECollisionGroup, CPhysicUserData*, const Math::Vect3f&, float>() )
    .def( "Move", ( void ( CPhysicController::* )( const Math::Vect3f&, float ) )&CPhysicController::Move )
    //.def( "Move", &Move_PhysicController )
    .def( "Jump", &CPhysicController::Jump )
    .def( "GetPosition", &CPhysicController::GetPosition )
    .def( "SetPosition", &CPhysicController::SetPosition )
    .def( "SetHeight", &CPhysicController::SetHeight )
    .def( "GetHeight", &CPhysicController::GetHeight )
    .def( "UpdateCharacterExtents", &CPhysicController::UpdateCharacterExtents )
  ];

  module( m_LS ) [
    class_<std::set<CPhysicUserData*>>( "ListaPUD" )
    .def( constructor<std::set<CPhysicUserData*>>() )
    .def( "size", &std::set<CPhysicUserData*>::size )
    .def( "getIdByResource", &set_getIdByResource<CPhysicUserData*> )
  ];

  module( m_LS ) [
    class_<std::vector<CPhysicUserData*>>( "vPUD" )
    .def( constructor<std::vector<CPhysicUserData*>>() )
    .def( "size", &std::vector<CPhysicUserData*>::size )
  ];
}