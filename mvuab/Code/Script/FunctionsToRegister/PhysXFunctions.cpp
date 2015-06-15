#include "PhysXFunctions.h"
#include "Utils\PhysicUserData.h"
#include "Actor\PhysicActor.h"
#include "Actor\PhysicController.h"
#include "PhysicsManager.h"
#include "Utils\Name.h"
#include "PhysicsDefs.h"
#include "Utils\Defines.h"

#include "Particles\ParticleManager.h"
#include "Particles\ParticleEmitter.h"
#include "Particles\SphereEmitter.h"
#include "Particles\CubeEmitter.h"

#include "EngineManagers.h"

#include "luabind_macros.h"

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

#include "Characters/Enemies/Enemy.h"
#include "Characters/Enemies/EnemyManager.h"

#define REGISTER_LUA_FUNCTION(LuaState, AddrFunction) {luabind::module(LuaState) [ luabind::def(#AddrFunction,AddrFunction) ];}

using namespace luabind;

bool Add_PhysicController( CPhysicsManager *PhysicManager, CPhysicController *PhysicController )
{
    return PhysicManager->AddPhysicController( PhysicController );
}

void Move_PhysicController( CPhysicController *PhysicController, const Math::Vect3f& Direction, float Dt )
{
    const Math::Vect3f& DirectionConst = Direction;
    PhysicController->Move( DirectionConst, Dt );
}

float RaycastDistance( CPhysicsManager *PhysicManager, Math::Vect3f position, Math::Vect3f direction, uint32 impactMask, float maxDist )
{
    SCollisionInfo hit_info;

    if ( !PhysicManager->RaycastClosestActor( position, direction, impactMask, hit_info, maxDist ) )
        return 0.0f;

    return hit_info.m_fDistance;
}

ECollisionGroup RaycastType( CPhysicsManager *PhysicManager, Math::Vect3f position, Math::Vect3f direction, uint32 impactMask )
{
    SCollisionInfo hit_info;
    CPhysicUserData *l_PUD = PhysicManager->RaycastClosestActor( position, direction, impactMask, hit_info );

    if ( l_PUD )
        if ( l_PUD->GetController() )
            return l_PUD->GetController()->GetColisionGroup();
        else
            return l_PUD->GetMyCollisionGroup();
    else
        return ( ECollisionGroup ) - 1;
}

template<class T> size_t set_getIdByResource( std::set<T>& vec, T val )
{
    std::set<T>::iterator it = vec.begin() , it_end = vec.end();
    size_t i = 0;

    for ( ; it != it_end ; ++it )
    {
        if ( *it == val ) return i;

        ++i;
    }

    return 0;
}

template<class T> void removeResource( T val )
{
    CHECKED_DELETE( val );
}

template<class T> T vector_get( std::vector<T>& vec, size_t i )
{
    return vec[i];
}

bool PlayerInSight( CPhysicsManager *PhysicManager, float _Distance, float _Angle, const Math::Vect3f& _Position, const Math::Vect3f& _Direction )
{
    const std::vector<CPhysicUserData *>& l_UserDatas = PhysicManager->OverlapConeActor( _Distance, _Angle, _Position, _Direction, 0xffffffff );
    std::vector<CPhysicUserData *>::const_iterator it = l_UserDatas.begin(),
                                                   it_end = l_UserDatas.end();

    for ( ; it != it_end; ++it )
    {
        CPhysicUserData *l_UserData = *it;
        CPhysicController *l_Controller = l_UserData->GetController();
        std::string name = l_Controller ? l_Controller->GetUserData()->GetName() : l_UserData->GetName();

        if ( name == "Player" ) return true;
    }

    return false;
}

