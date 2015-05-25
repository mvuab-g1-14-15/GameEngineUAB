#include "Process.h"
#include "EngineManagers.h"
#include "Utils\Defines.h"
#include "Effects\EffectManager.h"
#include "RenderableObject\RenderableObjectsManager.h"
#include "RenderableObject\RenderableObjectsLayerManager.h"
#include "RenderableObject\RenderableObjectTechniqueManager.h"
#include "Lights\LightManager.h"
#include "StaticMeshes\StaticMeshManager.h"
#include "SceneRenderComands\SceneRendererCommandManager.h"
#include "AnimatedModels\AnimatedModelsManager.h"
#include "Particles\ParticleManager.h"
#include "Billboard\BillboardManager.h"
#include "ActionManager.h"
#include "ScriptManager.h"
#include "GUIManager.h"
#include "Cameras\CameraManager.h"

namespace
{
	void OnOptionSceneClicked()
	{
		EffectManagerInstance->Reload();
		LightMInstance->ReLoad();
		ROTMInstance->ReLoad();
		SMeshMInstance->Reload();
		ROLMInstance->Reload();
		LightMInstance->ReLoad();
		SRCMInstance->ReLoad();
	}

	//---------------------------------------------------------------------------------------------------------------
	void OnOptionMeshClicked()
	{

	}

	void OnOptionAnimatedModelsClicked()
	{
		EffectManagerInstance->Reload();
		LightMInstance->ReLoad();
		ROTMInstance->ReLoad();
		SMeshMInstance->Destroy();
		ROLMInstance->Reload();
		AnimatedMInstance->Reload();
		LightMInstance->ReLoad();
		SRCMInstance->ReLoad();
	}

	//---------------------------------------------------------------------------------------------------------------
	void OnOptionParticlesClicked()
	{
		PSMan->Refresh();
	}

	//---------------------------------------------------------------------------------------------------------------
	void OnOptionLigthsClicked()
	{
		LightMInstance->ReLoad();
	}

	//---------------------------------------------------------------------------------------------------------------
	void OnOptionEffectsClicked()
	{
		// NOTE this must be in this order
		EffectManagerInstance->Reload();
		ROTMInstance->ReLoad();
		SMeshMInstance->Reload();
		ROLMInstance->Reload();
		LightMInstance->ReLoad();
		SRCMInstance->ReLoad();
	}

	//---------------------------------------------------------------------------------------------------------------
	void OnOptionReloadLua()
	{
		ScriptMInstance->Reload();
	}

	void OnOptionReloadGui()
	{
		GUIInstance->Reload();
		ScriptMInstance->RunCode( "ReloadGUI()" );
	}

	void OnOptionReloadInputs()
	{
		ActionManagerInstance->Reload();
	}

	void OnChangeCamera()
	{
		CCameraManager * lCM = CameraMInstance;
		if ( "FreeCam" == lCM->GetCurrentCameraName() )
			lCM->SetCurrentCamera( "TestProcessCam" );
		else
			lCM->SetCurrentCamera( "FreeCam" );
	}
}

//---------------------------------------------------------------------------------------------------------------
CProcess::CProcess() : m_pCamera( 0 )
{
}

//---------------------------------------------------------------------------------------------------------------
CProcess::CProcess( CCamera* ap_Camera ) : m_pCamera( ap_Camera )
{
}

//---------------------------------------------------------------------------------------------------------------s
void CProcess::RenderDebugInfo()
{
}

//---------------------------------------------------------------------------------------------------------------
void CProcess::OnClickedMenuOption( EMenuOption aMenuOption )
{
    switch( aMenuOption )
    {
        case eOptionScene:
            OnOptionSceneClicked();
            break;
        case eOptionMesh:
            OnOptionMeshClicked();
            break;
        case eOptionAnimatedModels:
            OnOptionAnimatedModelsClicked();
            break;
        case eOptionParticles:
            OnOptionParticlesClicked();
            break;
        case eOptionLights:
            OnOptionLigthsClicked();
            break;
        case eOptionEffect:
            OnOptionEffectsClicked();
            break;
        default:
            LOG_ERROR_APPLICATION("Invalid option for menu");
    }
}

void CProcess::ProcessReloads()
{
  CActionManager* lAM = ActionManagerInstance;
  if ( lAM->DoAction( "ReloadStaticMesh" ) )
  {
	  OnOptionMeshClicked();
  }
  else if( lAM->DoAction( "ReloadLUA" ) )
  {
	  OnOptionReloadLua();
  }
  else if( lAM->DoAction( "ReloadGUI" ) )
  {
	  OnOptionReloadGui();
  }
  else if( lAM->DoAction("ChangeCamera") )
  {
	  OnChangeCamera();
  }
  else if ( lAM->DoAction( "ReloadShaders" ) )
  {
	  OnOptionEffectsClicked();
  }
}