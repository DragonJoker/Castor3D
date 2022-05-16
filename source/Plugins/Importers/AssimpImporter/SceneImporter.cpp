#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/SceneImporter.hpp"
#include "AssimpImporter/AssimpHelpers.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Animation/AnimatedSkeleton.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>

namespace c3d_assimp
{
	namespace scenes
	{
		static castor::Matrix4x4f getTranslation( aiMatrix4x4 const & transform )
		{
			aiQuaternion quat;
			aiVector3D tran;
			transform.DecomposeNoScaling( quat, tran );

			castor::Matrix4x4f result;
			castor::matrix::setTranslate( result, fromAssimp( tran ) );
			return result;
		}
	}

	using SceneNodeAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;

	SceneImporter::SceneImporter( AssimpImporter & importer
		, SkeletonImporter const & skeletons
		, castor3d::SceneNodePtrArray & nodes
		, castor3d::GeometryPtrStrMap & geometries )
		: m_importer{ importer }
		, m_skeletons{ skeletons }
		, m_nodes{ nodes }
		, m_geometries{ geometries }
	{
	}

	void SceneImporter::import( aiScene const & aiScene
		, castor3d::Scene & scene
		, std::map< uint32_t, MeshData * > const & meshes )
	{
		doProcessNodes( aiScene
			, *aiScene.mRootNode
			, scene
			, meshes );
		doProcessAnimGroups();
	}

	void SceneImporter::import( aiScene const & aiScene
		, MeshIndices const & meshes )
	{
		doTransformMesh( *aiScene.mRootNode
			, meshes );
	}

	void SceneImporter::importAnims()
	{
		doProcessAnimGroups();
	}

	void SceneImporter::doTransformMesh( aiNode const & aiNode
		, MeshIndices const & meshes
		, aiMatrix4x4 transformAcc )
	{
		transformAcc = transformAcc * aiNode.mTransformation;

		for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
		{
			auto it = meshes.find( aiMeshIndex );

			if ( it != meshes.end() )
			{
				auto transform = ( it->second->hasComponent( castor3d::BonesComponent::Name )
					? scenes::getTranslation( transformAcc )
					: fromAssimp( transformAcc ) );

				for ( auto & vertex : it->second->getPositions() )
				{
					vertex = transform * vertex;
				}

				auto indexMapping = it->second->getIndexMapping();
				indexMapping->computeNormals( true );
			}
		}

		for ( auto child : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doTransformMesh( *child, meshes, transformAcc );
		}
	}

	void SceneImporter::doProcessAnimGroups()
	{
		for ( auto geometryIt : m_geometries )
		{
			auto geometry = geometryIt.second;
			auto & scene = *geometry->getScene();
			auto mesh = geometry->getMesh().lock();
			auto node = geometry->getParent();
			auto skeleton = mesh->getSkeleton();

			if ( node->hasAnimation() )
			{
				auto animGroup = scene.addNewAnimatedObjectGroup( node->getName() + "Node", scene ).lock();
				auto animObject = animGroup->addObject( *node, node->getName() );

				for ( auto & animation : node->getAnimations() )
				{
					animGroup->addAnimation( animation.first );
					animGroup->setAnimationLooped( animation.first, true );
				}
			}

			if ( mesh->hasAnimation() )
			{
				auto animGroup = scene.addNewAnimatedObjectGroup( geometry->getName() + "Morph", scene ).lock();
				auto animObject = animGroup->addObject( *mesh
					, *geometry
					, geometry->getName() );

				for ( auto & animation : mesh->getAnimations() )
				{
					animGroup->addAnimation( animation.first );
					animGroup->setAnimationLooped( animation.first, true );
				}
			}

			if ( skeleton && skeleton->hasAnimation() )
			{
				doAddAnimatedObjectGroup( scene
					, *node
					, *skeleton
					, *mesh
					, *geometry );
			}
		}
	}

