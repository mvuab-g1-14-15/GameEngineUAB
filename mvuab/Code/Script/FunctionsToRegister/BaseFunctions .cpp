#include "BaseFunctions.h"
#include "luabind_macros.h"

#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/operator.hpp>

#include "Math\Matrix44.h"
#include "Math\Matrix34.h"
#include "Math\Vector3.h"
#include "Math\Vector4.h"
#include "Utils\Visible.h"
#include "Utils\Name.h"
#include "Timer\Timer.h"
#include "Logger\Logger.h"
#include "Utils\Manager.h"
#include "Math\MathTypes.h"
#include "Utils\Types.h"
#include "Math\Color.h"
#include "Utils\TemplatedVectorMapManager.h"
#include "Timer\CounTDownTimerManager.h"
#include "Math/LerpAnimator1D.h"

#include "Utils/DebugHelper.h"

#include "XML\XMLTreeNode.h"

#include <sstream>

#define REGISTER_LUA_FUNCTION(LuaState, AddrFunction) {luabind::module(LuaState) [ luabind::def(#AddrFunction,AddrFunction) ];}

bool GetAttributeBool( CXMLTreeNode& Node, const char* aAttName, const bool& aDefaultAttValue )
{
  return Node.GetAttribute<bool>( aAttName, aDefaultAttValue );
}

int32 GetAttributeInt( CXMLTreeNode& Node, const char* aAttName, const int32& aDefaultAttValue )
{
  return Node.GetAttribute<int32>( aAttName, aDefaultAttValue );
}

uint32 GetAttributeUint( CXMLTreeNode& Node, const char* aAttName, const uint32& aDefaultAttValue )
{
  return Node.GetAttribute<uint32>( aAttName, aDefaultAttValue );
}

float32 GetAttributeFloat( CXMLTreeNode& Node, const char* aAttName, const float32& aDefaultAttValue )
{
  return Node.GetAttribute<float32>( aAttName, aDefaultAttValue );
}

std::string GetAttributeString( CXMLTreeNode& Node, const char* aAttName, const std::string& aDefaultAttValue )
{
  return Node.GetAttribute<std::string>( aAttName, aDefaultAttValue );
}

Math::CColor GetAttributeColor( CXMLTreeNode& Node, const char* aAttName, const Math::CColor& aDefaultAttValue )
{
  return Node.GetAttribute<Math::CColor>( aAttName, aDefaultAttValue );
}

Math::Vect2f GetAttributeVect2f( CXMLTreeNode& Node, const char* aAttName, const Math::Vect2f& aDefaultAttValue )
{
  return Node.GetAttribute<Math::Vect2f>( aAttName, aDefaultAttValue );
}

Math::Vect2i GetAttributeVect2i( CXMLTreeNode& Node, const char* aAttName, const Math::Vect2i& aDefaultAttValue )
{
  return Node.GetAttribute<Math::Vect2i>( aAttName, aDefaultAttValue );
}

Math::Vect2u GetAttributeVect2u( CXMLTreeNode& Node, const char* aAttName, const Math::Vect2u& aDefaultAttValue )
{
  return Node.GetAttribute<Math::Vect2u>( aAttName, aDefaultAttValue );
}

Math::Vect4f GetAttributeVect4f( CXMLTreeNode& Node, const char* aAttName, const Math::Vect4f& aDefaultAttValue )
{
  return Node.GetAttribute<Math::Vect4f>( aAttName, aDefaultAttValue );
}

Math::Vect3f GetAttributeVect3f( CXMLTreeNode& Node, const char* aAttName, const Math::Vect3f& aDefaultAttValue )
{
  return Node.GetAttribute<Math::Vect3f>( aAttName, aDefaultAttValue );
}

float aTan2( float x, float y )
{
  return Math::Utils::ATan2( x, y );
}

float Abs( float x )
{
  return Math::Utils::Abs( x );
}
using namespace luabind;

