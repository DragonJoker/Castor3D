#include "Animation.hpp"

namespace castor3d
{
	template< typename AnimableHanlerT >
	AnimationT< AnimableHanlerT >::AnimationT( AnimationType type
		, AnimableT< AnimableHanlerT > & animable
		, castor::String const & name )
		: castor::Named{ name }
		, castor::OwnedBy< AnimableT< AnimableHanlerT > >{ animable }
		, m_type{ type }
	{
	}
		
	template< typename AnimableHanlerT >
	AnimationT< AnimableHanlerT >::~AnimationT()
	{
	}
	
	template< typename AnimableHanlerT >
	void AnimationT< AnimableHanlerT >::addKeyFrame( AnimationKeyFrameUPtr keyFrame )
	{
		auto it = std::lower_bound( m_keyframes.begin()
			, m_keyframes.end()
			, keyFrame
			, []( AnimationKeyFrameUPtr const & lhs, AnimationKeyFrameUPtr const & rhs )
			{
				return lhs->getTimeIndex() < rhs->getTimeIndex();
			} );
		keyFrame->initialise();
		m_keyframes.insert( it, std::move( keyFrame ) );
		updateLength();
	}

	template< typename AnimableHanlerT >
	AnimationKeyFrameArray::iterator AnimationT< AnimableHanlerT >::find( castor::Milliseconds const & time )
	{
		return std::find_if( m_keyframes.begin()
			, m_keyframes.end()
			, [&time]( AnimationKeyFrameUPtr const & lookup )
			{
				return lookup->getTimeIndex() == time;
			} );
	}

	template< typename AnimableHanlerT >
	void AnimationT< AnimableHanlerT >::findKeyFrame( castor::Milliseconds const & time
		, AnimationKeyFrameArray::iterator & prv
		, AnimationKeyFrameArray::iterator & cur )const
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

	template< typename AnimableHanlerT >
	void AnimationT< AnimableHanlerT >::updateLength()
	{
		for ( auto const & keyFrame : m_keyframes )
		{
			m_length = std::max( m_length, keyFrame->getTimeIndex() );
		}
	}
}
