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
		, MeshSPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry, node }
		, m_mesh{ mesh }
	{
		doUpdateMesh();
	}
	
	Geometry::Geometry( String const & name
		, Scene & scene
		, MeshSPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry }
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
				log::info << cuT( "Geometry [" ) << getName()
					<< cuT( "] - NbVertex: " ) << nbVertex
					<< cuT( ", NbFaces: " ) << nbFaces << std::endl;
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
		bool hasEnvironmentMapping = std::any_of( mesh->begin()
			, mesh->end()
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
			CU_Require( it != mesh->end() );

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
			log::error << cuT( "Geometry::getMaterial - Wrong submesh" ) << std::endl;
		}

		return result;
	}

	void Geometry::updateContainers( SubmeshBoundingBoxList const & boxes )
	{
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
		m_submeshesBoxes.clear();
		m_submeshesSpheres.clear();

		if ( mesh )
		{
			m_meshName = mesh->getName();

			for ( auto submesh : *mesh )
			{
				submesh->getInstantiation().ref( submesh->getDefaultMaterial() );
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