void registerXML( lua_State* aLuaState )
{
  LUA_BEGIN_DECLARATION( aLuaState )
  LUA_DECLARE_CLASS( CXMLTreeNode )
  LUA_DECLARE_DEFAULT_CTOR
  LUA_DECLARE_METHOD( CXMLTreeNode, Done )
  LUA_DECLARE_METHOD( CXMLTreeNode, IsOk )
  LUA_DECLARE_METHOD( CXMLTreeNode, LoadFile )
  LUA_DECLARE_METHOD( CXMLTreeNode, Exists )
  LUA_DECLARE_METHOD( CXMLTreeNode, ExistsKey )
  LUA_DECLARE_METHOD( CXMLTreeNode, GetName )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeBool )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeInt )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeUint )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeFloat )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeString )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeColor )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeVect2f )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeVect2i )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeVect2u )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeVect4f )
  .LUA_DECLARE_METHOD_WITHOUT_CLASS( GetAttributeVect3f )
  LUA_DECLARE_METHOD( CXMLTreeNode, LoadAndFindNode )
  LUA_DECLARE_METHOD( CXMLTreeNode, GetNumChildren )

  LUA_DECLARE_METHOD( CXMLTreeNode, GetChildren )
  LUA_DECLARE_METHOD( CXMLTreeNode, GetNode )
  LUA_END_DECLARATION

}

void DA_Normalize( Math::Vect3f* vector3 )
{
  vector3->Normalize( Math::One<float32>() );
}

Math::Vect3f DA_MulMat44fVect3f( Math::Mat44f* mat, const Math::Vect3f& vec )
{
  return ( *mat ) * vec;
}

std::string Vect3f2String( Math::Vect3f vector3 )
{
  std::ostringstream ss;
  ss << "(" << vector3.x << ", " << vector3.y << ", " << vector3.z << ")";
  std::string s( ss.str() );

  return s;
}

int BitOr( int a, int b )
{
  return a | b;
}

void registerTemplatedVectorMapManager( /*lua_State* aState*/ )
{
	/*
	LUA_BEGIN_DECLARATION( aState )
		LUA_DECLARE_CLASS( CTemplatedVectorMapManager )
		LUA_DECLARE_DEFAULT_CTOR
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, GetResource )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, Exist )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, RemoveResource )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, GetResourceById )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, AddResource )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, Destroy )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, GetResourcesMap )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, Clear )
		LUA_DECLARE_METHOD( CTemplatedVectorMapManager, GetResourcesCount )
		LUA_END_DECLARATION
		*/
}