CEnemy *GetClosestEnemy( CPhysicsManager *PhysicManager )
{
    CPhysicController *l_PlayerController = PhysXMInstance->CMapManager<CPhysicController>::GetResource( "Player" );
    Math::Vect3f l_PlayerPos = l_PlayerController->GetPosition();
    const std::vector<CPhysicUserData *>& l_UserDatas = PhysicManager->OverlapSphereController( 10, l_PlayerPos, 0xffffffff );
    std::vector<CPhysicUserData *>::const_iterator it = l_UserDatas.begin(),
                                                   it_end = l_UserDatas.end();

    float l_ActualDistance = 999999.9f;
    std::string l_EnemyName( "no_enemy" );

    for ( ; it != it_end; ++it )
    {
        CPhysicUserData *l_UserData = *it;

        if ( l_UserData->GetName() != "Player" )
        {
            CPhysicController *l_Controller = l_UserData->GetController();
            float l_Dist = ( l_PlayerPos - l_Controller->GetPosition() ).Length();

            if ( l_ActualDistance > l_Dist )
            {
                l_ActualDistance = l_Dist;
                l_EnemyName = l_Controller->GetUserData()->GetName();
            }
        }
    }

    CEnemy *l_Enemy = EnemyMInstance->GetResource( l_EnemyName );

    if ( l_Enemy )
        return l_Enemy;
    else
        return 0;
}

bool AddGrenade( CPhysicsManager *PhysicManager, const std::string& name, const std::string& group, const Math::Vect3f& dimensions,
                 const Math::Vect3f& position, unsigned int mask )
{
    return PhysicManager->AddActor
           (
               name, group, dimensions, Math::colWHITE, false,
               position, Math::Vect3f(), Math::Vect3f(),
               0, mask
           );
    return true;
}

