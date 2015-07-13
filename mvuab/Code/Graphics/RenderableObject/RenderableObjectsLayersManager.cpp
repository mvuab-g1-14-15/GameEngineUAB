#include "AnimatedModels\AnimatedInstanceModel.h"
#include "AnimatedModels\AnimatedInstanceModel.h"

#include "RenderableObjectsLayersManager.h"
#include "StaticMeshes\InstanceMesh.h"

#include "EngineConfig.h"
#include "Utils\Defines.h"

#include "PhysicsManager.h"
#include "EngineManagers.h"

#include "StaticMeshes\StaticMeshManager.h"
#include "CookingMesh\PhysicCookingMesh.h"

#include "Actor\PhysicActor.h"
#include "Utils\PhysicUserData.h"
#include "Math\AABB.h"


CRenderableObjectsLayersManager::CRenderableObjectsLayersManager()
    : m_DefaultRenderableObjectManager( 0 )
{
}

CRenderableObjectsLayersManager::CRenderableObjectsLayersManager( const CXMLTreeNode& atts )
    : m_DefaultRenderableObjectManager( 0 )
{
}

CRenderableObjectsLayersManager::~CRenderableObjectsLayersManager()
{
    Destroy();

    for ( unsigned int i = 0; i < m_PhyscsUserData.size(); i++ )
        CHECKED_DELETE( m_PhyscsUserData.at( i ) );
}
void CRenderableObjectsLayersManager::Destroy()
{
    CTemplatedVectorMapManager::Destroy();
}

bool CRenderableObjectsLayersManager::LoadLayers( const std::string& l_FilePath )
{
    CXMLTreeNode l_Node, l_Root;

    if ( !l_Root.LoadAndFindNode( l_FilePath.c_str(), "room", l_Node ) )
        return false;

    const std::string& l_Path = l_Root.GetAttribute<std::string>( "path", "no_path" );

    for ( int i = 0, l_NumChilds = l_Node.GetNumChildren(); i < l_NumChilds; ++i )
    {
        CXMLTreeNode& l_CurrentNode = l_Node( i );

        const std::string& l_Name = l_CurrentNode.GetAttribute<std::string>( "name", "no_path" );
        const std::string& l_File = l_CurrentNode.GetAttribute<std::string>( "file", "no_file" );


        CRenderableObjectsManager* l_ROM  = LoadRenderableObject( l_Path + "/" + l_File );

        if ( !AddResource( l_Name, l_ROM ) )
            CHECKED_DELETE( l_ROM );
    }

    return true;
}

CRenderableObjectsManager* CRenderableObjectsLayersManager::LoadRenderableObject( const std::string& l_FilePath )
{

    CXMLTreeNode l_Node, l_Root;
    CRenderableObjectsManager* lRenderableObjectManager = new CRenderableObjectsManager();

    if ( !l_Root.LoadAndFindNode( l_FilePath.c_str(), "RenderableObjects", l_Node ) )
        return 0;

    for ( int i = 0, l_NumChilds = l_Node.GetNumChildren(); i < l_NumChilds; ++i )
    {
        const CXMLTreeNode& lNode = l_Node( i );
        const std::string& lTagName = lNode.GetName();
        const std::string& lName = lNode.GetAttribute<std::string>( "name", "" );

        if ( lTagName == "MeshInstance" )
            AddNewInstaceMesh( lNode );
        else if ( lTagName == "AnimatedInstance" )
        {
            CAnimatedInstanceModel* l_AnimatedInstance = new CAnimatedInstanceModel( lNode );

            if ( !lRenderableObjectManager->AddResource( lName, l_AnimatedInstance ) )
            {
                LOG_ERROR_APPLICATION( "Error adding animated mesh %s!", lName.c_str() );
                CHECKED_DELETE( l_AnimatedInstance );
            }
        }
    }

    return lRenderableObjectManager;
}

