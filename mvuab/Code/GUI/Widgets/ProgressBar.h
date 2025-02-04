//----------------------------------------------------------------------------------
// CProgressBar class
// Author: Enric Vergara
//
// Description:
// A progress bar is a component in a graphical user interface used to convey the progress of a
// task, such as a download or file transfer.
//----------------------------------------------------------------------------------
#pragma once
#ifndef INC_PROGRESS_BAR_H
#define INC_PROGRESS_BAR_H

#include "GuiElement\GuiElement.h"
#include "Math/Color.h"


//---Forward Declarations---
class CTexture;
//--------------------------

class CProgressBar: public CGuiElement
{
    public:
        CProgressBar( const CXMLTreeNode& aNode, const Math::Vect2i& screenResolution );

        virtual ~CProgressBar()
        {
            /*NOTHING*/;
        }

        //---------------CGuiElement Interface----------------------
        virtual void    Render                                    ();
        virtual void    Update                                    ();
        virtual void    OnClickedChild                    (const std::string& name)
        {
            /*NOTHING*/;
        }


        //---------------CProgressBar Interface----------------------
        void                    SetTextures                            (CTexture* background, CTexture* progress);
        void                    SetColors                                (const Math::CColor& background,
                const Math::CColor& progress);
        void                    SetFont                                    (uint32 fontID = 0,
                const Math::CColor& textColor = Math::colBLACK);
        void                    SetProgress                            (float progress);
        virtual float                    GetProgress                            () const
        {
            return m_fProgress;
        }
        void                    OnComplete                            ();
        void                    SetOnComplete                        (const std::string& inAction)
        {
            m_sLuaCode_OnComplete = inAction;
        }

    private:
        CTexture*            m_pBackgroundTexture;
        CTexture*            m_pProgressTexture;
        Math::CColor                m_BackGroundColor;
        Math::CColor                m_ProgressColor;
        float                    m_fProgress;
        uint32                m_uFontID;
        Math::CColor                m_TextColor;
        std::string        m_sText;
        float                    m_fCountTime;
        float                    m_fTimeToUpdate;
        std::string        m_sLuaCode_OnComplete;
};

#endif //INC_PROGRESS_BAR_H