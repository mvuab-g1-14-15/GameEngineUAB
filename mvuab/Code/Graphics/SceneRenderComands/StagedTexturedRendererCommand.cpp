#include "SceneRenderComands\StagedTexturedRendererCommand.h"
#include "Texture\Texture.h"
#include "GraphicsManager.h"

CStagedTexturedRendererCommand::CStagedTexturedRendererCommand(CXMLTreeNode &atts):CSceneRendererCommand(atts)
{
	CGraphicsManager* gm = CGraphicsManager::GetSingletonPtr();

    if(atts.Exists())
    {
        int count = atts.GetNumChildren();
        for( int i = 0; i < count; ++i )
        {
            std::string TagName = atts(i).GetName();
            if( TagName == "dynamic_texture" )
            {
				std::string l_Name = atts(i).GetPszProperty("name", "");
				int l_StageId = atts(i).GetIntProperty("stage_id", -1);
				bool l_WidthAsFB = atts(i).GetBoolProperty("texture_width_as_frame_buffer", true);
				std::string l_FormatType = atts(i).GetPszProperty("format_type", "");

				CTexture::TFormatType l_iFormatType = (l_FormatType == "R32F") ? (CTexture::TFormatType)3 : (CTexture::TFormatType)0;

				uint32 l_Width, l_Height;
				gm->GetWidthAndHeight(l_Width, l_Height);
				CTexture* l_Texture = new CTexture();
				l_Texture->Create(l_Name, l_Width, l_Height, 0, CTexture::RENDERTARGET, CTexture::DEFAULT, l_iFormatType);

				AddStageTexture(l_StageId, l_Texture);
            }
		}
	}
}

CStagedTexturedRendererCommand::~CStagedTexturedRendererCommand()
{
    
}

void CStagedTexturedRendererCommand::ActivateTextures()
{

}

void CStagedTexturedRendererCommand::AddStageTexture(int StageId, CTexture *Texture)
{
    CKGStageTexture StageTexture = CKGStageTexture(StageId, Texture);
    m_StageTextures.push_back(StageTexture);
}