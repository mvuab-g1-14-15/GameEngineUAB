#include "TestProcess.h"
#include "Utils\Defines.h"
#include "GraphicsManager.h"
#include "InputManager.h"
#include "ActionManager.h"
#include "Fonts\FontManager.h"
#include "Object3D.h"
#include "Math\Matrix44.h"
#include "Logger\Logger.h"
#include "Utils\LogRender.h"
#include "Utils\Defines.h"
#include "Math\AABB.h"
#include "Core.h"
#include "Utils\BaseUtils.h"
#include "Timer\Timer.h"

#include "ScriptManager\ScriptManager.h"
#include "StaticMeshes\StaticMeshManager.h"
#include "StaticMeshes\InstanceMesh.h"
#include "StaticMeshes\StaticMesh.h"

#include "RenderableVertex\RenderableVertex.h"
#include "RenderableVertex\IndexedVertexs.h"
#include "RenderableVertex\VertexTypes.h"
#include "Texture\Texture.h"

#include "StaticMeshes\StaticMesh.h"
#include "Cameras\CameraFPShooter.h"
#include "Cameras\CameraManager.h"

#include "Effects\EffectManager.h"
#include "RenderableVertex\VertexTypes.h"

#include <d3dx9.h>

void GetFilesFromPath(const std::string &Path, std::vector<std::string> &_OutFiles);

CTestProcess::CTestProcess() : CProcess(),
  m_Speed( 0.1f ),
  m_Amount( 0.0f ), m_Angle( 0.0f ),  m_AngleMoon( 0.0f ), m_PaintAll(false)
{
  // CCameraManager::GetSingletonPtr()->NewCamera(CCamera::FirstPerson, "TestProcessCam", Math::Vect3f(15.0f,2.0f,0.0f),
  //                                             Math::Vect3f(0.0f,2.0f,0.0f) );
  //CCameraManager::GetSingletonPtr()->SetCurrentCamera("TestProcessCam");

  //unsigned short debug = VERTEX_TYPE_GEOMETRY | VERTEX_TYPE_INDICES | VERTEX_TYPE_WEIGHT | VERTEX_TYPE_NORMAL | VERTEX_TYPE_TANGENT | VERTEX_TYPE_BINORMAL | VERTEX_TYPE_TEXTURE1;

  //int i = 0;
}

CTestProcess::~CTestProcess()
{
  CLogger::GetSingletonPtr()->SaveLogsInFile();
}

void CTestProcess::Update()
{
  m_Amount +=  0.01f;
  m_Angle  += deltaTime * 0.05f;
  m_AngleMoon += deltaTime * 0.05f;

  CActionManager* pActionManager = CActionManager::GetSingletonPtr();

  if( pActionManager->DoAction("ReloadStaticMesh") )
    CCore::GetSingletonPtr()->GetStaticMeshManager()->Reload();

  if( pActionManager->DoAction("ReloadLUA") )
    CCore::GetSingletonPtr()->GetScriptManager()->Reload();

  if( pActionManager->DoAction("ReloadShaders") )
    CCore::GetSingletonPtr()->GetEffectManager()->Reload();

  if( pActionManager->DoAction("ReloadActionToInput") )
    CCore::GetSingletonPtr()->GetActionManager()->Reload();

  if( pActionManager->DoAction("SaveActionToInput") )
    CCore::GetSingletonPtr()->GetActionManager()->SaveXML("Data/Prueba.xml");

  if( pActionManager->DoAction("SetActionToInput") )
  {
    std::vector<S_INPUT_ACTION> vecInputAction;
    S_INPUT_ACTION sInputAction;
    sInputAction.m_DeviceType = IDV_KEYBOARD;
    sInputAction.m_EventType = EVENT_DOWN_UP;
    sInputAction.m_Code = KEY_H;
    sInputAction.m_sDeviceType = "IDV_KEYBOARD";
    sInputAction.m_sEventType = "EVENT_DOWN_UP";
    sInputAction.m_sCode = "KEY_H";
    vecInputAction.push_back(sInputAction);
    sInputAction.m_DeviceType = IDV_KEYBOARD;
    sInputAction.m_EventType = EVENT_DOWN;
    sInputAction.m_Code = KEY_LCTRL;
    sInputAction.m_sDeviceType = "IDV_KEYBOARD";
    sInputAction.m_sEventType = "EVENT_DOWN";
    sInputAction.m_sCode = "KEY_LCTRL";
    vecInputAction.push_back(sInputAction);
    CCore::GetSingletonPtr()->GetActionManager()->SetAction("SaveActionToInput", vecInputAction);
  }
  //CTPSCamera* pTPSCam = dynamic_cast<CTPSCamera*>(m_pCamera);
  //if(pTPSCam) pTPSCam->AddZoom(delta.z * m_Speed);

  CCore::GetSingletonPtr()->GetScriptManager()->RunCode("update()");
}

void CTestProcess::Init()
{
  CCore::GetSingletonPtr()->GetScriptManager()->RunCode("init()");
}

void CTestProcess::Render()
{
  CGraphicsManager* pGraphicsManager = GraphicsInstance;

  pGraphicsManager->DrawAxis(5);
  pGraphicsManager->DrawGrid(100, Math::colORANGE, 50, 50);
  //pGraphicsManager->DrawTeapot();

  CCore::GetSingletonPtr()->GetScriptManager()->RunCode("render()");

  // START: TO DELETE LATER IF IS NOT NECESSARY,
  unsigned int v = CGPUStatics::GetSingletonPtr()->GetVertexCount();
  unsigned int f = CGPUStatics::GetSingletonPtr()->GetFacesCount();
  unsigned int d = CGPUStatics::GetSingletonPtr()->GetDrawCount();
  CGPUStatics::GetSingletonPtr()->SetToZero();
  CCore::GetSingletonPtr()->GetFontManager()->DrawDefaultText(300, 0, Math::CColor(0.0f, 0.0f, 0.0f), "Vertex: %u   Faces: %u   Draws:%u", v, f, d);
  unsigned short VertexTypeBox1 = CCore::GetSingletonPtr()->GetStaticMeshManager()->GetResource("Box001")->GetRVs()[0]->GetVertexType();
  unsigned short VertexTypeBox2 = CCore::GetSingletonPtr()->GetStaticMeshManager()->GetResource("Box002")->GetRVs()[0]->GetVertexType();
  unsigned short VertexTypeBox3 = CCore::GetSingletonPtr()->GetStaticMeshManager()->GetResource("Box003")->GetRVs()[0]->GetVertexType();
  CCore::GetSingletonPtr()->GetFontManager()->DrawDefaultText(300, 15, Math::CColor(0.0f, 0.0f, 0.0f), "VertexType Box1: %u   VertexType Box2: %u   VertexType Box3:%u", VertexTypeBox1, VertexTypeBox2, VertexTypeBox3);

  unsigned short FVF = TNORMAL_T1_VERTEX::GetFVF();
  unsigned short FVF1 = TNORMAL_T2_VERTEX::GetFVF();
  // END: TO DELETE LATER IF IS NOT NECESSARY
}

void CTestProcess::RenderDebugInfo()
{
  CProcess::RenderDebugInfo();
}