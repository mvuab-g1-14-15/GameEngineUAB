#include "PhysicProcess.h"

//BASE
#include "Timer\Timer.h"
#include "Logger\Logger.h"
#include "Utils\BaseUtils.h"
#include "Fonts\FontManager.h"
#include "ScriptManager.h"

//CORE

#include "EngineManagers.h"

//GRAPHICS
#include "GraphicsManager.h"
#include "Lights\LightManager.h"
#include "Cameras\CameraManager.h"
#include "Effects\EffectManager.h"
#include "StaticMeshes\StaticMeshManager.h"
#include "RenderableVertex\IndexedVertexs.h"
#include "SceneRenderComands\SceneRendererCommandManager.h"
#include "RenderableObject\RenderableObjectsLayersManager.h"
#include "RenderableObject\RenderableObjectTechniqueManager.h"

//INPUT
#include "InputManager.h"
#include "ActionManager.h"
#include "Periphericals\Mouse.h"

//PHYSX
#include "PhysicsManager.h"
#include "Triggers\Trigger.h"
#include "Actor\PhysicActor.h"
#include "Utils/PhysicUserData.h"
#include "Triggers\TriggerManager.h"
#include "Joints\PhysicRevoluteJoint.h"
#include "Joints\PhysicSphericalJoint.h"
#include "Reports\PhysicTriggerReport.h"






CPhysicProcess::CPhysicProcess() : CProcess(),
  m_Salir( false ), m_Time( 0 )

{
}

CPhysicProcess::~CPhysicProcess()
{
  CLogger::GetSingletonPtr()->SaveLogsInFile();

  for ( size_t i = 0; i < m_vPA.size(); ++i )
    CHECKED_DELETE( m_vPA[i] );

  m_vPA.clear();

  for ( size_t i = 0; i < m_vPUD.size(); ++i )
    CHECKED_DELETE( m_vPUD[i] );

  m_vPUD.clear();

}

