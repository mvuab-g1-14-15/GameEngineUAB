#include "SceneRenderComands\StagedTexturedRendererCommand.h"
#include "Texture\Texture.h"
#include "GraphicsManager.h"

#include "Texture\TextureManager.h"
#include "EngineManagers.h"

#ifdef _DEBUG
#include <sstream>
#endif

CStagedTexturedRendererCommand::CStagedTexturedRendererCommand( CXMLTreeNode& atts ):
  CSceneRendererCommand( atts )
{
  CGraphicsManager* gm = GraphicsInstance;

  if ( atts.Exists() )
  {
    int count = atts.GetNumChildren();

    for ( int i = 0; i < count; ++i )
    {
      const CXMLTreeNode& lCurrentNode = atts( i );
      const std::string& TagName = lCurrentNode.GetName();

      if ( TagName == "dynamic_texture" )
      {
        std::string l_Name = lCurrentNode.GetAttribute<std::string>( "name", "" );
        int l_StageId = lCurrentNode.GetAttribute<int32>( "stage_id", -1 );
        bool l_WidthAsFB = lCurrentNode.GetAttribute<bool>( "texture_width_as_frame_buffer", false );
        bool l_CreateDepthStencilBuffer = atts( i ).GetAttribute<bool>( "create_depth_stencil_buffer", true );
        uint32 l_Width, l_Height;

        if ( !l_WidthAsFB )
        {
          l_Width = lCurrentNode.GetAttribute<int32>( "width", 0 );
          l_Height = lCurrentNode.GetAttribute<int32>( "height", 0 );
        }
        else
          gm->GetWidthAndHeight( l_Width, l_Height );

        std::string l_FormatType = lCurrentNode.GetAttribute<std::string>( "format_type", "" );
        CTexture::TFormatType l_iFormatType = ( l_FormatType == "R32F" ) ? ( CTexture::TFormatType )3 :
                                              ( CTexture::TFormatType )0;
        CTexture* l_Texture = new CTexture();
        l_Texture->Create( l_Name, l_Width, l_Height, 0, CTexture::eUsageRenderTarget, CTexture::eDefaultPool,
                           l_iFormatType, l_CreateDepthStencilBuffer );

        if ( TextureMInstance->AddResource( l_Name, l_Texture ) )
          AddStageTexture( l_StageId, l_Texture, true );
        else
        {
          AddStageTexture( l_StageId, TextureMInstance->GetResource( l_Name ), true );
          CHECKED_DELETE( l_Texture );
        }
      }

      if ( TagName == "texture" )
      {
        std::string l_Name = lCurrentNode.GetAttribute<std::string>( "name", "" );
        int l_StageId = lCurrentNode.GetAttribute<int32>( "stage_id", -1 );
        CTexture* l_Texture = TextureMInstance->GetTexture( l_Name );

        if ( !l_Texture )
        {
          LOG_ERROR_APPLICATION( "Error loading Texture \"%s\".", l_Name.c_str() );
          continue;
        }

        AddStageTexture( l_StageId, l_Texture, false );
      }
    }
  }
}

CStagedTexturedRendererCommand::~CStagedTexturedRendererCommand()
{
}

void CStagedTexturedRendererCommand::ActivateTextures()
{
  for ( size_t i = 0; i < m_StageTextures.size(); ++i )
    m_StageTextures[i].m_Texture->Activate( m_StageTextures[i].m_StageId );

  DebugTextures();
}

void CStagedTexturedRendererCommand::AddStageTexture( int StageId, CTexture* Texture, bool aIsDynamic )
{
  CKGStageTexture StageTexture = CKGStageTexture( StageId, Texture, aIsDynamic );
  m_StageTextures.push_back( StageTexture );
}

void CStagedTexturedRendererCommand::DebugTextures()
{
#ifdef _DEBUG
  if ( false )
  {
    for ( uint32 i = 0, lCount = m_StageTextures.size(); i < lCount; ++i )
    {
        CTexture* lTexture = m_StageTextures[i].m_Texture;
        if( lTexture )
        {
            std::string l_TextureName = GetName() + "_" + lTexture->GetName();
            std::stringstream l_CompletedTextureName;
            l_CompletedTextureName << l_TextureName << i;
            lTexture->Save( l_CompletedTextureName.str() );
        }
    }
  }
#endif
}