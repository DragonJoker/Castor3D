#include "AnimatedObject.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Animation.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/MovableObject.hpp"
#include "Scene/Animation/AnimationInstance.hpp"

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
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			auto & l_animation = *l_it->second;

			if ( l_animation.GetState() != AnimationState::ePlaying
					&& l_animation.GetState() != AnimationState::ePaused )
			{
				l_animation.Play();
				DoStartAnimation( l_animation );
			}
		}
	}

	void AnimatedObject::StopAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			auto & l_animation = *l_it->second;

			if ( l_animation.GetState() != AnimationState::eStopped )
			{
				l_animation.Stop();
				DoStopAnimation( l_animation );
			}
		}
	}

	void AnimatedObject::PauseAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_it->second->Pause();
		}
	}

	void AnimatedObject::StartAllAnimations()
	{
		DoClearAnimations();

		for ( auto & l_it : m_animations )
		{
			l_it.second->Play();
			DoStartAnimation( *l_it.second );
		}
	}

	void AnimatedObject::StopAllAnimations()
	{
		for ( auto & l_it : m_animations )
		{
			l_it.second->Stop();
		}

		DoClearAnimations();
	}

	void AnimatedObject::PauseAllAnimations()
	{
		for ( auto & l_it : m_animations )
		{
			l_it.second->Pause();
		}
	}

	AnimationInstance & AnimatedObject::GetAnimation( Castor::String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *l_it->second;
	}
}
