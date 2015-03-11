#ifndef _PLAYER_PHYSIC_PROCESS_H
#define _PLAYER_PHYSIC_PROCESS_H
#pragma once

#include "Process.h"
#include "PhysicsDefs.h"
#include "Utils\Types.h"
#include "Math\Vector3.h"

#include <vector>

class CPhysicActor;
class CPhysicUserData;
class CPhysicController;
class CCharacter;
class CGrenade;
class CBlaster;
class CAStar;

class CPlayerPhysicProcess : public CProcess
{
private:
  CAStar*           m_pAStarScene;
  Math::Vect3f          m_PointInicial;
  Math::Vect3f          m_PointFinal;
  std::vector<Math::Vect3f>   m_Path;
  std::vector<CPhysicActor*>    m_vPA;
  std::vector<CPhysicUserData*> m_vPUD;
  std::vector<CCharacter*>      m_vCharacter;
  CPhysicController*            m_PhysicController;
  CGrenade*                     m_Grenade;
  CBlaster*                     m_Blaster;
  //CCharacter*               m_Character;

public:
  CPlayerPhysicProcess();
  virtual ~CPlayerPhysicProcess();

  virtual void Init();
  virtual void Update();
  virtual void Render();
  virtual void RenderDebugInfo();

  GET_SET_PTR( CAStar, AStarScene );
  GET_SET( Math::Vect3f, PointInicial );
  GET_SET( Math::Vect3f, PointFinal );
  CPhysicUserData*    GetNewPUD( const std::string& Name );
  CPhysicActor*       GetNewPhysicActor( CPhysicUserData* PUD );
  CPhysicController*  GetNewController( float _fRadius, float _fHeight, float _fSlope, float _fSkinwidth, float _fStepOffset,
                                        ECollisionGroup _uiCollisionGroups, CPhysicUserData* _pUserData, const Math::Vect3f& _vPos, float _fGravity );
  CCharacter* GetNewCharacter( const std::string& Name );
  void                AddPudVector( CPhysicUserData* PUD );
  void                AddPhysicActorVector( CPhysicActor* PA );
  CPhysicUserData*    GetLastPUDInserted();

  void InitSceneCharacterController();


};

#endif // _PLAYER_PHYSIC_PROCESS_H
