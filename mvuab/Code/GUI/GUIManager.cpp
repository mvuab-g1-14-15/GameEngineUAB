

#include "Utils\Defines.h"
#include "GUIManager.h"
#include "EngineManagers.h"
#include "InputManager.h"
#include "Window\Windows.h"
#include "XML/XMLTreeNode.h"
#include "Texture\TextureManager.h"
#include "GraphicsManager.h"
//--Includes GuiElements---
#include "Exceptions/Exception.h"
#include "Widgets\TextBox.h"
#include "MousePointer\PointerMouse.h"
#include "Widgets\Button.h"
#include "Widgets\CheckButton.h"
#include "Widgets\DialogBox.h"
#include "Widgets\EditableTextBox.h"
#include "Widgets\Image.h"
#include "Widgets\TextBox.h"
#include "Widgets\ProgressBar.h"
#include "Widgets\RadioBox.h"
#include "Widgets\Slider.h"
#include "Widgets\Statictext.h"
#include "EngineConfig.h"
#include "Widgets\ConsoleGUI.h"
#include "Widgets\Map.h"
#include "Timer\Timer.h"
//-------------------------


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
CGUIManager::CGUIManager( const Math::Vect2i& resolution )
    :
    CManager()
    , m_sCurrentWindows( "HUD.xml" )
    , m_TextBox( NULL )
    , m_PointerMouse( NULL )
    , m_bRenderError( false )
    , m_bUpdateError( false )
    , m_ScreenResolution( EngineConfigInstance->GetScreenResolution() )
    , m_bIsOk( false )
    , m_bLoadedGuiFiles( false )
    , m_sLastLoadpathGUI_XML( "" )
    , m_bFirstUpdate( true )
    , m_bVisiblePointerMouse( true )
    , m_Console( 0 )
    //, m_Map( 0 )
{}


CGUIManager::CGUIManager( const CXMLTreeNode& atts )
    :
    CManager( atts )
    , m_sCurrentWindows( "HUD.xml" )
    , m_TextBox( NULL )
    , m_PointerMouse( NULL )
    , m_bRenderError( false )
    , m_bUpdateError( false )
    , m_ScreenResolution( EngineConfigInstance->GetScreenResolution() )
    , m_bIsOk( false )
    , m_bLoadedGuiFiles( false )
    , m_sLastLoadpathGUI_XML( "" )
    , m_bFirstUpdate( true )
    , m_bVisiblePointerMouse( true )
    , m_Console( 0 )
    // , m_Map( 0 )
{
}
//----------------------------------------------------------------------------
// Finalize data
//----------------------------------------------------------------------------
void CGUIManager::Done()
{
    if ( IsOk() )
    {

        //  CHECKED_DELETE( m_Map );
        Release();
        m_bIsOk = false;
    }
}

//----------------------------------------------------------------------------
// Free memory
//----------------------------------------------------------------------------
void CGUIManager::Release()
{
    LOG_INFO_APPLICATION( "GUIManager:: shutting down GUI" );

    std::map<std::string, CWindows*>::iterator it;
    std::map<std::string, CWindows*>::iterator itEnd( m_WindowsMap.end() );

    for ( it = m_WindowsMap.begin(); it != itEnd; ++it )
    {
        CWindows* windows = it->second;
        CHECKED_DELETE( windows );
    }

    m_WindowsMap.clear();

    CHECKED_DELETE( m_TextBox );
    CHECKED_DELETE( m_PointerMouse );
    CHECKED_DELETE( m_Console );

    LOG_INFO_APPLICATION( "GUIManager:: offline (ok)" );
}

