#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Geometry.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/SceneNode.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

bool Geometry::TextLoader :: operator ()( Geometry const & p_geometry, TextFile & p_file)
{
	Logger::LogMessage( cuT( "Writing Geometry ") + p_geometry.GetName());

	bool l_bReturn = p_file.WriteText( cuT( "object " ) + p_geometry.GetName() + cuT( "\n{\n" ) ) > 0;

	if (l_bReturn)
	{
		l_bReturn = MovableObject::TextLoader()( p_geometry, p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "\tmesh " ) + p_geometry.GetMesh()->GetName() + cuT( "\n" ) ) > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteText( cuT( "}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

Geometry :: Geometry ( Scene * p_pScene, MeshSPtr p_mesh, SceneNodeSPtr p_sn, String const & p_name)
	:	MovableObject	( p_pScene, (SceneNode *)p_sn.get(), p_name, eMOVABLE_TYPE_GEOMETRY	)
	,	m_mesh			( p_mesh															)
	,	m_changed		( true																)
	,	m_listCreated	( false																)
	,	m_visible		( true																)
	,	m_uiMeshId		( 0xFFFFFFFF														)
{
	if( p_mesh && p_mesh->GetSubmesh( 0 ) )
	{
		m_uiMeshId = p_mesh->GetSubmesh( 0 )->GetRefCount();
	}
}

Geometry :: ~Geometry()
{
	Cleanup();
}

void Geometry :: Cleanup()
{
}

void Geometry :: CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex )
{
	if( !m_listCreated )
	{
		Cleanup();
		MeshSPtr l_pMesh = GetMesh();

		if( l_pMesh )
		{
			uint32_t l_nbFaces = l_pMesh->GetNbFaces();
			uint32_t l_nbVertex = l_pMesh->GetNbVertex();

			p_nbFaces += l_nbFaces;
			p_nbVertex += l_nbVertex;

			l_pMesh->ComputeContainers();

			uint32_t l_nbSubmeshes = l_pMesh->GetSubmeshCount();
			m_listCreated = l_nbSubmeshes > 0;

			Logger::LogMessage( cuT( "Geometry :: CreateBuffers - NbVertex : %d, NbFaces : %d" ), l_nbVertex, l_nbFaces );
			m_listCreated = l_pMesh->GetSubmeshCount() > 0;
		}
	}
}

void Geometry :: Render()
{
	uint32_t l_nbSubmeshes;
	SubmeshSPtr l_submesh;
	MeshSPtr l_pMesh = GetMesh();

	if( !m_listCreated )
	{
		uint32_t l_nbFaces = 0, l_nbVertex = 0;
		CreateBuffers( l_nbFaces, l_nbVertex );
	}

	if( m_visible && l_pMesh && m_listCreated )
	{
		l_nbSubmeshes = static_cast< uint32_t >( l_pMesh->GetSubmeshCount() );

		std::for_each( l_pMesh->Begin(), l_pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
		{
			p_pSubmesh->Render();
		} );
	}
}

void Geometry :: SetMesh( MeshSPtr p_pMesh )
{
	m_mesh = p_pMesh;

	if( p_pMesh )
	{
		m_strMeshName = p_pMesh->GetName();

		if( p_pMesh->GetSubmesh( 0 ) )
		{
			m_uiMeshId = p_pMesh->GetSubmesh( 0 )->GetRefCount();
		}
	}
	else
	{
		m_strMeshName = cuEmptyString;
		m_uiMeshId = 0xFFFFFFFF;
	}
}

//*************************************************************************************************
