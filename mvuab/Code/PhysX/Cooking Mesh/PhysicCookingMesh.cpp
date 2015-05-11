#define __DONT_INCLUDE_MEM_LEAKS__
#include <sstream>
#include <string>

#include "Logger/Logger.h"

//---PhysX includes----
#undef min
#undef max
#include "NxPhysics.h"
#include "NxCooking.h"
#include "Utils/PhysicUserAllocator.h"
#include "PhysXLoader.h"
#include "Utils/PhysicStream.h"
#include "Utils/PhysicASELoader.h"
#include "Cooking Mesh/PhysicCookingMesh.h"
//--------------------------

#include "Utils/Defines.h"

#if defined( _DEBUG )
#include "Memory\MemLeaks.h"
#endif //defined(_DEBUG)

//----------------------------------------------------------------------------
// Init data
//----------------------------------------------------------------------------
CPhysicCookingMesh::CPhysicCookingMesh( void )
  : m_bIsOk( false )
  , m_pPhysicSDK( NULL )
  , m_pMyAllocator( NULL )
  , m_pCooking( NULL )
{
}

bool CPhysicCookingMesh::Init( NxPhysicsSDK* _pPhysicSDK, CPhysicUserAllocator* _pMyAllocator )
{

  m_pPhysicSDK  = _pPhysicSDK;
  m_pMyAllocator  = new CPhysicUserAllocator();
  assert( m_pPhysicSDK && m_pMyAllocator );
  m_bIsOk = ( m_pMyAllocator != NULL && m_pPhysicSDK != NULL );

  if ( m_bIsOk )
  {
    m_pCooking = NxGetCookingLib( NX_PHYSICS_SDK_VERSION );
    m_bIsOk = ( m_pCooking != NULL );

    if ( m_bIsOk )
      m_bIsOk = m_pCooking->NxInitCooking( m_pMyAllocator, NULL );
  }

  if ( !m_bIsOk )
    Release();

  return m_bIsOk;
}

//----------------------------------------------------------------------------
// Finalize data
//----------------------------------------------------------------------------
void CPhysicCookingMesh::Done( void )
{
  if ( IsOk() )
  {
    Release();
    m_bIsOk = false;
  }
}

//----------------------------------------------------------------------------
// Free memory
//----------------------------------------------------------------------------
void CPhysicCookingMesh::Release( void )
{
  //---Delete TriangleMeshes-----------
  VecMeshes::iterator it( m_TriangleMeshes.begin() );
  VecMeshes::iterator endIt( m_TriangleMeshes.end() );

  while ( it != endIt )
  {
    NxTriangleMesh* mesh = it->second;

    if ( mesh != NULL && mesh->getReferenceCount() == 0 )
      m_pPhysicSDK->releaseTriangleMesh( *mesh );

    ++it;
  }

  m_TriangleMeshes.clear();
  //------------------------------------

  //Close Cooking
  if ( m_pCooking != NULL )
    m_pCooking->NxCloseCooking();

  CHECKED_DELETE( m_pMyAllocator );
}

NxTriangleMesh* CPhysicCookingMesh::GetPhysicMesh( const std::string& _Name )
{
  VecMeshes::iterator it = m_TriangleMeshes.find( _Name );

  if ( it != m_TriangleMeshes.end() )
    return it->second;

  return NULL;
}

//----------------------------------------------------------------------------
// PhysicMesh created from a Bin file. This file has been saved previously
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::CreatePhysicMesh( const std::string& _Bin_Filename, const std::string& _NameMesh )
{
  bool isOk = false;
  VecMeshes::iterator it = m_TriangleMeshes.find( _NameMesh );

  if ( it == m_TriangleMeshes.end() )
  {
    NxTriangleMesh* triangleMesh = NULL;
    triangleMesh = m_pPhysicSDK->createTriangleMesh( CPhysicUserStream( _Bin_Filename.c_str(), true ) );
    isOk = ( triangleMesh != NULL );

    if ( isOk )
      m_TriangleMeshes.insert( std::pair<std::string, NxTriangleMesh*>( _NameMesh, triangleMesh ) );
  }

  return isOk;
}


//----------------------------------------------------------------------------
// Creating a PhysicMesh from a buffer
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::CreatePhysicMesh(std::string _NameMesh, std::vector<std::vector<Math::Vect3f>> &_Vertices, std::vector<std::vector<unsigned int>> &_Faces )
{
	bool isOk = false;

	for(unsigned int i = 0; i < _Vertices.size(); i++)
	{
		std::ostringstream s; s << _NameMesh << i;
		std::string l_Name(s.str());

		std::map<std::string, NxTriangleMesh*>::iterator it = m_TriangleMeshes.find( l_Name );
		if ( it == m_TriangleMeshes.end() )
		{
			// Build physical model
			NxTriangleMeshDesc triangleMeshDesc;
			triangleMeshDesc.numVertices      = ( NxU32 ) _Vertices[i].size();
			triangleMeshDesc.numTriangles     = ( NxU32 ) _Faces[i].size() / 3;
			
			triangleMeshDesc.pointStrideBytes     = sizeof( Math::Vect3f );
			triangleMeshDesc.triangleStrideBytes  = 3 * sizeof( uint32 );
			
			triangleMeshDesc.points         = &_Vertices[i][0].x;
			triangleMeshDesc.triangles      = &_Faces[i][0];
			triangleMeshDesc.flags          = 0;
			
			assert( m_pCooking ); //by if the flies...
			
			CPhysicMemoryWriteBuffer buf;
			
			if ( m_pCooking->NxCookTriangleMesh( triangleMeshDesc, buf ) )
			{
				NxTriangleMesh* l_TriangleMesh = NULL;
				l_TriangleMesh = m_pPhysicSDK->createTriangleMesh( CPhysicMemoryReadBuffer(buf.data) );
				
				isOk = ( l_TriangleMesh != NULL );
				if ( isOk ) m_TriangleMeshes.insert( std::pair<std::string, NxTriangleMesh*>( l_Name, l_TriangleMesh ) );
			}
		}
	}
	
	return isOk;
}