//----------------------------------------------------------------------------
// Init
//----------------------------------------------------------------------------
void CGUIManager::Init()
{

    m_bIsOk = false;

    LOG_INFO_APPLICATION( "GUIManager:: calling initialization" );

    CXMLTreeNode parser;

    if ( !parser.LoadFile( mConfigPath.c_str() ) )
    {
        std::string msg_error = "CGUIManager::Init-> Error al leer el archivo de configuracion GUI: " + mConfigPath;
        LOG_ERROR_APPLICATION( msg_error.c_str() );
        throw CException( __FILE__, __LINE__, msg_error );
    }
    else
    {
        CTextureManager* textureM = TextureMInstance;

        CXMLTreeNode m = parser["GuiFiles"];

        if ( m.Exists() )
        {
            std::string path = m.GetPszProperty( "path", "" );

            if ( path.compare( "" ) != 0 )
            {
                m_bIsOk = LoadGuiFiles( path );
            }
            else
            {
                m_bIsOk = true;
            }
        }
        else
        {
            std::string msg_error =
                "CGUIManager::Init-> Error al intentar leer el tag <GuiFiles> del archivo de configuracion GUI: " + mConfigPath;
            LOG_WARNING_APPLICATION( msg_error.c_str() );
        }

        if ( m_bIsOk )
        {
            m = parser["TextBox"];

            if ( m.Exists() )
            {
                float posx = m.GetFloatProperty( "posx",
                                                 30.f );
                float posy = m.GetFloatProperty( "posy",
                                                 30.f );
                float w = m.GetFloatProperty( "width",
                                              35.f );
                float h = m.GetFloatProperty( "height",
                                              35.f );
                float button_w  = m.GetFloatProperty( "button_width", 5.f );
                float button_h  = m.GetFloatProperty( "button_height", 5.f );
                std::string buttonClose_normal  = m.GetPszProperty( "buttonClose_normal",
                                                  "/Data/textures/GUI/Textures_Test/CloseDialegBox.jpg" );
                std::string buttonClose_over = m.GetPszProperty( "buttonClose_over",
                                               "/Data/textures/GUI/Textures_Test/CloseDialegBoxO.jpg" );
                std::string buttonClose_clicked = m.GetPszProperty( "buttonClose_clicked",
                                                  "/Data/textures/GUI/Textures_Test/CloseDialegBoxC.jpg" );
                std::string buttonClose_deactivated = m.GetPszProperty( "buttonClose_deactivated",
                                                      "/Data/textures/GUI/Textures_Test/CloseDialegBoxC.jpg" );
                std::string buttonMove_normal  = m.GetPszProperty( "buttonMove_normal",
                                                 "/Data/textures/GUI/Textures_Test/ButtonDialegBoxN.jpg" );
                std::string buttonMove_over  = m.GetPszProperty( "buttonMove_over",
                                               "/Data/textures/GUI/Textures_Test/ButtonDialegBoxO.jpg" );
                std::string buttonMove_clicked  = m.GetPszProperty( "buttonMove_clicked",
                                                  "/Data/textures/GUI/Textures_Test/ButtonDialegBoxC.jpg" );
                std::string buttonMove_deactivated = m.GetPszProperty( "buttonMove_deactivated",
                                                     "/Data/textures/GUI/Textures_Test/ButtonDialegBoxC.jpg" );
                std::string quad = m.GetPszProperty( "quad",
                                                     "/Data/textures/GUI/Textures_Test/BaseDialegBox.jpg" );

                CTexture* Close_normal  = textureM->GetTexture( buttonClose_normal );
                CTexture* Close_over = textureM->GetTexture( buttonClose_over );
                CTexture* Close_clicked = textureM->GetTexture( buttonClose_clicked );
                CTexture* Close_deactivated = textureM->GetTexture( buttonClose_deactivated );
                CTexture* Move_normal  = textureM->GetTexture( buttonMove_normal );
                CTexture* Move_over  = textureM->GetTexture( buttonMove_over );
                CTexture* Move_clicked  = textureM->GetTexture( buttonMove_clicked );
                CTexture* Move_deactivated = textureM->GetTexture( buttonMove_deactivated );
                CTexture* back = textureM->GetTexture( quad );

                m_TextBox = new CTextBox( m_ScreenResolution.x, m_ScreenResolution.y, h, w, Math::Vect2f( posx, posy ), button_w,
                                          button_h );
                assert( m_TextBox );
                m_TextBox->SetName( "TextBox" );
                m_TextBox->SetCloseButtonTextures( Close_normal, Close_over, Close_clicked, Close_deactivated );
                m_TextBox->SetMoveButtonTextures( Move_normal, Move_over, Move_clicked, Move_deactivated );
                m_TextBox->SetDialogTexture( back );
                m_TextBox->SetVisible( false );
            }
            else
            {
                std::string msg_error = "CGUIManager::Init-> Error al intentar leer el tag <TextBox> del archivo de configuracion GUI: "
                                        + mConfigPath;
                LOG_WARNING_APPLICATION( msg_error.c_str() );
            }

            m = parser["PointerMouse"];

            if ( m.Exists() )
            {
                float posx = m.GetFloatProperty( "posx", 5.f );
                float posy = m.GetFloatProperty( "posy", 5.f );
                float w = m.GetFloatProperty( "width",  5.f );
                float h = m.GetFloatProperty( "height", 5.f );
                std::string texture = m.GetPszProperty( "texture",
                                                        "/Data/textures/GUI/Textures_Test/gui_pointer_mouse.tga" );
                bool isQuadrant = m.GetBoolProperty( "isQuadrant",  true );

                CTexture* texture_pointer = textureM->GetTexture( texture );

                m_PointerMouse = new CPointerMouse( m_ScreenResolution.y, m_ScreenResolution.x, h, w, Math::Vect2f( posx, posy ) );
                assert( m_PointerMouse );
                m_PointerMouse->SetTexture( texture_pointer, "default" );
                m_PointerMouse->SetActiveTexture( "default" );
                m_PointerMouse->SetQuadrant( isQuadrant );
            }
            else
            {
                std::string msg_error =
                    "CGUIManager::Init-> Error al intentar leer el tag <PointerMouse> del archivo de configuracion GUI: " + mConfigPath;
                LOG_WARNING_APPLICATION( msg_error.c_str() );
            }

            m_bIsOk = m_TextBox && m_PointerMouse;

        }//END if (m_bIsOk)

    } //END if (!parser.LoadFile(initGuiXML.c_str()))


    //CONSOLA GUI LUA
    m_Console = new CConsoleGUI( m_ScreenResolution.y, m_ScreenResolution.x, 8, 40, Math::Vect2f( 0, 90 ), Math::colBLACK,
                                 0U, "", 2U, 2U, false,
                                 true );

    //m_Map = new CMap( m_ScreenResolution.y, m_ScreenResolution.x, 30, 30, Math::Vect2f( 68, 65 ) );

    /*  m_Map->SetTexture(TextureMInstance->GetTexture("Data/textures/metal_plain.jpg"), "mapa");
        m_Map->SetActiveTexture("mapa");*/
    if ( !m_bIsOk )
    {
        Release();
    }
    else
    {
        LOG_INFO_APPLICATION( "CSoundManager:: online (ok)" );
    }

    //return m_bIsOk;

}

