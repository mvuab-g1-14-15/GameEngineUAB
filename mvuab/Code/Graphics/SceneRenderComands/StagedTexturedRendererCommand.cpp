#include "SceneRenderComands\StagedTexturedRendererCommand.h"
#include "Texture\Texture.h"
#include "GraphicsManager.h"
#include "Texture\TextureManager.h"

#ifdef _DEBUG
#include <sstream>
#endif

CStagedTexturedRendererCommand::CStagedTexturedRendererCommand(CXMLTreeNode& atts ): CSceneRendererCommand( atts )
{
  CGraphicsManager* gm = CGraphicsManager::GetSingletonPtr();

  if ( atts.Exists() )
  {
    int count = atts.GetNumChildren();

    for ( int i = 0; i < count; ++i )
    {
      const std::string& TagName = atts( i ).GetName();

      if ( TagName == "dynamic_texture" )
      {
        std::string l_Name = atts( i ).GetPszProperty( "name", "" );

        int l_StageId = atts( i ).GetIntProperty( "stage_id", -1 );
        bool l_WidthAsFB = atts( i ).GetBoolProperty( "texture_width_as_frame_buffer", true );

        std::string l_FormatType = atts( i ).GetPszProperty( "format_type", "" );
        CTexture::TFormatType l_iFormatType = ( l_FormatType == "R32F" ) ? ( CTexture::TFormatType )3 : ( CTexture::TFormatType )0;

        uint32 l_Width, l_Height;
        gm->GetWidthAndHeight( l_Width, l_Height );

        CTexture* l_Texture = new CTexture();
        l_Texture->Create( l_Name, l_Width, l_Height, 0, CTexture::RENDERTARGET, CTexture::DEFAULT, l_iFormatType );

		if(CTextureManager::GetSingletonPtr()->AddResource(l_Name, l_Texture))
        {
            AddStageTexture( l_StageId, l_Texture );
        }
        else
        {
            AddStageTexture(l_StageId, CTextureManager::GetSingletonPtr()->GetResource(l_Name));
            CHECKED_DELETE(l_Texture);
        }
      }

      if ( TagName == "texture" )
      {
        const std::string& l_Name = atts( i ).GetPszProperty( "name", "" );
        int l_StageId = atts( i ).GetIntProperty( "stage_id", -1 );
        CTexture* l_Texture = CTextureManager::GetSingletonPtr()->GetTexture( l_Name );

		if(!l_Texture)
		{
			CLogger::GetSingletonPtr()->AddNewLog(ELL_ERROR, "CStagedTexturedRendererCommand::Constructor: Error loading Texture \"%s\".", l_Name.c_str());
			continue;
		}

        AddStageTexture( l_StageId, l_Texture );
      }
    }
  }
}

CStagedTexturedRendererCommand::~CStagedTexturedRendererCommand()
{
    //for(std::vector<CKGStageTexture>::iterator it = m_StageTextures.begin(); it != m_StageTextures.end(); ++it)
        //CHECKED_DELETE(it);

    //m_StageTextures.clear();
}

void CStagedTexturedRendererCommand::ActivateTextures()
{
  for ( size_t i = 0; i < m_StageTextures.size(); ++i )
    m_StageTextures[i].m_Texture->Activate( m_StageTextures[i].m_StageId );
}

void CStagedTexturedRendererCommand::AddStageTexture( int StageId, CTexture* Texture )
{
  CKGStageTexture StageTexture = CKGStageTexture( StageId, Texture );
  m_StageTextures.push_back( StageTexture );
}

void CStagedTexturedRendererCommand::DebugTextures()
{
#ifdef _DEBUG
  for ( size_t i = 0; i < m_StageTextures.size(); ++i )
  {
    std::string l_TextureName = "CDrawQuadRendererCommand_" + GetName() + "_";
    std::stringstream l_CompletedTextureName;
    l_CompletedTextureName << l_TextureName << i;
    m_StageTextures[i].m_Texture->Save(l_CompletedTextureName.str());
  }
#endif
}