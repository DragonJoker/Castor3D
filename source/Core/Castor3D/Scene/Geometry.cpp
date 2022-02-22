#include "Castor3D/Scene/Geometry.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	Geometry::Geometry( String const & name
		, Scene & scene
		, SceneNode & node
		, MeshResPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry, node }
		, m_mesh{ mesh }
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		doUpdateMesh();
	}
	
	Geometry::Geometry( String const & name
		, Scene & scene
		, MeshResPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry }
		, m_mesh{ mesh }
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		doUpdateMesh();
	}

	void Geometry::prepare( uint32_t & faceCount
		, uint32_t & vertexCount )
	{
		if ( !m_listCreated )
		{
			auto mesh = getMesh().lock();

			if ( mesh )
			{
				uint32_t nbFaces = mesh->getFaceCount();
				uint32_t nbVertex = mesh->getVertexCount();
				faceCount += nbFaces;
				vertexCount += nbVertex;
				mesh->computeContainers();
				m_listCreated = mesh->getSubmeshCount() > 0;
			}
		}
	}

	void Geometry::setMesh( MeshResPtr mesh )
	{
		m_mesh = mesh;
		auto lock( castor::makeUniqueLock( m_mutex ) );
		doUpdateMesh();
		doUpdateContainers();
		bool hasEnvironmentMapping = std::any_of( mesh.lock()->begin()
			, mesh.lock()->end()
			, []( SubmeshSPtr const & submesh )
			{
				return submesh->getDefaultMaterial()
					? submesh->getDefaultMaterial()->hasEnvironmentMapping()
					: false;
			} );

		if ( hasEnvironmentMapping )
		{
			getScene()->addEnvironmentMap( *getParent() );
		}
	}

	void Geometry::setMaterial( Submesh & submesh
		, MaterialRPtr material
		, bool updateSubmesh )
	{
		if ( auto mesh = getMesh().lock() )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			CU_Require( submesh.getId() < mesh->getSubmeshCount() );
			bool changed = false;
			MaterialRPtr oldMaterial{};
			auto itSubMat = m_submeshesMaterials.find( &submesh );

			if ( itSubMat != m_submeshesMaterials.end() )
			{
				oldMaterial = itSubMat->second;

				if ( oldMaterial != material )
				{
					itSubMat->second = material;
					changed = true;
				}
			}
			else if ( material )
			{
				oldMaterial = submesh.getDefaultMaterial();
				CU_Require( &submesh.getParent() == mesh.get() );
				m_submeshesMaterials.emplace( &submesh, material );
				changed = true;
			}

			if ( changed )
			{
				submesh.setMaterial( oldMaterial, material, updateSubmesh );

				if ( material->hasEnvironmentMapping() )
				{
					getScene()->addEnvironmentMap( *getParent() );
				}

				onMaterialChanged( *this, submesh, oldMaterial, material );
			}
		}
		else
		{
			CU_Exception( "No mesh" );
		}
	}

	MaterialRPtr Geometry::getMaterial( Submesh const & submesh )const
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		MaterialRPtr result{};
		auto it = m_submeshesMaterials.find( &submesh );

		if ( it != m_submeshesMaterials.end() )
		{
			result = it->second;
		}
		else
		{
			log::error << cuT( "Geometry::getMaterial - Wrong submesh" ) << std::endl;
		}

		return result;
	}

	void Geometry::updateContainers( SubmeshBoundingBoxList const & boxes )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_submeshesBoxes.clear();
		m_submeshesSpheres.clear();

		if ( !boxes.empty() )
		{
			m_submeshesBoxes.emplace( boxes[0].first, boxes[0].second );
			m_submeshesSpheres.emplace( boxes[0].first, BoundingSphere{ boxes[0].second } );

			for ( auto i = 1u; i < boxes.size(); ++i )
			{
				m_submeshesBoxes.emplace( boxes[i].first, boxes[i].second );
				m_submeshesSpheres.emplace( boxes[i].first, BoundingSphere{ boxes[i].second } );
			}

			doUpdateContainers();
		}
	}

	BoundingBox const & Geometry::getBoundingBox( Submesh const & submesh )const
	{
		static BoundingBox const dummy;
		auto lock( castor::makeUniqueLock( m_mutex ) );
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
		auto lock( castor::makeUniqueLock( m_mutex ) );
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
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_submeshesBoxes[&submesh] = box;
		m_submeshesSpheres[&submesh] = BoundingSphere{ box };
		doUpdateContainers();
	}

	uint32_t Geometry::getId( Submesh const & submesh )const
	{
		auto it = m_id.find( submesh.getId() );
		return it == m_id.end() ? 0u : it->second;
	}

	void Geometry::setId( Submesh const & submesh, uint32_t id )
	{
		m_id[submesh.getId()] = id;
	}

	void Geometry::doUpdateMesh()
	{
		m_submeshesMaterials.clear();
		m_submeshesBoxes.clear();
		m_submeshesSpheres.clear();

		if ( auto mesh = m_mesh.lock() )
		{
			m_meshName = mesh->getName();

			for ( auto submesh : *mesh )
			{
				CU_Require( &submesh->getParent() == mesh.get() );
				m_submeshesMaterials.emplace( submesh.get(), submesh->getDefaultMaterial() );
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