//----------------------------------------------------------------------------
// Render
//----------------------------------------------------------------------------
void CGUIManager::Render()
{
    if ( m_bIsOk )
    {
        if ( m_bLoadedGuiFiles )
        {
            std::map<std::string, CWindows*>::iterator it;
            it = m_WindowsMap.find( m_sCurrentWindows );

            if ( it != m_WindowsMap.end() )
            {
                CWindows* currentWindows = it->second;
                currentWindows->Render();
                RenderTransitionEffect();
                m_bRenderError = false;
            }
            else
            {
                if ( !m_bRenderError )
                {
                    LOG_ERROR_APPLICATION( "CGUIManager:: Se ha intentado pintar la windows %s no registrada ", m_sCurrentWindows.c_str() );
                    m_bRenderError = true;
                }
            }
        }//END if (m_bLoadedGuiFiles)

        //Siempre los �ltimos en pintarse
        assert( m_TextBox );
        m_TextBox->Render();
        assert( m_Console );
        m_Console->Render();
        /*  assert( m_Map );
            m_Map->Render();*/
        //RenderPointerMouse();

    }//END if (m_bIsOk)
}


void  CGUIManager::RenderPointerMouse()
{
    if ( m_bVisiblePointerMouse )
    {
        m_PointerMouse->Render();
    }
}
//----------------------------------------------------------------------------
// Update
//----------------------------------------------------------------------------

void CGUIManager::Update()
{
    if ( m_bIsOk )
    {
        assert( m_TextBox );
        assert( m_PointerMouse );

        //Si es la primera vez que actualizamos la GUI debemos hacer un load de la main.xml:
        if ( m_bFirstUpdate )
        {
            std::map<std::string, CWindows*>::iterator itCurrentWindows;
            itCurrentWindows = m_WindowsMap.find( m_sCurrentWindows );

            if ( itCurrentWindows != m_WindowsMap.end() )
            {
                CWindows* currentWindow = itCurrentWindows->second;
                currentWindow->LoadWindows();
            }

            m_bFirstUpdate = false;
        }

        m_PointerMouse->Update();

        m_TextBox->Update();

        m_Console->Update();

        //m_Map->Update();

        if ( !m_TextBox->IsVisible() && m_bLoadedGuiFiles )
        {
            std::map<std::string, CWindows*>::iterator it;
            it = m_WindowsMap.find( m_sCurrentWindows );

            if ( it != m_WindowsMap.end() )
            {
                CWindows* currentWindow = it->second;

                if ( !UpdateTransitionEffect() )
                {
                    currentWindow->Update();
                }

                m_bUpdateError = false;
            }
            else
            {
                if ( !m_bUpdateError )
                {
                    LOG_ERROR_APPLICATION( "CGUIManager:: Se ha intentado updatear la windows %s no registrada ",
                                           m_sCurrentWindows.c_str() );
                    m_bUpdateError = true;
                }
            }
        }//END if( !m_TextBox.IsVisible() )
    }
}

