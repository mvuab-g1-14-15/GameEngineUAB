#include "GuiElement\GuiElement.h"

#include "ScriptManager.h"
#include "Language/LanguageManager.h"
#include "GraphicsManager.h"
#include "Fonts/FontManager.h"
#include "EngineManagers.h"

CGuiElement::CGuiElement( const CXMLTreeNode& aNode, const Math::Vect2i& screenResolution )
  : CName( aNode.GetAttribute<std::string>( "name", "DefaultGuiElement" ) )
  , CVisible( aNode.GetAttribute<bool>( "visible", true ) )
  , m_uWindowsHeight( screenResolution.y )
  , m_uWindowsWidth( screenResolution.x )
  , m_fWidthPercent( aNode.GetAttribute<float>( "width", 50.f ) )
  , m_fHeightPercent( aNode.GetAttribute<float>( "height", 50.f ) )
  , m_PositionPercent( Math::Vect2f( aNode.GetAttribute<Math::Vect2f>( "pos", Math::Vect2f( 0.f, 0.f ) ) ) )
  , m_bIsActive( aNode.GetAttribute<bool>( "active", true ) )
  , m_bIsOver( false )
  , m_bIsInside( false )
  , m_bFocus( false )
  , m_sLuaCode_OnLoadValue( aNode.GetAttribute<std::string>( "OnLoadValue", "" ) )
  , m_sLuaCode_OnSaveValue( aNode.GetAttribute<std::string>( "OnSaveValue", "" ) )
  , m_pParent( NULL )
  , m_eEditMode( DISABLE )
  , m_eType( GUI )
  , m_z( 0 )
  , m_sLiteral( aNode.GetAttribute<std::string>( "Literal", "" ) )
  , m_uTextHeightOffset( ( uint32 )( screenResolution.y * 0.01f * aNode.GetAttribute<uint32>( "heightOffset", 0 ) ) )
  , m_uTextWidthOffset( ( uint32 )( screenResolution.x * 0.01f * aNode.GetAttribute<uint32>( "widthOffset", 0 ) ) )
{
  m_uWidth      = ( uint32 )( m_fWidthPercent * 0.01f * m_uWindowsWidth );
  m_uHeight     = ( uint32 )( m_fHeightPercent * 0.01f * m_uWindowsHeight );
  m_Position.x  = ( uint32 )( m_uWindowsWidth * 0.01f * m_PositionPercent.x );
  m_Position.y  = ( uint32 )( m_uWindowsHeight * 0.01f * m_PositionPercent.y );
}


void CGuiElement::SetWindowsWidth( uint32  w )
{
  m_uWindowsWidth = w;
  m_uWidth  = ( uint32 )( m_fWidthPercent * 0.01f * m_uWindowsWidth );
  m_Position.x  = ( uint32 )( m_uWindowsWidth * 0.01f * m_PositionPercent.x );
}

void CGuiElement::SetWindowsHeight( uint32 h )
{
  m_uWindowsHeight = h;
  m_uHeight  = ( uint32 )( m_fHeightPercent * 0.01f * m_uWindowsHeight );
  m_Position.y  = ( uint32 )( m_uWindowsHeight * 0.01f * m_PositionPercent.y );
}

void CGuiElement::SetHeightPercent( float height_percent )
{
  m_fHeightPercent = height_percent;
  m_uHeight  = ( uint32 )( m_fHeightPercent * 0.01f * m_uWindowsHeight );
}

void CGuiElement::SetWidthPercent( float width_percent )
{
  m_fWidthPercent = width_percent;
  m_uWidth  = ( uint32 )( m_fWidthPercent * 0.01f * m_uWindowsWidth );
}

void CGuiElement::SetPositionPercent( const Math::Vect2f& pos )
{
  m_PositionPercent = pos;
  m_Position.x  = ( uint32 )( m_uWindowsWidth * 0.01f * m_PositionPercent.x );
  m_Position.y  = ( uint32 )( m_uWindowsHeight * 0.01f * m_PositionPercent.y );
}
void  CGuiElement::SetPosition( const Math::Vect2i& pos )
{
  m_Position = pos;
  m_PositionPercent.x = ( float )( ( float )pos.x / ( float )m_uWindowsWidth ) * 100;
  m_PositionPercent.y = ( float )( ( float )pos.y / ( float ) m_uWindowsHeight ) * 100;
}

