#include "AnimationInstance.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimationInstance::AnimationInstance( AnimatedObject & p_object, Animation const & p_animation )
		: OwnedBy< AnimatedObject >{ p_object }
		, m_animation{ p_animation }
	{
	}

	AnimationInstance::~AnimationInstance()
	{
	}

	void AnimationInstance::Update( std::chrono::milliseconds const & p_tslf )
	{
		auto l_length = m_animation.GetLength();
		auto l_scale = m_scale;
		auto l_looped = m_looped;

		if ( m_state != AnimationState::eStopped && l_length > 0_ms )
		{
			if ( m_state == AnimationState::ePlaying )
			{
				m_currentTime += std::chrono::milliseconds( int64_t( p_tslf.count() * l_scale ) );

				if ( m_currentTime >= l_length )
				{
					if ( !l_looped )
					{
						m_state = AnimationState::ePaused;
						m_currentTime = l_length;
					}
					else
					{
						do
						{
							m_currentTime -= l_length;
						}
						while ( m_currentTime >= l_length );
					}
				}
				else if ( m_currentTime < 0_ms )
				{
					if ( !l_looped )
					{
						m_state = AnimationState::ePaused;
						m_currentTime = 0_ms;
					}
					else
					{
						do
						{
							m_currentTime += m_animation.GetLength();
						}
						while ( m_currentTime < 0_ms );
					}
				}
			}

			DoUpdate();
		}
	}

	void AnimationInstance::Play()
	{
		m_state = AnimationState::ePlaying;
	}

	void AnimationInstance::Pause()
	{
		if ( m_state == AnimationState::ePlaying )
		{
			m_state = AnimationState::ePaused;
		}
	}

	void AnimationInstance::Stop()
	{
		if ( m_state != AnimationState::eStopped )
		{
			m_state = AnimationState::eStopped;
			m_currentTime = 0_ms;
		}
	}

	//*************************************************************************************************
}
