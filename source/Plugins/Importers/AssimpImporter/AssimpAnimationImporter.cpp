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
			, const c3d::String & nodeName )
		{
			aiNodeAnim const * result = nullptr;

			if ( auto it = std::find_if( animation.mChannels
				, animation.mChannels + animation.mNumChannels
				, [&nodeName]( aiNodeAnim const * const nodeAnim )
				{
					return makeString( nodeAnim->mNodeName ) == nodeName;
				} );
				it != animation.mChannels + animation.mNumChannels )
			{
				result = *it;
			}

			return result;
		}

		static void fillKeyFrame( c3d::ArrayView< uint32_t > values
			, c3d::ArrayView< double > weights
			, c3d::Submesh const & submesh
			, c3d::MeshMorphTarget & keyFrame )
		{
			c3d::Vector< float > res;
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

	using SceneNodeAnimationKeyFrameMap = c3d::Map< c3d::Milliseconds, c3d::SceneNodeAnimationKeyFrameUPtr >;

	AssimpAnimationImporter::AssimpAnimationImporter( c3d::Engine & engine )
		: c3d::AnimationImporter{ engine, cuT( "Assimp" ) }
	{
	}

	bool AssimpAnimationImporter::doImportSkeleton( c3d::SkeletonAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & skeleton = static_cast< c3d::Skeleton const & >( *animation.getAnimable() );
		auto & animations = file.getSkeletonsAnimations( skeleton );
		auto it = animations.find( name );

		if ( it == animations.end() )
		{
			return false;
		}

		auto & aiAnimation = *it->second;
		auto [frameCount, minFrameTicks, maxFrameTicks] = getAnimationFrameTicks( aiAnimation );
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25LL;
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessSkeletonAnimationNodes( animation
			, fromAssimp( minFrameTicks, ticksPerSecond )
			, fromAssimp( maxFrameTicks, ticksPerSecond )
			, ticksPerSecond
			, skeleton
			, aiAnimation
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			auto & objTransform = object->getNodeTransform();

			for ( auto const & [_, keyFrame] : keyframes )
			{
				auto kfit = keyFrame->find( *object );

				if ( kfit == keyFrame->end() )
				{
					keyFrame->addAnimationObject( *object
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

		for ( auto & [_, keyFrame] : keyframes )
		{
			animation.addKeyFrame( c3d::ptrRefCast< c3d::AnimationKeyFrame >( keyFrame ) );
		}
		return true;
	}

	bool AssimpAnimationImporter::doImportMesh( c3d::MeshAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & mesh = static_cast< c3d::Mesh const & >( *animation.getAnimable() );

		for ( auto & submesh : mesh )
		{
			auto index = submesh->getId();
			auto & animations = file.getMeshesAnimations( mesh, index );
			auto animIt = animations.find( name );

			if ( animIt != animations.end() )
			{
				auto & aiAnimation = *animIt->second.second;

				c3d::MeshAnimationSubmesh animSubmesh{ animation, *submesh };
				animation.addChild( c3d::move( animSubmesh ) );

				for ( auto & morphKey : c3d::makeArrayView( aiAnimation.mKeys, aiAnimation.mNumKeys ) )
				{
					auto timeIndex = c3d::Milliseconds{ uint64_t( morphKey.mTime ) };
					auto kfit = animation.find( timeIndex );
					c3d::MeshMorphTarget * kf{};

					if ( kfit == animation.end() )
					{
						auto keyFrame = c3d::makeUnique< c3d::MeshMorphTarget >( animation, timeIndex );
						kf = keyFrame.get();
						animation.addKeyFrame( c3d::ptrRefCast< c3d::AnimationKeyFrame >( keyFrame ) );
					}
					else
					{
						kf = &static_cast< c3d::MeshMorphTarget & >( **kfit );
					}

					anims::fillKeyFrame( c3d::makeArrayView( morphKey.mValues, morphKey.mNumValuesAndWeights )
						, c3d::makeArrayView( morphKey.mWeights, morphKey.mNumValuesAndWeights )
						, *submesh
						, *kf );
				}
			}
		}

		return true;
	}

	bool AssimpAnimationImporter::doImportNode( c3d::SceneNodeAnimation & animation )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = animation.getName();
		auto & node = static_cast< c3d::SceneNode const & >( *animation.getAnimable() );
		auto & animations = file.getNodesAnimations( node );
		auto it = animations.find( name );

		if ( it == animations.end() )
		{
			return false;
		}

		auto & aiAnimation = *it->second.first;
		auto & aiNodeAnim = *it->second.second;
		auto [frameCount, minFrameTicks, maxFrameTicks] = getNodeAnimFrameTicks( aiNodeAnim );
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25LL;
		SceneNodeAnimationKeyFrameMap keyframes;
		processAnimationNodeKeys( aiNodeAnim
			, getEngine()->getWantedFps()
			, fromAssimp( minFrameTicks, ticksPerSecond )
			, fromAssimp( maxFrameTicks, ticksPerSecond )
			, ticksPerSecond
			, animation
			, keyframes
			, []( c3d::SceneNodeAnimationKeyFrame & keyframe
				, c3d::Point3f const & position
				, c3d::Quaternion const & orientation
				, c3d::Point3f const & scale )
			{
				keyframe.setTransform( position, orientation, scale );
			} );

		for ( auto & [_, keyFrame] : keyframes )
		{
			animation.addKeyFrame( c3d::ptrRefCast< c3d::AnimationKeyFrame >( keyFrame ) );
		}

		return true;
	}

	void AssimpAnimationImporter::doProcessSkeletonAnimationNodes( c3d::SkeletonAnimation & animation
		, c3d::Milliseconds minTime
		, c3d::Milliseconds maxTime
		, int64_t ticksPerSecond
		, c3d::Skeleton const & skeleton
		, aiAnimation const & aiAnimation
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )const
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );

		for ( auto & skelNode : skeleton.getNodes() )
		{
			auto name = skelNode->getName();
			auto nodeName = file.getExternalName( name );
			const aiNodeAnim * aiNodeAnim = anims::findSkelNodeAnim( aiAnimation
				, nodeName );
			auto parentSkelNode = skelNode->getParent();
			c3d::SkeletonAnimationObjectRPtr parent{};

			if ( parentSkelNode )
			{
				parent = animation.getObject( parentSkelNode->getType()
					, parentSkelNode->getName() );
				CU_Require( parent );
			}

			c3d::SkeletonAnimationObjectRPtr object{};
			CU_Require( !animation.hasObject( skelNode->getType(), name ) );

			if ( skelNode->getType() == c3d::SkeletonNodeType::eBone )
			{
				object = animation.addObject( static_cast< c3d::BoneNode & >( *skelNode )
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
					, minTime
					, maxTime
					, ticksPerSecond
					, animation
					, keyFrames
					, [&object]( c3d::SkeletonAnimationKeyFrame & keyframe
						, c3d::Point3f const & position
						, c3d::Quaternion const & orientation
						, c3d::Point3f const & scale )
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

	bool AssimpAnimationImporter::doImportTexture( c3d::TextureAnimation & animation )
	{
		return true;
	}
}