void CGuiElement::CalculatePosMouse( const Math::Vect2i& mousePosition )
{
  if ( m_bIsActive && IsVisible() )
  {
    // (0,0) ________
    //  |  |
    //  |GuiElement|
    //  |  |
    // ----------(m_fWidth, m_fHeight)

    if ( ( mousePosition.x > m_Position.x ) && ( mousePosition.x < m_Position.x + ( int32 )m_uWidth ) &&
         ( mousePosition.y > m_Position.y ) && ( mousePosition.y < m_Position.y + ( int32 )m_uHeight ) )
    {
      //Esta dentro de la zona que representa el GuiElement

      //Antes de actulizar mIsInside a true, observamos si anteriormente el puntero del
      //mouse estaba fuera de la zona (pas� de fuera a dentro-> mIsOver=true).
      if ( m_bIsInside == true )
      {
        //Si el puntero de mouse estaba ya dentro ==> no ha pasado de fuera a dentro
        m_bIsOver = false;
      }
      else
      {
        //Si el puntero de mouse estaba fuera ==> ha pasado de fuera a dentro
        m_bIsOver = true;
      }

      //Finalmente actualizamos el valor de mIsInside a true
      m_bIsInside = true;
    }
    else
    {
      m_bIsInside = false;
      m_bIsOver = false;
    }
  }
}

bool CGuiElement::AddChild( CGuiElement* child )
{
  assert( child );
  //Primero comprobamos que no lo hayan registrado ya como hijo
  std::vector<CGuiElement*>::iterator it = m_Children.begin();
  std::vector<CGuiElement*>::iterator itEnd = m_Children.end();

  while ( it != itEnd )
  {
    CGuiElement* guiElement = *it;

    if ( guiElement == child )
      return false;

    it++;
  }

  m_Children.push_back( child );
  child->SetParent( this );
  return true;
}

bool CGuiElement::RemoveChild( CGuiElement* child )
{
  assert( child );
  bool ok = false;
  std::vector<CGuiElement*>::iterator it = m_Children.begin();
  std::vector<CGuiElement*>::iterator itEnd = m_Children.end();

  while ( it != itEnd )
  {
    CGuiElement* guiElement = *it;

    if ( guiElement == child )
    {
      m_Children.erase( it );
      ok = true;
      break;
    }

    it++;
  }

  return ok;
}

void CGuiElement::OnLoadValue( void )
{
  if ( m_sLuaCode_OnLoadValue.compare( "" ) != 0 )
  {
    //Ejecutar el c�digo Lua:
    //TODO RAUL
    //CScriptManager * scriptManager = CORE->GetScriptManager();
    ScriptMInstance->RunCode( m_sLuaCode_OnLoadValue );
  }
}

void CGuiElement::OnSaveValue( void )
{
  if ( m_sLuaCode_OnSaveValue.compare( "" ) != 0 )
  {
    //Ejecutar el c�digo Lua:
    //TODO RAUL
    //CScriptManager * scriptManager = CORE->GetScriptManager();
    ScriptMInstance->RunCode( m_sLuaCode_OnSaveValue );
  }
}


void CGuiElement::Render()
{
  if ( IsVisible() )
  {
    //Renderizamos todos los hijos que cuelgan de este CGuiElement:
    std::vector<CGuiElement*>::iterator it = m_Children.begin();
    std::vector<CGuiElement*>::iterator itEnd = m_Children.end();

    while ( it != itEnd )
    {
      CGuiElement* guiElement = *it;
      guiElement->Render();
      it++;
    }
  }//if (m_sLiteral != "")
}

void CGuiElement::RenderText()
{
  if ( IsVisible() )
  {
    //Render text:
    if ( m_sLiteral != "" )
    {
      uint32 posX = CGuiElement::m_Position.x + m_uTextWidthOffset;
      uint32 posY = CGuiElement::m_Position.y + m_uTextHeightOffset;

      FontInstance->DrawLiteral( posX, posY, m_sLiteral );
    }//if (m_sLiteral != "")

  }//if (m_bIsVisible)
}

void CGuiElement::Update()
{
  if ( IsVisible() && m_bIsActive )
  {
    //Renderizamos todos los hijos que cuelgan de este CGuiElement:
    std::vector<CGuiElement*>::iterator it = m_Children.begin();
    std::vector<CGuiElement*>::iterator itEnd = m_Children.end();

    while ( it != itEnd )
    {
      CGuiElement* guiElement = *it;
      guiElement->Update();
      it++;
    }
  }
}