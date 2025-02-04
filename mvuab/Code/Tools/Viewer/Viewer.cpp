#include <Windows.h>
#include "Math/Vector3.h"
#include "XML/XMLTreeNode.h"

#include "Logger\Logger.h"
#include "Utils\Defines.h"
#include "Engine.h"
#include "EngineConfig.h"
#include "Utils\Defines.h"
#include <string>
#include "Exceptions\Exception.h"

#include "Console\Console.h"

#include "ViewerProcess.h"

#include <iostream>
#include <ctime>
#include <AntTweakBar.h>

#include "resource.h"

#if _DEBUG
#include "Memory\MemLeaks.h"
#endif


#define APPLICATION_NAME  "VIEWER"

CViewerProcess* m_ViewerProcess = NULL;

void ShowErrorMessage( const std::string& message )
{
    bool logSaved = CLogger::GetSingletonPtr()->SaveLogsInFile();
    std::string end_message = "";

    if ( logSaved )
    {
        end_message += "Sorry, Application failed. Logs saved\n";
    }
    else
    {
        end_message += "Sorry, Application failed. Logs could not be saved\n";
    }

    end_message += message;
    MessageBox( 0, end_message.c_str(), "Engine Report", MB_OK | MB_ICONERROR );
}

LRESULT WINAPI OnExit(HWND hWnd )
{
    PostMessage( hWnd, WM_CLOSE, 0, 0 );
    return 0;
}

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    if( TwEventWin(hWnd, msg, wParam, lParam) ) // send event message to AntTweakBar
    {
        return 0;    // event has been handled by AntTweakBar
    }

    switch ( msg )
    {
    case WM_CREATE:
    {
        HMENU l_TopMenu, l_MenuLoad, l_MainMenu;
        l_TopMenu  = CreateMenu();
        l_MenuLoad = CreateMenu();
        l_MainMenu = CreateMenu();
        AppendMenu( l_MenuLoad, MF_STRING, eOptionScene, TEXT( "Scene" ) );
        AppendMenu( l_MenuLoad, MF_STRING, eOptionAnimatedModels, TEXT( "Animated models" ) );
        AppendMenu( l_MenuLoad, MF_STRING, eOptionParticles, TEXT( "Particles" ) );
        AppendMenu( l_MenuLoad, MF_STRING, eOptionLights, TEXT( "Lights" ) );
        AppendMenu( l_MainMenu, MF_STRING, eOptionExit, TEXT( "&Exit" ) );

        AppendMenu( l_TopMenu, MF_STRING, eOptionLights, TEXT( "Reload Lights" ) );
        AppendMenu( l_TopMenu, MF_STRING, eOptionParticles, TEXT( "Reload Particles" ) );
        AppendMenu( l_TopMenu, MF_STRING, eOptionGUI, TEXT( "Reload GUI" ) );
        AppendMenu( l_TopMenu, MF_STRING, eOptionEffect, TEXT( "Reload Effects" ) );

        AppendMenu( l_TopMenu, MF_STRING | MF_POPUP, ( UINT )l_MainMenu, TEXT( "&Principal" ) );
        AppendMenu( l_TopMenu, MF_STRING | MF_POPUP, ( UINT )l_MenuLoad, TEXT( "Reload" ) );
        SetMenu( hWnd, l_TopMenu );
    }
    break;

    case WM_COMMAND:
        switch ( LOWORD( wParam ) )
        {
        case eOptionExit:
            return OnExit(hWnd);
        default:
            m_ViewerProcess->OnClickedMenuOption( EMenuOption(wParam) );
        }

        break;

    case WM_DESTROY:
    {
        return OnExit(hWnd);
    }
    break;

    case WM_KEYDOWN:
    {
        switch ( wParam )
        {
        case VK_ESCAPE:
            PostQuitMessage(EXIT_SUCCESS);
        }
    }
    break;
    }//end switch( msg )

    return DefWindowProc( hWnd, msg, wParam, lParam );
}



