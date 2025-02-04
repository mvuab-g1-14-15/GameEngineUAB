//----------------------------------------------------------------------------------
// CPhysicController class
// Author: Enric Vergara
//
// Description:
// Esta clase representa un controlador de caracter.
//----------------------------------------------------------------------------------
#pragma once

#ifndef __INC_PHYSIC_CONTROLLER_H__
#define __INC_PHYSIC_CONTROLLER_H__

#include "Jump\Jump.h"
#include "Object3D.h"
#include "PhysicsDefs.h"
#include "NxSimpleTypes.h"
#include "Utils\Defines.h"

//---Forward Declarations---
class NxController;
class NxCapsuleControllerDesc;
class NxBoxControllerDesc;
class NxControllerDesc;
class NxScene;
class CPhysicUserData;
class CPhysicsControllerHitReport;

//--------------------------

class CPhysicController : public CObject3D
{
public:
  //--- Init and End protocols------------------------------------------
  CPhysicController( float _fRadius, float _fHeight, float _fSlope, float _fSkinwidth, float _fStepOffset
                     , ECollisionGroup _uiCollisionGroups, CPhysicUserData* _pUserData
                     , const Math::Vect3f& _vPos = Math::Vect3f( 0.f, 0.f, 0.f ), float _fGravity = -9.8f );

  CPhysicController( Math::Vect3f _Dim, float _fSlope, float _fSkinwidth, float _fStepOffset, ECollisionGroup _uiCollisionGroups,
                     CPhysicUserData* _pUserData,
                     const Math::Vect3f& _vPos = Math::Vect3f( 0.f, 0.f, 0.f ), float _fGravity = -9.8f );

  ~CPhysicController( void );

  //---- Main Functions ------------------------------------------------
  void            Move( const Math::Vect3f& _vDirection, float _fElapsedTime );
  void Jump( float _fAmmount, float height );
  void Jump( float );
  void            CreateController( NxController* _pController, NxScene* _PScene );

  //---- Functions -----------------------------------------------------
  bool            UpdateCharacterExtents( bool _bBent, float _fAmmount );

  //---- Properties ( get & Set )---------------------------------------
  CPhysicUserData*      GetUserData( void )
  {
    return m_pUserData;
  }

  void            SetCollision( bool _bFlag );

  Math::Vect3f            GetPosition( void );
  void            SetPosition( const Math::Vect3f& _vPos );

  float           GetGravity( void ) const
  {
    return m_fGravity;
  }
  void            SetGravity( float _fGravity )
  {
    m_fGravity = _fGravity;
  }

  void            SetGroup( int _iGroup );

  void            SetHeight( float _fHeight );
  float           GetHeight( void ) const
  {
    return m_fHeightControler;
  }

  void            SetActive( bool _bActive );

  float           GetRadius( void ) const
  {
    return m_fRadiusControler;
  }
  void            SetRadius( float _fRadius );

  ECollisionGroup       GetColisionGroup( void ) const
  {
    return m_uCollisionGroups;
  }

  //---PhsX Info---
  NxController*       GetPhXController( void )
  {
    return m_pPhXController;
  }
  NxControllerDesc*     GetPhXControllerDesc( void );

  void            UseGravity( bool _bUseGravity )
  {
    m_bUseGravity = _bUseGravity;
  }

  EControleType       GetType( void ) const
  {
    return m_Type;
  }
  void            SetType( EControleType _Type )
  {
    m_Type = _Type;
  }

  bool IsJumping()
  {
    return m_bJumping;
  }

  GET_SET( bool, bCrouch );
  //---- Members ---------------------------------------

private:
  CPhysicUserData*        m_pUserData;
  //CPhysicsControllerHitReport   m_Report;

  void*             m_pReport;

  ECollisionGroup         m_uCollisionGroups;
  CJump             m_Jump;
  float             m_fGravity;
  float             m_fRadiusControler;
  float             m_fHeightControler;
  float             m_fSlopeLimitControler;
  float             m_fSkinWidthControler;
  float             m_fStepOffsetControler;

  float             m_fVelocity;

  bool              m_bJumping;

  Math::Vect3f              m_vExtensio;

  EControleType         m_Type;

  bool              m_bUseGravity;
  bool              m_bRotado;
  bool              m_bCrouch;
  //--- PhysX--
  NxCapsuleControllerDesc*    m_pPhXCapsuleControllerDesc;
  NxBoxControllerDesc*      m_pPhXBoxControllerDesc;
  NxController*         m_pPhXController;
  NxScene*            m_pPhXScene;

};

#endif //__INC_PHYSIC_CONTROLLER_H__