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
	Geometry::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< Geometry >{ tabs }
	{
	}

	bool Geometry::TextWriter::operator()( Geometry const & geometry, TextFile & file )
	{
		bool result{ true };

		if ( geometry.getMesh() )
		{
			Logger::logInfo( m_tabs + cuT( "Writing Geometry " ) + geometry.getName() );
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "object \"" ) + geometry.getName() + cuT( "\"\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			castor::TextWriter< Geometry >::checkError( result, "Geometry name" );

			if ( result )
			{
				result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( geometry, file );
			}

			if ( result )
			{
				result = RenderedObject::TextWriter{ m_tabs + cuT( "\t" ) }( geometry, file );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmesh \"" ) + geometry.getMesh()->getName() + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< Geometry >::checkError( result, "Geometry mesh" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\tmaterials\n" ) ) > 0
						   && file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
				castor::TextWriter< Geometry >::checkError( result, "Geometry materials" );

				if ( result )
				{
					uint16_t index{ 0u };

					for ( auto submesh : *geometry.getMesh() )
					{
						result &= file.writeText( m_tabs + cuT( "\t\tmaterial " ) + string::toString( index++ ) + cuT( " \"" ) + geometry.getMaterial( *submesh )->getName() + cuT( "\"\n" ) ) > 0;
						castor::TextWriter< Geometry >::checkError( result, "Geometry material" );
					}

					if ( result )
					{
						result = file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
					}
				}
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Geometry::Geometry( String const & name
		, Scene & scene
		, SceneNodeSPtr node
		, MeshSPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry, node }
		, m_mesh{ mesh }
	{
		doUpdateMesh();
	}

	void Geometry::prepare( uint32_t & faceCount
		, uint32_t & vertexCount )
	{
		if ( !m_listCreated )
		{
			MeshSPtr mesh = getMesh();

			if ( mesh )
			{
				uint32_t nbFaces = mesh->getFaceCount();
				uint32_t nbVertex = mesh->getVertexCount();
				faceCount += nbFaces;
				vertexCount += nbVertex;
				mesh->computeContainers();
				Logger::logInfo( StringStream()
					<< cuT( "Geometry [" ) << getName() 
					<< cuT( "] - NbVertex: " ) << nbVertex 
					<< cuT( ", NbFaces: " ) << nbFaces );
				m_listCreated = mesh->getSubmeshCount() > 0;
			}
		}
	}

	void Geometry::setMesh( MeshSPtr mesh )
	{
		m_submeshesMaterials.clear();
		m_mesh = mesh;
		doUpdateMesh();
		doUpdateContainers();
	}

	void Geometry::setMaterial( Submesh & submesh
		, MaterialSPtr material
		, bool updateSubmesh )
	{
		MeshSPtr mesh = getMesh();

		if ( mesh )
		{
			auto it = std::find_if( mesh->begin()
				, mesh->end()
				, [&submesh]( SubmeshSPtr lookup )
				{
					return lookup.get() == &submesh;
				} );
			REQUIRE( it != mesh->end() );

			bool changed = false;
			MaterialSPtr oldMaterial;
			auto itSubMat = m_submeshesMaterials.find( &submesh );

			if ( itSubMat != m_submeshesMaterials.end() )
			{
				MaterialSPtr oldMaterial = itSubMat->second.lock();

				if ( oldMaterial != material )
				{
					itSubMat->second = material;
					changed = true;
				}
			}
			else if ( material )
			{
				m_submeshesMaterials.emplace( &submesh, material );
				changed = true;
			}

			if ( changed )
			{
				submesh.setMaterial( oldMaterial, material, updateSubmesh );

				if ( material->hasEnvironmentMapping() )
				{
					getScene()->createEnvironmentMap( *getParent() );
				}
			}
		}
		else
		{
			CASTOR_EXCEPTION( "No mesh" );
		}
	}

	MaterialSPtr Geometry::getMaterial( Submesh const & submesh )const
	{
		MaterialSPtr result;
		auto it = m_submeshesMaterials.find( &submesh );

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

	void Geometry::updateContainers( SubmeshBoundingBoxList const & boxes )
	{
		m_submeshesBoxes.clear();

		if ( !boxes.empty() )
		{
			m_submeshesBoxes.emplace( boxes[0].first, boxes[0].second );
			m_submeshesSpheres.emplace( boxes[0].first, BoundingSphere{ boxes[0].second } );

			for ( auto i = 1u; i < boxes.size(); ++i )
			{
				m_submeshesBoxes.emplace( boxes[i].first, boxes[i].second );
				m_submeshesSpheres.emplace( boxes[1].first, BoundingSphere{ boxes[1].second } );
			}

			doUpdateContainers();
		}
	}

	BoundingBox const & Geometry::getBoundingBox( Submesh const & submesh )const
	{
		static BoundingBox const dummy;
		auto it = m_submeshesBoxes.find( &submesh );

		if ( it != m_submeshesBoxes.end() )
		{
			return it->second;
		}

		return dummy;
	}

	BoundingSphere const & Geometry::getBoundingSphere( Submesh const & submesh )const
	{
		static BoundingSphere const dummy;
		auto it = m_submeshesSpheres.find( &submesh );

		if ( it != m_submeshesSpheres.end() )
		{
			return it->second;
		}

		return dummy;
	}

	void Geometry::setBoundingBox( Submesh const & submesh
		, BoundingBox const & box )
	{
		m_submeshesBoxes[&submesh] = box;
		m_submeshesSpheres[&submesh] = BoundingSphere{ box };
		doUpdateContainers();
	}

	void Geometry::doUpdateMesh()
	{
		auto mesh = m_mesh.lock();

		if ( mesh )
		{
			m_meshName = mesh->getName();

			for ( auto submesh : *mesh )
			{
				m_submeshesMaterials[submesh.get()] = submesh->getDefaultMaterial();
				m_submeshesBoxes.emplace( submesh.get(), submesh->getBoundingBox() );
				m_submeshesSpheres.emplace( submesh.get(), submesh->getBoundingSphere() );
			}
		}
		else
		{
			m_meshName = cuEmptyString;
		}
	}

	void Geometry::doUpdateContainers()
	{
		if ( !m_submeshesBoxes.empty() )
		{
			auto it = m_submeshesBoxes.begin();
			m_box = it->second;
			++it;

			while ( it != m_submeshesBoxes.end() )
			{
				m_box = m_box.getUnion( it->second );
				++it;
			}

			m_sphere.load( m_box );
		}
	}
}
