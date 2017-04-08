#include "Geometry.hpp"

#include "Engine.hpp"

#include "Scene/Scene.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Material/Material.hpp"
#include "Mesh/Submesh.hpp"

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
			Castor::TextWriter< Geometry >::CheckError( l_return, "Geometry name" );

			if ( l_return )
			{
				l_return = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_geometry, p_file );
			}

			if ( l_return )
			{
				l_return = RenderedObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_geometry, p_file );
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tmesh \"" ) + p_geometry.GetMesh()->GetName() + cuT( "\"\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t\timport \"Meshes/" ) + p_geometry.GetMesh()->GetName() + cuT( ".cmsh\"\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
				Castor::TextWriter< Geometry >::CheckError( l_return, "Geometry mesh" );
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tmaterials\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0;
				Castor::TextWriter< Geometry >::CheckError( l_return, "Geometry materials" );
			}

			if ( l_return )
			{
				uint16_t l_index{ 0u };

				for ( auto l_submesh : *p_geometry.GetMesh() )
				{
					l_return = p_file.WriteText( m_tabs + cuT( "\t\tmaterial " ) + string::to_string( l_index++ ) + cuT( " \"" ) + p_geometry.GetMaterial( *l_submesh )->GetName() + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< Geometry >::CheckError( l_return, "Geometry material" );
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
		: MovableObject{ p_name, p_scene, MovableType::eGeometry, p_sn }
		, m_mesh{ p_mesh }
	{
		if ( p_mesh )
		{
			m_strMeshName = p_mesh->GetName();

			for ( auto l_submesh : *p_mesh )
			{
				m_submeshesMaterials[l_submesh.get()] = l_submesh->GetDefaultMaterial();
			}
		}
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
				m_submeshesMaterials[l_submesh.get()] = l_submesh->GetDefaultMaterial();
			}
		}
		else
		{
			m_strMeshName = cuEmptyString;
		}
	}

	void Geometry::SetMaterial( Submesh & p_submesh, MaterialSPtr p_material )
	{
		MeshSPtr l_mesh = GetMesh();

		if ( l_mesh )
		{
			auto l_it = std::find_if( l_mesh->begin(), l_mesh->end(), [&p_submesh]( SubmeshSPtr l_submesh )
			{
				return l_submesh.get() == &p_submesh;
			} );

			if ( l_it != l_mesh->end() )
			{
				auto l_pair = m_submeshesMaterials.insert( { &p_submesh, p_material } );

				if ( !l_pair.second && l_pair.first->second.lock() != p_submesh.GetDefaultMaterial() )
				{
					p_submesh.UnRef( p_material );
				}

				l_pair.first->second = p_material;
				auto l_count = p_submesh.Ref( p_material );

				if ( l_count >= 1 )
				{
					if ( !p_submesh.HasMatrixBuffer() && l_count == 1 )
					{
						// We need to update the render nodes afterwards (since the submesh's geometry buffers are now invalid).
						GetScene()->SetChanged();
						GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::eQueueRender, [this, &p_submesh]()
						{
							// TODO: Find a better way, since this forbids the suppression of RAM storage of the VBO data.
							p_submesh.ResetGpuBuffers();
						} ) );
					}
					else if ( p_submesh.HasMatrixBuffer() && l_count > 1 )
					{
						// We need to update the matrix buffers only.
						GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender, [this, &p_submesh]()
						{
							p_submesh.ResetMatrixBuffers();
						} ) );
					}
				}

				if ( p_material->HasEnvironmentMapping() )
				{
					GetScene()->CreateEnvironmentMap( *GetParent() );
				}
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

	MaterialSPtr Geometry::GetMaterial( Submesh const & p_submesh )const
	{
		MaterialSPtr l_return;
		auto l_it = m_submeshesMaterials.find( &p_submesh );

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
