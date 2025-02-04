//----------------------------------------------------------------------------------
// CGuiElement class
// Author: Enric Vergara
//
// Description:
// Clase Base con los datos y funcionalidad com�n a todos los elementos de la GUI.
//----------------------------------------------------------------------------------
#pragma once
#ifndef INC_GUI_ELEMENT_H
#define INC_GUI_ELEMENT_H

#include <string>
#include <vector>
#include <assert.h>
#include "Utils/Types.h"
#include "Math\Vector2.h"
#include "Utils/Defines.h"
#include "XML\XMLTreeNode.h"
#include "Utils/Name.h"
#include "Utils/Visible.h"

class CGuiElement : public CName, public CVisible
{
    public:
        typedef enum
        {
            GUI = 0,
            BUTTON = 1,
            CHECKBUTTON,
            STATIC_TEXT,
            SLIDER,
            IMAGE,
            EDITABLE_TEXT_BOX,
            PROGRESS_BAR,
            DIALOG_BOX,
            RADIO_BOX
        } TypeGuiElement;

        typedef enum { EDITPOS, EDITSIZE, DISABLE } TypeEditMode;

    public:
        CGuiElement( const CXMLTreeNode& aNode, const Math::Vect2i& screenResolution );

        virtual ~CGuiElement()
        {
            /*Nothing*/;
        }

        virtual void                Render                                ();
        virtual void                RenderText                        ();
        virtual void                Update                                ();
        virtual void                OnClickedChild                (const std::string& name) = 0;

        virtual void                SetProgress(float) {}
        virtual float                GetProgress() const { return 0; }

        void                                OnLoadValue                        ();
        void                                OnSaveValue                        ();

        void                                SetOnLoadValueAction    (const std::string& inAction)
        {
            m_sLuaCode_OnLoadValue = inAction;
        }
        void                                SetOnSaveValueAction    (const std::string& inAction)
        {
            m_sLuaCode_OnSaveValue = inAction;
        }


        void                                CalculatePosMouse            (const Math::Vect2i& mousePosition);

        bool                                IsOver                                ()
        const
        {
            return m_bIsOver;
        }
        bool                                IsInside                            ()
        const
        {
            return m_bIsInside;
        }
        bool                                IsActive                            ()
        const
        {
            return m_bIsActive;
        }
        void SetActive( bool flag )
        {
            m_bIsActive = flag;
        }

        uint32                            GetWidth                            ()
        const
        {
            return m_uWidth;
        }
        float                                GetWidthPercent                ()
        const
        {
            return m_fWidthPercent;
        }
        uint32                            GetHeight                            ()
        const
        {
            return m_uHeight;
        }
        float                                GetHeightPercent            () const
        {
            return m_fHeightPercent;
        }

        Math::Vect2i                            GetPosition                        ()
        const
        {
            return m_Position;
        }
        const Math::Vect2f&                GetPositionPercent        () const
        {
            return m_PositionPercent;
        }

        void                                SetWindowsWidth                (uint32    w);
        void                                SetWindowsHeight            (uint32    h);
        void                                SetHeightPercent            (float height_percent);
        void                                SetWidthPercent                (float width_percent);
        void                                SetPositionPercent        (const Math::Vect2f& pos);
        virtual void                        SetPosition                        (const Math::Vect2i& pos);
        void                                SetLiteral                        (const std::string& lit)
        {
            m_sLiteral = lit;
        }
        void                                SetTextHeightOffset        (uint32 textHeightOffset)
        {
            m_uTextHeightOffset = textHeightOffset;
        }
        void                                SetTextWidthOffset        (uint32 textWidthOffset)
        {
            m_uTextWidthOffset = textWidthOffset;
        }
        const std::string&    GetLiteral                        ()
        {
            return m_sLiteral;
        }
        uint32                            GetTextHeightOffset        ()
        {
            return m_uTextHeightOffset;
        }
        uint32                            GetTextWidthOffset        ()
        {
            return m_uTextWidthOffset;
        }


        void                                SetParent                            (CGuiElement* parent)
        {
            assert(parent);
            m_pParent = parent;
        }
        bool                                AddChild                            (CGuiElement* child);
        bool                                RemoveChild                        (CGuiElement* child);


        void
        GainFocus                            ()
        {
            m_bFocus = true;
        }
        void
        LoseFocus                            ()
        {
            m_bFocus = false;
        }
        bool                                HasFocus                            ()
        const
        {
            return m_bFocus;
        }
        TypeGuiElement            GetType                                () const
        {
            return m_eType;
        }

        GET_SET(TypeGuiElement, eType)
        GET_SET(uint32, uWindowsWidth)
        GET_SET(uint32, uWindowsHeight)
    protected:

        float         m_fWidthPercent;                    //% del Ancho del GuiElement respecto el size del Window
        uint32        m_uWidth;                           //Ancho en pixeles del GuiElement
        float         m_fHeightPercent;                   //% de Altura del GuiElement respecto el size del Window
        uint32        m_uHeight;                          //Altura en pixeles del GuiElement
        uint32        m_uWindowsWidth;                    //Ancho en pixeles de la windows que contiene a este GuiElement
        uint32        m_uWindowsHeight;                   //Altura en pixeles de la windows que contiene a este GuiElement
        Math::Vect2f  m_PositionPercent;                  //Posici�n del GuiElement (x,y) en % respecto el size del Window
        Math::Vect2i  m_Position;                         //Posici�n del GuiElement (x,y) en pixeles
        bool          m_bIsActive;                        //Indica si el elemento est� o no activado
        bool          m_bIsOver;                          //Indica si el puntero del mouse es la primera vez que ha entrado
        bool          m_bIsInside;                        //Indica si el puntero del mouse est� dentro
        bool          m_bFocus;                                    //Indica que es el guielement que tiene el foco dentro de la windows actual
        std::string
        m_sLuaCode_OnLoadValue;        //Codigo LUA que ha de ejecutar cuando se entre por primera vez en la ventana que contiene al GuiElement
        std::string
        m_sLuaCode_OnSaveValue;        //Codigo LUA que ha de ejecutar cuando se salga de la ventana que contiene al GuiElement
        std::vector<CGuiElement*>    m_Children;                                //GuiElements que estan atachados a este.
        CGuiElement*
        m_pParent;                                //GuiElements al que esta atachado este.
        TypeEditMode
        m_eEditMode;                            //Indica si esta en estado de edicion o navegacion.
        TypeGuiElement
        m_eType;                                    //Indica el tipo de GuiElement que sera (button, checkbutton...)
        uint32
        m_z;                                            //Indice de profundidad en la que se pintara el elemento
        std::string                                m_sLiteral;
        uint32                                        m_uTextHeightOffset;
        uint32                                        m_uTextWidthOffset;
};

#endif //INC_GUI_ELEMENT_H