bool CPhysicCookingMesh::CreatePhysicMesh(std::string _NameMesh, std::vector<Math::Vect3f> &_Vertices, std::vector<unsigned int> &_Faces )
{
    bool isOk = false;
    std::map<std::string, NxTriangleMesh*>::iterator it = m_TriangleMeshes.find( _NameMesh );
    
    if ( it != m_TriangleMeshes.end() ) return isOk;
    
    // Build physical model
    NxTriangleMeshDesc triangleMeshDesc;
    triangleMeshDesc.numVertices      = ( NxU32 ) _Vertices.size();
    triangleMeshDesc.numTriangles     = ( NxU32 ) _Faces.size() / 3;
   
    triangleMeshDesc.pointStrideBytes     = sizeof( Math::Vect3f );
    triangleMeshDesc.triangleStrideBytes  = 3 * sizeof( uint32 );
    
    triangleMeshDesc.points         = &_Vertices[0].x;
    triangleMeshDesc.triangles      = &_Faces[0];
    triangleMeshDesc.flags          = 0;
    
    assert( m_pCooking ); //by if the flies...
    CPhysicMemoryWriteBuffer buf;

    if(m_pCooking->NxCookTriangleMesh(triangleMeshDesc, buf))
    {
        NxTriangleMesh* l_TriangleMesh = NULL;
        l_TriangleMesh = m_pPhysicSDK->createTriangleMesh( CPhysicMemoryReadBuffer(buf.data) );
        
        isOk = ( l_TriangleMesh != NULL );
        if(isOk) m_TriangleMeshes.insert( std::pair<std::string, NxTriangleMesh*>(_NameMesh, l_TriangleMesh));
	}
	
	return isOk;
}


//----------------------------------------------------------------------------
// Save a PhysicMesh in a bin file
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::SavePhysicMesh( const std::vector<Math::Vect3f>& _Vertices, const std::vector<uint32>& _Faces, const std::string& _BinFilename )
{
  // Build physical model
  NxTriangleMeshDesc triangleMeshDesc;
  triangleMeshDesc.numVertices      = ( NxU32 )_Vertices.size();
  triangleMeshDesc.numTriangles     = ( NxU32 )_Faces.size() / 3;
  triangleMeshDesc.pointStrideBytes   = sizeof( Math::Vect3f );
  triangleMeshDesc.triangleStrideBytes  = 3 * sizeof( uint32 );
  triangleMeshDesc.points         = &_Vertices[0].x;
  triangleMeshDesc.triangles        = &_Faces[0];
  triangleMeshDesc.flags          = 0;

  assert( m_pCooking );
  bool isOk = false;

  isOk = m_pCooking->NxCookTriangleMesh( triangleMeshDesc, CPhysicUserStream( _BinFilename.c_str(), false ) );

  return isOk;
}

//----------------------------------------------------------------------------
// Create a Cloth mesh
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::CookClothMesh( const NxClothMeshDesc& desc, NxStream& stream )
{
  assert( m_pCooking );
  bool isOk = false;
  isOk = m_pCooking->NxCookClothMesh( desc, stream );
  return isOk;
}


//----------------------------------------------------------------------------
// Load the ASE File thought ASE Loader
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::ReadMeshFromASE( std::string _FileName, std::vector<std::vector<Math::Vect3f>> &_Vertices, std::vector<std::vector<unsigned int>> &_Faces )
{
  bool l_bOK = CPhysicASELoader::ReadMeshFromASE( _FileName, _Vertices, _Faces );
  return l_bOK;
}

bool CPhysicCookingMesh::CreateMeshFromASE( std::string _FileName, std::string _Name )
{
  std::vector<std::vector<Math::Vect3f>> l_Vertices;
  std::vector<std::vector<unsigned int>> l_Faces;

  if ( ReadMeshFromASE( _FileName, l_Vertices, l_Faces ) )
    if ( CreatePhysicMesh( _Name, l_Vertices, l_Faces ) )
      return true;
    else
    {
      LOG_ERROR_APPLICATION( "Error al leer la mesh del fichero ASE: %s",
                                             _FileName );
      return false;
    }
  else
  {
    LOG_ERROR_APPLICATION( "Error al leer la mesh del fichero ASE: %s",
                                           _FileName );
    return false;
  }
}

void CPhysicCookingMesh::ReleasePhysicMesh( const std::string& name )
{
  VecMeshes::iterator it = m_TriangleMeshes.find( name );

  if ( it != m_TriangleMeshes.end() )
    m_TriangleMeshes.erase( it );
}