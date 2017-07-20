#include "AnimatedObject.hpp"

#include "Mesh/Mesh.hpp"
#include "Scene/Geometry.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedObject::AnimatedObject( String const & p_name )
		: Named{ p_name }
	{
	}

	AnimatedObject :: ~AnimatedObject()
	{
	}

	void AnimatedObject::AddAnimation( String const & p_name )
	{
		DoAddAnimation( p_name );
	}

	void AnimatedObject::StartAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it != m_animations.end() )
		{
			auto & animation = *it->second;

			if ( animation.GetState() != AnimationState::ePlaying )
			{
				if ( animation.GetState() != AnimationState::ePaused )
				{
					animation.Play();
					DoStartAnimation( animation );
				}
				else
				{
					animation.Play();
				}
			}
		}
	}

	void AnimatedObject::StopAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it != m_animations.end() )
		{
			auto & animation = *it->second;

			if ( animation.GetState() != AnimationState::eStopped )
			{
				animation.Stop();
				DoStopAnimation( animation );
			}
		}
	}

	void AnimatedObject::PauseAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it != m_animations.end() )
		{
			it->second->Pause();
		}
	}

	void AnimatedObject::StartAllAnimations()
	{
		DoClearAnimations();

		for ( auto & it : m_animations )
		{
			it.second->Play();
			DoStartAnimation( *it.second );
		}
	}

	void AnimatedObject::StopAllAnimations()
	{
		for ( auto & it : m_animations )
		{
			it.second->Stop();
		}

		DoClearAnimations();
	}

	void AnimatedObject::PauseAllAnimations()
	{
		for ( auto & it : m_animations )
		{
			it.second->Pause();
		}
	}

	AnimationInstance & AnimatedObject::GetAnimation( Castor::String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *it->second;
	}
}
