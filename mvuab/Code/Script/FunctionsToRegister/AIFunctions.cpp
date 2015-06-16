#include "AIFunctions.h"
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/operator.hpp>

#include "Characters/Character.h"
#include "Characters\Enemies\Enemy.h"
#include "Characters\Enemies\PatrolEnemy.h"
#include "Characters\Enemies\BossEnemy.h"
#include "Characters\Enemies\EasyEnemy.h"
#include "Characters\Enemies\EnemyManager.h"
#include "Characters\Enemies\Shoot.h"
#include "StateMachine\StateMachine.h"
#include "StateMachine\State.h"
#include "Utils\MapManager.h"
#include "XML\XMLTreeNode.h"
#include "Object3D.h"
#include <vector>
#include "Math\Vector3.h"
#include "Pathfinding\AStar.h"
#include "RenderableObject\RenderableObject.h"
#include "AnimatedModels\AnimatedInstanceModel.h"

#include "luabind_macros.h"

using namespace luabind;
using namespace Math;
template<class T>
void vector_add( std::vector<T>& vec, T val )
{
    vec.push_back( val );
}
template<class T>
size_t vector_getIdByResource( std::vector<T>& vec, T val )
{
    for ( size_t i = 0 ; i < vec.size(); ++i )
        if ( vec[i] == val )
            return i;

    return 0;
}

template<class T>
T vector_get( std::vector<T>& vec, size_t i )
{
    return vec[i];
}


void registerCharacters( lua_State *aLuaState )
{
    ASSERT( aLuaState, "LuaState error in Register Characters" );
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // CHARACTER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( aLuaState )
    LUA_DECLARE_DERIVED_CLASS2( CCharacter, CObject3D, CName )
    LUA_DECLARE_CTOR_1( const std::string& )
    LUA_DECLARE_METHOD( CCharacter, GetHeight )
    LUA_DECLARE_METHOD( CCharacter, GetRadius )
    LUA_DECLARE_METHOD( CCharacter, GetTargetPosition )
    LUA_DECLARE_METHOD( CCharacter, SetTargetPosition )
    LUA_DECLARE_METHOD( CCharacter, SetTargetPositionOriginal )
    LUA_DECLARE_METHOD( CCharacter, AddDamage )
    LUA_DECLARE_METHOD( CCharacter, GetLife )
    LUA_DECLARE_METHOD( CCharacter, Move )
    LUA_DECLARE_METHOD( CCharacter, GetPosition )
    LUA_DECLARE_METHOD( CCharacter, GetDirection )
    LUA_DECLARE_METHOD( CCharacter, GetTimeToShoot )
    LUA_DECLARE_METHOD( CCharacter, GetMaxTimeToShoot )
    LUA_DECLARE_METHOD( CCharacter, SetTimeToShoot )
    LUA_END_DECLARATION
}

struct CEnemy_wrapper : CEnemy, luabind::wrap_base
{
    CEnemy_wrapper( CXMLTreeNode& Node, CStateMachine *aStateMachine )
        : CEnemy( Node, aStateMachine )
    {}

    virtual void Update()
    {
        call<void>( "Update" );
    }

    static void default_Update( CEnemy *ptr )
    {
        return ptr->CEnemy::Update();
    }
};

void registerEnemies( lua_State *aLuaState )
{
    ASSERT( aLuaState, "LuaState error in Register Enemies" );
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // ENEMY
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( aLuaState )
    LUA_DECLARE_DERIVED_CLASS( CEnemy, CCharacter )
    LUA_DECLARE_METHOD( CEnemy, ChangeState )
    LUA_DECLARE_METHOD( CEnemy, SetOnExit )
    LUA_DECLARE_METHOD( CEnemy, GetCurrentState )
    LUA_DECLARE_METHOD( CEnemy, GetLife )
    LUA_DECLARE_METHOD( CEnemy, GetRenderableObject )
    LUA_DECLARE_METHOD( CEnemy, GetAnimationModel )
    LUA_DECLARE_METHOD( CEnemy, MakeShoot )
    LUA_DECLARE_METHOD( CEnemy, MoveAStar )
    LUA_END_DECLARATION

    /*
    module(aLuaState)
    [
        class_<CEnemy, CEnemy_wrapper>("CEnemy")
            .def(constructor<CXMLTreeNode&, CStateMachine*>())
            .def("Update", &CEnemy::Update, &CEnemy_wrapper::default_Update)
    ];
    */

    LUA_BEGIN_DECLARATION( aLuaState )
    LUA_DECLARE_CLASS( CMapManager<CEnemy> )
    LUA_END_DECLARATION

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // PATROL ENEMY
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( aLuaState )
    LUA_DECLARE_DERIVED_CLASS( CPatrolEnemy, CEnemy )
    LUA_DECLARE_METHOD( CPatrolEnemy, GetWaypoints )
    LUA_DECLARE_METHOD( CPatrolEnemy, GetCurrentPoint )
    LUA_DECLARE_METHOD( CPatrolEnemy, SetCurrentPoint )
    LUA_DECLARE_METHOD( CPatrolEnemy, GetCount )
    LUA_END_DECLARATION

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // ENEMY MANAGER
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LUA_BEGIN_DECLARATION( aLuaState )
    LUA_DECLARE_DERIVED_CLASS( CEnemyManager, CMapManager<CEnemy> )
    LUA_DECLARE_METHOD( CEnemyManager, GetResource )
    LUA_DECLARE_METHOD( CEnemyManager, GetActualEnemy )
    LUA_END_DECLARATION
}

void registerAI( lua_State *aLuaState )
{
    LUA_BEGIN_DECLARATION( aLuaState )
    LUA_DECLARE_DERIVED_CLASS( CShoot, CObject3D )
    LUA_DECLARE_DEFAULT_CTOR
    LUA_DECLARE_CTOR_4( float, Math::Vect3f, Math::Vect3f, float )
    LUA_DECLARE_METHOD( CShoot, Init )
    LUA_DECLARE_METHOD( CShoot, Update )
    LUA_DECLARE_METHOD( CShoot, Render )
    LUA_DECLARE_METHOD( CShoot, Impacted )
    LUA_END_DECLARATION

    registerCharacters( aLuaState );
    registerEnemies( aLuaState );

    module( aLuaState )
    [
        class_<std::vector<Math::Vect3f>>( "VectorWaypoints" )
        .def( constructor<std::vector<Math::Vect3f>>() )
        .def( "GetResource", &vector_get<Math::Vect3f> )
    ];

}