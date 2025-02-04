#ifndef __CONSOLE_H__
#define __CONSOLE_H__

//Some defines we will be requiring
#define SC_LINES    0x0001
#define SC_COLUMNS  0x0002
#define SC_ATTRIB   0x0004
#define SC_MAXLINES 0x0008

#include "Utils/Manager.h"
#include "XML/XMLTreeNode.h"

class CConsole : public CManager
{
    public:
        //Constructor & Destructor
        CConsole( const CXMLTreeNode& atts );
        ~CConsole ();

        virtual void Init();
        virtual void Update();
        virtual void Render();

        //Properties
        short GetNumberOfLines();
        short SetNumberOfLines (short sLines);
        short SetNumberOfColumns (short sColumns);
        short GetNumberOfColumns ();
        WORD GetAttributes ();
        WORD SetAttributes (WORD wAttrib, short NumChars = 0);
        short SetMaxLinesInWindow (short maxLines);
        short GetMaxLinesInWindow ();
        void  SetSize(int w, int h);
        void  MoveConsole( int x, int y );
        void SetFullSize();

        //Methods
        void RedirectToConsole (WORD wFlags);
        BOOL SetupConsole(WORD wFlags);
        HANDLE GetHandle (DWORD dwFlag);
        BOOL Clear ();
        BOOL ApplyAttrib (short NumChars);
        WORD GetSettings (WORD wFlags);


        //Helpers
        BOOL CreateConsole ();
        BOOL DestroyConsole ();

    protected:
        BOOL m_bRedirected;
        short m_sNumColumns;
        short m_sNumLines;
        WORD m_wAttrib;
        short m_sMaxLines;
        DWORD m_dwError;
        static BOOL sm_bConsole;
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        CXMLTreeNode mConsoleNode;
        void Recalculate();
};
#endif //__CONSOLE_H__