void CPhysicProcess::Update()
{
  /////////////////////////////////////////////////////////////
  ////////////      RELOADS ACTIONS           /////////////////
  /////////////////////////////////////////////////////////////
  CActionManager* pActionManager = ActionManagerInstance;

  if ( pActionManager->DoAction( "ReloadStaticMesh" ) )
    SMeshMInstance->Reload();

  if ( pActionManager->DoAction( "ReloadLUA" ) )
    ScriptMInstance->Reload();

  if ( pActionManager->DoAction( "ReloadShaders" ) )
  {
    // NOTE this must be in this order
    EffectManagerInstance->ReloadEffects();
    LightMInstance->ReLoad();
    ROTMInstance->ReLoad();
    SMeshMInstance->Reload();
    ROLMInstance->Reload();
    SRCMInstance->ReLoad();
  }

  if ( pActionManager->DoAction( "ReloadActionToInput" ) )
    ActionManagerInstance->Reload();


  /////////////////////////////////////////////////////////////
  ////////////      RAYCAST (DISPARO)         /////////////////
  /////////////////////////////////////////////////////////////
  if ( pActionManager->DoAction( "LeftMouseButtonPressed" ) )
  {
    CCamera* l_CurrentCamera =
      CameraMInstance->GetCurrentCamera();

    if ( l_CurrentCamera )
    {
      CPhysicsManager* l_PM = PhysXMInstance;
      CPhysicUserData* l_PhysicUserData = new CPhysicUserData( "Disparo" );
      l_PhysicUserData->SetColor( colRED );
      l_PhysicUserData->SetPaint( true );
      m_vPUD.push_back( l_PhysicUserData );
      CPhysicActor* l_Actor = new CPhysicActor( l_PhysicUserData );
      //If don't want box, you can remove this line
      CPhysicController* l_PC = l_PM->GetUserData( "CharacterController" )->GetController();
      l_Actor->AddBoxShapeHardcoded( Math::Vect3f( 0.05f, 0.05f, 0.05f ),
                                     l_PC->GetPosition(), Math::Vect3f( 0, 0, 0 ), Math::Vect3f( 0, 0, 0 ) );
      l_Actor->CreateBody( 1.0f );
      // Add at the end allways it needs to have a shape
      l_PM->AddPhysicActor( l_Actor );
      m_vPA.push_back( l_Actor );
      Math::Vect3f l_Direction( Math::Utils::Cos( l_PC->GetYaw() ) , 0.0f, Math::Utils::Sin( l_PC->GetYaw() ) );
      l_Actor->SetLinearVelocity( l_Direction.GetNormalized() *
                                  20.0f );
      SCollisionInfo& l_SCollisionInfo = SCollisionInfo::SCollisionInfo();
      uint32 mask = 1 << ECG_ESCENE;
      //CPhysicUserData* l_PUD = l_PM->RaycastClosestActor(l_CurrentCamera->GetPos(), l_CurrentCamera->GetDirection().GetNormalized(), mask, l_SCollisionInfo);
      CPhysicUserData* l_PUD = l_PM->RaycastClosestActorShoot(
                                 l_PC->GetPosition(), l_Direction.GetNormalized(),
                                 mask, l_SCollisionInfo, 40.0f );
    }
  }

  //////////////////////////////////////////////////////////////////
  ////////////      Enable or disable Current camera    ////////////
  //////////////////////////////////////////////////////////////////
  if ( pActionManager->DoAction( "DisableCamera" ) )
  {
    CCamera* l_CurrentCamera =
      CameraMInstance->GetCurrentCamera();
  }

  //////////////////////////////////////////////////////////////////
  ////////////        Draw Actor with color red         ////////////
  //////////////////////////////////////////////////////////////////
  //
  if ( pActionManager->DoAction( "DrawActor" ) )
  {
    CCamera* l_CurrentCamera = CameraMInstance->GetCurrentCamera();
    Vect2i l_PosMouse =
      InputManagerInstance->GetMouse()->GetPosition();
    Vect3f l_Pos, l_Dir;
    GraphicsInstance->GetRay( l_PosMouse, l_Pos, l_Dir );
    CPhysicsManager* l_PM = PhysXMInstance;
    SCollisionInfo& l_SCollisionInfo = SCollisionInfo::SCollisionInfo();
    uint32 mask = 1 << ECG_ESCENE;
    CPhysicUserData* l_PUD = l_PM->RaycastClosestActor( l_CurrentCamera->GetPosition(),
                             l_CurrentCamera->GetDirection().GetNormalized(), mask, l_SCollisionInfo );

    //CPhysicUserData* l_PUD = l_PM->RaycastClosestActor(l_Pos, l_Dir.GetNormalized(), mask, l_SCollisionInfo);
    if ( l_PUD )
      l_PUD->SetColor( colRED );
  }


  ScriptMInstance->RunCode( "update()" );

  //Change gravity
  //PhysXMInstance->AddGravity(Math::Vect3f(0,1*deltaTime,0));

  //////////////////////////////////////////////////////
  ////////////        TEST TRIGGER.LUA       ///////////
  //////////////////////////////////////////////////////
  if ( m_Salir && ( m_Time >= 1 ) )
  {
    PhysXMInstance->ReleasePhysicActor( m_vPA[m_vPA.size() - 1] );
    m_Salir = false;
    m_Time = 0;
  }
  else if ( m_Salir )
    m_Time += deltaTimeMacro;


  //////////////////////////////////////////////////////
  ////////////          MOVE BRIDGE          ///////////
  //////////////////////////////////////////////////////
  if ( pActionManager->DoAction( "LeftBridge" ) )
    m_PRJ->ActiveMotor( -200 );

  if ( pActionManager->DoAction( "RightBridge" ) )
    m_PRJ->ActiveMotor( 200 );
}

