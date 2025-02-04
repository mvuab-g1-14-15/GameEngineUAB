#include "RenderableObjectsManager.h"
#include "RenderableObject.h"
#include "XML\XMLTreeNode.h"
#include "Logger\Logger.h"
#include "StaticMeshes\InstanceMesh.h"
#include "AnimatedModels\AnimatedInstanceModel.h"
#include "Math\MathTypes.h"
#include "Cinematics\Cinematic.h"

CRenderableObjectsManager::CRenderableObjectsManager()
{
}

CRenderableObjectsManager::~CRenderableObjectsManager()
{
    CleanUp();
}

bool CRenderableObjectsManager::Load( const std::string& FileName )
{
    CXMLTreeNode newFile;
    CXMLTreeNode m;

    if ( !newFile.LoadFile( FileName.c_str() ) )
    {
        LOG_ERROR_APPLICATION( "CRenderableObjectsManager::Load --> Error loading XML %s.", FileName.c_str() );
        return false;
    }

    m = newFile["RenderableObjects"];

    if ( !m.Exists() )
    {
        LOG_ERROR_APPLICATION("CRenderableObjectsManager::Load --> RenderableObjects tag not found in the file %s", FileName.c_str());
        return false;
    }

    for ( uint32 i = 0, lCount = m.GetNumChildren(); i < lCount; ++i )
    {
        const CXMLTreeNode& lNode = m(i);
        const std::string & l_TagName = lNode.GetName();
        const std::string& l_Name = lNode.GetAttribute<std::string>( "name", "" );

        if ( l_TagName == "MeshInstance" )
        {
            CInstanceMesh* l_InstanceMesh = new CInstanceMesh( lNode );
            if ( !AddResource( l_Name, l_InstanceMesh ) )
            {
                CHECKED_DELETE( l_InstanceMesh );
            }
        }
        else if ( l_TagName == "AnimatedInstance" )
        {
            CAnimatedInstanceModel* l_AnimatedInstance = new CAnimatedInstanceModel( lNode );
            if ( !AddResource( l_Name, l_AnimatedInstance ) )
            {
                CHECKED_DELETE( l_AnimatedInstance );
            }
        }
    }

    //CCinematic* l_CinematicInstance = new CCinematic("Data/cinematic.xml");
    //l_CinematicInstance->Play(true);
    //AddResource(l_CinematicInstance->GetName(), l_CinematicInstance);
    return true;
}

void CRenderableObjectsManager::CleanUp()
{
    Destroy();
}

void CRenderableObjectsManager::Render()
{
    for ( unsigned int i = 0; i < m_ResourcesVector.size(); ++i )
    {
		if(m_ResourcesVector[i]->GetActive())
			m_ResourcesVector[i]->Render();
    }
}

void CRenderableObjectsManager::Update()
{
    for ( unsigned int i = 0; i < m_ResourcesVector.size(); ++i )
    {
        m_ResourcesVector[i]->Update();
    }
}

//CRenderableObject* CRenderableObjectsManager::AddMeshInstance( const std::string& CoreMeshName,
//    const std::string& InstanceName, const Math::Vect3f& Position )
//{
//  return NULL; //TODO RAUl
//}
//
//CRenderableObject* CRenderableObjectsManager::AddAnimatedInstanceModel(
//  const std::string& CoreModelName, const std::string& InstanceModelName,
//  const Math::Vect3f& Position )
//{
//  return NULL; //TODO RAUl
//}

CCinematic* CRenderableObjectsManager::CreateCinematic( const std::string& FileName )
{
    CCinematic* l_Cinematic = new CCinematic( FileName );

    if(!AddResource(l_Cinematic->GetName(), l_Cinematic))
    {
        CHECKED_DELETE(l_Cinematic);
        return NULL;
    }

    return l_Cinematic;
}