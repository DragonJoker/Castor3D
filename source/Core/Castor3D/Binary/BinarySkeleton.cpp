#include "Castor3D/Binary/BinarySkeleton.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinaryAnimation.hpp"
#include "Castor3D/Binary/BinaryBone.hpp"
#include "Castor3D/Model/Skeleton/Bone.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::doWrite( Skeleton const & obj )
	{
		bool result = doWriteChunk( obj.getGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );

		for ( auto bone : obj.m_bones )
		{
			result = result && BinaryWriter< Bone >{}.write( *bone, m_chunk );
		}

		for ( auto const & it : obj.m_animations )
		{
			result = result && BinaryWriter< Animation >{}.write( *it.second, m_chunk );
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
				bone = std::make_shared< Bone >( obj, castor::Matrix4x4f{ 1.0f } );
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

			default:
				break;
			}
		}

		return result;
	}
}
