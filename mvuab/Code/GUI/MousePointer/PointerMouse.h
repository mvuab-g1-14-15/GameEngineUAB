//----------------------------------------------------------------------------------
// CPointerMouse class
// Author: Enric Vergara
//
// Description:
// A pointermouse...
//----------------------------------------------------------------------------------
#pragma once
#ifndef INC_POINTER_MOUSE_H
#define INC_POINTER_MOUSE_H

#include "GuiElement\GuiElement.h"
#include "GraphicsManager.h"
#include "Math/Color.h"
#include <map>


//---Forward Declarations---
class CTexture;
//--------------------------

class CPointerMouse: public CGuiElement
{
    private:
        typedef std::map<std::string, CTexture*>    tTexturesMap;

    public:
        CPointerMouse( const CXMLTreeNode& aNode, const Math::Vect2i& screenResolution );

        virtual ~CPointerMouse()
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


        //---------------CPointerMouse Interface--------------------
        void                    SetTexture                            (CTexture* texture, std::string name );
        void                    SetActiveTexture                (const std::string& inName)
        {
            m_sActiveTexture = inName;
        }
        std::string&    GetActiveTexture                ()
        {
            return m_sActiveTexture;
        }
        void                    SetAlignment                        (CGraphicsManager::ETypeAlignment alignment)
        {
            m_Alignment = alignment;
        }
        bool                    IsQuadrant                            () const
        {
            return m_bIsQuadrant;
        }
        void                    SetQuadrant                            (bool flag)
        {
            m_bIsQuadrant = flag;
        }


    private:
        tTexturesMap        m_Textures;
        std::string            m_sActiveTexture;
        bool                        m_bAnimated;
        bool                        m_bLoop;
        float                        m_fTime;
        float                        m_fCounter;
        CGraphicsManager::ETypeAlignment    m_Alignment;
        bool                        m_bIsQuadrant;
};

#endif //INC_POINTER_MOUSE_H