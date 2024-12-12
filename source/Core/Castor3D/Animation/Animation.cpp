#include "Castor3D/Animation/Animation.hpp"

CU_ImplementSmartPtr( castor3d, Animation )

namespace castor3d
{
	Animation::Animation( Engine & handler
		, AnimationType type
		, Animable & animable
		, castor::String const & name )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ handler}
		, m_animable{ &animable }
		, m_type{ type }
	{
	}

	Animation::Animation( Engine & handler
		, AnimationType type
		, castor::String const & name )
		: castor::Named{ name }
		, castor::OwnedBy< Engine >{ handler }
		, m_type{ type }
	{
	}
	
	void Animation::addKeyFrame( AnimationKeyFrameUPtr keyFrame )
	{
		auto it = std::lower_bound( m_keyframes.begin()
			, m_keyframes.end()
			, keyFrame
			, []( AnimationKeyFrameUPtr const & lhs, AnimationKeyFrameUPtr const & rhs )
			{
				return lhs->getTimeIndex() < rhs->getTimeIndex();
			} );
		keyFrame->initialise();
		m_keyframes.insert( it, castor::move( keyFrame ) );
		updateLength();
	}

	AnimationKeyFrameArray::iterator Animation::find( castor::Milliseconds const & time )
	{
		return std::find_if( m_keyframes.begin()
			, m_keyframes.end()
			, [&time]( AnimationKeyFrameUPtr const & lookup )
			{
				return lookup->getTimeIndex() == time;
			} );
	}

	void Animation::findKeyFrame( castor::Milliseconds const & time
		, AnimationKeyFrameArray::iterator & prv
		, AnimationKeyFrameArray::iterator & cur )const
	{
		if ( m_keyframes.size() > 1 )
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

			CU_Ensure( prv != cur );
		}
	}

	void Animation::updateLength()
	{
		for ( auto const & keyFrame : m_keyframes )
		{
			m_length = std::max( m_length, keyFrame->getTimeIndex() );
		}
	}

	void Animation::cloneInto( Animation & output )const
	{
		for ( auto const & keyFrame : m_keyframes )
		{
			output.m_keyframes.push_back( keyFrame->clone( output ) );
		}

		output.m_length = m_length;
		doCloneInto( output );
	}
}
