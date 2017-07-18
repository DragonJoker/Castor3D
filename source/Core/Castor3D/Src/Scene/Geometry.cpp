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
		bool result{ true };

		if ( p_geometry.GetMesh() )
		{
			Logger::LogInfo( m_tabs + cuT( "Writing Geometry " ) + p_geometry.GetName() );
			result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "object \"" ) + p_geometry.GetName() + cuT( "\"\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Castor::TextWriter< Geometry >::CheckError( result, "Geometry name" );

			if ( result )
			{
				result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_geometry, p_file );
			}

			if ( result )
			{
				result = RenderedObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_geometry, p_file );
			}

			if ( result )
			{
				result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tmesh \"" ) + p_geometry.GetMesh()->GetName() + cuT( "\"\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t\timport \"Meshes/" ) + p_geometry.GetMesh()->GetName() + cuT( ".cmsh\"\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
				Castor::TextWriter< Geometry >::CheckError( result, "Geometry mesh" );
			}

			if ( result )
			{
				result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tmaterials\n" ) ) > 0
						   && p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0;
				Castor::TextWriter< Geometry >::CheckError( result, "Geometry materials" );
			}

			if ( result )
			{
				uint16_t index{ 0u };

				for ( auto submesh : *p_geometry.GetMesh() )
				{
					result = p_file.WriteText( m_tabs + cuT( "\t\tmaterial " ) + string::to_string( index++ ) + cuT( " \"" ) + p_geometry.GetMaterial( *submesh )->GetName() + cuT( "\"\n" ) ) > 0;
					Castor::TextWriter< Geometry >::CheckError( result, "Geometry material" );
				}
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Geometry::Geometry( String const & p_name, Scene & p_scene, SceneNodeSPtr p_sn, MeshSPtr p_mesh )
		: MovableObject{ p_name, p_scene, MovableType::eGeometry, p_sn }
		, m_mesh{ p_mesh }
	{
		if ( p_mesh )
		{
			m_strMeshName = p_mesh->GetName();

			for ( auto submesh : *p_mesh )
			{
				m_submeshesMaterials[submesh.get()] = submesh->GetDefaultMaterial();
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
			MeshSPtr mesh = GetMesh();

			if ( mesh )
			{
				uint32_t nbFaces = mesh->GetFaceCount();
				uint32_t nbVertex = mesh->GetVertexCount();
				p_nbFaces += nbFaces;
				p_nbVertex += nbVertex;
				mesh->ComputeContainers();
				Logger::LogInfo( StringStream() << cuT( "Geometry::CreateBuffers - NbVertex: " ) << nbVertex << cuT( ", NbFaces: " ) << nbFaces );
				m_listCreated = mesh->GetSubmeshCount() > 0;
			}
		}
	}

	void Geometry::Render()
	{
		if ( !m_listCreated )
		{
			uint32_t nbFaces = 0, nbVertex = 0;
			CreateBuffers( nbFaces, nbVertex );
		}
	}

	void Geometry::SetMesh( MeshSPtr p_mesh )
	{
		m_submeshesMaterials.clear();
		m_mesh = p_mesh;

		if ( p_mesh )
		{
			m_strMeshName = p_mesh->GetName();

			for ( auto submesh : *p_mesh )
			{
				m_submeshesMaterials[submesh.get()] = submesh->GetDefaultMaterial();
			}
		}
		else
		{
			m_strMeshName = cuEmptyString;
		}
	}

	void Geometry::SetMaterial( Submesh & p_submesh, MaterialSPtr p_material, bool p_updateSubmesh )
	{
		MeshSPtr mesh = GetMesh();

		if ( mesh )
		{
			auto it = std::find_if( mesh->begin(), mesh->end(), [&p_submesh]( SubmeshSPtr submesh )
			{
				return submesh.get() == &p_submesh;
			} );

			if ( it != mesh->end() )
			{
				bool changed = false;
				auto itSubMat = m_submeshesMaterials.find( &p_submesh );
				MaterialSPtr oldMaterial;
				auto oldCount = p_submesh.GetMaxRefCount();

				if ( itSubMat != m_submeshesMaterials.end() )
				{
					oldMaterial = itSubMat->second.lock();

					if ( oldMaterial != p_material )
					{
						if ( oldMaterial != p_submesh.GetDefaultMaterial() )
						{
							p_submesh.UnRef( oldMaterial );
						}

						itSubMat->second = p_material;
						changed = true;
					}
				}
				else if ( p_material )
				{
					m_submeshesMaterials.emplace( &p_submesh, p_material );
					changed = true;
				}

				if ( changed )
				{
					GetScene()->SetChanged();
					auto count = p_submesh.Ref( p_material ) + 1;

					if ( count > oldCount && p_updateSubmesh )
					{
						if ( count == 1 )
						{
							// We need to update the matrix buffers only.
							GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::ePreRender
								, [this, &p_submesh]()
								{
									p_submesh.ResetMatrixBuffers();
								} ) );
						}
						else
						{
							// We need to update the render nodes afterwards (since the submesh's geometry buffers are now invalid).
							GetScene()->SetChanged();
							GetScene()->GetListener().PostEvent( MakeFunctorEvent( EventType::eQueueRender
								, [this, &p_submesh]()
								{
									// TODO: Find a better way, since this forbids the suppression of RAM storage of the VBO data.
									p_submesh.ResetGpuBuffers();
								} ) );
						}
					}

					if ( p_material->HasEnvironmentMapping() )
					{
						GetScene()->CreateEnvironmentMap( *GetParent() );
					}
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
		MaterialSPtr result;
		auto it = m_submeshesMaterials.find( &p_submesh );

		if ( it != m_submeshesMaterials.end() )
		{
			result = it->second.lock();
		}
		else
		{
			Logger::LogError( cuT( "Geometry::GetMaterial - Wrong submesh" ) );
		}

		return result;
	}
}