//-----------------------------------------------------------------------
// WinMain
//-----------------------------------------------------------------------
int APIENTRY WinMain( HINSTANCE _hInstance, HINSTANCE _hPrevInstance,
                      LPSTR _lpCmdLine, int _nCmdShow )
{
    try
    {
#if _DEBUG
        MemLeaks::MemoryBegin();
#endif
        // Register the window class
        WNDCLASSEX wc =
        {
            sizeof( WNDCLASSEX ),
            CS_CLASSDC, MsgProc,
            0L,
            0L,
            GetModuleHandle( NULL ),
            NULL,
            NULL,
            NULL,
            NULL,
            APPLICATION_NAME,
            NULL
        };

        wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_ICON1));
        RegisterClassEx( &wc );
        // For random number generation
        srand( ( unsigned int )time( 0 ) );
        // Obtain the engine instance
        // Read the configuration of the engine
        CEngineConfig* lEngineConfig = new CEngineConfig();
        lEngineConfig->Load( "Data/config.xml" );

        CLogger* pLogger = new CLogger();

        CEngine* pEngine = new CEngine();

        m_ViewerProcess = new CViewerProcess();
        pEngine->SetRunnigProcess( m_ViewerProcess );
        DWORD style;

        if ( lEngineConfig->GetFullScreenMode() || lEngineConfig->GetFitDesktop() )
        {
            uint32 lTaskBarHeight = 0;

            if ( lEngineConfig->GetFitDesktop() )
            {
                RECT rect;
                HWND taskBar = FindWindow( "Shell_traywnd", NULL );

                if ( taskBar && GetWindowRect( taskBar, &rect ) )
                {
                    lTaskBarHeight = rect.bottom - rect.top;
                }
            }

            lEngineConfig->SetScreenPosition( Math::Vect2i( 0, 0 ) );
            lEngineConfig->SetScreenSize( Math::Vect2i( GetSystemMetrics( SM_CXSCREEN ),
                                          GetSystemMetrics( SM_CYSCREEN ) - lTaskBarHeight ) );
            lEngineConfig->SetScreenResolution( Math::Vect2i( GetSystemMetrics( SM_CXSCREEN ),
                                                GetSystemMetrics( SM_CYSCREEN ) - lTaskBarHeight ) );
            style = WS_POPUP | WS_VISIBLE;
        }
        else
        {
            style = WS_OVERLAPPED | WS_SYSMENU;
        }

        Math::Vect2i lScreenPosition = lEngineConfig->GetScreenPosition();
        Math::Vect2i lScreenSize     = lEngineConfig->GetScreenSize();
        HWND hWnd = CreateWindow
                    (
                        APPLICATION_NAME,
                        APPLICATION_NAME,
                        WS_OVERLAPPEDWINDOW,
                        lScreenPosition.x,
                        lScreenPosition.y,
                        lScreenSize.x,
                        lScreenSize.y,
                        NULL,
                        NULL,
                        wc.hInstance,
                        NULL
                    );

        lEngineConfig->SetWindowId( hWnd );
        pEngine->Init( lEngineConfig );
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );
        MSG msg;
        ZeroMemory( &msg, sizeof( msg ) );

        while ( msg.message != WM_QUIT )
        {
            if ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else
            {
                pEngine->ProcessInputs();
                pEngine->Update();
                pEngine->Render();
            }
        }

        UnregisterClass( APPLICATION_NAME, wc.hInstance );
        // A�adir una llamada a la alicaci�n para finalizar/liberar memoria de todos sus datos
        CHECKED_DELETE( pEngine );
        CHECKED_DELETE( pLogger );
        CHECKED_DELETE( lEngineConfig );
#if _DEBUG
        MemLeaks::MemoryEnd();
#endif
    }
    catch ( CException& e )
    {
        ShowErrorMessage( e.GetDescription() );
    }
    catch ( ... )
    {
        ShowErrorMessage( "Exception Occured" );
    }

    return 0;
}