void CGUIManager::RenderTransitionEffect()
{
    if ( m_sTransitionEffect.m_bDoEffect )
    {
        switch ( m_sTransitionEffect.m_eType )
        {
            case TE_SHADOW:
                {
                    //Dibujamos un quad2d en toda la pantalla:
                    // - Durante la primera mitad de tiempo ir� de totalmente transparente a totalmente opaco negro
                    // - Durante la segunda mitad de tiempo ir� de totalmente opaco negro a totalmente transparente
                    Math::CColor color = Math::colBLACK;
                    float alpha;

                    if ( m_sTransitionEffect.m_fTimeCounter < m_sTransitionEffect.m_fTransitionTime * 0.5f )
                    {
                        //Durante la primera mitad del tiempo: alpha de 0.f -> 1.f
                        alpha = m_sTransitionEffect.m_fTimeCounter / m_sTransitionEffect.m_fTransitionTime * 0.5f;
                        color.SetAlpha( alpha );
                    }
                    else
                    {
                        //Durante la segunda mitad del tiempo: alpha de 1.f -> 0.f
                        alpha = m_sTransitionEffect.m_fTimeCounter / m_sTransitionEffect.m_fTransitionTime * 0.5f; //esto va de 1->2
                        color.SetAlpha( abs( alpha - 2 ) );
                    }

                    GraphicsInstance->DrawQuad2D( Math::Vect2i( 0, 0 ), m_ScreenResolution.x, m_ScreenResolution.y,
                                                  CGraphicsManager::UPPER_LEFT, color );
                }
                break;

            default:
                {
                    LOG_ERROR_APPLICATION( "CGUIManager::RenderTransitionEffect-> No se reconoce el efecto a realizar!" );
                }
        }
    }
}

bool CGUIManager::UpdateTransitionEffect()
{
    if ( m_sTransitionEffect.m_bDoEffect )
    {
        m_sTransitionEffect.m_fTimeCounter += deltaTimeMacro;

        if ( !m_sTransitionEffect.m_bActiveWindows &&
                m_sTransitionEffect.m_fTimeCounter > m_sTransitionEffect.m_fTransitionTime * 0.5f )
        {
            ActiveWindows( m_sTransitionEffect.m_sWindowsName );
            m_sTransitionEffect.m_bActiveWindows = true;

        }

        if ( m_sTransitionEffect.m_fTimeCounter > m_sTransitionEffect.m_fTransitionTime )
        {
            m_sTransitionEffect.m_bDoEffect = false;
            m_sTransitionEffect.m_fTimeCounter = 0.f;
            m_sTransitionEffect.m_bActiveWindows = false;
        }

        return true;
    }

    return false;
}

void CGUIManager::ActiveWindowsWithEffect( const std::string& inNameWindow, EtypeTransitionEffect type,
        float transitionTime )
{
    m_sTransitionEffect.m_bActiveWindows  = false;
    m_sTransitionEffect.m_bDoEffect = true;
    m_sTransitionEffect.m_eType  = type;
    m_sTransitionEffect.m_fTransitionTime  = transitionTime;
    m_sTransitionEffect.m_fTimeCounter = 0.f;
    m_sTransitionEffect.m_sWindowsName = inNameWindow;
}

void CGUIManager::ActiveWindows( const std::string& inNameWindow )
{
    std::map<std::string, CWindows*>::iterator it;
    it = m_WindowsMap.find( inNameWindow );

    if ( it != m_WindowsMap.end() )
    {
        //Primero finalizamos la ventana actual
        std::map<std::string, CWindows*>::iterator itCurrentWindows;
        itCurrentWindows = m_WindowsMap.find( m_sCurrentWindows );

        if ( itCurrentWindows != m_WindowsMap.end() )
        {
            CWindows* currentWindow = itCurrentWindows->second;
            currentWindow->SaveWindows();

            //A continuaci�n leemos los valores de la nueva ventana
            CWindows* windows = it->second;
            windows->LoadWindows();
            m_sCurrentWindows = inNameWindow;
        }
        else
        {
            LOG_ERROR_APPLICATION( "CGUIManager:: Al intentar cambiar de windows, la actual-->%s no se ha encontrado registrada",
                                   m_sCurrentWindows.c_str() );
        }
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: Al intentar cambiar a la windows-->%s esta no se ha encontrado registrada",
                               inNameWindow.c_str() );
    }
}


void CGUIManager::PushWindows( const std::string& inNameWindow )
{
    std::map<std::string, CWindows*>::iterator it;
    it = m_WindowsMap.find( inNameWindow );

    if ( it != m_WindowsMap.end() )
    {
        m_PrevWindows.push_back( m_sCurrentWindows );
        ActiveWindows( inNameWindow );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager::PushWindows Al intentar cambiar a la windows-->%s etsa no se ha encontrado registrada",
                               inNameWindow.c_str() );
    }
}

