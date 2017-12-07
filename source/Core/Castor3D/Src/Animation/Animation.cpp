#include "Animation.hpp"

#include "AnimationKeyFrame.hpp"
#include "Skeleton/SkeletonAnimation.hpp"
#include "Mesh/MeshAnimation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< Animation >::doWrite( Animation const & obj )
	{
		bool result = true;

		if ( result )
		{
			result = doWriteChunk( obj.getName(), ChunkType::eName, m_chunk );
		}

		if ( result )
		{
			result = doWriteChunk( real( obj.getLength().count() ) / 1000.0_r, ChunkType::eAnimLength, m_chunk );
		}

		if ( result )
		{
			switch ( obj.getType() )
			{
			case AnimationType::eSkeleton:
				BinaryWriter< SkeletonAnimation >{}.write( static_cast< SkeletonAnimation const & >( obj ), m_chunk );
				break;

			case AnimationType::eMesh:
				BinaryWriter< MeshAnimation >{}.write( static_cast< MeshAnimation const & >( obj ), m_chunk );
				break;

			default:
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Animation >::doParse( Animation & obj )
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
					obj.m_name = name;
				}

				break;

			case ChunkType::eAnimLength:
				result = doParseChunk( length, chunk );
				obj.m_length = Milliseconds( uint64_t( length * 1000 ) );
				break;

			case ChunkType::eSkeletonAnimation:
				BinaryParser< SkeletonAnimation >{}.parse( static_cast< SkeletonAnimation & >( obj ), chunk );
				break;

			case ChunkType::eMeshAnimation:
				BinaryParser< MeshAnimation >{}.parse( static_cast< MeshAnimation & >( obj ), chunk );
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Animation::Animation( AnimationType type
		, Animable & animable
		, String const & name )
		: Named{ name }
		, OwnedBy< Animable >{ animable }
		, m_type{ type }
	{
	}

	Animation::~Animation()
	{
	}

	void Animation::addKeyFrame( AnimationKeyFrameUPtr && keyFrame )
	{
		auto it = std::lower_bound( m_keyframes.begin()
			, m_keyframes.end()
			, keyFrame
			, []( AnimationKeyFrameUPtr const & lhs, AnimationKeyFrameUPtr const & rhs )
			{
				return lhs->getTimeIndex() < rhs->getTimeIndex();
			} );
		m_keyframes.insert( it, std::move( keyFrame ) );
		updateLength();
	}

	void Animation::findKeyFrame( Milliseconds const & time
		, AnimationKeyFrameArray::const_iterator & prv
		, AnimationKeyFrameArray::const_iterator & cur )const
	{
		while ( prv != m_keyframes.begin() && ( *prv )->getTimeIndex() >= time )
		{
			// Time has gone too fast backward.
			--prv;
			--cur;
		}

		auto end = ( m_keyframes.end() - 1 );

		while ( cur != end && ( *cur )->getTimeIndex() < time )
		{
			// Time has gone too fast forward.
			++prv;
			++cur;
		}

		ENSURE( prv != cur );
	}

	void Animation::updateLength()
	{
		for ( auto const & keyFrame : m_keyframes )
		{
			m_length = std::max( m_length, keyFrame->getTimeIndex() );
		}
	}

	//*************************************************************************************************
}
