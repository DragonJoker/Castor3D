#include "Geometry.hpp"

#include "Engine.hpp"

#include "Scene/Scene.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Event/Frame/FrameListener.hpp"
#include "Material/Material.hpp"
#include "Mesh/Submesh.hpp"

using namespace castor;

namespace castor3d
{
	Geometry::TextWriter::TextWriter( String const & p_tabs )
		: castor::TextWriter< Geometry >{ p_tabs }
	{
	}

	bool Geometry::TextWriter::operator()( Geometry const & p_geometry, TextFile & p_file )
	{
		bool result{ true };

		if ( p_geometry.getMesh() )
		{
			Logger::logInfo( m_tabs + cuT( "Writing Geometry " ) + p_geometry.getName() );
			result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "object \"" ) + p_geometry.getName() + cuT( "\"\n" ) ) > 0
					   && p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			castor::TextWriter< Geometry >::checkError( result, "Geometry name" );

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
				result = p_file.writeText( m_tabs + cuT( "\tmesh \"" ) + p_geometry.getMesh()->getName() + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Geometry >::checkError( result, "Geometry mesh" );
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\tmaterials\n" ) ) > 0
						   && p_file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
				castor::TextWriter< Geometry >::checkError( result, "Geometry materials" );

				if ( result )
				{
					uint16_t index{ 0u };

					for ( auto submesh : *p_geometry.getMesh() )
					{
						result &= p_file.writeText( m_tabs + cuT( "\t\tmaterial " ) + string::toString( index++ ) + cuT( " \"" ) + p_geometry.getMaterial( *submesh )->getName() + cuT( "\"\n" ) ) > 0;
						castor::TextWriter< Geometry >::checkError( result, "Geometry material" );
					}

					if ( result )
					{
						result = p_file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
					}
				}
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
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
			m_strMeshName = p_mesh->getName();

			for ( auto submesh : *p_mesh )
			{
				m_submeshesMaterials[submesh.get()] = submesh->getDefaultMaterial();
			}
		}
	}

	Geometry::~Geometry()
	{
		cleanup();
	}

	void Geometry::cleanup()
	{
	}

	void Geometry::createBuffers( uint32_t & p_nbFaces, uint32_t & p_nbVertex )
	{
		if ( !m_listCreated )
		{
			cleanup();
			MeshSPtr mesh = getMesh();

			if ( mesh )
			{
				uint32_t nbFaces = mesh->getFaceCount();
				uint32_t nbVertex = mesh->getVertexCount();
				p_nbFaces += nbFaces;
				p_nbVertex += nbVertex;
				mesh->computeContainers();
				Logger::logInfo( StringStream() << cuT( "Geometry::createBuffers - NbVertex: " ) << nbVertex << cuT( ", NbFaces: " ) << nbFaces );
				m_listCreated = mesh->getSubmeshCount() > 0;
			}
		}
	}

	void Geometry::render()
	{
		if ( !m_listCreated )
		{
			uint32_t nbFaces = 0, nbVertex = 0;
			createBuffers( nbFaces, nbVertex );
		}
	}

	void Geometry::setMesh( MeshSPtr p_mesh )
	{
		m_submeshesMaterials.clear();
		m_mesh = p_mesh;

		if ( p_mesh )
		{
			m_strMeshName = p_mesh->getName();

			for ( auto submesh : *p_mesh )
			{
				m_submeshesMaterials[submesh.get()] = submesh->getDefaultMaterial();
			}
		}
		else
		{
			m_strMeshName = cuEmptyString;
		}
	}

	void Geometry::setMaterial( Submesh & p_submesh, MaterialSPtr p_material, bool p_updateSubmesh )
	{
		MeshSPtr mesh = getMesh();

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
				auto oldCount = p_submesh.getMaxRefCount();

				if ( itSubMat != m_submeshesMaterials.end() )
				{
					oldMaterial = itSubMat->second.lock();

					if ( oldMaterial != p_material )
					{
						if ( oldMaterial != p_submesh.getDefaultMaterial() )
						{
							p_submesh.unref( oldMaterial );
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
					getScene()->setChanged();
					auto count = p_submesh.ref( p_material ) + 1;

					if ( count > oldCount && p_updateSubmesh )
					{
						if ( count == 1 )
						{
							// We need to update the matrix buffers only.
							getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender
								, [this, &p_submesh]()
								{
									p_submesh.resetMatrixBuffers();
								} ) );
						}
						else
						{
							// We need to update the render nodes afterwards (since the submesh's geometry buffers are now invalid).
							getScene()->setChanged();
							getScene()->getListener().postEvent( makeFunctorEvent( EventType::eQueueRender
								, [this, &p_submesh]()
								{
									// TODO: Find a better way, since this forbids the suppression of RAM storage of the VBO data.
									p_submesh.resetGpuBuffers();
								} ) );
						}
					}

					if ( p_material->hasEnvironmentMapping() )
					{
						getScene()->createEnvironmentMap( *getParent() );
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

	MaterialSPtr Geometry::getMaterial( Submesh const & p_submesh )const
	{
		MaterialSPtr result;
		auto it = m_submeshesMaterials.find( &p_submesh );

		if ( it != m_submeshesMaterials.end() )
		{
			result = it->second.lock();
		}
		else
		{
			Logger::logError( cuT( "Geometry::getMaterial - Wrong submesh" ) );
		}

		return result;
	}
}
