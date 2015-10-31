#pragma once
#ifndef INC_VISIBLE_H_
#define INC_VISIBLE_H_

class CVisible
{
public:
	CVisible() : m_Visible( false )
	{
	}

  CVisible( bool aVisible ) : m_Visible( aVisible )
  {
  }

    ~CVisible()
	{
	}
	
	bool GetVisible(){return m_Visible;}
  bool IsVisible(){return m_Visible;}
	void SetVisible( bool visible ){ m_Visible = visible; }

protected:
    bool m_Visible;
};

#endif 