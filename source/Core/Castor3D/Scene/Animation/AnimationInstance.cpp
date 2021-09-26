#include "Castor3D/Scene/Animation/AnimationInstance.hpp"

#include "Castor3D/Animation/Animation.hpp"

using namespace castor;

namespace castor3d
{
	AnimationInstance::AnimationInstance( AnimatedObject & object, Animation & animation )
		: OwnedBy< AnimatedObject >{ object }
		, m_animation{ animation }
	{
	}

	void AnimationInstance::update( Milliseconds const & elapsed )
	{
		auto length = m_stoppingPoint == 0_ms
			? m_animation.getLength()
			: std::min( m_stoppingPoint, m_animation.getLength() );
		double scale = m_scale;
		auto looped = m_looped;

		if ( m_state != AnimationState::eStopped && length > 0_ms )
		{
			if ( m_state == AnimationState::ePlaying )
			{
				m_currentTime += Milliseconds( int64_t( double( elapsed.count() ) * scale ) );

				if ( m_currentTime >= length )
				{
					if ( !looped )
					{
						m_state = AnimationState::ePaused;
						m_currentTime = length;
					}
					else
					{
						do
						{
							m_currentTime -= length;
						}
						while ( m_currentTime >= length );

						m_currentTime += m_startingPoint;
					}
				}
				else if ( m_currentTime < m_startingPoint )
				{
					if ( !looped )
					{
						m_state = AnimationState::ePaused;
						m_currentTime = m_startingPoint;
					}
					else
					{
						do
						{
							m_currentTime += length;
						}
						while ( m_currentTime < m_startingPoint );
					}
				}
			}

			doUpdate();
		}
	}

	void AnimationInstance::play()
	{
		m_state = AnimationState::ePlaying;
	}

	void AnimationInstance::pause()
	{
		if ( m_state == AnimationState::ePlaying )
		{
			m_state = AnimationState::ePaused;
		}
	}

	void AnimationInstance::stop()
	{
		if ( m_state != AnimationState::eStopped )
		{
			m_state = AnimationState::eStopped;
			m_currentTime = m_startingPoint;
		}
	}

	//*************************************************************************************************
}