//////////////////////////////////////////////////////////////////////////////////////
////////////        INICIALIZACI�N DE LA ESCENA PARA EL TEST DE JOINTS     ///////////
//////////////////////////////////////////////////////////////////////////////////////
void CPhysicProcess::InitScenePhysicsSamplers()
{
  //Ejercicio 1 - Pendulo
  CPhysicsManager* l_PM = PhysXMInstance;

  CPhysicUserData* l_PUD = new CPhysicUserData( "Box" );
  l_PUD->SetPaint( true );
  l_PUD->SetColor( colBLACK );
  m_vPUD.push_back( l_PUD );
  CPhysicActor* l_pPhysicActor = new CPhysicActor( l_PUD );
  l_pPhysicActor->AddSphereShape( 0.4f, Math::Vect3f( 10, 4, 6 ) );
  l_pPhysicActor->CreateBody( 1.0f );
  m_vPA.push_back( l_pPhysicActor );
  l_PM->AddPhysicActor( l_pPhysicActor );
  l_pPhysicActor->AddVelocityAtPos( Math::Vect3f( 10, 0, 6 ), Math::Vect3f( 10, 4,
                                    6 ), 10 );
  m_PSJ = new CPhysicSphericalJoint();
  m_PSJ->SetInfo( Math::Vect3f( 5, 4, 6 ), l_pPhysicActor );
  l_PM->AddPhysicSphericalJoint( m_PSJ );
  //Ejercicio 2 - Puente levadiso
  l_PUD = new CPhysicUserData( "Box2" );
  l_PUD->SetPaint( true );
  l_PUD->SetColor( colWHITE );
  m_vPUD.push_back( l_PUD );
  l_pPhysicActor = new CPhysicActor( l_PUD );
  l_pPhysicActor->AddBoxShape( Math::Vect3f( 0.2f, 4, 1 ), Math::Vect3f( 0, 0, 0 ), Math::Vect3f( 0,
                               4, 0 ) );
  l_pPhysicActor->CreateBody( 1.0f );
  l_PM->AddPhysicActor( l_pPhysicActor );
  m_vPA.push_back( l_pPhysicActor );
  m_PRJ = new CPhysicRevoluteJoint();
  m_PRJ->SetInfo( Math::Vect3f( 0, 0, -1 ), Math::Vect3f( 0, 0, 0 ),
                  l_pPhysicActor );
  m_PRJ->SetMotor( 20, 5.f );
  l_PM->AddPhysicRevoluteJoint( m_PRJ );
  l_PUD = new CPhysicUserData( "Box3" );
  l_PUD->SetPaint( true );
  l_PUD->SetColor( 1, 0, 0, 1 );
  m_vPUD.push_back( l_PUD );
  l_pPhysicActor = new CPhysicActor( l_PUD );
  l_pPhysicActor->AddBoxShape( Math::Vect3f( 1, 1, 1 ), Math::Vect3f( -7, 0,
                               0 ) );
  m_vPA.push_back( l_pPhysicActor );
  l_PM->AddPhysicActor( l_pPhysicActor );
  l_PUD = new CPhysicUserData( "Box4" );
  l_PUD->SetPaint( true );
  l_PUD->SetColor( colBLACK );
  m_vPUD.push_back( l_PUD );
  l_pPhysicActor = new CPhysicActor( l_PUD );
  l_pPhysicActor->AddBoxShape( Math::Vect3f( 1, 1, 1 ), Math::Vect3f( 7, 0,
                               0 ) );
  m_vPA.push_back( l_pPhysicActor );
  l_PM->AddPhysicActor( l_pPhysicActor );
}

void CPhysicProcess::Init()
{
  ScriptMInstance->RunCode( "init()" );
  CPhysicsManager* l_PM = PhysXMInstance;
  ////////////////////////////////////////////////////////////////////////////////
  ////////////        SET THIS CLASS FOR GENERATE EVENTS TRIGGER       ///////////
  ////////////////////////////////////////////////////////////////////////////////

  l_PM->SetTriggerReport( this );

  InitScenePhysicsSamplers();

  CPhysicUserData* l_PUD = new CPhysicUserData( "Plane" );
  l_PUD->SetPaint( false );
  l_PUD->SetColor( colWHITE );
  m_vPUD.push_back( l_PUD );
  CPhysicActor* l_pPhysicActor = new CPhysicActor( l_PUD );
  l_pPhysicActor->AddBoxShape( Math::Vect3f( 1000.0f, 0.1f, 1000.0f ), Math::Vect3f( 0, 0, 0 ), Math::Vect3f( 0, 0,
                               0 ) );
  PhysXMInstance->AddPhysicActor( l_pPhysicActor );
  m_vPA.push_back( l_pPhysicActor );
}

