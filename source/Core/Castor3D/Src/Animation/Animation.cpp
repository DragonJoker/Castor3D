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
			l_return = DoWriteChunk( p_obj.GetLength(), ChunkType::eAnimLength, m_chunk );
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
				l_return = DoParseChunk( p_obj.m_length, l_chunk );
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