void CGUIManager::PopWindows()
{
    if ( m_PrevWindows.size() == 0 )
    {
        LOG_ERROR_APPLICATION( "CGUIManager::PopWindows -> El vector de PrevWindows esta vac�o!" );
    }
    else
    {
        std::string popWindows = m_PrevWindows[m_PrevWindows.size() - 1];
        m_PrevWindows.pop_back();
        ActiveWindows( popWindows );
    }
}


void CGUIManager::SetScreenResolution( const Math::Vect2i& resolution )
{
    std::map<std::string, CGuiElement*>::iterator it( m_ElementsMap.begin() );
    std::map<std::string, CGuiElement*>::iterator itEnd( m_ElementsMap.end() );

    while ( it != itEnd )
    {
        CGuiElement* guiElement = it->second;
        guiElement->SetWindowsWidth( resolution.x );
        guiElement->SetWindowsHeight( resolution.y );
        it++;
    }

}

bool CGUIManager::LoadGuiFiles( const std::string& pathGUI_XML )
{
    m_bLoadedGuiFiles = false;
    LOG_INFO_APPLICATION( "GUIManager:: Empezando a leer los .xml del directorio->%s", pathGUI_XML.c_str() );
    //Read xml files:
    std::map<std::string, CWindows*>::iterator it( m_WindowsMap.begin() );
    std::map<std::string, CWindows*>::iterator itEnd( m_WindowsMap.end() );

    while ( it != itEnd )
    {
        CWindows* windows = it->second;
        CHECKED_DELETE( windows );
        ++it;
    }

    m_WindowsMap.clear();
    m_ElementsMap.clear();
    m_sCurrentWindows = "HUD.xml";

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    // We read the XmlGui files
    m_sLastLoadpathGUI_XML = pathGUI_XML;
    std::string path_xmls = pathGUI_XML + "/*.xml";
    hFind = FindFirstFile( path_xmls.c_str(), &FindFileData );

    // we check the existence of the XmlGui directory
    if ( hFind == INVALID_HANDLE_VALUE )
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: Error al intentar leer los .xml del directorio %s", pathGUI_XML.c_str() );
        return false;
    }
    else
    {
        std::string FileName = FindFileData.cFileName;
        CWindows* windows = new CWindows();
        std::string pathFile = pathGUI_XML + "/";
        pathFile += FileName;
        bool isOk = false;
        isOk = windows->LoadXML( pathFile, m_ScreenResolution );

        if ( !isOk )
        {
            return false;
        }

        windows->RegisterElements( m_ElementsMap );
        windows->SetName( FileName );
        m_WindowsMap.insert( std::pair<std::string, CWindows*>( FileName, windows ) );

        while ( FindNextFile( hFind, &FindFileData ) != 0 )
        {
            std::string FileName = FindFileData.cFileName;
            CWindows* windows = new CWindows();
            std::string pathFile = pathGUI_XML + "/";
            pathFile += FileName;
            isOk = windows->LoadXML( pathFile, m_ScreenResolution );

            if ( !isOk )
            {
                return false;
            }

            windows->RegisterElements( m_ElementsMap );
            windows->SetName( FileName );
            m_WindowsMap.insert( std::pair<std::string, CWindows*>( FileName, windows ) );
        }

        FindClose( hFind );
    }

    m_bLoadedGuiFiles = true;
    return true;
}

bool CGUIManager::ReloadGuiFiles()
{
    bool isOk = false;
    std::string windows = m_sCurrentWindows;
    isOk = LoadGuiFiles( m_sLastLoadpathGUI_XML );

    std::map<std::string, CWindows*>::iterator it;
    it = m_WindowsMap.find( windows );

    if ( it != m_WindowsMap.end() )
    {
        m_sCurrentWindows = windows;
    }

    return isOk;
}

void CGUIManager::SetMessageBox( const std::string& text )
{
    if ( m_bIsOk )
    {
        assert( m_TextBox );

        if ( !m_TextBox->IsVisible() )
        {
            m_TextBox->SetMessage( text );
            m_TextBox->SetVisible( true );
            m_TextBox->SetActive( true );
            m_TextBox->SetPosition( Math::Vect2i( 30, 30 ) );
        }
    }
}

CWindows*  CGUIManager::GetWindow( const std::string& NameWindow )
{
    std::map<std::string, CWindows*>::iterator it;
    it = m_WindowsMap.find( NameWindow );

    if ( it != m_WindowsMap.end() )
    {
        return it->second;
    }

    return 0;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//  Funciones para modificar los GuiElements
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void CGUIManager::SetActiveGuiElement( const std::string& inNameGuiElement, bool flag )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameGuiElement );

    if ( it != m_ElementsMap.end() )
    {
        it->second->SetActive( flag );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetActiveGuiElement",
                               inNameGuiElement.c_str() );
    }
}

