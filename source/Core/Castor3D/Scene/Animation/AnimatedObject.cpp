#include "Castor3D/Scene/Animation/AnimatedObject.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Scene/Geometry.hpp"

CU_ImplementSmartPtr( castor3d, AnimatedObject )

namespace castor3d
{
	AnimatedObject::AnimatedObject( AnimationType kind
		, castor::String const & name )
		: castor::Named{ name }
		, m_kind{ kind }
	{
	}

	void AnimatedObject::addAnimation( castor::String const & name )
	{
		doAddAnimation( name );
	}

	void AnimatedObject::startAnimation( castor::String const & name )
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

	void AnimatedObject::stopAnimation( castor::String const & name )
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

	void AnimatedObject::pauseAnimation( castor::String const & name )
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

	AnimationInstance & AnimatedObject::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CU_Exception( "No animation named " + castor::toUtf8( name ) );
		}

		return *it->second;
	}
}
