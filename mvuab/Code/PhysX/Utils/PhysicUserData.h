#pragma once

#ifndef __PHYSIC_USER_DATA_CLASS_H__
#define __PHYSIC_USER_DATA_CLASS_H__

#include <string>

#include "Utils\Name.h"
#include "Math\Color.h"
////#include "Utils\Base.h"
#include "Actor\PhysicController.h"
#include "PhysicsDefs.h"

using namespace Math;

// ----- FOWARD Declarations -------//
class CObject3D;
class CPhysicActor;
class Math::CColor;
//class CSteeringEntity;
// ---------------------------------//

enum EUserDataFlag
{
  UD_IS_HW_ACTOR          = ( 1 << 0 ),
  UD_IS_HW_CONTROLLER       = ( 1 << 1 ),
  UD_IS_TRIGGER             = ( 1 << 2 ),
  UD_IS_INSIDE_TRIGGER            = ( 1 << 3 ),
  UD_PASSES_INTERSECTION_TEST     = ( 1 << 4 ),
  UD_HIT_BY_RAYCAST               = ( 1 << 5 ),
  UD_NO_RENDER                    = ( 1 << 6 ),
  UD_IS_DRAIN                     = ( 1 << 7 ),
  UD_IS_ASLEEP            = ( 1 << 8 ),
  UD_RENDER_USING_LIGHT1      = ( 1 << 9 ),
};

class CPhysicUserData : public CName
{
public:
  CPhysicUserData( const std::string& _szName, EUserDataFlag _eflags = UD_IS_HW_ACTOR,
                   ECollisionGroup _uiGrup = ::ECG_ESCENE )  // Afegeixo el flag per distingir-lo
    : CName( _szName )
    , m_bPaintPhysicObject( true )
    , m_ColorPhysicObject( colWHITE )
    , m_pEntity( 0 )
    , m_pActor( 0 )
    , mController( 0 )
    , m_eFlags( _eflags )
    , m_uiCollisionGroup( _uiGrup )
    , m_myCollisionGroup( _uiGrup )
  {
  }

  ~CPhysicUserData()
  {
    m_pActor = NULL;
    m_pEntity = NULL;
    mController = NULL;
  }

  void            SetPaint( bool _bFlag )
  {
    m_bPaintPhysicObject = _bFlag;
  };
  void            SetColor( const Math::CColor& _Color )
  {
    m_ColorPhysicObject = _Color;
  };
  void            SetColor( const float _ColorRed, const float _ColorGreen, const float _ColorBlue,
                            const float _ColorAlpha )
  {
    m_ColorPhysicObject = Math::CColor( _ColorRed, _ColorGreen, _ColorBlue, _ColorAlpha );
  };
  void            SetEntity( CObject3D* _pEntity )
  {
    m_pEntity = _pEntity;
  };
  void            SetActor( CPhysicActor* _pActor )
  {
    m_pActor = _pActor;
  };
  void            SetController( CPhysicController* _pController )
  {
    mController = _pController;
  };
  void            SetRadius( float _Radius )
  {
    m_fRadius = _Radius;
  };
  void            SetGroup( ECollisionGroup _uiGroup )
  {
    m_uiCollisionGroup = _uiGroup;
    SetMyCollisionGroup( _uiGroup );
  };
  void            SetMyCollisionGroup( ECollisionGroup _uiGroup )
  {
    m_myCollisionGroup = _uiGroup;
  };

  inline bool         GetPaint() const
  {
    return m_bPaintPhysicObject;
  };
  inline const Math::CColor&    GetColor() const
  {
    return m_ColorPhysicObject;
  };
  inline CObject3D*       GetEntity() const
  {
    return m_pEntity;
  };
  inline CPhysicActor*    GetActor() const
  {
    return m_pActor;
  };
  inline CPhysicController* GetController() const
  {
    return mController;
  };
  inline float        GetRadius() const
  {
    return m_fRadius;
  };
  inline ECollisionGroup    GetGroup() const
  {
    return m_uiCollisionGroup;
  };
  inline ECollisionGroup    GetMyCollisionGroup() const
  {
    return m_myCollisionGroup;
  };

  // Per permetre saber info del objecte
  void            setFlags( EUserDataFlag _eFlags )
  {
    m_eFlags = _eFlags;
  }
  EUserDataFlag       GetFlags() const
  {
    return m_eFlags;
  }


private:
  float       m_fRadius;
  bool        m_bPaintPhysicObject;
  Math::CColor        m_ColorPhysicObject;
  CPhysicActor*   m_pActor;
  CPhysicController*  mController;
  CObject3D*      m_pEntity;
  EUserDataFlag   m_eFlags;
  ECollisionGroup   m_uiCollisionGroup;
  ECollisionGroup   m_myCollisionGroup;
};

#endif __PHYSIC_USER_DATA_CLASS_H__