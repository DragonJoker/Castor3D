#include "Castor3D/Binary/BinarySkeleton.hpp"

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Binary/BinaryAnimation.hpp"
#include "Castor3D/Binary/BinaryBone.hpp"
#include "Castor3D/Model/Skeleton/Bone.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace
	{
		BonePtrArray reorderBones( BonePtrArray bones )
		{
			BonePtrArray result;
			std::set< Bone * > inserted;

			while ( !bones.empty() )
			{
				auto bone = bones.front();
				bones.erase( bones.begin() );

				if ( !bone->getParent()
					|| inserted.find( bone->getParent().get() ) != inserted.end() )
				{
					result.push_back( bone );
					inserted.insert( bone.get() );
				}
				else
				{
					bones.push_back( bone );
				}
			}

			return result;
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Skeleton >::doWrite( Skeleton const & obj )
	{
		bool result = doWriteChunk( obj.getGlobalInverseTransform(), ChunkType::eSkeletonGlobalInverse, m_chunk );

		for ( auto bone : reorderBones( obj.m_bones ) )
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

	template<>
	castor::String BinaryParserBase< Skeleton >::Name = cuT( "Skeleton" );

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
				checkError( result, "Couldn't parse global inverse matrix." );
				break;

			case ChunkType::eSkeletonBone:
				bone = std::make_shared< Bone >( obj, castor::Matrix4x4f{ 1.0f } );
				result = createBinaryParser< Bone >().parse( *bone, chunk );
				checkError( result, "Couldn't parse bone." );

				if ( result )
				{
					obj.m_bones.push_back( bone );
				}

				break;

			case ChunkType::eAnimation:
				animation = std::make_unique< SkeletonAnimation >( obj );
				result = createBinaryParser< Animation >().parse( *animation, chunk );
				checkError( result, "Couldn't parse animation." );

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
