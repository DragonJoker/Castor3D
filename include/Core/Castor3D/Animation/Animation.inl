#include "Castor3D/Animation/Animation.hpp"

namespace castor3d
{
	template< typename OwnerT >
	AnimationT< OwnerT >::AnimationT( AnimationType type
		, Animable< OwnerT > & animable
		, castor::String const & name )
		: Named{ name }
		, OwnedBy< Animable >{ animable }
		, m_type{ type }
	{
	}
		
	template< typename OwnerT >
	AnimationT< OwnerT >::~AnimationT()
	{
	}
	
	template< typename OwnerT >
	void AnimationT< OwnerT >::addKeyFrame( AnimationKeyFrameUPtr keyFrame )
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

	template< typename OwnerT >
	AnimationKeyFrameArray::iterator AnimationT< OwnerT >::find( castor::Milliseconds const & time )
	{
		return std::find_if( m_keyframes.begin()
			, m_keyframes.end()
			, [&time]( AnimationKeyFrameUPtr const & lookup )
			{
				return lookup->getTimeIndex() == time;
			} );
	}

	template< typename OwnerT >
	void AnimationT< OwnerT >::findKeyFrame( castor::Milliseconds const & time
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

	template< typename OwnerT >
	void AnimationT< OwnerT >::updateLength()
	{
		for ( auto const & keyFrame : m_keyframes )
		{
			m_length = std::max( m_length, keyFrame->getTimeIndex() );
		}
	}
}
