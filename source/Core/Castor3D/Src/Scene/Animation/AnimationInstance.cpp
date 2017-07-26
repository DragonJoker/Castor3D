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

	void AnimationInstance::Update( Milliseconds const & p_tslf )
	{
		auto length = m_animation.GetLength();
		auto scale = m_scale;
		auto looped = m_looped;

		if ( m_state != AnimationState::eStopped && length > 0_ms )
		{
			if ( m_state == AnimationState::ePlaying )
			{
				m_currentTime += Milliseconds( int64_t( p_tslf.count() * scale ) );

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
