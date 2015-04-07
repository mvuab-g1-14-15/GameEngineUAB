#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "Types.h"
#include <assert.h>
#include "Logger\Logger.h"
#include <Windows.h>

// MACROS
#define GET_SET(TYPE, PARAMETER_NAME) \
    void Set##PARAMETER_NAME(TYPE PARAMETER_NAME) \
    { \
        m_##PARAMETER_NAME=PARAMETER_NAME; \
    } \
    TYPE Get##PARAMETER_NAME() \
    { \
        return m_##PARAMETER_NAME; \
    }

#define GET_SET_PTR(TYPE, PARAMETER_NAME) \
    void Set##PARAMETER_NAME(TYPE##* PARAMETER_NAME) \
    { \
        m_p##PARAMETER_NAME=PARAMETER_NAME; \
    } \
    TYPE##* Get##PARAMETER_NAME() \
    { \
        return m_p##PARAMETER_NAME; \
    }

#define GET_SET_REF(TYPE, PARAMETER_NAME) \
    void Set##PARAMETER_NAME(const TYPE &##PARAMETER_NAME) \
    { \
        m_##PARAMETER_NAME=PARAMETER_NAME; \
    } \
    const TYPE & Get##PARAMETER_NAME() const \
    { \
        return m_##PARAMETER_NAME; \
    }

#define GET_SET_ARRAY(TYPE, PARAMETER_NAME, SIZE) \
  void Set##PARAMETER_NAME(TYPE PARAMETER_NAME##[]) \
    { \
      for (int i = 0; i < SIZE; ++i)\
      {\
        m_##PARAMETER_NAME##[i] = PARAMETER_NAME##[i];\
      }\
    } \
    TYPE##* Get##PARAMETER_NAME() \
    { \
        return m_##PARAMETER_NAME; \
    }
#define DIRECTINPUT_VERSION     0x0800

#define CHECKED_DELETE(x)       if(x != NULL){ delete x; x = NULL; }
#define CHECKED_RELEASE(x)      if(x != NULL){ x->Release(); x = NULL; }
#define CHECKED_DELETE_ARRAY(x) if(x != NULL){ delete [] x; x = NULL; }

#define CoreInstance			CCore::GetSingletonPtr()
#define EngineInstance			CEngine::GetSingletonPtr()
#define GraphicsInstance		CCore::GetSingletonPtr()->GetGraphicsManager()
#define SoundInstance			CSoundManager::GetSingletonPtr()
#define EffectManagerInstance	CCore::GetSingletonPtr()->GetEffectManager()
#define InputManagerInstance	CCore::GetSingletonPtr()->GetInputManager()
#define ActionManagerInstance	CCore::GetSingletonPtr()->GetActionManager()

#define LOG_ERROR_APPLICATION( x, ... ) CLogger::GetSingletonPtr()->AddNewLog( ELL_ERROR, x, __VA_ARGS__ )
#define LOG_WARNING_APPLICATION( x, ...  ) CLogger::GetSingletonPtr()->AddNewLog( ELL_WARNING, x, __VA_ARGS__ )
#define LOG_INFO_APPLICATION( x, ...  ) CLogger::GetSingletonPtr()->AddNewLog( ELL_INFORMATION, x, __VA_ARGS__ )

#ifdef _DEBUG

#define ASSERT(expr, msg) \
  {\
    static char s_text[199] = ""; \
    if ( !( expr ) ) \
    {\
      static int callIt = 1; \
      if ( callIt )\
      { \
        wsprintf( s_text, "Expression: %s\nMessage: ( %s )\nFile '%s' Line %d\nOk:Continue\nCancel:Do not show more asserts", #expr, #msg, __FILE__, __LINE__ ); \
        switch ( ::MessageBox( NULL, s_text, "ASSERTION ERROR", MB_ICONEXCLAMATION | MB_OKCANCEL ) ) \
        {\
          case IDCANCEL: \
          { \
            callIt = 0; \
            _asm { int 3 } \
            break;\
          } \
        } \
      } \
    } \
  }\

#else

#define ASSERT(expr, msg) assert( expr && msg );

#endif

#define FATAL_ERROR(msg) \
  {\
    static char s_text[199] = ""; \
    wsprintf( s_text, "Message: ( %s )\nFile '%s' Line %d", #msg, __FILE__, __LINE__ ); \
    switch ( ::MessageBox( NULL, s_text, "FATAL ERROR", MB_ICONERROR | MB_OK ) ) \
    {\
      case IDOK: \
      { \
        ::exit( EXIT_FAILURE ); \
        break;\
      } \
    } \
  }\

#endif