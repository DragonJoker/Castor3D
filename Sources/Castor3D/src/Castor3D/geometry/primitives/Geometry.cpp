#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/primitives/Geometry.h"
#include "geometry/basic/Face.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/basic/SmoothingGroup.h"
#include "scene/NodeBase.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"
#include "shader/ShaderProgram.h"
#include "main/Root.h"
#include "material/Material.h"
#include "material/TextureUnit.h"
#include "material/Pass.h"

using namespace Castor3D;

Geometry :: Geometry ( MeshPtr p_mesh, GeometryNodePtr p_sn, const String & p_name)
	:	MovableObject( (NodeBase *)p_sn.get(), p_name),
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

	if ( ! m_mesh.null())
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



void Geometry :: Render( eDRAW_TYPE p_displayMode)
{
	if (m_dirty)
	{
		size_t l_nbFaces = 0, l_nbVertex = 0;
		CreateBuffers( m_normalsMode, l_nbFaces, l_nbVertex);
	}

	if (m_visible && ! m_mesh.null() && m_mesh->IsOk())
	{
		if ( ! m_listCreated)
		{
			return;
		}
		m_orientation.ToRotationMatrix( m_matrix);

		RenderSystem::GetSingletonPtr()->ApplyTransformations( m_center, m_matrix);

		unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_mesh->GetNbSubmeshes());
		SubmeshPtr l_submesh;

		for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
		{
			l_submesh = m_mesh->GetSubmesh( i);
			l_submesh->Render( p_displayMode);
/*
			l_submeshRenderer = l_submesh->GetRenderer();
			l_passes = l_submesh->GetMaterial()->GetPasses();

			for (size_t j = 0 ; j < l_passes.size() ; j++)
			{
				l_pass = l_passes[j];
				l_textures = l_pass->GetTextureUnits();
				l_pass->Apply( l_submesh, p_displayMode);
				l_submeshRenderer->Draw( p_displayMode, l_pass);
				l_pass->Remove();
			}
*/
		}

		RenderSystem::GetSingletonPtr()->RemoveTransformations();
	}
}



bool Geometry :: Write( File & p_file)const
{
	Logger::LogMessage( CU_T( "Writing Geometry ") + m_strName);

	bool l_bReturn = p_file.WriteLine( "object " + m_strName + "\n{\n");

	if ( ! MovableObject::Write( p_file))
	{
		return false;
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "\tmesh\n\t{\n\t\ttype custom\n\t\tfile " + m_mesh->GetName() + ".cmsh\n\t}\n");
	}

	if (l_bReturn)
	{
		l_bReturn = p_file.WriteLine( "}\n");
	}

	return true;
}

void Geometry :: Subdivide( unsigned int p_index, SubdivisionMode p_mode)
{
	m_visible = false;
	m_dirty = m_mesh->Subdivide( p_index, p_mode);
	m_visible = true;
}

const Matrix4x4r & Geometry :: GetRotationMatrix()
{
	m_orientation.ToRotationMatrix( m_matrix);
	return m_matrix;
}
