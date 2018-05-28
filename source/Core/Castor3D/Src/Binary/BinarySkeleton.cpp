#include "BinarySkeleton.hpp"

#include "Animation/Skeleton/SkeletonAnimation.hpp"
#include "Binary/BinaryAnimation.hpp"
#include "Binary/BinaryBone.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::doWrite( Skeleton const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.getGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );
		}

		for ( auto bone : obj.m_bones )
		{
			result &= BinaryWriter< Bone >{}.write( *bone, m_chunk );
		}

		for ( auto const & it : obj.m_animations )
		{
			result = BinaryWriter< Animation >{}.write( *it.second, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Skeleton >::doParse( Skeleton & obj )
	{
		bool result = true;
		BoneSPtr bone;
		BinaryChunk chunk;
		SkeletonAnimationUPtr animation;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSkeletonGlobalInverse:
				result = doParseChunk( obj.m_globalInverse, chunk );
				break;

			case ChunkType::eSkeletonBone:
				bone = std::make_shared< Bone >( obj, Matrix4x4r{ 1.0_r } );
				result = createBinaryParser< Bone >().parse( *bone, chunk );

				if ( result )
				{
					obj.m_bones.push_back( bone );
				}

				break;

			case ChunkType::eAnimation:
				animation = std::make_unique< SkeletonAnimation >( obj );
				result = createBinaryParser< Animation >().parse( *animation, chunk );

				if ( result )
				{
					obj.m_animations.insert( { animation->getName(), std::move( animation ) } );
				}

				break;
			}
		}

		return result;
	}
}
