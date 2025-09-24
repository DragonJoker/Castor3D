#include "Castor3D/Scene/Geometry.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, Geometry )

namespace c3d
{
	namespace object
	{

		static CU_ImplementAttributeParserBlock( parserParent, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				SceneNodeRPtr parent;

				if ( name == Scene::ObjectRootNode )
				{
					parent = blockContext->scene->scene->getObjectRootNode();
				}
				else if ( name == Scene::CameraRootNode )
				{
					parent = blockContext->scene->scene->getCameraRootNode();
				}
				else if ( name == Scene::RootNode )
				{
					parent = blockContext->scene->scene->getRootNode();
				}
				else
				{
					parent = blockContext->scene->scene->findSceneNode( name );
				}

				if ( parent )
				{
					parent->attachObject( *blockContext->geometry );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaterial, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->geometry->getMesh() )
				{
					auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
					auto material = getEngine( *blockContext )->tryFindMaterial( name );

					if ( material )
					{
						for ( auto const & submesh : *blockContext->geometry->getMesh() )
						{
							blockContext->geometry->setMaterial( *submesh, material );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserMesh, ObjectContext, MeshContext )
		{
			if ( blockContext->geometry )
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				auto scene = blockContext->geometry->getScene();
				newBlockContext->geometry = blockContext;
				newBlockContext->scene = blockContext->scene;
				newBlockContext->root = blockContext->scene->root;
				newBlockContext->mesh = scene->tryFindMesh( name );

				if ( !newBlockContext->mesh )
				{
					newBlockContext->ownMesh = scene->createMesh( name, *scene );
					newBlockContext->mesh = newBlockContext->ownMesh.get();
				}
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMesh )

		static CU_ImplementAttributeParserBlock( parserMaterials, ObjectContext )
		{
			// Only push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eObjectMaterials, blockContext )

		static CU_ImplementAttributeParserBlock( parserCastShadows, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->geometry->setShadowCaster( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserReceivesShadows, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->geometry->setShadowReceiver( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCullable, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->geometry->setCullable( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, ObjectContext )
		{
			blockContext->parentNode = nullptr;
			log::info << "Loaded geometry [" << blockContext->geometry->getName() << "]" << std::endl;

			if ( blockContext->ownGeometry )
			{
				blockContext->scene->scene->addGeometry( c3d::move( blockContext->ownGeometry ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserMaterialsMaterial, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->geometry->getMesh() )
			{
				if ( auto name = getPrefixedName( params[1]->get< String >(), *blockContext );
					auto material = getEngine( *blockContext )->tryFindMaterial( name ) )
				{
					uint16_t index;

					if ( blockContext->geometry->getMesh()->getSubmeshCount() > params[0]->get( index ) )
					{
						auto submesh = blockContext->geometry->getMesh()->getSubmesh( index );
						blockContext->geometry->setMaterial( *submesh, material );
					}
					else
					{
						CU_ParsingError( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMaterialsEnd, ObjectContext )
		{
			// Only push the block
		}
		CU_EndAttributePop()
	}

	Geometry::Geometry( String const & name
		, Scene & scene
		, SceneNode & node
		, MeshResPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry, node }
		, m_mesh{ mesh }
	{
		auto lock( makeUniqueLock( m_mutex ) );
		doUpdateMesh();
	}
	
	Geometry::Geometry( String const & name
		, Scene & scene
		, MeshResPtr mesh )
		: MovableObject{ name, scene, MovableType::eGeometry }
		, m_mesh{ mesh }
	{
		auto lock( makeUniqueLock( m_mutex ) );
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
		auto lock( makeUniqueLock( m_mutex ) );
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
			auto lock( makeUniqueLock( m_mutex ) );

			if ( submesh.getId() >= mesh->getSubmeshCount() )
			{
				CU_Failure( "Geometry::setMaterial" );
			}
			else
			{
				bool changed = false;
				MaterialObs oldMaterial{};

				if ( auto itSubMat = m_submeshesMaterials.find( &submesh );
					itSubMat != m_submeshesMaterials.end() )
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

						for ( auto const & pass : *oldMaterial )
						{
							auto itPass = m_ids.find( pass.get() );

							if ( itPass != m_ids.end() )
							{
								auto itSubmesh = itPass->second.find( submesh.getId() );

								if ( itSubmesh != itPass->second.end() )
								{
									itPass->second.erase( itSubmesh );
									if ( itPass->second.empty() )
										m_ids.erase( itPass );
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
		auto lock( makeUniqueLock( m_mutex ) );
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
			for ( auto const & submesh : *mesh )
			{
				m_submeshesBoxes.try_emplace( submesh.get(), submesh->getBoundingBox() );
				m_submeshesSpheres.try_emplace( submesh.get(), submesh->getBoundingSphere() );
			}

			doUpdateContainers();
		}
	}

	void Geometry::updateContainers( SubmeshBoundingBoxList const & boxes )
	{
		auto lock( makeUniqueLock( m_mutex ) );
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

	BoundingBox const & Geometry::getBoundingBox( Submesh const & submesh )const
	{
		static BoundingBox const dummy;
		auto lock( makeUniqueLock( m_mutex ) );

		if ( auto it = m_submeshesBoxes.find( &submesh );
			it != m_submeshesBoxes.end() )
		{
			return it->second;
		}

		return dummy;
	}

	BoundingSphere const & Geometry::getBoundingSphere( Submesh const & submesh )const
	{
		static BoundingSphere const dummy;
		auto lock( makeUniqueLock( m_mutex ) );

		if ( auto it = m_submeshesSpheres.find( &submesh );
			it != m_submeshesSpheres.end() )
		{
			return it->second;
		}

		return dummy;
	}

	void Geometry::setBoundingBox( Submesh const & submesh
		, BoundingBox const & box )
	{
		auto lock( makeUniqueLock( m_mutex ) );
		m_submeshesBoxes[&submesh] = box;
		m_submeshesSpheres[&submesh] = BoundingSphere{ box };
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

	Matrix4x4f Geometry::getGlobalTransform()const
	{
		auto result = getParent()->getDerivedTransformationMatrix();

		if ( auto skeleton = getMesh()->getSkeleton() )
		{
			result = result * skeleton->getGlobalInverseTransform();
		}

		return result;
	}

	void Geometry::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< ObjectContext > objectCtx{ result, CSCNSection::eObject, CSCNSection::eScene };
		BlockParserContextT< ObjectContext > materialsCtx{ result, CSCNSection::eObjectMaterials, CSCNSection::eObject };

		objectCtx.addParser( cuT( "parent" ), object::parserParent, { makeParameter< ParameterType::eName >() } );
		objectCtx.addParser( cuT( "material" ), object::parserMaterial, { makeParameter< ParameterType::eName >() } );
		objectCtx.addParser( cuT( "cast_shadows" ), object::parserCastShadows, { makeParameter< ParameterType::eBool >() } );
		objectCtx.addParser( cuT( "receive_shadows" ), object::parserReceivesShadows, { makeParameter< ParameterType::eBool >() } );
		objectCtx.addParser( cuT( "cullable" ), object::parserCullable, { makeParameter< ParameterType::eBool >() } );
		objectCtx.addPushParser( cuT( "mesh" ), CSCNSection::eMesh, object::parserMesh, { makeParameter< ParameterType::eName >() } );
		objectCtx.addPushParser( cuT( "materials" ), CSCNSection::eObjectMaterials, object::parserMaterials );
		objectCtx.addPopParser( cuT( "}" ), object::parserEnd );

		materialsCtx.addParser( cuT( "material" ), object::parserMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
		materialsCtx.addPopParser( cuT( "}" ), object::parserMaterialsEnd );
	}

	void Geometry::doUpdateMesh()
	{
		m_submeshesMaterials.clear();

		if ( auto mesh = m_mesh )
		{
			m_meshName = mesh->getName();
			m_onMeshChanged = mesh->onChange.connect( [this]( Mesh const & )
				{
					markDirty();
				} );

			for ( auto const & submesh : *mesh )
			{
				CU_Require( &submesh->getParent() == mesh );
				auto material = submesh->getDefaultMaterial();
				m_submeshesMaterials.emplace( submesh.get(), material );

				if ( material )
					submesh->instantiate( *this, {}, material, true );
			}

			initContainers();
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
