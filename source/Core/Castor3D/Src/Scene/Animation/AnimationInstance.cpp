#include "AnimationInstance.hpp"

#include "Animation/Animation.hpp"

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

	void AnimationInstance::Update( real p_tslf )
	{
		auto l_length{ m_animation.GetLength() };
		auto l_scale{ m_scale };
		auto l_looped{ m_looped };

		if ( m_state != AnimationState::Stopped && l_length > 0 )
		{
			if ( m_state == AnimationState::Playing )
			{
				m_currentTime += ( p_tslf * l_scale );

				if ( m_currentTime >= l_length )
				{
					if ( !l_looped )
					{
						m_state = AnimationState::Paused;
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
				else if ( m_currentTime < 0.0_r )
				{
					if ( !l_looped )
					{
						m_state = AnimationState::Paused;
						m_currentTime = 0.0_r;
					}
					else
					{
						do
						{
							m_currentTime += m_animation.GetLength();
						}
						while ( m_currentTime < 0.0_r );
					}
				}
			}

			DoUpdate( p_tslf );
		}
	}

	void AnimationInstance::Play()
	{
		m_state = AnimationState::Playing;
	}

	void AnimationInstance::Pause()
	{
		if ( m_state == AnimationState::Playing )
		{
			m_state = AnimationState::Paused;
		}
	}

	void AnimationInstance::Stop()
	{
		if ( m_state != AnimationState::Stopped )
		{
			m_state = AnimationState::Stopped;
			m_currentTime = 0.0;
		}
	}

	//*************************************************************************************************
}
