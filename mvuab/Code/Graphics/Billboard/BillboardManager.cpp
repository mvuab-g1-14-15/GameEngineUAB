#include "BillboardManager.h"

#include "GraphicsManager.h"
#include "Utils\Defines.h"
#include "EngineManagers.h"
#include "Texture\TextureManager.h"
#include "RenderableVertex\VertexTypes.h"
#include "RenderableVertex\IndexedVertexs.h"
#include "RenderableVertex\RenderableVertex.h"
#include "BillboardInstance.h"
#include "Cameras/CameraManager.h"

CRenderableVertexs* CBillboardManager::sRV = 0;

namespace
{
  const uint32 lIdxCount = 6;
  const uint32 lVtxCount = 4;

  TT1_VERTEX lVtx[lVtxCount] =
  {
    {  -0.5f, 0.0f, -0.5f, 0, 0 },    // vertex 0
    {  -0.5f, 0.0f,  0.5f, 0, 1 },    // vertex 1
    {   0.5f, 0.0f,  0.5f, 1, 1 },    // vertex 2
    {   0.5f, 0.0f, -0.5f, 1, 0 }     // vertex 3
  };

  unsigned short int lIdx[lIdxCount] = { 0, 1, 2,  2, 3, 0 };
}

CBillboardManager::CBillboardManager( const CXMLTreeNode& atts )
  : CManager( atts )
{
}

CBillboardManager::~CBillboardManager()
{
  DestroyBillBoardGeometry();
  Destroy();
}

void CBillboardManager::Render()
{
  BROFILER_CATEGORY( "CBillboardManager::Render()", Profiler::Color::Orchid )
  const Math::Vect3f& lCameraPosition = CameraMInstance->GetCurrentCamera()->GetPosition();
  for(unsigned int i = 0, l_Size = m_ResourcesVector.size(); i < l_Size; ++i)
  {
      m_ResourcesVector[i]->Render(sRV, mGM, lCameraPosition );
  }
}

void CBillboardManager::Update()
{BROFILER_CATEGORY( "CBillboardManager::Update()", Profiler::Color::Orchid )
    for(unsigned int i = 0, l_Size = m_ResourcesVector.size(); i < l_Size; ++i)
    {
        m_ResourcesVector[i]->Update();
    }
}

void CBillboardManager::Init()
{
  CXMLTreeNode newFile, node;

  if ( newFile.LoadAndFindNode( mConfigPath.c_str(), "billboards", node ) )
  {
    // Create the shared geometry
    CreateBillBoardGeometry();
    mGM = GraphicsInstance;
    for ( uint32 i = 0, lCount = node.GetNumChildren(); i < lCount ; ++i )
    {
      const CXMLTreeNode& lCurrentNode = node( i );
      const std::string&  lCurrentNodeName = lCurrentNode.GetName();

      if ( lCurrentNodeName == "billboard" )
      {
        const std::string& lBillboardName = lCurrentNode.GetAttribute<std::string>( "name", "unknown" );
        CBillboardCore* lBillBoard = new CBillboardCore();

        if ( lBillBoard->Init( lCurrentNode ) ) AddResource( lBillboardName, lBillBoard );
        else
        {
          CHECKED_DELETE( lBillBoard );
          LOG_ERROR_APPLICATION( "Error creating the billboard %s check the configuration file!", lBillboardName.c_str() );
        }
      }
    }
  }
}

void CBillboardManager::LoadInstances( const std::string& aFileName )
{
  CXMLTreeNode newFile, node;

  if ( newFile.LoadAndFindNode( aFileName.c_str(), "billboards_instances", node ) )
  {
    for ( uint32 i = 0, lCount = node.GetNumChildren(); i < lCount ; ++i )
    {
      const CXMLTreeNode& lCurrentNode = node( i );
      const std::string& lCurrentNodeName = lCurrentNode.GetName();

      if ( lCurrentNodeName == "billboard_instance" )
      {
        const std::string& lBillboardCoreName = lCurrentNode.GetAttribute<std::string>( "core", "unknown" );
        CBillboardCore* lBillBoard = GetResource( lBillboardCoreName );

        if( lBillBoard )
        {
          CBillboardInstance* lBillboardInstance = new CBillboardInstance();
          if( lBillboardInstance->Init( lCurrentNode ) )
            lBillBoard->AddInstance(lBillboardInstance);
        }
        else
        {
          LOG_ERROR_APPLICATION( "The billboard core %s", lBillboardCoreName.c_str() );
        }
      }
    }
  }
}

void CBillboardManager::FlushInstances()
{
  for ( TVectorResources::iterator lItb = m_ResourcesVector.begin(), lIte = m_ResourcesVector.end(); lItb != lIte; ++lItb )
    (*lItb)->Flush();
}

void CBillboardManager::CreateBillBoardGeometry()
{
  if( !sRV )
  {
    sRV = new CIndexedVertexs<TT1_VERTEX, short>(GraphicsInstance, lVtx, lIdx, lVtxCount, lIdxCount);
  }
}

void CBillboardManager::DestroyBillBoardGeometry()
{
  CHECKED_DELETE(sRV);
}

CBillboardInstance* CBillboardManager::CreateInstance( const std::string& aCoreName, const Math::Vect3f& aPosition, bool aActive )
{
  CBillboardInstance* lInstance = NULL;
  CBillboardCore* lCore = GetResource(aCoreName);
  if( lCore )
  {
    lInstance = new CBillboardInstance();
    lInstance->ChangePosition( aPosition );
    lInstance->ChangeVisibility( aActive );
    lCore->AddInstance( lInstance );
  }

  return lInstance;
}
