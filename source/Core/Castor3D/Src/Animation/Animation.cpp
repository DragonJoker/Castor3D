#include "Animation.hpp"

#include "Skeleton/SkeletonAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Animation >::DoWrite( Animation const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			result = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = DoWriteChunk( real( p_obj.GetLength().count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( result )
		{
			switch ( p_obj.GetType() )
			{
			case AnimationType::eSkeleton:
				BinaryWriter< SkeletonAnimation >{}.Write( static_cast< SkeletonAnimation const & >( p_obj ), m_chunk );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Animation >::DoParse( Animation & p_obj )
	{
		bool result = true;
		String name;
		BinaryChunk chunk;
		real length{ 0.0_r };

		while ( result && DoGetSubChunk( chunk ) )
		{
			switch ( chunk.GetChunkType() )
			{
			case ChunkType::eName:
				result = DoParseChunk( name, chunk );

				if ( result )
				{
					p_obj.m_name = name;
				}

				break;

			case ChunkType::eAnimLength:
				result = DoParseChunk( length, chunk );
				p_obj.m_length = Milliseconds( uint64_t( length * 1000 ) );
				break;

			case ChunkType::eSkeletonAnimation:
				BinaryParser< SkeletonAnimation >{}.Parse( static_cast< SkeletonAnimation & >( p_obj ), chunk );
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

	void Animation::UpdateLength()
	{
		DoUpdateLength();
	}

	//*************************************************************************************************
}
