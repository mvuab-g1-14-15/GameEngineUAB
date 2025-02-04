

#include "Widgets\EditableTextBox.h"
#include "InputManager.h"
#include "Texture/Texture.h"
#include "GraphicsManager.h"
#include "Fonts/FontManager.h"
#include "EngineManagers.h"
#include "ScriptManager.h"

#include "Timer\Timer.h"

CEditableTextBox::CEditableTextBox( const CXMLTreeNode& aNode, const Math::Vect2i& screenResolution )
    : CGuiElement( aNode, screenResolution )
    , m_uFontID( aNode.GetAttribute<int32>( "id_font", 0 ) )
    , m_TextColor( aNode.GetAttribute<Math::Vect3f>( "font_color", Math::Vect3f()) )
    , m_pBackGroundTexture( aNode.GetAttribute<CTexture>( "texture_quad" ) )
    , m_sBuffer( aNode.GetAttribute<std::string>( "buffer", "" ) )
    , m_sFocusObject( " " )
    , m_bShift( false )
    , m_fTime( 0.5f )
    , m_fTimeCount( 0.f )
    , m_fTimeWriteFast( 0.8f )
    , m_fTimeWriteFastCount( 0.f )
    , m_bReturnPress( false )
    , m_fSpeed( 0.05f )
    , m_fSpeedCount( 0.f )
    , m_fWidthMargin( /* 0.05f * witdh_percent * windowsWidth * 0.01f */)
    , m_fHeightMargin( 0.2f )
    , m_BackTime1( 0.f )
    , m_BackTime2( 0.f )
    , m_uCursorPos( 0 )
{
    /*
     std::string OnSaveValue = pNewNode.GetAttribute<std::string>( "OnSaveValue", "" );
     std::string OnLoadValue = pNewNode.GetAttribute<std::string>( "OnLoadValue", "" );
     */
    for ( int cont = 0; cont < 256; cont++ )
    {
        m_bFlagDiks[cont] = false;
    }

    m_Result = new uint16;
}

CEditableTextBox::~CEditableTextBox()
{
    CHECKED_DELETE( m_Result );
}

//---------------Interfaz de GuiElement----------------------
void CEditableTextBox::Render()
{
    if ( GetVisible() )
    {

        if ( m_pBackGroundTexture )
        {
            //TODO RAUL
            GraphicsInstance->DrawQuad2D( CGuiElement::m_Position, CGuiElement::m_uWidth, CGuiElement::m_uHeight,
                                          CGraphicsManager::UPPER_LEFT, m_pBackGroundTexture );
        }
        else
        {
            GraphicsInstance->DrawQuad2D( CGuiElement::m_Position, CGuiElement::m_uWidth, CGuiElement::m_uHeight,
                                          CGraphicsManager::UPPER_LEFT, m_BackGroundColor );
        }

        Math::Vect2i pos;
        uint32 sizeY = FontInstance->SizeY( "X", m_uFontID );
        pos.x = CGuiElement::m_Position.x + ( uint32 )m_fWidthMargin;
        pos.y = CGuiElement::m_Position.y + ( uint32 )( CGuiElement::m_uHeight * 0.5f - sizeY * 0.5 );

        if ( CGuiElement::m_bIsActive )
        {
            //Draw Info Text
            std::string total_text;

            if ( m_uCursorPos > m_sBuffer.size() )
            {
                total_text = m_sBuffer + m_sFocusObject;
                FontInstance->DrawText( pos.x, pos.y, m_TextColor, m_uFontID, total_text.c_str() );
            }
            else
            {
                total_text = m_sBuffer.substr( 0, m_uCursorPos );
                total_text = total_text + m_sFocusObject;
                total_text = total_text + m_sBuffer.substr( m_uCursorPos );
                FontInstance->DrawText( pos.x, pos.y, m_TextColor, m_uFontID, total_text.c_str() );
            }
        }
        else
        {
            FontInstance->DrawText( pos.x, pos.y, m_TextColor, m_uFontID, m_sBuffer.c_str() );
        }

        //Finalmente renderizamos el texto:
        CGuiElement::RenderText();

    }//END if( GetVisible() )
}

