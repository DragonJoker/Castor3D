#include "Castor3D/Scene/Geometry.hpp"

#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( castor3d, Geometry )

namespace castor3d
{
	Geometry::Geometry( castor::String const & name
		, Scene & scene
		, SceneNode & node
		, MeshResPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry, node }
		, m_mesh{ mesh }
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		doUpdateMesh();
	}
	
	Geometry::Geometry( castor::String const & name
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
			if ( auto mesh = getMesh() )
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
		m_onMeshChanged = {};
		m_mesh = mesh;
		auto lock( castor::makeUniqueLock( m_mutex ) );
		doUpdateMesh();
		doUpdateContainers();
		bool hasEnvironmentMapping = std::any_of( mesh->begin()
			, mesh->end()
			, []( SubmeshUPtr const & submesh )
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
		, MaterialObs material )
	{
		if ( auto mesh = getMesh() )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );

			if ( submesh.getId() >= mesh->getSubmeshCount() )
			{
				CU_Failure( "Geometry::setMaterial" );
			}
			else
			{
				bool changed = false;
				MaterialObs oldMaterial{};
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
					CU_Require( &submesh.getParent() == mesh );
					m_submeshesMaterials.emplace( &submesh, material );
					changed = true;
				}

				if ( changed )
				{
					if ( oldMaterial )
					{
						getScene()->getRenderNodes().reportPassChange( submesh
							, *this
							, *oldMaterial
							, *material );
						submesh.instantiate( *this, oldMaterial, material, true );

						for ( auto & pass : *oldMaterial )
						{
							auto itPass = m_ids.find( pass.get() );

							if ( itPass != m_ids.end() )
							{
								auto itSubmesh = itPass->second.find( submesh.getId() );

								if ( itSubmesh != itPass->second.end() )
								{
									itPass->second.erase( itSubmesh );

									if ( itPass->second.empty() )
									{
										m_ids.erase( itPass );
									}
								}
							}
						}
					}
					else
					{
						submesh.instantiate( *this, oldMaterial, material, true );
					}

					if ( material->hasEnvironmentMapping() )
					{
						getScene()->addEnvironmentMap( *getParent() );
					}

					markDirty();
				}
			}
		}
		else
		{
			CU_Exception( "No mesh" );
		}
	}

	MaterialObs Geometry::getMaterial( Submesh const & submesh )const
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		MaterialObs result{};

		if ( auto it = m_submeshesMaterials.find( &submesh );
			it != m_submeshesMaterials.end() )
		{
			result = it->second;
		}
		else
		{
			log::error << cuT( "Geometry::getMaterial - Wrong submesh" ) << std::endl;
		}

		return result;
	}

	void Geometry::initContainers()
	{
		m_submeshesBoxes.clear();
		m_submeshesSpheres.clear();

		if ( auto mesh = m_mesh )
		{
			for ( auto & submesh : *mesh )
			{
				m_submeshesBoxes.emplace( submesh.get(), submesh->getBoundingBox() );
				m_submeshesSpheres.emplace( submesh.get(), submesh->getBoundingSphere() );
			}

			doUpdateContainers();
		}
	}

	void Geometry::updateContainers( SubmeshBoundingBoxList const & boxes )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_submeshesBoxes.clear();
		m_submeshesSpheres.clear();

		if ( !boxes.empty() )
		{
			m_submeshesBoxes.try_emplace( boxes[0].first, boxes[0].second );
			m_submeshesSpheres.try_emplace( boxes[0].first, boxes[0].second );

			for ( auto i = 1u; i < boxes.size(); ++i )
			{
				m_submeshesBoxes.try_emplace( boxes[i].first, boxes[i].second );
				m_submeshesSpheres.try_emplace( boxes[i].first, boxes[i].second );
			}

			doUpdateContainers();
		}
	}

	castor::BoundingBox const & Geometry::getBoundingBox( Submesh const & submesh )const
	{
		static castor::BoundingBox const dummy;
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( auto it = m_submeshesBoxes.find( &submesh );
			it != m_submeshesBoxes.end() )
		{
			return it->second;
		}

		return dummy;
	}

	castor::BoundingSphere const & Geometry::getBoundingSphere( Submesh const & submesh )const
	{
		static castor::BoundingSphere const dummy;
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( auto it = m_submeshesSpheres.find( &submesh );
			it != m_submeshesSpheres.end() )
		{
			return it->second;
		}

		return dummy;
	}

	void Geometry::setBoundingBox( Submesh const & submesh
		, castor::BoundingBox const & box )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_submeshesBoxes[&submesh] = box;
		m_submeshesSpheres[&submesh] = castor::BoundingSphere{ box };
		doUpdateContainers();
	}

	uint32_t Geometry::getId( Pass const & pass
		, Submesh const & submesh )const
	{
		if ( auto itPass = m_ids.find( &pass );
			itPass != m_ids.end() )
		{
			auto it = itPass->second.find( submesh.getId() );
			return it == itPass->second.end() ? 0u : it->second.first;
		}

		return 0u;
	}

	SubmeshRenderNode * Geometry::getRenderNode( Pass const & pass
		, Submesh const & submesh )const
	{
		if ( auto itPass = m_ids.find( &pass );
			itPass != m_ids.end() )
		{
			auto it = itPass->second.find( submesh.getId() );
			return it == itPass->second.end() ? nullptr : it->second.second;
		}

		return nullptr;
	}

	void Geometry::setId( Pass const & pass
		, Submesh const & submesh
		, SubmeshRenderNode * node
		, uint32_t id )noexcept
	{
		auto itPass = m_ids.try_emplace( &pass ).first;
		itPass->second[submesh.getId()] = { id, node };
	}

	size_t Geometry::getHash( Pass const & pass
		, Submesh const & submesh )const noexcept
	{
		return submesh.isDynamic()
			? std::hash< Geometry const * >{}( this )
			: std::hash< uint32_t >{}( pass.getHash() );
	}

	castor::Matrix4x4f Geometry::getGlobalTransform()const
	{
		auto result = getParent()->getDerivedTransformationMatrix();

		if ( auto skeleton = getMesh()->getSkeleton() )
		{
			result = result * skeleton->getGlobalInverseTransform();
		}

		return result;
	}

	void Geometry::doUpdateMesh()
	{
		m_submeshesMaterials.clear();

		if ( auto mesh = m_mesh )
		{
			m_meshName = mesh->getName();
			m_onMeshChanged = mesh->onChange.connect( [this]( Mesh const & mesh )
				{
					markDirty();
				} );

			for ( auto & submesh : *mesh )
			{
				CU_Require( &submesh->getParent() == mesh );
				auto material = submesh->getDefaultMaterial();
				m_submeshesMaterials.emplace( submesh.get(), material );

				if ( material )
				{
					submesh->instantiate( *this, {}, material, true );
				}
			}

			initContainers();
		}
		else
		{
			m_meshName = castor::cuEmptyString;
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
