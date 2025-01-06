#include "Castor3D/Scene/Animation/AnimationInstance.hpp"

#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Animation/Animation.hpp"

CU_ImplementSmartPtr( castor3d, AnimationInstance )

namespace castor3d
{
	AnimationInstance::AnimationInstance( AnimatedObject & object
		, Animation & animation
		, bool looped )
		: castor::OwnedBy< AnimatedObject >{ object }
		, m_animation{ &animation }
		, m_looped{ looped }
		, m_totalTime{ m_animation->getLength() }
	{
	}

	void AnimationInstance::update( castor::Milliseconds const & elapsed )
	{
		if ( m_totalTime == castor::Milliseconds{ std::numeric_limits< int64_t >::max() } )
		{
			m_totalTime = m_animation->getLength();
		}

		auto length = m_stoppingPoint == 0_ms
			? m_totalTime
			: std::min( m_stoppingPoint, m_totalTime );
		double scale = m_scale;

		if ( m_state != AnimationState::eStopped && length > 0_ms )
		{
			if ( m_state == AnimationState::ePlaying )
			{
				m_currentTime += castor::Milliseconds( int64_t( double( elapsed.count() ) * scale ) );

				if ( m_currentTime >= length )
				{
					if ( !m_looped )
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
					if ( !m_looped )
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

	void AnimationInstance::setTotalLength( castor::Milliseconds const & time )
	{
		m_totalTime = time;
	}

	//*************************************************************************************************
}
