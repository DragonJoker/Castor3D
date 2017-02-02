#include "Animation.hpp"

#include "Skeleton/SkeletonAnimation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	bool BinaryWriter< Animation >::DoWrite( Animation const & p_obj )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetName(), ChunkType::eName, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( real( p_obj.GetLength().count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( l_return )
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

		return l_return;
	}

	//*************************************************************************************************

	bool BinaryParser< Animation >::DoParse( Animation & p_obj )
	{
		bool l_return = true;
		String l_name;
		BinaryChunk l_chunk;
		real l_length{ 0.0_r };

		while ( l_return && DoGetSubChunk( l_chunk ) )
		{
			switch ( l_chunk.GetChunkType() )
			{
			case ChunkType::eName:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
				{
					p_obj.m_name = l_name;
				}

				break;

			case ChunkType::eAnimLength:
				l_return = DoParseChunk( l_length, l_chunk );
				p_obj.m_length = std::chrono::milliseconds( uint64_t( l_length * 1000 ) );
				break;

			case ChunkType::eSkeletonAnimation:
				BinaryParser< SkeletonAnimation >{}.Parse( static_cast< SkeletonAnimation & >( p_obj ), l_chunk );
				break;
			}
		}

		return l_return;
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
