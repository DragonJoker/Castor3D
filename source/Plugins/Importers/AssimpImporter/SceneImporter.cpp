#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/SceneImporter.hpp"
#include "AssimpImporter/AssimpHelpers.hpp"
#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
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
			castor::matrix::setTranslate( result, convert( tran ) );
			return result;
		}

		static std::map< aiAnimation const *, aiNodeAnim const * > findNodeAnims( aiNode const & aiNode
			, castor::ArrayView< aiAnimation * > const & animations )
		{
			std::map< aiAnimation const *, aiNodeAnim const * > result;

			for ( auto aiAnimation : animations )
			{
				auto channels = castor::makeArrayView( aiAnimation->mChannels, aiAnimation->mNumChannels );
				auto it = std::find_if( channels.begin()
					, channels.end()
					, [&aiNode]( aiNodeAnim const * lookup )
					{
						return lookup->mNodeName == aiNode.mName;
					} );

				if ( it != channels.end() )
				{
					result.emplace( aiAnimation, *it );
				}
			}

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
		doProcessSceneNodes( aiScene
			, *aiScene.mRootNode
			, scene
			, meshes
			, scene.getObjectRootNode()
			, castor::Matrix4x4f{ 1.0f } );

		for ( aiLight * aiLight : castor::makeArrayView( aiScene.mLights, aiScene.mNumLights ) )
		{
			doProcessLight( *aiLight, scene );
		}

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

	void SceneImporter::doProcessLight( aiLight const & aiLight
		, castor3d::Scene & scene )
	{
		castor::String name = m_importer.getInternalName( aiLight.mName );
		castor3d::SceneNodeSPtr node;

		if ( scene.getSceneNodeCache().has( name ) )
		{
			node = scene.getSceneNodeCache().find( name ).lock();
		}
		else
		{
			node = std::make_shared< castor3d::SceneNode >( name
				, scene );

			if ( aiLight.mType == aiLightSource_DIRECTIONAL
				|| aiLight.mType == aiLightSource_SPOT )
			{
				auto direction = castor::point::getNormalised( castor::Point3f{ aiLight.mDirection.x, aiLight.mDirection.y, aiLight.mDirection.z } );
				auto up = castor::point::getNormalised( castor::Point3f{ aiLight.mUp.x, aiLight.mUp.y, aiLight.mUp.z } );
				node->setOrientation( castor::Quaternion::fromMatrix( makeMatrix4x4f( direction, up ) ) );
			}

			if ( aiLight.mType != aiLightSource_DIRECTIONAL )
			{
				auto position = castor::Point3f{ aiLight.mPosition.x, aiLight.mPosition.y, aiLight.mPosition.z };
				node->setPosition( position );
			}

			node->attachTo( *scene.getObjectRootNode() );
			node = scene.getSceneNodeCache().add( name, node ).lock();
		}

		castor3d::LightSPtr light;

		switch ( aiLight.mType )
		{
		case aiLightSource_DIRECTIONAL:
			{
				light = std::make_shared< castor3d::Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::eDirectional );
			}
			break;
		case aiLightSource_POINT:
			{
				light = std::make_shared< castor3d::Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::ePoint );
				auto point = light->getPointLight();
				point->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
			}
			break;
		case aiLightSource_SPOT:
			{
				light = std::make_shared< castor3d::Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::eSpot );
				auto spot = light->getSpotLight();
				spot->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
				spot->setInnerCutOff( castor::Angle::fromRadians( aiLight.mAngleInnerCone ) );
				spot->setOuterCutOff( castor::Angle::fromRadians( aiLight.mAngleOuterCone ) );
			}
			break;
		default:
			break;
		}

		if ( light )
		{
			m_nodes.push_back( node );
			castor::Point3f colour{ aiLight.mColorDiffuse.r, aiLight.mColorDiffuse.g, aiLight.mColorDiffuse.b };
			auto length = float( castor::point::length( colour ) );

			if ( length != 0.0 )
			{
				castor::point::normalise( colour );
				light->setIntensity( length, length );
			}

			light->setColour( castor::RgbColour::fromComponents( colour->x, colour->y, colour->z ) );
			node->attachObject( *light );
			scene.getLightCache().add( name, light );
		}
	}

	void SceneImporter::doProcessSceneNodes( aiScene const & aiScene
		, aiNode const & aiNode
		, castor3d::Scene & scene
		, std::map< uint32_t, MeshData * > const & meshes
		, castor3d::SceneNodeSPtr parent
		, castor::Matrix4x4f transform )
	{
		if ( m_skeletons.isBoneNode( aiNode ) )
		{
			return;
		}

		auto anims = scenes::findNodeAnims( aiNode
			, castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) );
		auto aiMeshes = castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes );
		auto meshIt = std::find_if( aiMeshes.begin()
			, aiMeshes.end()
			, [&meshes]( uint32_t aiMeshIndex )
			{
				return meshes.find( aiMeshIndex ) != meshes.end();
			} );

		if ( meshIt != aiMeshes.end()
			|| !anims.empty() )
		{
			castor::String name = m_importer.getInternalName( aiNode.mName );
			auto lnode = scene.getSceneNodeCache().tryFind( name );

			if ( !lnode.lock() )
			{
				auto node = scene.getSceneNodeCache().create( name, scene );
				node->attachTo( *parent );
				aiVector3D scale, position;
				aiQuaternion orientation;
				aiNode.mTransformation.Decompose( scale, orientation, position );
				node->setPosition( { position.x, position.y, position.z } );
				node->setScale( { scale.x, scale.y, scale.z } );
				node->setOrientation( castor::Quaternion{ castor::Point4f{ orientation.x, orientation.y, orientation.z, orientation.w } } );
				m_nodes.push_back( node );
				lnode = scene.getSceneNodeCache().add( node->getName(), node );
				node = lnode.lock();

				for ( auto nodeAnim : anims )
				{
					doProcessAnimationSceneNodes( *node, *nodeAnim.first, aiNode, *nodeAnim.second );
				}
			}
			else
			{
				m_nodes.push_back( lnode.lock() );
			}

			parent = lnode.lock();
		}
		else
		{
			transform = transform * makeMatrix4x4f( aiNode.mTransformation );
		}

		// continue for all child nodes
		for ( auto aiChild : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessSceneNodes( aiScene, *aiChild, scene, meshes, parent, transform );
		}
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
					: makeMatrix4x4f( transformAcc ) );

				for ( auto & vertex : it->second->getPoints() )
				{
					vertex.pos = transform * vertex.pos;
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
				auto animGroup = scene.getAnimatedObjectGroupCache().add( node->getName() + "Node", scene ).lock();
				auto animObject = animGroup->addObject( *node, node->getName() );

				for ( auto & animation : node->getAnimations() )
				{
					animGroup->addAnimation( animation.first );
					animGroup->setAnimationLooped( animation.first, true );
				}
			}

			if ( mesh->hasAnimation() )
			{
				auto animGroup = scene.getAnimatedObjectGroupCache().add( geometry->getName() + "Morph", scene ).lock();
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

			for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto meshIt = meshes.find( aiMeshIndex );

				if ( meshIt != meshes.end() )
				{
					CU_Require( nodeIt != m_nodes.end() );
					auto node = *nodeIt;
					auto * meshRepl = meshIt->second;
					auto lgeom = scene.getGeometryCache().tryFind( name + castor::string::toString( aiMeshIndex ) );

					if ( !lgeom.lock() )
					{
						auto geom = scene.getGeometryCache().create( name + castor::string::toString( aiMeshIndex )
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
						scene.getGeometryCache().add( geom );
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
				, scene.getAnimatedObjectGroupCache().add( name + cuT( "Skin" ), scene ).lock() );
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

	void SceneImporter::doProcessAnimationSceneNodes( castor3d::SceneNode & node
		, aiAnimation const & aiAnimation
		, aiNode const & aiNode
		, aiNodeAnim const & aiNodeAnim )
	{
		if ( aiNodeAnim.mNumPositionKeys <= 1
			&& aiNodeAnim.mNumScalingKeys <= 1
			&& aiNodeAnim.mNumRotationKeys <= 1 )
		{
			return;
		}

		castor::String name{ normalizeName( makeString( aiAnimation.mName ) ) };

		if ( name.empty() )
		{
			name = normalizeName( node.getName() );
		}

		castor3d::log::info << cuT( "  SceneNode Animation found: [" ) << name << cuT( "]" ) << std::endl;
		auto & animation = node.createAnimation( name );
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SceneNodeAnimationKeyFrameMap keyframes;
		processAnimationNodeKeys( aiNodeAnim
			, m_importer.getEngine()->getRenderLoop().getWantedFps()
			, ticksPerSecond
			, animation
			, keyframes
			, []( castor3d::SceneNodeAnimationKeyFrame & keyframe
				, castor::Point3f const & position
				, castor::Quaternion const & orientation
				, castor::Point3f const & scale )
			{
				keyframe.setTransform( position, orientation, scale );
			} );

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}
	}

	//*********************************************************************************************
}
