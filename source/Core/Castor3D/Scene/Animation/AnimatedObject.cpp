#include "Castor3D/Scene/Animation/AnimatedObject.hpp"

CU_ImplementSmartPtr( c3d, AnimatedObject )

namespace c3d
{
	AnimatedObject::AnimatedObject( AnimationType kind
		, String const & name )
		: Named{ name }
		, m_kind{ kind }
	{
	}

	void AnimatedObject::addAnimation( String const & name )
	{
		doAddAnimation( name );
	}

	void AnimatedObject::startAnimation( String const & name )
	{
		if ( auto it = m_animations.find( name );
			it != m_animations.end() )
		{
			auto & animation = *it->second;

			if ( animation.getState() != AnimationState::ePlaying )
			{
				if ( animation.getState() != AnimationState::ePaused )
				{
					animation.play();
					doStartAnimation( animation );
				}
				else
				{
					animation.play();
				}
			}
		}
	}

	void AnimatedObject::stopAnimation( String const & name )
	{
		if ( auto it = m_animations.find( name );
			it != m_animations.end() )
		{
			auto & animation = *it->second;

			if ( animation.getState() != AnimationState::eStopped )
			{
				animation.stop();
				doStopAnimation( animation );
			}
		}
	}

	void AnimatedObject::pauseAnimation( String const & name )
	{
		if ( auto it = m_animations.find( name );
			it != m_animations.end() )
		{
			it->second->pause();
		}
	}

	void AnimatedObject::startAllAnimations()
	{
		doClearAnimations();

		for ( auto const & [nm, anim] : m_animations )
		{
			anim->play();
			doStartAnimation( *anim );
		}
	}

	void AnimatedObject::stopAllAnimations()
	{
		for ( auto const & [nm, anim] : m_animations )
		{
			anim->stop();
		}

		doClearAnimations();
	}

	void AnimatedObject::pauseAllAnimations()
	{
		for ( auto const & [nm, anim] : m_animations )
		{
			anim->pause();
		}
	}

	AnimationInstance & AnimatedObject::getAnimation( String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( "No animation named " + toUtf8( name ) );
		}

		return *it->second;
	}
}
