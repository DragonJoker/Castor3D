#include "Castor3D/Animation/Animation.hpp"

#include "Castor3D/Animation/AnimationKeyFrame.hpp"
#include "Castor3D/Animation/Skeleton/SkeletonAnimation.hpp"
#include "Castor3D/Animation/Mesh/MeshAnimation.hpp"

using namespace castor;

namespace castor3d
{
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
		keyFrame->initialise();
		m_keyframes.insert( it, std::move( keyFrame ) );
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

	void Animation::findKeyFrame( Milliseconds const & time
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

	void Animation::updateLength()
	{
		for ( auto const & keyFrame : m_keyframes )
		{
			m_length = std::max( m_length, keyFrame->getTimeIndex() );
		}
	}
}