	void SceneImporter::doProcessNodes( aiScene const & aiScene
		, aiNode const & aiNode
		, castor3d::Scene & scene
		, std::map< uint32_t, MeshData * > const & meshes )
	{
		if ( m_skeletons.isBoneNode( aiNode ) )
		{
			return;
		}

		if ( aiNode.mNumMeshes > 0 )
		{
			castor::String name = m_importer.getInternalName( aiNode.mName );
			auto nodeIt = std::find_if( m_nodes.begin()
				, m_nodes.end()
				, [&name]( castor3d::SceneNodeSPtr lookup )
				{
					return lookup->getName() == name;
				} );
			castor3d::SceneNodeSPtr node;

			if ( nodeIt != m_nodes.end() )
			{
				node = *nodeIt;
			}
			else
			{
				auto it = m_sortedNodes.find( makeString( aiNode.mName ) );

				if ( it != m_sortedNodes.end() )
				{
					node = it->second;
				}
			}

			if ( !node )
			{
				return;
			}

			for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto meshIt = meshes.find( aiMeshIndex );

				if ( meshIt != meshes.end() )
				{
					auto * meshRepl = meshIt->second;
					auto lgeom = scene.tryFindGeometry( name + castor::string::toString( aiMeshIndex ) );

					if ( !lgeom.lock() )
					{
						auto geom = scene.createGeometry( name + castor::string::toString( aiMeshIndex )
							, scene
							, *node
							, meshRepl->lmesh );
						// TODO: Why should it be uncullable ?
						geom->setCullable( false );
						uint32_t index = 0u;
						auto matIt = meshRepl->submeshes.find( aiMeshIndex );
						CU_Assert( matIt != meshRepl->submeshes.end(), "Couldn't find material in imported materials." );

						for ( auto material : matIt->second.materials )
						{
							geom->setMaterial( *meshRepl->mesh->getSubmesh( index ), material );
							++index;
						}

						m_geometries.emplace( geom->getName(), geom );
						node->attachObject( *geom );
						scene.addGeometry( geom );
					}
				}
			}
		}

		// continue for all child nodes
		for ( auto aiChild : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessNodes( aiScene, *aiChild, scene, meshes );
		}
	}

	void SceneImporter::doAddAnimatedObjectGroup( castor3d::Scene & scene
		, castor3d::SceneNode const & sceneNode
		, castor3d::Skeleton & skeleton
		, castor3d::Mesh & mesh
		, castor3d::Geometry & geometry )
	{
		auto name = geometry.getName();
		auto it = std::find_if( m_animGroups.begin()
			, m_animGroups.end()
			, [&sceneNode, &skeleton]( NodeAnimGroup const & lookup )
			{
				auto & objects = lookup.second->getObjects();
				auto objIt = std::find_if( objects.begin()
					, objects.end()
					, [&skeleton]( auto const & objLookup )
					{
						auto animObject = objLookup.second;
						return animObject
							&& animObject->getKind() == castor3d::AnimationType::eSkeleton
							&& &static_cast< castor3d::AnimatedSkeleton const & >( *animObject ).getSkeleton() == &skeleton;
					} );
				auto result = objIt != objects.end();

				if ( result )
				{
					result = lookup.first->getParent() == sceneNode.getParent()
						&& lookup.first->getPosition() == sceneNode.getPosition()
						&& lookup.first->getOrientation() == sceneNode.getOrientation()
						&& lookup.first->getScale() == sceneNode.getScale();
				}

				return result;
			} );
		castor3d::AnimatedObjectGroupSPtr animGroup;

		if ( it == m_animGroups.end() )
		{
			m_animGroups.emplace_back( &sceneNode
				, scene.addNewAnimatedObjectGroup( name + cuT( "Skin" ), scene ).lock() );
			animGroup = m_animGroups.back().second;
		}
		else
		{
			animGroup = it->second;
		}

		for ( auto & animation : skeleton.getAnimations() )
		{
			if ( animGroup->addAnimation( animation.first ) )
			{
				animGroup->setAnimationLooped( animation.first, true );
			}
		}

		animGroup->addObject( skeleton
			, mesh
			, geometry
			, name );
	}

	//*********************************************************************************************
}
