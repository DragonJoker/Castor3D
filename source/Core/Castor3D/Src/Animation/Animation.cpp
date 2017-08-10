#include "Animation.hpp"

#include "Skeleton/SkeletonAnimation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Animation >::doWrite( Animation const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( p_obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( real( p_obj.getLength().count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( result )
		{
			switch ( p_obj.getType() )
			{
			case AnimationType::eSkeleton:
				BinaryWriter< SkeletonAnimation >{}.write( static_cast< SkeletonAnimation const & >( p_obj ), m_chunk );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Animation >::doParse( Animation & p_obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;
		real length{ 0.0_r };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eName:
				result = doParseChunk( name, chunk );

				if ( result )
				{
					p_obj.m_name = name;
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				p_obj.m_length = Milliseconds( uint64_t( length * 1000 ) );
				break;

			case ChunkType::eSkeletonAnimation:
				BinaryParser< SkeletonAnimation >{}.parse( static_cast< SkeletonAnimation & >( p_obj ), chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Animation::Animation( AnimationType p_type, Animable & p_animable, String const & p_name )
		: Named{ p_name }
		, OwnedBy< Animable >{ p_animable }
		, m_type{ p_type }
	{
	}

	Animation::~Animation()
	{
	}

	void Animation::updateLength()
	{
		doUpdateLength();
	}

	//*************************************************************************************************
}