void CEditableTextBox::Update()
{
    std::string buffer_prev = m_sBuffer;

    if ( GetVisible() && CGuiElement::m_bIsActive )
    {
        Math::Vect2i mousePosition;
        InputManagerInstance->GetPosition( IDV_MOUSE, mousePosition );
        CGuiElement::CalculatePosMouse( mousePosition );

        if ( CGuiElement::IsInside() )
        {
            if ( InputManagerInstance->IsUpDown( IDV_MOUSE, MOUSE_BUTTON_LEFT ) )
            {
                CGuiElement::m_bFocus = true;
            }
        }
    }

    if ( CGuiElement::m_bFocus )
    {
        m_bShift = ( InputManagerInstance->IsDown( IDV_KEYBOARD, KEY_RSHIFT ) ||
                     InputManagerInstance->IsDown( IDV_KEYBOARD, KEY_LSHIFT ) );

        if ( InputManagerInstance->IsUpDown( IDV_KEYBOARD, KEY_LEFT ) )
        {

            if ( m_uCursorPos > 0 )
            {
                m_uCursorPos--;
            }
        }

        if ( InputManagerInstance->IsUpDown( IDV_KEYBOARD, KEY_RIGHT ) )
        {
            m_uCursorPos++;

            if ( m_uCursorPos > m_sBuffer.size() )
            {
                m_uCursorPos = ( uint8 )m_sBuffer.size();
            }
        }

        if ( InputManagerInstance->IsUpDown( IDV_KEYBOARD, KEY_BACK ) )
        {
            Remove1character();
        }
        else
        {
            ProcessNewEntries();
        }

        if ( InputManagerInstance->IsUpDown( IDV_KEYBOARD, KEY_RETURN ) )
        {
            m_bReturnPress = true;
            m_uCursorPos = ( uint8 )m_sBuffer.size();
        }

        if ( InputManagerInstance->IsDown( IDV_KEYBOARD, KEY_BACK ) )
        {
            m_BackTime1 += constFrameTime;

            if ( m_BackTime1 > 0.7f )
            {
                m_BackTime2 += constFrameTime;

                if ( m_BackTime2 > 0.05f )
                {
                    Remove1character();
                    m_BackTime2 = 0.f;
                }
            }
        }
        else
        {
            m_BackTime1 = 0.f;
            m_BackTime2 = 0.f;
        }

        if ( InputManagerInstance->IsUpDown( IDV_KEYBOARD, KEY_RSHIFT ) )
        {
            if ( m_uCursorPos < m_sBuffer.size() )
            {
                m_sBuffer.erase( m_sBuffer.begin() + m_uCursorPos );
            }
        }



        m_fTimeCount += constFrameTime;

        if ( m_fTimeCount > m_fTime )
        {
            if ( !m_sFocusObject.compare( " " ) )
            {
                m_sFocusObject = "_";
            }
            else
            {
                m_sFocusObject = " ";
            }

            m_fTimeCount = 0.f;
        }
    }// End if (CGuiElement::m_bFocus)

    std::string temp = m_sBuffer + m_sFocusObject;
    CFontManager* fm = FontInstance;
    uint32 sizeX = fm->SizeX( temp.c_str(), m_uFontID ) + ( uint32 )( 2 * m_fWidthMargin );

    if ( sizeX > CGuiElement::m_uWidth )
    {
        m_sBuffer = buffer_prev;
    }
}

void CEditableTextBox::Remove1character()
{
    if ( m_uCursorPos > 0 )
    {
        m_sBuffer.erase( m_sBuffer.begin() + m_uCursorPos - 1 );
        m_uCursorPos--;
    }
}

//---------------Interfaz de EditableTextBox---------------------------
bool CEditableTextBox::IsReturnPress()
{
    if ( m_bReturnPress )
    {
        m_bReturnPress = false;
        return true;
    }
    else
    {
        return false;
    }
}

void CEditableTextBox::ProcessNewEntries()
{
    uint32 index = 237;

    for ( uint32 cont = 0; cont <= index; cont++ )
    {
        if ( InputManagerInstance->IsUpDown( IDV_KEYBOARD, cont ) )
        {
            if ( InputManagerInstance->Scan2ascii( cont, m_Result ) )
            {
                std::string aux = m_sBuffer.substr( 0, m_uCursorPos );
                aux += char( m_Result[0] );
                m_sBuffer = aux + m_sBuffer.substr( m_uCursorPos );
                m_uCursorPos++;
            }
        }
    }
}

void CEditableTextBox::SetFont( Math::CColor textColor, uint32 fontID )
{
    m_uFontID  = fontID;
    m_TextColor = textColor;
}




