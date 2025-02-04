#include "CinematicObject.h"
#include "XML/XMLTreeNode.h"
#include "CinematicObjectKeyFrame.h"

#include "RenderableObject/RenderableObjectsManager.h"
#include "RenderableObject/RenderableObjectsLayersManager.h"
#include "GraphicsManager.h"
#include "Math\LerpAnimator3D.h"
#include "Math\LerpAnimator1D.h"
#include "Math\MathTypes.h"
#include "Math\Vector3.h"
#include "Utils\BaseUtils.h"
#include "EngineManagers.h"
#include "StaticMeshes\InstanceMesh.h"

#include "RenderableObject/Room.h"
#include "RenderableObject/Scene.h"
#include "PhysicsManager.h"
#include "Actor\PhysicActor.h"
#include "Cameras\CameraManager.h"
#include "Cameras\CameraCinematical.h"
#include "ScriptManager.h"

CCinematicObject::CCinematicObject( CXMLTreeNode& atts )
  : m_Actor(0)
{
  CRoom* lRoom = SceneInstance->GetResource( atts.GetAttribute<std::string>("room", "" ) );
  ASSERT( lRoom, "The room %s doesn't exist", atts.GetAttribute<std::string>("room", "" ).c_str() );
  if( lRoom )
  {
      CRenderableObjectsManager* lLayer = lRoom->GetLayer( "solid" );
      ASSERT( lLayer, "The layer %s doesn't exist in the room %s", "solid", atts.GetAttribute<std::string>("room", "" ).c_str() );
      if( lLayer )
      {
          const std::string& resource = atts.GetAttribute<std::string>( "resource", "" );
          m_RenderableObject = lLayer->GetResource( resource );
		  ASSERT(m_RenderableObject, "Error al cargar el renderable %s", resource.c_str());
		  m_CurrentTime = atts.GetAttribute<float>("init_keyframe", 0.0f);
		  m_PlayerUpdate = atts.GetAttribute<bool>("update_player", false);
		  m_LuaCode = atts.GetAttribute<std::string>("lua_code", "");
		  m_KeyAction = atts.GetAttribute<float>("key_action", 0.0f);
		  m_bMoveBack = atts.GetAttribute<bool>("move_back", false);
		  if(m_LuaCode != "")
			  m_bLuaEnable = true;
		  if( atts.GetAttribute<bool>("physx", false) )
		  {
			  m_Actor = PhysXMInstance->CMapManager<CPhysicActor>::GetResource(atts.GetAttribute<std::string>("name_physx", ""));
			  m_Actor->SetKinematic(true);
		  }
          for ( uint32 i = 0, lCount = atts.GetNumChildren(); i < lCount ; ++i )
              m_CinematicObjectKeyFrames.push_back( new CCinematicObjectKeyFrame( atts( i ) ) );
      }
  }
}

CCinematicObject::~CCinematicObject()
{
  for ( std::vector<CCinematicObjectKeyFrame*>::iterator it = m_CinematicObjectKeyFrames.begin();
        it != m_CinematicObjectKeyFrames.end(); ++it )
    CHECKED_DELETE( ( *it ) );

  m_CinematicObjectKeyFrames.clear();
}

bool CCinematicObject::IsOk()
{
  return true;
}

void CCinematicObject::AddCinematicObjectKeyFrame( CCinematicObjectKeyFrame* CinematicObjectKeyFrame )
{
  m_CinematicObjectKeyFrames.push_back( CinematicObjectKeyFrame );
}