void CGUIManager::SetVisibleGuiElement( const std::string& inNameGuiElement, bool flag )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameGuiElement );

    if ( it != m_ElementsMap.end() )
    {
        it->second->SetVisible( flag );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetVisibleGuiElement",
                               inNameGuiElement.c_str() );
    }
}

bool CGUIManager::GetProgressBarValue( const std::string& inNameGuiElement, float& outValue )
{

    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameGuiElement );

    if ( it != m_ElementsMap.end() )
    {
        CProgressBar* progress = ( CProgressBar* )( it->second );
        outValue = progress->GetProgress();
        return true;
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetProgressBarValue",
                               inNameGuiElement.c_str() );
    }

    return false;
}

bool CGUIManager::SetProgressBarValue( const std::string& inNameGuiElement, float inValue )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameGuiElement );

    if ( it != m_ElementsMap.end() )
    {
        CProgressBar* progress = ( CProgressBar* )( it->second );
        progress->SetProgress( inValue );
        return true;
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetProgressBarValue",
                               inNameGuiElement.c_str() );
    }

    return false;
}

std::string CGUIManager::GetButtonCheckInRadioBox( const std::string& inNameRadioBox )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameRadioBox );

    if ( it != m_ElementsMap.end() )
    {
        CRadioBox* rb = ( CRadioBox* )( it->second );
        return rb->GetDefaultCheckButton();
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetButtonCheckInRadioBox",
                               inNameRadioBox.c_str() );
    }

    return "";
}

void CGUIManager::SetButtonCheckInRadioBox( const std::string& inNameRadioBox, const std::string& button )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameRadioBox );

    if ( it != m_ElementsMap.end() )
    {
        CRadioBox* rb = ( CRadioBox* )( it->second );
        rb->SetDefaultCheckButton( button );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetButtonCheckInRadioBox",
                               inNameRadioBox.c_str() );
    }
}

void CGUIManager::SetStateCheckButton( const std::string& inCheckButtonName, bool state )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inCheckButtonName );

    if ( it != m_ElementsMap.end() )
    {
        CCheckButton* checkButton = ( CCheckButton* )( it->second );
        checkButton->SetOn( state );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetStateCheckButton",
                               inCheckButtonName.c_str() );
    }
}

bool CGUIManager::GetStateCheckButton( const std::string& inCheckButtonName )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inCheckButtonName );

    if ( it != m_ElementsMap.end() )
    {
        CCheckButton* checkButton = ( CCheckButton* )( it->second );
        return checkButton->GetState();

    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetStateCheckButton",
                               inCheckButtonName.c_str() );
    }

    return false;
}

void CGUIManager::SetEditableTextBox( const std::string& inEditableText, const std::string& text )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inEditableText );

    if ( it != m_ElementsMap.end() )
    {
        CEditableTextBox* editableTextBox = ( CEditableTextBox* )( it->second );
        editableTextBox->SetBuffer( text );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetEditableTextBox",
                               inEditableText.c_str() );
    }
}

std::string CGUIManager::GetEditableTextBox( const std::string& inEditableText )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inEditableText );

    if ( it != m_ElementsMap.end() )
    {
        CEditableTextBox* editableTextBox = ( CEditableTextBox* )( it->second );
        return editableTextBox->GetBuffer();
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetEditableTextBox",
                               inEditableText.c_str() );
    }

    return "";
}

void CGUIManager::SetImage( const std::string& inImageName, const std::string& activeImage )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inImageName );

    if ( it != m_ElementsMap.end() )
    {
        CImage* image = ( CImage* )( it->second );
        image->SetActiveTexture( activeImage );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetImage",
                               inImageName.c_str() );
    }
}

std::string CGUIManager::GetImage( const std::string& inImageName )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inImageName );

    if ( it != m_ElementsMap.end() )
    {
        CImage* image = ( CImage* )( it->second );
        return image->GetActiveTexture();

    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetImage",
                               inImageName.c_str() );
    }

    return "";
}

void CGUIManager::PlayImage( const std::string& inImageName, float timePerImage, bool loop )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inImageName );

    if ( it != m_ElementsMap.end() )
    {
        CImage* image = ( CImage* )( it->second );
        image->PlayAnimation( timePerImage, loop );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetImage",
                               inImageName.c_str() );
    }
}

void CGUIManager::SetStateSlider( const std::string& inSliderName, float amount )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inSliderName );

    if ( it != m_ElementsMap.end() )
    {
        CSlider* slider = ( CSlider* )( it->second );
        slider->SetValue( amount );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetStateSlider",
                               inSliderName.c_str() );
    }
}

