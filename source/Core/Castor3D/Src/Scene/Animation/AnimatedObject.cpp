#include "AnimatedObject.hpp"

#include "Mesh/Mesh.hpp"
#include "Scene/Geometry.hpp"

using namespace castor;

namespace castor3d
{
	AnimatedObject::AnimatedObject( String const & p_name )
		: Named{ p_name }
	{
	}

	AnimatedObject :: ~AnimatedObject()
	{
	}

	void AnimatedObject::addAnimation( String const & p_name )
	{
		doAddAnimation( p_name );
	}

	void AnimatedObject::startAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

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

	void AnimatedObject::stopAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

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

	void AnimatedObject::pauseAnimation( String const & p_name )
	{
		auto it = m_animations.find( p_name );

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

	AnimationInstance & AnimatedObject::getAnimation( castor::String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *it->second;
	}
}
