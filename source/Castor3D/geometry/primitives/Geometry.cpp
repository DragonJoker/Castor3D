#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/primitives/Geometry.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/mesh/Mesh.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/mesh/MeshManager.h"
#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/scene/SceneNode.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/shader/ShaderProgram.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/material/TextureUnit.h"
#include "Castor3D/material/Pass.h"

using namespace Castor3D;

Geometry :: Geometry ( Scene * p_pScene, MeshPtr p_mesh, SceneNodePtr p_sn, const String & p_name)
	:	MovableObject( p_pScene, (SceneNode *)p_sn.get(), p_name, eGeometry),
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
}

void Geometry :: CreateBuffers( eNORMALS_MODE p_nm, size_t & p_nbFaces, size_t & p_nbVertex)
{
	Logger::LogMessage( CU_T( "Geometry :: CreateBuffers - %d"), p_nm);
	Cleanup();

	if (m_mesh != NULL)
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
		Logger::LogMessage( CU_T( "Geometry :: CreateBuffers - NbVertex : %d, NbFaces : %d"), l_nbVertex, l_nbFaces);
		m_listCreated = m_mesh->GetNbSubmeshes() > 0;
		m_dirty = false;
	}
}

void Geometry :: Render( ePRIMITIVE_TYPE p_displayMode)
{
	if (m_dirty)
	{
		size_t l_nbFaces = 0, l_nbVertex = 0;
		CreateBuffers( m_normalsMode, l_nbFaces, l_nbVertex);
	}

	if (m_visible && m_mesh != NULL && m_mesh->IsOk())
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

bool Geometry :: Write( File & p_file)const
{
	Logger::LogMessage( CU_T( "Writing Geometry ") + m_strName);

	bool l_bReturn = p_file.WriteLine( "object " + m_strName + "\n{\n") > 0;

	if (l_bReturn)
	{
		l_bReturn = MovableObject::Write( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\tmesh " + m_mesh->GetName() + "\n") > 0;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "}\n") > 0;
	}

	return l_bReturn;
}

bool Geometry :: Save( File & p_file)const
{
	bool l_bReturn = p_file.Write( m_strName);

	if (l_bReturn)
	{
		l_bReturn = MovableObject::Save( p_file);
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.Write( m_mesh->GetName());
	}

	return l_bReturn;
}

bool Geometry :: Load( File & p_file)
{
	bool l_bReturn = MovableObject::Load( p_file);
	String l_strName;

	if (l_bReturn)
	{
		l_bReturn = p_file.Read( l_strName);
	}

	if (l_bReturn)
	{
		m_mesh = RenderSystem::GetSingletonPtr()->GetSceneManager()->GetMeshManager()->GetElementByName( l_strName);
	}

	return l_bReturn;
}