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

using namespace castor3d;

namespace c3d_assimp
{
	using SceneNodeAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;

	SceneImporter::SceneImporter( castor3d::MeshImporter & importer
		, SkeletonImporter const & skeletons
		, MeshesImporter const & meshes
		, castor3d::SceneNodePtrArray & nodes
		, castor3d::GeometryPtrStrMap & geometries )
		: m_importer{ importer }
		, m_skeletons{ skeletons }
		, m_meshes{ meshes }
		, m_nodes{ nodes }
		, m_geometries{ geometries }
	{
	}

	void SceneImporter::import( castor::String const & prefix
		, aiScene const & aiScene
		, castor3d::Scene & scene
		, std::map< uint32_t, MeshData * > const & meshes )
	{
		m_prefix = prefix;
		castor::Matrix4x4f transform;
		transform.setIdentity();
		doProcessSceneNodes( aiScene
			, *aiScene.mRootNode
			, scene
			, scene.getObjectRootNode()
			, transform );

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

	void SceneImporter::import( castor::String const & prefix
		, aiScene const & aiScene
		, MeshIndices const & meshes )
	{
		m_prefix = prefix;
		doScaleMesh( *aiScene.mRootNode
			, meshes );
	}

	void SceneImporter::importAnims()
	{
		doProcessAnimGroups();
	}

	void SceneImporter::doProcessLight( aiLight const & aiLight
		, Scene & scene )
	{
		castor::String name = m_prefix + aiLight.mName.C_Str();
		SceneNodeSPtr node;

		if ( scene.getSceneNodeCache().has( name ) )
		{
			node = scene.getSceneNodeCache().find( name ).lock();
		}
		else
		{
			node = std::make_shared< SceneNode >( name
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

		LightSPtr light;

		switch ( aiLight.mType )
		{
		case aiLightSource_DIRECTIONAL:
			{
				light = std::make_shared< Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::eDirectional );
			}
			break;
		case aiLightSource_POINT:
			{
				light = std::make_shared< Light >( name
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
				light = std::make_shared< Light >( name
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
		, Scene & scene
		, SceneNodeSPtr parent
		, castor::Matrix4x4f accTransform )
	{
		if ( m_skeletons.isBoneNode( aiNode ) )
		{
			return;
		}

		castor::String name = m_prefix + aiNode.mName.C_Str();
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

			if ( aiScene.HasAnimations() )
			{
				for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
				{
					auto it = std::find_if( aiAnimation->mChannels
						, aiAnimation->mChannels + aiAnimation->mNumChannels
						, [&name]( aiNodeAnim const * lookup )
						{
							return lookup->mNodeName.C_Str() == name;
						} );

					if ( it != aiAnimation->mChannels + aiAnimation->mNumChannels )
					{
						doProcessAnimationSceneNodes( *node, *aiAnimation, aiNode, *( *it ) );
					}
				}
			}
		}
		else
		{
			m_nodes.push_back( lnode.lock() );
		}

		parent = lnode.lock();
		accTransform = makeMatrix4x4f( aiNode.mTransformation ) * accTransform;

		// continue for all child nodes
		for ( auto aiChild : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessSceneNodes( aiScene, *aiChild, scene, parent, accTransform );
		}
	}

	void SceneImporter::doScaleMesh( aiNode const & aiNode
		, MeshIndices const & meshes )
	{
		for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
		{
			auto it = meshes.find( aiMeshIndex );

			if ( it != meshes.end() )
			{
				castor::Matrix4x4f transform;

				if ( it->second->hasComponent( castor3d::BonesComponent::Name ) )
				{
					aiQuaternion quat;
					aiVector3D tran;
					aiVector3D scal;
					aiNode.mTransformation.DecomposeNoScaling( quat, tran );
					castor::matrix::setTransform( transform
						, convert( tran )
						, castor::Point3f{ 1.0f, 1.0f, 1.0f }
						, castor::Quaternion::identity() );
					auto parent = aiNode.mParent;

					while ( parent )
					{
						parent->mTransformation.DecomposeNoScaling( quat, tran );
						castor::Matrix4x4f parentTransform;
						castor::matrix::setTransform( parentTransform
							, convert( tran )
							, castor::Point3f{ 1.0f, 1.0f, 1.0f }
							, castor::Quaternion::identity() );
						transform = transform * parentTransform;
						parent = parent->mParent;
					}
				}
				else
				{
					transform = makeMatrix4x4f( aiNode.mTransformation );
					auto parent = aiNode.mParent;

					while ( parent )
					{
						transform = transform * makeMatrix4x4f( parent->mTransformation );
						parent = parent->mParent;
					}
				}

				for ( auto & vertex : it->second->getPoints() )
				{
					vertex.pos = transform * vertex.pos;
				}
			}
		}

		for ( auto child : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doScaleMesh( *child, meshes );
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
		, Scene & scene
		, std::map< uint32_t, MeshData * > const & meshes )
	{
		if ( m_skeletons.isBoneNode( aiNode ) )
		{
			return;
		}

		if ( aiNode.mNumMeshes > 0 )
		{
			castor::String name = m_prefix + aiNode.mName.C_Str();
			auto node = *std::find_if( m_nodes.begin()
				, m_nodes.end()
				, [&name]( SceneNodeSPtr lookup )
				{
					return lookup->getName() == name;
				} );

			for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto meshIt = meshes.find( aiMeshIndex );

				if ( meshIt != meshes.end() )
				{
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

		castor::String name{ normalizeName( m_prefix + castor::string::stringCast< xchar >( aiAnimation.mName.C_Str() ) ) };

		if ( name.empty() )
		{
			name = node.getName();
		}

		log::info << cuT( "  SceneNode Animation found: [" ) << name << cuT( "]" ) << std::endl;
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
			, []( SceneNodeAnimationKeyFrame & keyframe
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
