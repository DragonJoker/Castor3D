#include "AnimatedObject.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Animation.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Skeleton/Bone.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/MovableObject.hpp"
#include "Scene/Animation/AnimationInstance.hpp"
#include "Shader/MatrixFrameVariable.hpp"

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

	void AnimatedObject::Update( real p_tslf )
	{
		for ( auto l_animation : m_playingAnimations )
		{
			l_animation->Update( p_tslf );
		}
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
			auto l_animation = l_it->second;

			if ( l_animation->GetState() != AnimationState::Playing
					&& l_animation->GetState() != AnimationState::Paused )
			{
				l_animation->Play();
				m_playingAnimations.push_back( l_animation );
			}
		}
	}

	void AnimatedObject::StopAnimation( String const & p_name )
	{
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			auto l_animation = l_it->second;

			if ( l_animation->GetState() != AnimationState::Stopped )
			{
				l_animation->Stop();
				m_playingAnimations.erase( std::find( m_playingAnimations.begin(), m_playingAnimations.end(), l_animation ) );
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
		m_playingAnimations.clear();

		for ( auto l_it : m_animations )
		{
			l_it.second->Play();
			m_playingAnimations.push_back( l_it.second );
		}
	}

	void AnimatedObject::StopAllAnimations()
	{
		m_playingAnimations.clear();

		for ( auto l_it : m_animations )
		{
			l_it.second->Stop();
		}
	}

	void AnimatedObject::PauseAllAnimations()
	{
		for ( auto l_it : m_animations )
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