float CGUIManager::GetStateSlider( const std::string& inSliderName )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inSliderName );

    if ( it != m_ElementsMap.end() )
    {
        CSlider* slider = ( CSlider* )( it->second );
        float kk = slider->GetValue();
        return slider->GetValue();

    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion GetStateSlider",
                               inSliderName.c_str() );
    }

    return 0.f;
}


void CGUIManager::SetLiteralInStaticText( const std::string& inStaticText, const std::string& lit )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inStaticText );

    if ( it != m_ElementsMap.end() )
    {
        CStaticText* st = ( CStaticText* )( it->second );
        st->SetLiteral( lit );
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion SetVariableText",
                               inStaticText.c_str() );
    }
}


bool CGUIManager::NextBlockInRadioBox( const std::string& inNameRadioBox )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameRadioBox );

    if ( it != m_ElementsMap.end() )
    {
        CRadioBox* rb = ( CRadioBox* )( it->second );
        rb->NextBlock();
        return true;
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion NextBlockInRadioBox",
                               inNameRadioBox.c_str() );
    }

    return false;
}

bool CGUIManager::PrevBlockInRadioBox( const std::string& inNameRadioBox )
{
    std::map<std::string, CGuiElement*>::iterator it;
    it = m_ElementsMap.find( inNameRadioBox );

    if ( it != m_ElementsMap.end() )
    {
        CRadioBox* rb = ( CRadioBox* )( it->second );
        rb->PrevBlock();
        return true;
    }
    else
    {
        LOG_ERROR_APPLICATION( "CGUIManager:: No se ha encontrado el guiElement %s al ejecutar la funcion PrevBlockInRadioBox",
                               inNameRadioBox.c_str() );
    }

    return false;
}

