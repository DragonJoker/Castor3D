/*******************************************************************************
 Auteur:  Sylvain DOREMUS & Loïc DASSONVILLE
 But:    Moteur 3D

 Fichier: Geometry.h - Geometry.cpp

 Desc:  Classe g‚rant les figures primitives (cylindres, cônes, sphšres...)
        Contient les faces de la figure.

*******************************************************************************/
#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/primitives/Geometry.h"

#include "geometry/basic/Face.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/mesh/MeshManager.h"
#include "geometry/basic/SmoothingGroup.h"
#include "scene/SceneNode.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "render_system/RenderSystem.h"
#include "render_system/MaterialRenderer.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"
#include "shader/ShaderProgram.h"
#include "main/Root.h"
#include "material/Material.h"
#include "material/TextureUnit.h"
#include "material/Pass.h"

#include "Log.h"

using namespace Castor3D;

Geometry :: Geometry ( Mesh * p_mesh, SceneNode * p_sn, const String & p_name)
	:	MovableObject( p_sn, p_name),
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

void Geometry :: CreateBuffers( NormalsMode p_nm, size_t & p_nbFaces, size_t & p_nbVertex)
{
	Log::LogMessage( C3D_T( "Geometry :: CreateBuffers - %d"), p_nm);
	Cleanup();

	if (m_mesh == NULL)
	{
		return;
	}

	size_t l_nbFaces = m_mesh->GetNbFaces();
	size_t l_nbVertex = m_mesh->GetNbVertex();

	p_nbFaces += l_nbFaces;
	p_nbVertex += l_nbVertex;

	m_mesh->CreateNormalsBuffers( p_nm);

	Cleanup();

	unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_mesh->GetNbSubmeshes());

	for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
	{
		m_mesh->GetSubmesh( i)->GetRenderer()->GetTrianglesNormals()->Initialise();
		m_mesh->GetSubmesh( i)->GetRenderer()->GetTrianglesTangents()->Initialise();
		m_mesh->GetSubmesh( i)->GetRenderer()->GetLinesNormals()->Initialise();
	}
	m_listCreated = m_mesh->GetNbSubmeshes() > 0;

	m_mesh->CreateBuffers();

	m_normalsMode = p_nm;
	Log::LogMessage( C3D_T( "Geometry :: CreateBuffers - NbVertex : %d, NbFaces : %d"), l_nbVertex, l_nbFaces);
	m_listCreated = m_mesh->GetNbSubmeshes() > 0;
	m_dirty = false;
}



void Geometry :: Render( DrawType p_displayMode)
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
		m_orientation.ToRotationMatrix( m_matrix);

		RenderSystem::GetSingletonPtr()->ApplyTransformations( m_center, m_matrix);

		unsigned int l_nbSubmeshes = static_cast <unsigned int>( m_mesh->GetNbSubmeshes());
		Submesh * l_submesh;
		PassPtrArray l_passes;
		Pass * l_pass;
		SubmeshRenderer * l_submeshRenderer;
		TextureUnitPtrArray l_textures;

		for (unsigned int i = 0 ; i < l_nbSubmeshes ; i++)
		{
			l_submesh = m_mesh->GetSubmesh( i);
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
		}

		RenderSystem::GetSingletonPtr()->RemoveTransformations();
	}
}



bool Geometry :: Write( FileIO * p_pFile)const
{
	Log::LogMessage( C3D_T( "Writing Geometry ") + m_name);

	bool l_bReturn = p_pFile->WriteLine( "object " + m_name + "\n{\n");

	if ( ! MovableObject::Write( p_pFile))
	{
		return false;
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile->WriteLine( "\tmesh\n\t{\n\t\ttype custom\n\t\tfile " + m_mesh->GetName() + ".csmesh\n\t}\n");
	}

	if (l_bReturn)
	{
		l_bReturn = p_pFile->WriteLine( "}\n");
	}

	return true;
}

void Geometry :: Subdivide( unsigned int p_index, SubdivisionMode p_mode)
{
	m_visible = false;
	m_dirty = m_mesh->Subdivide( p_index, p_mode);
	m_visible = true;
}