void CRenderableObjectsLayersManager::Update()
{
    std::vector<CRenderableObjectsManager*>::iterator itb = GetResourcesVector().begin(), ite = GetResourcesVector().end();

    for ( ; itb != ite; ++itb )
        ( *itb )->Update();
}
void CRenderableObjectsLayersManager::Render()
{
    std::vector<CRenderableObjectsManager*>::iterator itb = GetResourcesVector().begin(), ite = GetResourcesVector().end();

    for ( ; itb != ite; ++itb )
        ( *itb )->Render();
}
void CRenderableObjectsLayersManager::Render( const std::string& LayerName )
{
    TMapResources l_ResourcesMap = GetResourcesMap();
    TMapResources::iterator itb = l_ResourcesMap.find( LayerName );

    if ( itb != l_ResourcesMap.end() )
        ( *itb ).second.m_Value->Render();
}
CRenderableObjectsManager* CRenderableObjectsLayersManager::GetRenderableObjectManager( const CXMLTreeNode& Node )
{
    const std::string& l_Layer = Node.GetAttribute<std::string>( "layer", "" );
    return ( l_Layer == "" ) ? m_DefaultRenderableObjectManager : GetResource( l_Layer.c_str() );
}

void CRenderableObjectsLayersManager::AddNewInstaceMesh( const CXMLTreeNode& atts )
{
    CInstanceMesh* l_InstanceMesh = new CInstanceMesh( atts );
    l_InstanceMesh->SetType( atts.GetAttribute<std::string>( "type", "static" ) );

    bool lOk = false;
    const string& l_Name = l_InstanceMesh->GetName();

    CPhysicUserData* l_pPhysicUserDataMesh = new CPhysicUserData( l_Name );

    if ( l_InstanceMesh->GetType() == "dynamic" )
        l_pPhysicUserDataMesh->SetGroup( ECG_DYNAMIC_OBJECTS );

    CPhysicActor* l_MeshActor = new CPhysicActor( l_pPhysicUserDataMesh );

    if ( l_InstanceMesh->GetType() == "dynamic" )
    {
        CStaticMesh* l_StaticMesh = l_InstanceMesh->GetStaticMesh();

        Math::AABB3f l_AABB = l_StaticMesh->GetAABB();
        Math::Vect3f l_Pos = l_InstanceMesh->GetTransform() * l_AABB.GetCenter();

        l_MeshActor->AddBoxShape( Vect3f( l_AABB.GetWidth() * 0.5f, l_AABB.GetHeight() * 0.5f, l_AABB.GetDepth() * 0.5f ), l_Pos );
        l_MeshActor->CreateBody( 1.0f );
    }
    else
    {
        NxTriangleMesh* l_TriangleMesh = PhysXMInstance->GetCookingMesh()->CreatePhysicMesh( l_InstanceMesh->GetVertexBuffer(),
                                         l_InstanceMesh->GetIndexBuffer() );
        l_MeshActor->AddMeshShape( l_TriangleMesh, l_InstanceMesh->GetTransform() );
    }

    if ( PhysXMInstance->CMapManager<CPhysicActor>::GetResource( l_Name ) == 0 && PhysXMInstance->AddPhysicActor( l_MeshActor ) &&
            PhysXMInstance->CMapManager<CPhysicActor>::AddResource( l_Name, l_MeshActor ) )
    {
        lOk = true;
        m_PhyscsUserData.push_back( l_pPhysicUserDataMesh );
    }

    if ( !lOk )
    {
        PhysXMInstance->ReleasePhysicActor( l_MeshActor );

        CHECKED_DELETE( l_MeshActor );
        CHECKED_DELETE( l_pPhysicUserDataMesh );
        CHECKED_DELETE( l_pPhysicUserDataMesh );
    }
    else if ( l_InstanceMesh->GetType() == "dynamic" )
    {
        l_MeshActor->SetCollisionGroup( ECG_DYNAMIC_OBJECTS );
        l_InstanceMesh->SetActor( l_MeshActor );
    }
    else
        l_InstanceMesh->SetActor( l_MeshActor );

    CRenderableObjectsManager* lRenderableObjectManager = GetRenderableObjectManager( atts );

    if ( !lRenderableObjectManager->AddResource( l_Name, l_InstanceMesh ) )
    {
        LOG_ERROR_APPLICATION( "Error adding instance mesh %s!", l_Name.c_str() );
        CHECKED_DELETE( l_InstanceMesh );
    }
}
