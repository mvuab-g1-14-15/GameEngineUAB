#include "TestProcess.h"

//BASE
#include <Fonts\FontManager.h>
#include "Logger\Logger.h"
#include "ScriptManager.h"

//CORE

#include "EngineManagers.h"

//GRAPHICS
#include "GraphicsManager.h"
#include "Lights\LightManager.h"
#include "Cameras\CameraManager.h"
#include "Effects\EffectManager.h"
#include "RenderableVertex\VertexTypes.h"
#include "StaticMeshes\StaticMeshManager.h"
#include "RenderableVertex\IndexedVertexs.h"
#include "SceneRenderComands\SceneRendererCommandManager.h"
#include "RenderableObject\RenderableObjectsLayersManager.h"
#include "RenderableObject\RenderableObjectTechniqueManager.h"

//INPUT
#include "InputManager.h"
#include "ActionManager.h"

CTestProcess::CTestProcess() : CProcess()

{
    //CameraMInstance->NewCamera(CCamera::FirstPerson, "TestProcessCam", Math::Vect3f(-15.0f,2.0f,0.0f),
    //  Math::Vect3f(0.0f,2.0f,0.0f) );
    CameraMInstance->SetCurrentCamera( "TestProcessCam" );
    unsigned short debug = VERTEX_TYPE_GEOMETRY | VERTEX_TYPE_NORMAL |
                           VERTEX_TYPE_TANGENT | VERTEX_TYPE_BINORMAL | VERTEX_TYPE_TEXTURE1 |
                           VERTEX_TYPE_DIFFUSE;
}

CTestProcess::~CTestProcess()
{
    CLogger::GetSingletonPtr()->SaveLogsInFile();
}

void CTestProcess::Update()
{
    /////////////////////////////////////////////////////////////
    ////////////      RELOADS ACTIONS           /////////////////
    /////////////////////////////////////////////////////////////
    CActionManager* pActionManager = ActionManagerInstance;

    if ( pActionManager->DoAction( "ReloadStaticMesh" ) )
    { SMeshMInstance->Reload(); }

    if ( pActionManager->DoAction( "ReloadLUA" ) )
    { ScriptMInstance->Reload(); }

    if ( pActionManager->DoAction( "ReloadShaders" ) )
    {
        // NOTE this must be in this order
        EffectManagerInstance->Reload();
        LightMInstance->ReLoad();
        ROTMInstance->ReLoad();
        SMeshMInstance->Reload();
        ROLMInstance->Reload();
        SRCMInstance->ReLoad();
    }

    if ( pActionManager->DoAction( "ReloadActionToInput" ) )
    { ActionManagerInstance->Reload(); }

    /////////////////////////////////////////////////////////////
    ////////////      SAVE ACTIONS IN FILE      /////////////////
    /////////////////////////////////////////////////////////////
    if ( pActionManager->DoAction( "SaveActionToInput" ) )
    { ActionManagerInstance->SaveXML( "Data/Prueba.xml" ); }

    ///////////////////////////////////////////////////////////////////
    ////////////      Cambiar el c�digo de una acci�n      ////////////
    ///////////////////////////////////////////////////////////////////
    if ( pActionManager->DoAction( "SetActionToInput" ) )
    {
        std::vector<S_INPUT_ACTION> vecInputAction;
        S_INPUT_ACTION sInputAction;
        sInputAction.m_DeviceType = IDV_KEYBOARD;
        sInputAction.m_EventType = EVENT_DOWN_UP;
        sInputAction.m_Code = KEY_H;
        sInputAction.m_sDeviceType = "IDV_KEYBOARD";
        sInputAction.m_sEventType = "EVENT_DOWN_UP";
        sInputAction.m_sCode = "KEY_H";
        vecInputAction.push_back( sInputAction );
        sInputAction.m_DeviceType = IDV_KEYBOARD;
        sInputAction.m_EventType = EVENT_DOWN;
        sInputAction.m_Code = KEY_LCTRL;
        sInputAction.m_sDeviceType = "IDV_KEYBOARD";
        sInputAction.m_sEventType = "EVENT_DOWN";
        sInputAction.m_sCode = "KEY_LCTRL";
        vecInputAction.push_back( sInputAction );
        ActionManagerInstance->SetAction( "SaveActionToInput",
                                          vecInputAction );
    }

    ScriptMInstance->RunCode( "update()" );
}

void CTestProcess::Init()
{
    ScriptMInstance->RunCode( "init()" );
}

void CTestProcess::Render()
{
    CGraphicsManager* pGraphicsManager = GraphicsInstance;

    /*  pGraphicsManager->DrawAxis(5);
        pGraphicsManager->DrawGrid(100, Math::colORANGE, 50, 50);*/
    //pGraphicsManager->DrawTeapot();
    ScriptMInstance->RunCode( "render()" );
    // START: TO DELETE LATER IF IS NOT NECESSARY,
    unsigned int v = CGPUStatics::GetSingletonPtr()->GetVertexCount();
    unsigned int f = CGPUStatics::GetSingletonPtr()->GetFacesCount();
    unsigned int d = CGPUStatics::GetSingletonPtr()->GetDrawCount();
    CGPUStatics::GetSingletonPtr()->SetToZero();
    FontInstance->DrawDefaultText( 300, 0,
                                   Math::CColor( 0.0f, 0.0f, 0.0f ), "Vertex: %u   Faces: %u   Draws:%u", v, f,
                                   d );
    // END: TO DELETE LATER IF IS NOT NECESSARY
}

void CTestProcess::RenderDebugInfo()
{
    CProcess::RenderDebugInfo();
}