void CCinematicObject::Update()
{
  bool KeyFrameChanged( false );
  CCinematicPlayer::Update();

  for ( unsigned int i = 0; i < m_CinematicObjectKeyFrames.size(); ++i )
  {
    if ( m_CinematicObjectKeyFrames[i]->GetKeyFrameTime() > m_CurrentTime )
    {
      size_t l_PrevKeyFrame = m_CurrentKeyFrame;
      m_CurrentKeyFrame = i - 1;

      if ( l_PrevKeyFrame < m_CurrentKeyFrame )
      {
        KeyFrameChanged = true;
        //baseUtils::Trace("KeyFrameChanged - Next=>%d\n", m_CurrentKeyFrame);
        //baseUtils::Trace("KeyFrameChanged - Prev=>%d\n", l_PrevKeyFrame);
      }

      break;
    }
  }

  if ( m_CurrentTime >= m_Duration )
  {
    if ( m_Cycle )
      OnRestartCycle();
    else
    {
      m_CurrentKeyFrame = m_CinematicObjectKeyFrames.size() - 2;
      m_CurrentTime = m_Duration;
      Pause();
    }
  }

  size_t l_NextKeyFrame = m_CurrentKeyFrame + 1;

  float l_CurrentP = ( m_CurrentTime - m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetKeyFrameTime() ) /
                     ( m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetKeyFrameTime() -
                       m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetKeyFrameTime() );

  //baseUtils::Trace("Percentage=>%f\nCurrent Time=>%f\n", l_CurrentP, m_CurrentTime);

  Math::CLerpAnimator3D lerp_animator_3D;
  lerp_animator_3D.SetValues( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetPosition(),
                              m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetPosition(), 1.0f, Math::FUNC_CONSTANT );
  Math::Vect3f pos;
  lerp_animator_3D.Update( l_CurrentP, pos );

  //baseUtils::Trace("Pos=>%f-%f-%f\n", pos.x, pos.y,pos.z);

  Math::CLerpAnimator1D lerp_animator_1D;
  float init = m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetYaw();
  float final = m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetYaw();

  if ( Math::Utils::Abs( final - init ) > Math::pi32 )
  {
    if ( final > init )
      init += Math::two_pi32;
    else
      final += Math::two_pi32;
  }

  lerp_animator_1D.SetValues( init, final, 1.0f, Math::FUNC_CONSTANT );
  float yaw;
  lerp_animator_1D.Update( l_CurrentP, yaw );
  //baseUtils::Trace("yaw=>%f\n",yaw);

  //Math::Vect3f actual(m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetYaw(), 0.0, 0.0);
  //Math::Vect3f next(m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetYaw(), 0.0, 0.0);
  //lerp_animator_3D.SetValues(actual, next,1.0f,Math::FUNC_CONSTANT);
  //Math::Vect3f yaw_vector;
  //lerp_animator_3D.Update(l_CurrentP, yaw_vector);
  //float yaw = yaw_vector.x;

  if ( KeyFrameChanged )
  {
    //baseUtils::Trace("Current Time=>%f\n", m_CurrentTime);
    //         baseUtils::Trace("YawPrev=>%f\n",m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetYaw());
    //         baseUtils::Trace("YawNext=>%f\n",m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetYaw());
    //         baseUtils::Trace("KeyFrameChanged - Next=>%d\n", l_NextKeyFrame);
    //         baseUtils::Trace("KeyFrameChanged - Prev=>%d\n", m_CurrentKeyFrame);
  }

  init = m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetPitch();
  final = m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetPitch();

  if ( Math::Utils::Abs( final - init ) > Math::pi32 )
  {
    if ( final > init )
      init += Math::two_pi32;
    else
      final += Math::two_pi32;
  }

  lerp_animator_1D.SetValues( init, final, 1.0f, Math::FUNC_CONSTANT );
  float pitch;
  lerp_animator_1D.Update( l_CurrentP, pitch );

  //baseUtils::Trace("pitch=>%f\n", pitch);

  init = m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetRoll();
  final = m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetRoll();

  if ( Math::Utils::Abs( final - init ) > Math::pi32 )
  {
    if ( final > init )
      init += Math::two_pi32;
    else
      final += Math::two_pi32;
  }

  lerp_animator_1D.SetValues( init, final, 1.0f, Math::FUNC_CONSTANT );
  float roll;
  lerp_animator_1D.Update( l_CurrentP, roll );

  //baseUtils::Trace("roll=>%f\n", roll);

  lerp_animator_3D.SetValues( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetScale(),
                              m_CinematicObjectKeyFrames[l_NextKeyFrame]->GetScale(), 1.0f, Math::FUNC_CONSTANT );
  Math::Vect3f scale;
  lerp_animator_3D.Update( l_CurrentP, scale );

  //baseUtils::Trace("Scale=>%f-%f-%f\n", scale.x, scale.y,scale.z);
  ASSERT( m_RenderableObject, "Puntero null del objeto cinematic");
  m_RenderableObject->SetPosition( pos );
  m_RenderableObject->SetYaw( yaw );
  m_RenderableObject->SetPitch( pitch );
  m_RenderableObject->SetRoll( roll );
  m_RenderableObject->SetScale( scale );
  if(m_Actor)
	  m_Actor->SetPosition(pos);
  if(m_PlayerUpdate)
  {
	  CCamera* lCamera = CameraMInstance->GetCurrentCamera();
	  Math::Vect3f lPos = lCamera->GetPosition();
	  std::stringstream lCode;
	  lCode << "g_Player:UpdatePlayer(Vect3f(" << lPos.x << ", " << lPos.y << ", " << lPos.z << "), " << lCamera->GetYaw() << ", " << lCamera->GetPitch() << ")";
	  ScriptMInstance->RunCode(lCode.str());
  }
  if(m_bLuaEnable && m_KeyAction < m_CurrentTime)
  {
	  ScriptMInstance->RunCode(m_LuaCode);
	  m_bLuaEnable = false;
  }

 /* if(m_bMoveBack)
  {
	  CCamera* lCamera = CameraMInstance->GetCurrentCamera();
	  Math::Vect3f lDirection = lCamera->GetDirection();
	  lDirection.y = 0.0f;
	  m_RenderableObject->SetPosition(pos - (lDirection/10.0f));
  }*/
  
  m_RenderableObject->MakeTransform();
  //baseUtils::Trace("KeyFrame:=>%d\n", m_CurrentKeyFrame);
}

void CCinematicObject::OnRestartCycle()
{
  m_CurrentKeyFrame = 0;
  m_CurrentTime = 0.0;
  m_RenderableObject->SetPosition( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetPosition() );
  m_RenderableObject->SetYaw( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetRoll() );
  m_RenderableObject->SetPitch( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetPitch() );
  m_RenderableObject->SetRoll( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetRoll() );
  m_RenderableObject->SetScale( m_CinematicObjectKeyFrames[m_CurrentKeyFrame]->GetScale() );
}

void CCinematicObject::Render()
{
  m_RenderableObject->Render();
}

void CCinematicObject::Stop()
{
  m_CurrentKeyFrame = 0;
}