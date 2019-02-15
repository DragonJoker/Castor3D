#include "BinarySkeletonAnimationObject.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Animation/Skeleton/SkeletonAnimationObject.hpp"
#include "Animation/Skeleton/SkeletonAnimationBone.hpp"
#include "Animation/Skeleton/SkeletonAnimationKeyFrame.hpp"
#include "Animation/Skeleton/SkeletonAnimationNode.hpp"
#include "Binary/BinarySkeletonAnimationBone.hpp"
#include "Binary/BinarySkeletonAnimationKeyFrame.hpp"
#include "Binary/BinarySkeletonAnimationNode.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		template< typename T >
		struct KeyFrameT
		{
			Milliseconds m_timeIndex;
			SquareMatrix< T, 4u > m_transform;
		};

		using KeyFrame = KeyFrameT< float >;
		using KeyFramed = std::array< double, 17 >;

		void doConvert( std::vector< KeyFramed > const & in
			, std::vector< KeyFrame > & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & kf : in )
			{
				size_t index{ 0u };
				Milliseconds timeIndex{ int64_t( kf[index++] * 1000.0 ) };
				Matrix4x4r transform{ &kf[index] };
				( *it ) = KeyFrame{ timeIndex, transform };
				++it;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< SkeletonAnimationObject >::doWrite( SkeletonAnimationObject const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.m_nodeTransform, ChunkType::eMovingTransform, m_chunk );
		}

		for ( auto const & moving : obj.m_children )
		{
			switch ( moving->getType() )
			{
			case SkeletonAnimationObjectType::eNode:
				result &= BinaryWriter< SkeletonAnimationNode >{}.write( *std::static_pointer_cast< SkeletonAnimationNode >( moving ), m_chunk );
				break;

			case SkeletonAnimationObjectType::eBone:
				result &= BinaryWriter< SkeletonAnimationBone >{}.write( *std::static_pointer_cast< SkeletonAnimationBone >( moving ), m_chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< SkeletonAnimationObject >::doParse( SkeletonAnimationObject & obj )
	{
		bool result = true;
		Matrix4x4r transform;
		std::vector< KeyFrame > keyframes;
		std::vector< KeyFramed > keyframesd;
		SkeletonAnimationNodeSPtr node;
		SkeletonAnimationObjectSPtr object;
		SkeletonAnimationBoneSPtr bone;
		BinaryChunk chunk;
		uint32_t count{ 0 };
		real length{ 0.0_r };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMovingTransform:
				result = doParseChunk( obj.m_nodeTransform, chunk );
				break;

			case ChunkType::eSkeletonAnimationBone:
				bone = std::make_shared< SkeletonAnimationBone >( *obj.getOwner() );
				obj.addChild( bone );
				result = createBinaryParser< SkeletonAnimationBone >().parse( *bone, chunk );

				if ( result )
				{
					obj.getOwner()->addObject( bone, obj.shared_from_this() );
				}

				break;

			case ChunkType::eSkeletonAnimationNode:
				node = std::make_shared< SkeletonAnimationNode >( *obj.getOwner() );
				obj.addChild( node );
				result = createBinaryParser< SkeletonAnimationNode >().parse( *node, chunk );

				if ( result )
				{
					obj.getOwner()->addObject( node, obj.shared_from_this() );
				}

				break;
			}
		}

		return result;
	}

	bool BinaryParser< SkeletonAnimationObject >::doParse_v1_1( SkeletonAnimationObject & obj )
	{
		bool result = true;
		Matrix4x4r transform;
		std::vector< KeyFrame > keyframes;
		std::vector< KeyFramed > keyframesd;
		SkeletonAnimationNodeSPtr node;
		SkeletonAnimationObjectSPtr object;
		SkeletonAnimationBoneSPtr bone;
		BinaryChunk chunk;
		uint32_t count{ 0 };
		real length{ 0.0_r };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eKeyframeCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					keyframesd.resize( count );
				}

				break;

			case ChunkType::eKeyframes:
				result = doParseChunk( keyframesd, chunk );

				if ( result )
				{
					doConvert( keyframesd, keyframes );
					auto & animation = *obj.getOwner();

					for ( auto & keyframe : keyframes )
					{
						auto it = animation.find( keyframe.m_timeIndex );

						if ( it == animation.end() )
						{
							animation.addKeyFrame( std::make_unique< SkeletonAnimationKeyFrame >( *obj.getOwner()
								, keyframe.m_timeIndex ) );
							it = animation.find( keyframe.m_timeIndex );
						}

						auto & keyFrame = static_cast< SkeletonAnimationKeyFrame & >( **it );
						keyFrame.addAnimationObject( obj, keyframe.m_transform );
					}

					for ( auto & keyFrame : animation )
					{
						keyFrame->initialise();
					}
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************
}