void registerPhysX( lua_State *m_LS )
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PHYSIC USER DATA
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
    LUA_DECLARE_DERIVED_CLASS( CPhysicUserData, CName )
    LUA_DECLARE_CTOR_1( const std::string& )
    LUA_DECLARE_METHOD_PROTO( CPhysicUserData, SetColor, void ( CPhysicUserData::* )( const float, const float, const float, const float ) )
    LUA_DECLARE_METHOD( CPhysicUserData, SetPaint )
    LUA_DECLARE_METHOD( CPhysicUserData, GetName )
    LUA_DECLARE_METHOD( CPhysicUserData, GetController )
    LUA_END_DECLARATION


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PHYSIC ACTOR
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
    LUA_DECLARE_CLASS( CPhysicActor )
    LUA_DECLARE_CTOR_1( CPhysicUserData * )
    //LUA_DECLARE_METHOD( CPhysicActor, GetUserData )
    LUA_DECLARE_METHOD( CPhysicActor, CreateBody )
    LUA_DECLARE_METHOD( CPhysicActor, AddImpulseAtPos )
    //LUA_DECLARE_METHOD( CPhysicActor, AddForceAtPos )
    LUA_DECLARE_METHOD( CPhysicActor, GetPosition )
    LUA_END_DECLARATION


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PHYSICS MANAGER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
    LUA_DECLARE_CLASS( CPhysicsManager )
    LUA_DECLARE_METHOD( CPhysicsManager, RaycastClosestActor )
    LUA_DECLARE_METHOD( CPhysicsManager, RaycastClosestActorName )
    LUA_DECLARE_METHOD( CPhysicsManager, AddPhysicActor )
    LUA_DECLARE_METHOD( CPhysicsManager, AddController )
    LUA_DECLARE_METHOD( CPhysicsManager, ReleasePhysicActor )
    LUA_DECLARE_METHOD_PROTO( CPhysicsManager, GetUserData, CPhysicUserData * ( CPhysicsManager::* )( const std::string& ) )
    LUA_DECLARE_METHOD( CPhysicsManager, OverlapSphereController )
    .def( "OverlapSphere", &CPhysicsManager::OverlapSphereHardcoded )
    .def( "AddPhysicController", &Add_PhysicController )
    .def( "GetController", &CPhysicsManager::CMapManager<CPhysicController>::GetResource )
    .def( "GetActor", &CPhysicsManager::CMapManager<CPhysicActor>::GetResource )
    .def( "RaycastType", &RaycastType )
    .def( "PlayerInSight", &PlayerInSight )
    .def( "GetClosestEnemy", &GetClosestEnemy )
    LUA_END_DECLARATION

    REGISTER_LUA_FUNCTION( m_LS, AddGrenade );
    /*LUA_BEGIN_DECLARATION( aLuaState )
      LUA_DECLARE_METHOD_WITHOUT_CLASS( CreateInstanceMesh )
    LUA_END_DECLARATION*/

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PHYSICCONTROLLER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
    LUA_DECLARE_DERIVED_CLASS( CPhysicController, CObject3D )
    LUA_DECLARE_CTOR_9( float, float, float, float, float, ECollisionGroup, CPhysicUserData *, const Math::Vect3f&, float )
    LUA_DECLARE_METHOD_PROTO( CPhysicController, Move, void ( CPhysicController::* )( const Math::Vect3f&, float ) )
    LUA_DECLARE_METHOD_PROTO( CPhysicController, Jump, void( CPhysicController::* )( float ) )
    LUA_DECLARE_METHOD( CPhysicController, GetPosition )
    LUA_DECLARE_METHOD( CPhysicController, SetPosition )
    LUA_DECLARE_METHOD( CPhysicController, SetHeight )
    LUA_DECLARE_METHOD( CPhysicController, GetHeight )
    LUA_DECLARE_METHOD( CPhysicController, UpdateCharacterExtents )
    LUA_DECLARE_METHOD( CPhysicController, IsJumping )
    LUA_END_DECLARATION

    module( m_LS ) [
        class_<std::set<CPhysicUserData *>>( "ListaPUD" )
        .def( constructor<std::set<CPhysicUserData *>>() )
        .def( "size", &std::set<CPhysicUserData *>::size )
        .def( "getIdByResource", &set_getIdByResource<CPhysicUserData *> )
    ];

    module( m_LS ) [
        class_<std::vector<CPhysicUserData *>>( "vPUD" )
        .def( constructor<std::vector<CPhysicUserData *>>() )
        .def( "size", &std::vector<CPhysicUserData *>::size )
        .def( "GetResource", &vector_get<CPhysicUserData *> )
    ];

    /*
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PARTICLE MANAGER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
      LUA_DECLARE_CLASS( CParticleManager )
      LUA_DECLARE_METHOD( CParticleManager, AddEmitter )
      LUA_DECLARE_METHOD( CParticleManager, CreateCubeEmitter )
      LUA_DECLARE_METHOD( CParticleManager, CreateSphereEmitter )
    LUA_END_DECLARATION

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PARTICLE EMITTER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
      LUA_DECLARE_CLASS( CParticleEmitter )
      LUA_DECLARE_METHOD( CParticleEmitter, SetActive )
      LUA_DECLARE_METHOD( CParticleEmitter, SetAcceleration )
      LUA_DECLARE_METHOD( CParticleEmitter, SetDirection )
      LUA_DECLARE_METHOD( CParticleEmitter, SetPosition )
      LUA_DECLARE_METHOD( CParticleEmitter, SetVelocity )
      LUA_DECLARE_METHOD( CParticleEmitter, SetLifeTime )
      LUA_DECLARE_METHOD( CParticleEmitter, SetEmitterLifeTime )
    LUA_DECLARE_METHOD( CParticleEmitter, SetSize )
      LUA_DECLARE_METHOD( CParticleEmitter, SetTextureName )
    LUA_DECLARE_METHOD( CParticleEmitter, SetTimeToEmit )
      LUA_DECLARE_METHOD( CParticleEmitter, Generate )
       LUA_DECLARE_METHOD( CParticleEmitter, SetOrientate )
    LUA_END_DECLARATION

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // CUBE EMITTER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
      LUA_DECLARE_DERIVED_CLASS( CCubeEmitter, CParticleEmitter )
      LUA_DECLARE_DEFAULT_CTOR
      LUA_DECLARE_METHOD( CCubeEmitter, SetDepth )
      LUA_DECLARE_METHOD( CCubeEmitter, SetWidth )
      LUA_DECLARE_METHOD( CCubeEmitter, SetHeight )
      LUA_DECLARE_METHOD( CCubeEmitter, SetRandom )
    LUA_END_DECLARATION

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // SPHERE EMITTER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( m_LS )
      LUA_DECLARE_DERIVED_CLASS( CSphereEmitter, CParticleEmitter )
      LUA_DECLARE_DEFAULT_CTOR
      LUA_DECLARE_METHOD( CSphereEmitter, SetRandom )
      LUA_DECLARE_METHOD( CSphereEmitter, SetRadius )
      LUA_DECLARE_METHOD( CSphereEmitter, SetPitch )
      LUA_DECLARE_METHOD( CSphereEmitter, SetYaw )
    LUA_END_DECLARATION
    */
}