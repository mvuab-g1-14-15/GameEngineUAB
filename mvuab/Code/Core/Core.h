#ifndef CORE_H
#define CORE_H
#pragma once

// Includes
#include <string>
#include <vector>
#include <Windows.h>

#include "Utils\SingletonPattern.h"
#include "Utils\Defines.h"
#include "Math\Vector3.h"

// Forward declarations
class CGraphicsManager;
class CInputManager;
class CLanguageManager;
class CFontManager;
class CActionManager;
class CTextureManager;
class CStaticMeshManager;
class CRenderableObjectsManager;
class CRenderableObjectsLayersManager;
class CRenderableObjectTechniqueManager;
class CAnimatedModelsManager;
class CScriptManager;
class CCameraManager;
class CEffectManager;
class CLightManager;
class CTimer;
class CConsole;
class CSceneRendererCommandManager;
class CPhysicsManager;
class CEnemyManager;
class CTriggerManager;

class CCore : public CSingleton<CCore>
{
private:

  // Configuration
  HWND                            m_WindowId;
  bool                            m_FullScreenMode;
  bool                            m_ExclusiveModeInMouse;
  bool                            m_DrawPointerMouse;
  uint32                          m_ScreenWidth;
  uint32                          m_ScreenHeight;
  uint32                          m_WindowXPos;
  uint32                          m_WindowYPos;
  std::string                     m_CurrentLanguage;
  std::string                     m_ConfigPath;
  std::string                     m_GUIPath;
  std::string                     m_SoundPath;
  std::string                     m_FontsPath;
  std::string                     m_InputPath;
  std::string                     m_StaticMeshesPath;
  std::string                     m_RenderableObjectsPath;
  std::string                     m_RenderableObjectTechniquePath;
  std::string                     m_AnimatedModelsPath;
  std::string                     m_LuaRunPath;
  std::string                     m_EffectsPath;
  std::string                     m_LightsPath;
  std::string           m_SceneRendererCommandPath;
  std::string                     m_TriggersPath;
  std::vector<std::string>        m_v_languages;

  // Managers
  CGraphicsManager*                   m_pGraphicsManager;
  CInputManager*                      m_pInputManager;
  CLanguageManager*                   m_pLanguageManager;
  CFontManager*                       m_pFontManager;
  CActionManager*                     m_pActionManager;
  CStaticMeshManager*                 m_pStaticMeshManager;
  CRenderableObjectsManager*          m_pRenderableObjectsManager;
  CRenderableObjectsLayersManager*    m_pRenderableObjectsLayersManager;
  CRenderableObjectTechniqueManager*  m_pRenderableObjectTechniqueManager;
  CAnimatedModelsManager*             m_pAnimatedModelsManager;
  CTextureManager*                    m_pTextureManager;
  CScriptManager*                     m_pScriptManager;
  CCameraManager*                     m_pCameraManager;
  CEffectManager*                     m_pEffectManager;
  CLightManager*                      m_pLightManager;
  CSceneRendererCommandManager*       m_pSceneRendererCommandManager;
  CEnemyManager*                      m_pEnemyManager;
  CPhysicsManager*                    m_pPhysicsManager;
  CTriggerManager*                    m_pTriggerManager;
  CTimer*                             m_pTimer;
  CConsole*                           m_pConsole;
  void LoadXml();
  void InitManagers();

public:
  CCore();
  ~CCore();

  void Init( const std::string& aConfigPath, HWND aWindowId );
  void Update();
  void Render();

  // Methods that trace Lua
  void Trace( const std::string& TraceStr );


  // Getters and setters
  GET_SET_REF( std::string, ConfigPath );
  GET_SET_REF( std::string, GUIPath );
  GET_SET_REF( std::string, SoundPath );
  GET_SET_REF( std::string, AnimatedModelsPath );
  GET_SET( uint32, ScreenWidth );
  GET_SET( uint32, ScreenHeight );
  GET_SET( uint32, WindowXPos );
  GET_SET( uint32, WindowYPos );
  GET_SET( bool, FullScreenMode );
  GET_SET( bool, ExclusiveModeInMouse );
  GET_SET( bool, DrawPointerMouse );

  GET_SET_PTR( CInputManager, InputManager );
  GET_SET_PTR( CGraphicsManager, GraphicsManager );
  GET_SET_PTR( CFontManager, FontManager );
  GET_SET_PTR( CLanguageManager, LanguageManager );
  GET_SET_PTR( CActionManager, ActionManager );
  GET_SET_PTR( CStaticMeshManager, StaticMeshManager );
  GET_SET_PTR( CRenderableObjectsManager, RenderableObjectsManager );
  GET_SET_PTR( CRenderableObjectsLayersManager, RenderableObjectsLayersManager );
  GET_SET_PTR( CRenderableObjectTechniqueManager,
               RenderableObjectTechniqueManager );
  GET_SET_PTR( CAnimatedModelsManager, AnimatedModelsManager );
  GET_SET_PTR( CTextureManager, TextureManager );
  GET_SET_PTR( CScriptManager, ScriptManager );
  GET_SET_PTR( CCameraManager, CameraManager );
  GET_SET_PTR( CEffectManager, EffectManager );
  GET_SET_PTR( CLightManager, LightManager );
  GET_SET_PTR( CSceneRendererCommandManager, SceneRendererCommandManager );
  GET_SET_PTR( CPhysicsManager, PhysicsManager );
  GET_SET_PTR( CEnemyManager, EnemyManager );
  GET_SET_PTR( CTriggerManager, TriggerManager );
  GET_SET_PTR( CTimer, Timer );
};

#endif // GRAPHICS_MANAGER_H