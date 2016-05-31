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
			l_return = DoWriteChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.GetScale(), eCHUNK_TYPE_ANIM_SCALE, m_chunk );
		}

		if ( l_return )
		{
			l_return = DoWriteChunk( p_obj.m_length, eCHUNK_TYPE_ANIM_LENGTH, m_chunk );
		}

		if ( l_return )
		{
			switch ( p_obj.GetType() )
			{
			case AnimationType::Skeleton:
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
			case eCHUNK_TYPE_NAME:
				l_return = DoParseChunk( l_name, l_chunk );

				if ( l_return )
				{
					p_obj.m_name = l_name;
				}

				break;

			case eCHUNK_TYPE_ANIM_SCALE:
				l_return = DoParseChunk( p_obj.m_scale, l_chunk );
				break;

			case eCHUNK_TYPE_ANIM_LENGTH:
				l_return = DoParseChunk( p_obj.m_length, l_chunk );
				break;

			case eCHUNK_TYPE_SKELETON_ANIMATION:
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

	bool Animation::Initialise()
	{
		return DoInitialise();
	}

	void Animation::Update( real p_tslf )
	{
		if ( m_state != AnimationState::Stopped && m_length > 0 )
		{
			if ( m_state == AnimationState::Playing )
			{
				m_currentTime += ( p_tslf * m_scale );

				if ( m_currentTime >= m_length )
				{
					if ( !m_looped )
					{
						m_state = AnimationState::Paused;
						m_currentTime = m_length;
					}
					else
					{
						do
						{
							m_currentTime -= m_length;
						}
						while ( m_currentTime >= m_length );
					}
				}
				else if ( m_currentTime < 0.0_r )
				{
					if ( !m_looped )
					{
						m_state = AnimationState::Paused;
						m_currentTime = 0.0_r;
					}
					else
					{
						do
						{
							m_currentTime += m_length;
						}
						while ( m_currentTime < 0.0_r );
					}
				}
			}

			DoUpdate( p_tslf );
		}
	}

	void Animation::Play()
	{
		m_state = AnimationState::Playing;
	}

	void Animation::Pause()
	{
		if ( m_state == AnimationState::Playing )
		{
			m_state = AnimationState::Paused;
		}
	}

	void Animation::Stop()
	{
		if ( m_state != AnimationState::Stopped )
		{
			m_state = AnimationState::Stopped;
			m_currentTime = 0.0;
		}
	}

	//*************************************************************************************************
}
