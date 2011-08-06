#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Geometry.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/SmoothingGroup.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/Pass.hpp"

using namespace Castor3D;

//*************************************************************************************************

bool Loader<Geometry> :: Write( const Geometry & p_geometry, Utils::File & p_file)
{
	Logger::LogMessage( cuT( "Writing Geometry ") + p_geometry.GetName());

	bool l_bReturn = p_file.WriteLine( "object " + p_geometry.GetName() + "\n{\n") > 0;

	if (l_bReturn)
	{
		l_bReturn = Loader<MovableObject>::Write( p_geometry, p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\tmesh " + p_geometry.GetMesh()->GetName() + "\n") > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "}\n") > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

Geometry :: Geometry ( Scene * p_pScene, MeshPtr p_mesh, SceneNodePtr p_sn, String const & p_name)
	:	MovableObject( p_pScene, (SceneNode *)p_sn.get(), p_name, eMOVABLE_TYPE_GEOMETRY),
		m_mesh( p_mesh),
		m_changed( true),
		m_listCreated( false),
		m_visible( true),
		m_dirty( false)
{
}

Geometry :: ~Geometry()
{
	Cleanup();
	m_dirty = false;
	m_visible = false;
}

void Geometry :: Cleanup()
{
	CASTOR_TRACK;
}

void Geometry :: CreateBuffers( eNORMALS_MODE p_nm, size_t & p_nbFaces, size_t & p_nbVertex)
{
	CASTOR_TRACK;
	Logger::LogMessage( cuT( "Geometry :: CreateBuffers - %d"), p_nm);
	Cleanup();

	if (m_mesh)
	{
		size_t l_nbFaces = m_mesh->GetNbFaces();
		size_t l_nbVertex = m_mesh->GetNbVertex();

		p_nbFaces += l_nbFaces;
		p_nbVertex += l_nbVertex;

		m_mesh->SetNormals( p_nm);

		size_t l_nbSubmeshes = m_mesh->GetNbSubmeshes();
		m_listCreated = l_nbSubmeshes > 0;

		m_mesh->InitialiseBuffers();

		m_normalsMode = p_nm;
		Logger::LogMessage( cuT( "Geometry :: CreateBuffers - NbVertex : %d, NbFaces : %d"), l_nbVertex, l_nbFaces);
		m_listCreated = m_mesh->GetNbSubmeshes() > 0;
		m_dirty = false;
	}
}

void Geometry :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	CASTOR_TRACK;
	if (m_dirty)
	{
		size_t l_nbFaces = 0, l_nbVertex = 0;
		CreateBuffers( m_normalsMode, l_nbFaces, l_nbVertex);
	}

	if (m_visible && m_mesh && m_mesh->IsOk())
	{
		if ( ! m_listCreated)
		{
			return;
		}

		unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_mesh->GetNbSubmeshes());
		SubmeshPtr l_submesh;

		for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
		{
			l_submesh = m_mesh->GetSubmesh( i);
			l_submesh->Render( p_displayMode);
		}
	}
}

void Geometry :: Subdivide( unsigned int p_index, SubdividerPtr p_pSubdivider, bool p_bThreaded)
{
	m_visible = false;
	m_dirty = m_mesh->Subdivide( p_index, p_pSubdivider, p_bThreaded);
	m_visible = true;
}

void Geometry :: SetMesh( MeshPtr p_pMesh)
{
	m_mesh = p_pMesh;

	if (m_mesh)
	{
		m_strMeshName = m_mesh->GetName();
	}
	else
	{
		m_strMeshName = cuEmptyString;
	}
}

BEGIN_SERIALISE_MAP( Geometry, MovableObject)
	ADD_ELEMENT( m_strMeshName)
END_SERIALISE_MAP()

BEGIN_POST_UNSERIALISE( Geometry, MovableObject)
	Collection<Mesh, String> l_mshCollection;
	SetMesh( l_mshCollection.GetElement( m_strMeshName));
END_POST_UNSERIALISE()

//*************************************************************************************************