void CPhysicProcess::Render()
{
}

void CPhysicProcess::RenderDebugInfo()
{
  CProcess::RenderDebugInfo();
}

//////////////////////////////////////////////////////////////////////////////////////
////////////   M�todo para pasarle el nombre del shape por parametro     /////////////
//////////////////////////////////////////////////////////////////////////////////////
std::string GetLuaCodeComplete( std::string LuaCode, std::string Other_Shape )
{
  std::ostringstream codeCat;
  size_t count = LuaCode.find( ")" );
  size_t count2 = LuaCode.find( "(" );
  std::string l_LuaCode2 = LuaCode.substr( 0, count );

  if ( ( count - count2 ) ==
       1 ) //Si es 1 es que no tiene parametro por defecto, por ejemplo  funcion() y pasar�a a function(other_shape)
    codeCat << l_LuaCode2 << "'" << Other_Shape.c_str() << "'" << ")";
  else //en este caso podr�a ser algo as� --> funcion(parametro1, parametro2) y a�adir el othershape como tercer parametro
    codeCat << l_LuaCode2 << "," << "'" << Other_Shape.c_str() << ")";

  return codeCat.str();
}
void CPhysicProcess::OnEnter( CPhysicUserData* _Entity_Trigger1,
                              CPhysicUserData* _Other_Shape )
{
  std::string l_Msg = "On Enter de " + _Other_Shape->GetName() + " a " +
                      _Entity_Trigger1->GetName();
  CTrigger* l_Trigger = TriggersMInstance->GetTriggerByName( _Entity_Trigger1->GetName() );


  //Get method name
  std::string l_LuaCode = l_Trigger->GetLUAByName( l_Trigger->ENTER );
  std::string l_NameShape = _Other_Shape->GetName();
  ScriptMInstance->RunCode( GetLuaCodeComplete( l_LuaCode, l_NameShape ) );
  LOG_INFO_APPLICATION( l_Msg.c_str() );
}
void CPhysicProcess::OnLeave( CPhysicUserData* _Entity_Trigger1,
                              CPhysicUserData* _Other_Shape )
{
  std::string l_Msg = "On Leave de " + _Other_Shape->GetName() + " a " +
                      _Entity_Trigger1->GetName();
  CTrigger* l_Trigger = TriggersMInstance->GetTriggerByName( _Entity_Trigger1->GetName() );
  //Get method name
  std::string l_LuaCode = l_Trigger->GetLUAByName( CTrigger::LEAVE );
  std::string l_NameShape = _Other_Shape->GetName();
  ScriptMInstance->RunCode( GetLuaCodeComplete( l_LuaCode, l_NameShape ) );
  LOG_INFO_APPLICATION( l_Msg.c_str() );
}
void CPhysicProcess::OnStay( CPhysicUserData* _Entity_Trigger1,
                             CPhysicUserData* _Other_Shape )
{
  std::string l_Msg = "On Stay de " + _Other_Shape->GetName() + " a " +
                      _Entity_Trigger1->GetName();
  CTrigger* l_Trigger = TriggersMInstance->GetTriggerByName( _Entity_Trigger1->GetName() );
  //Get method name
  std::string l_LuaCode = l_Trigger->GetLUAByName( CTrigger::STAY );
  std::string l_NameShape = _Other_Shape->GetName();
  ScriptMInstance->RunCode( GetLuaCodeComplete( l_LuaCode, l_NameShape ) );
  LOG_INFO_APPLICATION( l_Msg.c_str() );
}

CPhysicUserData* CPhysicProcess::GetNewPUD( const std::string& Name )
{
  return new CPhysicUserData( Name );
}
CPhysicActor* CPhysicProcess::GetNewPhysicActor( CPhysicUserData* PUD )
{
  return new CPhysicActor( PUD );
}

void CPhysicProcess::AddPudVector( CPhysicUserData* PUD )
{
  m_vPUD.push_back( PUD );
}
void CPhysicProcess::AddPhysicActorVector( CPhysicActor* PA )
{
  m_vPA.push_back( PA );
}
CPhysicUserData* CPhysicProcess::GetLastPUDInserted()
{
  return m_vPUD[m_vPUD.size() - 1];
}
