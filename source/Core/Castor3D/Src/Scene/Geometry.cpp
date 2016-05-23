#include "Geometry.hpp"

#include "Engine.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "ShaderManager.hpp"

#include "Scene.hpp"
#include "SceneNode.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Mesh/Submesh.hpp"

#include <Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	bool Geometry::TextLoader::operator()( Geometry const & p_geometry, TextFile & p_file )
	{
		Logger::LogInfo( cuT( "Writing Geometry " ) + p_geometry.GetName() );
		bool l_return = p_file.WriteText( cuT( "\tobject \"" ) + p_geometry.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = MovableObject::TextLoader()( p_geometry, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\tmesh\n" ) ) > 0;
			l_return = p_file.WriteText( cuT( "\t\t{\n" ) ) > 0;
			l_return = p_file.WriteText( cuT( "\t\t\timport " ) + p_geometry.GetMesh()->GetName() + cuT( ".cmsh" ) ) > 0;
			l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\tmaterials\n\t\t{\n" ) ) > 0;
		}

		if ( l_return )
		{
			for ( auto && l_submesh : *p_geometry.GetMesh() )
			{
				l_return = p_file.WriteText( cuT( "\t\t\t\tmaterial \"" ) + p_geometry.GetMaterial( l_submesh )->GetName() + cuT( "\"\n" ) ) > 0;
			}
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Geometry::Geometry( String const & p_name, Scene & p_scene, SceneNodeSPtr p_sn, MeshSPtr p_mesh )
		:	MovableObject( p_name, p_scene, eMOVABLE_TYPE_GEOMETRY, p_sn )
		,	m_mesh( p_mesh )
		,	m_changed( true )
		,	m_listCreated( false )
		,	m_visible( true )
	{
	}

	Geometry::~Geometry()
	{
		Cleanup();
	}

	void Geometry::Cleanup()
	{
	}

	void Geometry::CreateBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex )
	{
		if ( !m_listCreated )
		{
			Cleanup();
			MeshSPtr l_mesh = GetMesh();

			if ( l_mesh )
			{
				uint32_t l_nbFaces = l_mesh->GetFaceCount();
				uint32_t l_nbVertex = l_mesh->GetVertexCount();
				p_nbFaces += l_nbFaces;
				p_nbVertex += l_nbVertex;
				l_mesh->ComputeContainers();
				Logger::LogInfo( StringStream() << cuT( "Geometry::CreateBuffers - NbVertex: " ) << l_nbVertex << cuT( ", NbFaces: " ) << l_nbFaces );
				m_listCreated = l_mesh->GetSubmeshCount() > 0;
			}
		}
	}

	void Geometry::Render()
	{
		if ( !m_listCreated )
		{
			uint32_t l_nbFaces = 0, l_nbVertex = 0;
			CreateBuffers( l_nbFaces, l_nbVertex );
		}
	}

	void Geometry::SetMesh( MeshSPtr p_mesh )
	{
		m_submeshesMaterials.clear();
		m_mesh = p_mesh;

		if ( p_mesh )
		{
			m_strMeshName = p_mesh->GetName();

			for ( auto && l_submesh : *p_mesh )
			{
				SetMaterial( l_submesh, l_submesh->GetDefaultMaterial() );
			}
		}
		else
		{
			m_strMeshName = cuEmptyString;
		}
	}

	void Geometry::SetMaterial( SubmeshSPtr p_submesh, MaterialSPtr p_material )
	{
		MeshSPtr l_mesh = GetMesh();

		if ( l_mesh )
		{
			auto && l_it = std::find( l_mesh->begin(), l_mesh->end(), p_submesh );

			if ( l_it != l_mesh->end() )
			{
				m_submeshesMaterials[p_submesh] = p_material;
			}
			else
			{
				CASTOR_EXCEPTION( "Couldn't retrive the submesh in mesh's submeshes" );
			}
		}
		else
		{
			CASTOR_EXCEPTION( "No mesh" );
		}
	}

	MaterialSPtr Geometry::GetMaterial( SubmeshSPtr p_submesh )const
	{
		MaterialSPtr l_return;
		std::map< SubmeshSPtr, MaterialSPtr >::const_iterator l_it = m_submeshesMaterials.find( p_submesh );

		if ( l_it != m_submeshesMaterials.end() )
		{
			l_return = l_it->second;
		}
		else
		{
			Logger::LogError( cuT( "Geometry::GetMaterial - Wrong submesh" ) );
		}

		return l_return;
	}
}
