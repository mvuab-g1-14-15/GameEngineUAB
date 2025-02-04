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
#include "CookingMesh/PhysicCookingMesh.h"
//--------------------------

#include "Utils/Defines.h"
#include "Utils\BaseUtils.h"

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
  std::map<std::string, VecMeshes>::iterator it( m_TriangleMeshes.begin() );
  std::map<std::string, VecMeshes>::iterator endIt( m_TriangleMeshes.end() );

  while ( it != endIt )
  {
    VecMeshes Vectmesh = it->second;
	VecMeshes::iterator jt( Vectmesh.begin() );
	VecMeshes::iterator endJt( Vectmesh.end() );
	while ( jt != endJt )
	{
		NxTriangleMesh* mesh = jt->second;
		if ( mesh != NULL && mesh->getReferenceCount() == 0 )
			m_pPhysicSDK->releaseTriangleMesh( *mesh );
		++jt;
	}
	Vectmesh.clear();
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
	//TODO RULY
  /*VecMeshes::iterator it = m_TriangleMeshes.find( _Name );

  if ( it != m_TriangleMeshes.end() )
    return it->second;*/

  return NULL;
}

//----------------------------------------------------------------------------
// PhysicMesh created from a Bin file. This file has been saved previously
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::CreatePhysicMesh( const std::string& _Bin_Filename, const std::string& _NameMesh )
{
  bool isOk = false;
  //TODO RULY
  /*VecMeshes::iterator it = m_TriangleMeshes.find( _NameMesh );

  if ( it == m_TriangleMeshes.end() )
  {
    NxTriangleMesh* triangleMesh = NULL;
    triangleMesh = m_pPhysicSDK->createTriangleMesh( CPhysicUserStream( _Bin_Filename.c_str(), true ) );
    isOk = ( triangleMesh != NULL );

    if ( isOk )
      m_TriangleMeshes.insert( std::pair<std::string, NxTriangleMesh*>( _NameMesh, triangleMesh ) );
  }*/

  return isOk;
}

NxTriangleMesh* CPhysicCookingMesh::CreatePhysicMesh( const std::vector<Math::Vect3f>& l_VB,
    const std::vector<uint32>& l_IB )
{
  ASSERT( !l_VB.empty() && !l_IB.empty(), "Null VB or IB" );

  if ( !l_VB.empty() && !l_IB.empty() )
  {
    NxTriangleMeshDesc l_TriangleMeshDesc;
    l_TriangleMeshDesc.numVertices      = ( NxU32 ) l_VB.size();
    l_TriangleMeshDesc.numTriangles     = ( NxU32 ) l_IB.size() / 3;
    l_TriangleMeshDesc.pointStrideBytes     = sizeof( Math::Vect3f );
    l_TriangleMeshDesc.triangleStrideBytes  = 3 * sizeof( uint32 );
    l_TriangleMeshDesc.points         = &l_VB[0].x;
    l_TriangleMeshDesc.triangles      = &l_IB[0];
    l_TriangleMeshDesc.flags          = 0;
    CPhysicMemoryWriteBuffer buf;

    if ( !m_pCooking->NxCookTriangleMesh( l_TriangleMeshDesc, buf ) ) return 0;

    return m_pPhysicSDK->createTriangleMesh( CPhysicMemoryReadBuffer( buf.data ) );
  }

  return 0;
}


//----------------------------------------------------------------------------
// Creating a PhysicMesh from a buffer
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::CreatePhysicMesh( std::string _NameMesh, std::vector<std::vector<Math::Vect3f>>& _Vertices, std::vector<std::vector<unsigned int>>& _Faces, std::string _NameRoom )
{
    bool isOk = true;
    int lSize = _Vertices.size();
	VecMeshes lRoomVecMesh;
    //TIMER_START();
    #pragma omp parallel for
    for (int i = 0; i < lSize; ++i)
    {
        std::ostringstream s;
        s << _NameMesh << i;

        std::string l_Name( s.str() );
        bool loop_ok = CreatePhysicMesh(l_Name,  _Vertices[i], _Faces[i], _NameRoom, lRoomVecMesh);

        #pragma omp critical
        isOk = loop_ok;
    }
    if(isOk)
		if(m_TriangleMeshes.find(_NameRoom) == m_TriangleMeshes.end())
			m_TriangleMeshes.insert(std::pair<std::string, VecMeshes>( _NameRoom, lRoomVecMesh ));
    //TIMER_STOP( "Generating the physx mesh with the ase." );
    return isOk;
}

bool CPhysicCookingMesh::CreatePhysicMesh( std::string _NameMesh, std::vector<Math::Vect3f>& _Vertices, std::vector<unsigned int>& _Faces, std::string _NameRoom, VecMeshes& lRoomVecMesh )
{
	std::map<std::string, NxTriangleMesh*>::iterator it = lRoomVecMesh.find( _NameMesh );
	if ( it != lRoomVecMesh.end() ) return false;
	

    NxTriangleMesh* l_TriangleMesh = CreatePhysicMesh( _Vertices, _Faces );
    if ( l_TriangleMesh == 0 ) return false;

    #pragma omp critical
    lRoomVecMesh.insert( std::pair<std::string, NxTriangleMesh*>( _NameMesh, l_TriangleMesh ) );
	
    return true;
}


//----------------------------------------------------------------------------
// Save a PhysicMesh in a bin file
//----------------------------------------------------------------------------
bool CPhysicCookingMesh::SavePhysicMesh( const std::vector<Math::Vect3f>& _Vertices, const std::vector<uint32>& _Faces,
    const std::string& _BinFilename )
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
bool CPhysicCookingMesh::ReadMeshFromASE( std::string _FileName, std::vector<std::vector<Math::Vect3f>>& _Vertices,
    std::vector<std::vector<unsigned int>>& _Faces )
{
  bool l_bOK = CPhysicASELoader::ReadMeshFromASE( _FileName, _Vertices, _Faces );
  return l_bOK;
}

bool CPhysicCookingMesh::CreateMeshFromASE( std::string _FileName, std::string _Name,  std::string _RoomName)
{
  std::vector<std::vector<Math::Vect3f>> l_Vertices;
  std::vector<std::vector<unsigned int>> l_Faces;

  if ( ReadMeshFromASE( _FileName, l_Vertices, l_Faces ) )
    if ( CreatePhysicMesh( _Name, l_Vertices, l_Faces, _RoomName ) )
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
	//TODO RULY
  /*VecMeshes::iterator it = m_TriangleMeshes.find( name );

  if ( it != m_TriangleMeshes.end() )
    m_TriangleMeshes.erase( it );*/
}