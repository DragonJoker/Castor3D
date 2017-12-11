#include "AnimatedObject.hpp"

#include "Mesh/Mesh.hpp"
#include "Scene/Geometry.hpp"

using namespace castor;

namespace castor3d
{
	AnimatedObject::AnimatedObject( String const & name )
		: Named{ name }
	{
	}

	AnimatedObject :: ~AnimatedObject()
	{
	}

	void AnimatedObject::addAnimation( String const & name )
	{
		doAddAnimation( name );
	}

	void AnimatedObject::startAnimation( String const & name )
	{
		auto it = m_animations.find( name );

		if ( it != m_animations.end() )
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
		auto it = m_animations.find( name );

		if ( it != m_animations.end() )
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
		auto it = m_animations.find( name );

		if ( it != m_animations.end() )
		{
			it->second->pause();
		}
	}

	void AnimatedObject::startAllAnimations()
	{
		doClearAnimations();

		for ( auto & it : m_animations )
		{
			it.second->play();
			doStartAnimation( *it.second );
		}
	}

	void AnimatedObject::stopAllAnimations()
	{
		for ( auto & it : m_animations )
		{
			it.second->stop();
		}

		doClearAnimations();
	}

	void AnimatedObject::pauseAllAnimations()
	{
		for ( auto & it : m_animations )
		{
			it.second->pause();
		}
	}

	AnimationInstance & AnimatedObject::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + name );
		}

		return *it->second;
	}
}
