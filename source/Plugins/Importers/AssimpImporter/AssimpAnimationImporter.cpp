#include "AssimpImporter/AssimpAnimationImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp>

namespace c3d_assimp
{
	namespace anims
	{
		static aiNodeAnim const * findSkelNodeAnim( aiAnimation const & animation
			, const castor::String & nodeName )
		{
			aiNodeAnim const * result = nullptr;
			auto it = std::find_if( animation.mChannels
				, animation.mChannels + animation.mNumChannels
				, [&nodeName]( aiNodeAnim const * const nodeAnim )
				{
					return castor::string::stringCast< castor::xchar >( nodeAnim->mNodeName.data ) == nodeName;
				} );

			if ( it != animation.mChannels + animation.mNumChannels )
			{
				result = *it;
			}

			return result;
		}

		static void fillKeyFrame( castor::ArrayView< uint32_t > values
			, castor::ArrayView< double > weights
			, castor3d::Submesh const & submesh
			, castor3d::MeshMorphTarget & keyFrame )
		{
			std::vector< float > res;
			res.resize( submesh.getMorphTargetsCount() );
			auto valueIt = values.begin();
			auto weightIt = weights.begin();

			while ( valueIt != values.end() )
			{
				auto value = *valueIt;
				CU_Require( value < submesh.getMorphTargetsCount() );
				res[value] = float( *weightIt );
				++valueIt;
				++weightIt;
			}

			keyFrame.setTargetsWeights( submesh, res );
		}
	}

	using SceneNodeAnimationKeyFrameMap = std::map< castor::Milliseconds, castor3d::SceneNodeAnimationKeyFrameUPtr >;

	AssimpAnimationImporter::AssimpAnimationImporter( castor3d::Engine & engine )
		: castor3d::AnimationImporter{ engine }
	{
	}

	bool AssimpAnimationImporter::doImportSkeleton( castor3d::SkeletonAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & skeleton = static_cast< castor3d::Skeleton const & >( *animation.getAnimable() );
		auto & animations = file.getSkeletonsAnimations( skeleton );
		auto it = animations.find( name );

		if ( it == animations.end() )
		{
			return false;
		}

		auto & aiAnimation = *it->second;
		auto & aiNode = *file.getAiScene().mRootNode;
		auto [frameCount, frameTicks] = getAnimationFrameTicks( aiAnimation );
		castor3d::log::info << cuT( "  Skeleton Animation found: [" ) << name << cuT( "]" ) << std::endl;
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessSkeletonAnimationNodes( animation
			, fromAssimp( frameTicks, ticksPerSecond )
			, ticksPerSecond
			, skeleton
			, aiNode
			, aiAnimation
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			auto & objTransform = object->getNodeTransform();

			for ( auto & keyFrame : keyframes )
			{
				auto kfit = keyFrame.second->find( *object );

				if ( kfit == keyFrame.second->end() )
				{
					keyFrame.second->addAnimationObject( *object
						, objTransform.translate
						, objTransform.rotate
						, objTransform.scale );
				}
				else
				{
					kfit->transform.translate = objTransform.translate;
					kfit->transform.rotate = objTransform.rotate;
					kfit->transform.scale = objTransform.scale;
				}
			}
		}

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}