void CGUIManager::Reload()
{
    Release();
    Init();
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------




//void CGUIManager::RegisterFunctions (CScriptManager* scriptManager)
//{
// lua_State* l_pLUAState = scriptManager->GetLuaState();
//
// using namespace luabind;
//
// // ahora registramos lo que querramos
// module(l_pLUAState)
//  [
// // registramos la clase CGUIManager
//  class_<CGUIManager>(CScriptRegister::SetClassName("CGUIManager"))
//
//  // registramos su constructor
// .def(constructor<const Math::Vect2i&>())
//
// // registramos sus funciones publicas
//  .def(  CScriptRegister::PushFunctionName(AUTO_COMPLETE), &CScriptRegister::AutoComplete)
//
//  .def(  CScriptRegister::PushFunctionName(HELP,"void","void",
//  "Muestra todas las funciones de esta clase"),
//  &CScriptRegister::Help)
//
// .def(  CScriptRegister::PushFunctionName("activeWindows", "void", "const std::string& inNameWindow",
//  "Pasa de la current windows a la que se pasa como argumento"),
//  &CGUIManager::ActiveWindows)
//
//  .def(  CScriptRegister::PushFunctionName("activeWindowsWithEffect", "void", "const std::string& inNameWindow, EtypeTransitionEffect type, float transitionTime",
//  "Igual que activeWindows pero efectuando un efecto"),
//  &CGUIManager::ActiveWindowsWithEffect)
//
//  .def(  CScriptRegister::PushFunctionName("pushWindows", "void", "const std::string& inNameWindow",
//  "Igual que activeWindows pero se queda en memoria la pagina anterior por si queremos hacer un pop"),
//  &CGUIManager::PushWindows)
//
//  .def(  CScriptRegister::PushFunctionName("popWindows", "void", "void",
//  "Si anteriormente hemos hecho un push con esta funci�n volvemos a la windows antigua"),
//  &CGUIManager::PopWindows)
//
//  .def(  CScriptRegister::PushFunctionName("setScreenResolution", "void", "const Math::Vect2i& resolution",
//  "Seteamos la resoluci�n actual de la pantalla cliente"),
//  &CGUIManager::SetScreenResolution)
//
//  .def(  CScriptRegister::PushFunctionName("loadGuiFiles", "bool", "const std::string& pathWindows",
//  "Cargamos de nuevo todas los ficheros .xml, el path tiene que "),
//  &CGUIManager::LoadGuiFiles)
//
//  .def(  CScriptRegister::PushFunctionName("reloadGui", "bool", "void",
//  "Cargamos todos los ficheros del path ya seteado con la llamada loadGuiFiles"),
//  &CGUIManager::ReloadGuiFiles)
//
//  .def(  CScriptRegister::PushFunctionName("setMessageBox", "void", "const std::string& text",
//  "Seteamos un texto al messageBox"),
//  &CGUIManager::SetMessageBox)
//
//  .def(  CScriptRegister::PushFunctionName("isVisibleMessage", "bool", "void",
//  "Pregutnamos si es o no visible el messageBox"),
//  &CGUIManager::IsVisibleMessage)
//
//  .def(  CScriptRegister::PushFunctionName("setVisiblePointerMouse", "void", "bool flag",
//  "Seteamos la visibilidad del puntero del mouse"),
//  &CGUIManager::SetVisiblePointerMouse)
//
//  //-------Funciones para modificar los GuiElements--------------------------
//  .def(  CScriptRegister::PushFunctionName("setActiveGuiElement", "void", "const std::string& inNameGuiElement, bool flag",
//  "Activa/desactiva un guiElement"),
//  &CGUIManager::SetActiveGuiElement)
//
//  .def(  CScriptRegister::PushFunctionName("setVisibleGuiElement", "void", "const std::string& inNameGuiElement, bool flag",
//  "Pone a visible/invisible un guiElement"),
//  &CGUIManager::SetVisibleGuiElement)
//
//  .def(  CScriptRegister::PushFunctionName("getProgressBarValue", "bool", "const std::string& inNameGuiElement, float& outValue",
//  "Obtiene el estado de un progressBar"),
//  &CGUIManager::GetProgressBarValue)
//
//  .def(  CScriptRegister::PushFunctionName("setProgressBarValue", "bool", "const std::string& inNameGuiElement, float inValue",
//  "Setea el estado de un progressBar"),
//  &CGUIManager::SetProgressBarValue)
//
//  .def(  CScriptRegister::PushFunctionName("nextBlockInRadioBox", "bool", "const std::string& inNameRadioBox",
//  "Setea el siguiente bloque a visualizar de un radioBox"),
//  &CGUIManager::NextBlockInRadioBox)
//
//  .def(  CScriptRegister::PushFunctionName("prevBlockInRadioBox", "bool", "const std::string& inNameRadioBox",
//  "Setea el anterior bloque a visualizar de un radioBox"),
//  &CGUIManager::PrevBlockInRadioBox)
//
//  .def(  CScriptRegister::PushFunctionName("getButtonCheckInRadioBox", "std::string", "const std::string& inNameRadioBox",
//  "Obtiene el identificador del button activo en un radioBox"),
//  &CGUIManager::GetButtonCheckInRadioBox)
//
//  .def(  CScriptRegister::PushFunctionName("setButtonCheckInRadioBox", "void", "const std::string& inNameRadioBox, const std::string& button",
//  "Setea el button activo en un radioBox"),
//  &CGUIManager::SetButtonCheckInRadioBox)
//
//  .def(  CScriptRegister::PushFunctionName("setStateCheckButton", "void", "const std::string& inNameRadioBox, const std::string& button",
//  "Setea el estado (on/off) de un checkButton"),
//  &CGUIManager::SetStateCheckButton)
//
//  .def(  CScriptRegister::PushFunctionName("getStateCheckButton", "bool", "const std::string& inCheckButtonName",
//  "Obtiene el estado (on/of) de un checkButton"),
//  &CGUIManager::GetStateCheckButton)
//
//  .def(  CScriptRegister::PushFunctionName("setStateSlider", "void", "const std::string& inSliderName, float amount",
//  "Setea el estado de un slider"),
//  &CGUIManager::SetStateSlider)
//
//  .def(  CScriptRegister::PushFunctionName("getStateSlider", "float", "const std::string& inSliderName",
//  "Obtiene el estado de un slider"),
//  &CGUIManager::GetStateSlider)
//
//  .def(  CScriptRegister::PushFunctionName("setLiteralInStaticText", "void", "const std::string& inStaticText, const std::string& lit",
//  "Setea el literal de un staticText"),
//  &CGUIManager::SetLiteralInStaticText)
//
//  .def(  CScriptRegister::PushFunctionName("setEditableTextBox", "void", "const std::string& inEditableTextName, const std::string& text",
//  "Setea el texto de un editableTextBox"),
//  &CGUIManager::SetEditableTextBox)
//
//  .def(  CScriptRegister::PushFunctionName("getEditableTextBox", "std::string", "const std::string& inEditableTextName",
//  "Obtiene el texto de un editableTextBox"),
//  &CGUIManager::GetEditableTextBox)
//
//  .def(  CScriptRegister::PushFunctionName("setImage", "void", "const std::string& inImageName, const std::string& activeImage",
//  "Setea cual de las texturas cargadas por un Image debe visualizar"),
//  &CGUIManager::SetImage)
//
//  .def(  CScriptRegister::PushFunctionName("getImage", "std::string", "const std::string& inImageName",
//  "Obtiene la textura a mostrar por un Image"),
//  &CGUIManager::GetImage)
//
//  .def(  CScriptRegister::PushFunctionName("playImage", "void", "const std::string& inImageName, float timePerImage, bool loop",
//  "Muestra sencuencialmente las texturas cargada por un Image"),
//  &CGUIManager::PlayImage)
//  ];
//
//}