void registerBase( lua_State* m_LS )
{
  REGISTER_LUA_FUNCTION( m_LS, aTan2 )
  REGISTER_LUA_FUNCTION( m_LS, Abs )
  module( m_LS )
  [
    def( "BitOr", &BitOr )
  ];

  module( m_LS )
  [
    class_<Math::Vect2f>( "Vect2f" )
    .def( constructor<float, float>() )
    .def( constructor<const Math::Vect2f&>() )
    .def( constructor<float>() )
    .def( constructor<>() )

    .def( const_self + const_self )
    .def( const_self - const_self )
    .def( const_self * const_self )
    .def( const_self == const_self )
    .def( const_self * float() )
    .def( const_self / float() )
    .def( float() * const_self )
    .def( float() / const_self )
    .def( const_self + float() )
    .def( const_self - float() )
    .def( - const_self )

    .def_readwrite( "x", &Math::Vect2f::x )
    .def_readwrite( "y", &Math::Vect2f::y )

  ];

  module( m_LS )
  [
    class_<Math::Vect3f>( "Vect3f" )
    .def( constructor<float, float, float>() )
    .def( constructor<const Math::Vect3f&>() )
    .def( constructor<float>() )
    .def( constructor<>() )

    .def( const_self + const_self )
    .def( const_self - const_self )
    .def( const_self * const_self )
    .def( const_self ^ const_self )
    .def( const_self == const_self )
    .def( const_self * float() )
    .def( const_self / float() )
    .def( float() * const_self )
    .def( float() / const_self )
    .def( const_self + float() )
    .def( const_self - float() )
    .def( float() + const_self )
    .def( float() - const_self )
    .def( - const_self )

    .def_readwrite( "x", &Math::Vect3f::x )
    .def_readwrite( "y", &Math::Vect3f::y )
    .def_readwrite( "z", &Math::Vect3f::z )

    .def( "CrossProduct", &Math::Vect3f::CrossProduct )
    .def( "Normalize", &DA_Normalize )
    .def( "Length", &Math::Vect3f::Length )
    .def( "DotProduct", &Math::Vect3f::DotProduct )
    .def( "ToString", &Vect3f2String )
    .def( "Distance", &Math::Vect3f::Distance )
    .def( "RotateY", &Math::Vect3f::RotateY )
  ];
  module( m_LS )
  [
    class_<Math::Vect4f>( "Vect4f" )
    .def( constructor<float, float, float, float>() )
    .def( constructor<const Math::Vect4f&>() )
    .def( constructor<float>() )
    .def( constructor<>() )

    .def_readwrite( "x", &Math::Vect4f::x )
    .def_readwrite( "y", &Math::Vect4f::y )
    .def_readwrite( "z", &Math::Vect4f::z )
    .def_readwrite( "w", &Math::Vect4f::w )
  ];
  module( m_LS )
  [
    class_<Math::Mat44f>( "Mat44f" )
    .def( constructor<>() )
    .def( "Mul", DA_MulMat44fVect3f )

    .def( "SetRotByAngleX", &Math::Mat44f::SetRotByAngleX )
    .def( "SetRotByAngleY", &Math::Mat44f::SetRotByAngleY )
    .def( "SetRotByAngleZ", &Math::Mat44f::SetRotByAngleZ )
    //.def("SetScale", &Math::Mat44f::SetScale) //TODO to ask
    .def( "SetIdentity", &Math::Mat44f::SetIdentity )
    .def( "SetPos", ( Math::Mat44f & ( Math::Mat44f::* )( const Math::Vect3f& ) ) &Math::Mat44f::SetPos )
    .def( "GetPos", &Math::Mat44f::GetPos )
    .def( "GetScaleX", &Math::Mat44f::GetScaleX )
    .def( "GetScaleY", &Math::Mat44f::GetScaleY )
    .def( "GetScaleZ", &Math::Mat44f::GetScaleZ )
    .def( "GetAngleX", &Math::Mat44f::GetAngleX )
    .def( "GetAngleY", &Math::Mat44f::GetAngleY )
    .def( "GetAngleZ", &Math::Mat44f::GetAngleZ )
    .def( "GetRoll", &Math::Mat44f::GetRoll )
    .def( "GetPitch", &Math::Mat44f::GetPitch )
    .def( "GetYaw", &Math::Mat44f::GetYaw )
    .def( "GetPitchRollYaw", &Math::Mat44f::GetPitchRollYaw )
    .def( "GetRotedByAngleX", &Math::Mat44f::GetRotedByAngleX )
    .def( "GetRotedByAngleY", &Math::Mat44f::GetRotedByAngleY )
    .def( "GetRotedByAngleZ", &Math::Mat44f::GetRotedByAngleZ )
    .def( "GetRotedByAnglesXZ", &Math::Mat44f::GetRotedByAnglesXZ )
    .def( "GetRotedByAnglesYXZ", &Math::Mat44f::GetRotedByAnglesYXZ )
    .def( "GetTranslated", &Math::Mat44f::GetTranslated )
    .def( "GetScaled", &Math::Mat44f::GetScaled )
    .def( "RotByAngleX", &Math::Mat44f::RotByAngleX )
    .def( "RotByAngleY", &Math::Mat44f::RotByAngleY )
    .def( "RotByAngleZ", &Math::Mat44f::RotByAngleZ )
    .def( "RotByAnglesXZ", &Math::Mat44f::RotByAnglesXZ )
    .def( "RotByAnglesYXZ", &Math::Mat44f::RotByAnglesYXZ )
    .def( "GetScale", &Math::Mat44f::GetScale )
    .def( "GetScaleX", &Math::Mat44f::GetScaleX )
    .def( "GetScaleY", &Math::Mat44f::GetScaleY )
    .def( "GetScaleZ", &Math::Mat44f::GetScaleZ )
    .def( "Translate", &Math::Mat44f::Translate )
    .def( "Transpose", &Math::Mat44f::Transpose )
    .def( "RotByAngleX", &Math::Mat44f::RotByAngleX )
    .def( "RotByAngleY", &Math::Mat44f::RotByAngleY )
    .def( "RotByAngleZ", &Math::Mat44f::RotByAngleZ )
    .def( "RotByAnglesXZ", &Math::Mat44f::RotByAnglesXZ )
    .def( "RotByAnglesYXZ", &Math::Mat44f::RotByAnglesYXZ )
    .def( "GetAnglesYXZ", &Math::Mat44f::GetAnglesYXZ )
    .def( "GetAngleX", &Math::Mat44f::GetAngleX )
    .def( "GetAngleY", &Math::Mat44f::GetAngleY )
    .def( "GetAngleZ", &Math::Mat44f::GetAngleZ )
  ];
  module( m_LS )
  [
    class_<CName>( "CName" )
    .def( constructor<>() )
    .def( "SetName", &CName::SetName )
    .def( "GetName", &CName::GetName )
  ];
  module( m_LS )
  [
    class_<CVisible>( "CVisible" )
    .def( constructor<>() )
    .def( "SetVisible", &CVisible::SetVisible )
    .def( "SetVisible", &CVisible::IsVisible  )
    .def( "GetVisible", &CVisible::GetVisible )
  ];
  module( m_LS )
  [
    class_<CCountDownTimerManager>( "CCountDownTimerManager" )
    .def( constructor<>() )
    .def( "AddTimer", &CCountDownTimerManager::AddTimer )
    .def( "isTimerFinish", &CCountDownTimerManager::isTimerFinish )
    .def( "SetTime", &CCountDownTimerManager::SetTime )
    .def( "GetTime", &CCountDownTimerManager::GetTime )
    .def( "GetElapsedTimeInPercent", &CCountDownTimerManager::GetElapsedTimeInPercent )
    .def( "GetLeftoverTime", &CCountDownTimerManager::GetLeftoverTime )
    .def( "GetElpasedTime", &CCountDownTimerManager::GetElpasedTime )
    .def( "ChangeTotalTime", &CCountDownTimerManager::ChangeTotalTime )
    .def( "Reset", &CCountDownTimerManager::Reset )
    .def( "ExistTimer", &CCountDownTimerManager::ExistTimer )
    .def( "IsActive", &CCountDownTimerManager::IsActive )
    .def( "SetActive", &CCountDownTimerManager::SetActive )
  ];
  module( m_LS )
  [
    class_<CTimer>( "CTimer" )
    .def( "GetElapsedTime", &CTimer::GetElapsedTime )
    .def( "GetSpeedFactor", &CTimer::GetSpeedFactor)
    .def( "GetConstFrameTime", &CTimer::GetConstFrameTime)
    .def( "GetTime", &CTimer::GetTime )
    .def( "GetTimeMs", &CTimer::GetTimeMs )
  ];
  module( m_LS )
  [
    class_<Math::CColor>( "CColor" )
    .def( constructor<float, float, float, float>() )
    .def( constructor<>() )
    .def( constructor<const Math::Vect4f&>() )
  ];

  module( m_LS )
  [
    class_<CManager>( "CManager" )
  ];

  registerXML( m_LS );
/*  registerTemplatedVectorMapManager( m_LS );*/

  module( m_LS )
  [
      class_<Math::CLerpAnimator1D>("CLerpAnimator1D")
      .def( constructor<>() )
      .def( "SetValues", &Math::CLerpAnimator1D::SetValues )
      .def( "Value", &Math::CLerpAnimator1D::Value )
      .def( "IsFinish", &Math::CLerpAnimator1D::IsFinish )
  ];

    module( m_LS )
    [
        class_<CDebugHelper>("CDebugHelper")
        .def( constructor<>() )
        .def( "Set", &CDebugHelper::Set )
        .def( "Update", &CDebugHelper::Update )
    ];
}