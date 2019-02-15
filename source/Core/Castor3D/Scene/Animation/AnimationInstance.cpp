#include "Castor3D/Scene/Animation/AnimationInstance.hpp"

using namespace castor;

namespace castor3d
{
	AnimationInstance::AnimationInstance( AnimatedObject & object, Animation & animation )
		: OwnedBy< AnimatedObject >{ object }
		, m_animation{ animation }
	{
	}

	AnimationInstance::~AnimationInstance()
	{
	}

	void AnimationInstance::update( Milliseconds const & elapsed )
	{
		auto length = m_animation.getLength();
		auto scale = m_scale;
		auto looped = m_looped;

		if ( m_state != AnimationState::eStopped && length > 0_ms )
		{
			if ( m_state == AnimationState::ePlaying )
			{
				m_currentTime += Milliseconds( int64_t( elapsed.count() * scale ) );

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
					}
				}
				else if ( m_currentTime < 0_ms )
				{
					if ( !looped )
					{
						m_state = AnimationState::ePaused;
						m_currentTime = 0_ms;
					}
					else
					{
						do
						{
							m_currentTime += m_animation.getLength();
						}
						while ( m_currentTime < 0_ms );
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
			m_currentTime = 0_ms;
		}
	}

	//*************************************************************************************************
}