		return true;
	}

	bool AssimpAnimationImporter::doImportMesh( castor3d::MeshAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile & >( *m_file );
		auto name = animation.getName();
		auto & mesh = static_cast< castor3d::Mesh const & >( *animation.getAnimable() );

		for ( auto & submesh : mesh )
		{
			auto index = submesh->getId();
			auto & animations = file.getMeshesAnimations( mesh, index );
			auto animIt = animations.find( name );

			if ( animIt != animations.end() )
			{
				castor3d::log::info << cuT( "  Mesh Animation found for mesh [" ) << mesh.getName() << cuT( "], submesh " ) << index << cuT( ": [" ) << name << cuT( "]" ) << std::endl;
				auto & aiAnimation = *animIt->second.second;

				castor3d::MeshAnimationSubmesh animSubmesh{ animation, *submesh };
				animation.addChild( std::move( animSubmesh ) );

				for ( auto & morphKey : castor::makeArrayView( aiAnimation.mKeys, aiAnimation.mNumKeys ) )
				{
					auto timeIndex = castor::Milliseconds{ uint64_t( morphKey.mTime ) };
					auto kfit = animation.find( timeIndex );
					castor3d::MeshMorphTarget * kf{};

					if ( kfit == animation.end() )
					{
						auto keyFrame = std::make_unique< castor3d::MeshMorphTarget >( animation, timeIndex );
						kf = keyFrame.get();
						animation.addKeyFrame( std::move( keyFrame ) );
					}
					else
					{
						kf = &static_cast< castor3d::MeshMorphTarget & >( **kfit );
					}

					anims::fillKeyFrame( castor::makeArrayView( morphKey.mValues, morphKey.mNumValuesAndWeights )
						, castor::makeArrayView( morphKey.mWeights, morphKey.mNumValuesAndWeights )
						, *submesh
						, *kf );
				}
			}
		}

		return true;
	}

	bool AssimpAnimationImporter::doImportNode( castor3d::SceneNodeAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & node = static_cast< castor3d::SceneNode const & >( *animation.getAnimable() );
		auto & animations = file.getNodesAnimations( node );
		auto it = animations.find( name );

		if ( it == animations.end() )
		{
			return false;
		}

		auto & aiAnimation = *it->second.first;
		auto & aiNodeAnim = *it->second.second;
		auto [frameCount, frameTicks] = getNodeAnimFrameTicks( aiNodeAnim );
		castor3d::log::info << cuT( "  SceneNode Animation found: [" ) << name << cuT( "]" ) << std::endl;
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SceneNodeAnimationKeyFrameMap keyframes;
		processAnimationNodeKeys( aiNodeAnim
			, getEngine()->getWantedFps()
			, fromAssimp( frameTicks, ticksPerSecond )
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

		return true;
	}

	void AssimpAnimationImporter::doProcessSkeletonAnimationNodes( castor3d::SkeletonAnimation & animation
		, castor::Milliseconds maxTime
		, int64_t ticksPerSecond
		, castor3d::Skeleton const & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );

		for ( auto & skelNode : skeleton.getNodes() )
		{
			auto name = skelNode->getName();
			auto nodeName = file.getExternalName( name );
			const aiNodeAnim * aiNodeAnim = anims::findSkelNodeAnim( aiAnimation
				, nodeName );
			auto parentSkelNode = skelNode->getParent();
			castor3d::SkeletonAnimationObjectSPtr parent;

			if ( parentSkelNode )
			{
				parent = animation.getObject( parentSkelNode->getType()
					, parentSkelNode->getName() );
				CU_Require( parent );
			}

			castor3d::SkeletonAnimationObjectSPtr object;
			CU_Require( !animation.hasObject( skelNode->getType(), name ) );

			if ( skelNode->getType() == castor3d::SkeletonNodeType::eBone )
			{
				object = animation.addObject( static_cast< castor3d::BoneNode & >( *skelNode )
					, parent );
			}
			else
			{
				object = animation.addObject( *skelNode, parent );
			}

			if ( parent )
			{
				parent->addChild( object );
			}

			if ( aiNodeAnim )
			{
				processAnimationNodeKeys( *aiNodeAnim
					, getEngine()->getWantedFps()
					, maxTime
					, ticksPerSecond
					, animation
					, keyFrames
					, [&object]( castor3d::SkeletonAnimationKeyFrame & keyframe
						, castor::Point3f const & position
						, castor::Quaternion const & orientation
						, castor::Point3f const & scale )
					{
						keyframe.addAnimationObject( *object, position, orientation, scale );
					} );
			}
			else
			{
				notAnimated.insert( object );
			}
		}
	}
}
