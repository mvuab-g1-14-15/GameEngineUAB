#include "SceneRenderComands\RenderDebugCommand.h"
#include "GraphicsManager.h"
#include "Cameras/CameraManager.h"
#include "PathFinding\AStar.h"
#include "Lights\LightManager.h"
#include "Engine.h"

#include "PhysicsManager.h"
#include "EngineManagers.h"
#include "ScriptManager.h"
#include "Triggers/TriggerManager.h"

#include "Characters/Enemies/EnemyManager.h"

CRenderDebugCommand::CRenderDebugCommand( CXMLTreeNode& atts )
  : CSceneRendererCommand( atts )
  , m_RenderCameras( atts.GetAttribute<bool>( "render_cameras", false ) )
  , m_RenderLights( atts.GetAttribute<bool>( "render_lights", false ) )
  , m_RenderPhysics( atts.GetAttribute<bool>( "render_physics", false ) )
  , m_RenderGraph( atts.GetAttribute<bool>( "render_graph", false ) )
  , m_RenderTriggers( atts.GetAttribute<bool>( "render_triggers", false ) )
{
}

CRenderDebugCommand::~CRenderDebugCommand()
{
}

void CRenderDebugCommand::Execute( CGraphicsManager& GM )
{
  BROFILER_CATEGORY( "CRenderDebugCommand::Execute", Profiler::Color::Orchid )

  if ( m_RenderCameras )
    CameraMInstance->Render();

  if ( m_RenderLights )
    LightMInstance->Render();

  if ( m_RenderGraph )
    ScriptMInstance->RunCode( "g_EnemyManager:GetAStar():Render()" );

  if ( m_RenderPhysics )
    PhysXMInstance->Render();

  if ( m_RenderTriggers )
    TriggersMInstance->Render();
}