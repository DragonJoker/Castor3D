#include "Geometry.hpp"

#include "Engine.hpp"
#include "MaterialCache.hpp"
#include "MeshCache.hpp"
#include "ShaderCache.hpp"

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
	Geometry::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Geometry >{ p_tabs }
	{
	}

	bool Geometry::TextWriter::operator()( Geometry const & p_geometry, TextFile & p_file )
	{
		bool l_return{ true };

		if ( p_geometry.GetMesh() )
		{
			Logger::LogInfo( m_tabs + cuT( "Writing Geometry " ) + p_geometry.GetName() );
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "object \"" ) + p_geometry.GetName() + cuT( "\"\n" ) ) > 0
				&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

			if ( l_return )
			{
				l_return = MovableObject::TextWriter{ m_tabs }( p_geometry, p_file );
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tmesh \"" ) + p_geometry.GetMesh()->GetName() + cuT( "\"\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t\timport \"Meshes/" ) + p_geometry.GetMesh()->GetName() + cuT( ".cmsh\"\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tmaterials\n" ) ) > 0
					&& p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0;
			}

			if ( l_return )
			{
				uint16_t l_index{ 0u };

				for ( auto l_submesh : *p_geometry.GetMesh() )
				{
					l_return = p_file.WriteText( m_tabs + cuT( "\t\tmaterial " ) + string::to_string( l_index++ ) + cuT( " \"" ) + p_geometry.GetMaterial( l_submesh )->GetName() + cuT( "\"\n" ) ) > 0;
				}
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
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

			for ( auto l_submesh : *p_mesh )
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
			auto l_it = std::find( l_mesh->begin(), l_mesh->end(), p_submesh );

			if ( l_it != l_mesh->end() )
			{
				m_submeshesMaterials[p_submesh.get()] = p_material;
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
		auto l_it = m_submeshesMaterials.find( p_submesh.get() );

		if ( l_it != m_submeshesMaterials.end() )
		{
			l_return = l_it->second.lock();
		}
		else
		{
			Logger::LogError( cuT( "Geometry::GetMaterial - Wrong submesh" ) );
		}

		return l_return;
	}
}
