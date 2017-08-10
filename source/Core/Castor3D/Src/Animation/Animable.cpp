#include "Animable.hpp"

#include "Animation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Animable::Animable( Scene & p_scene )
		: OwnedBy< Scene >{ p_scene }
	{
	}

	Animable::~Animable()
	{
	}

	void Animable::cleanupAnimations()
	{
		m_animations.clear();
	}

	bool Animable::hasAnimation( castor::String const & p_name )const
	{
		return m_animations.find( p_name ) != m_animations.end();
	}

	Animation const & Animable::getAnimation( castor::String const & p_name )const
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *it->second;
	}

	Animation & Animable::getAnimation( castor::String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *it->second;
	}

	void Animable::doAddAnimation( AnimationSPtr && p_animation )
	{
		m_animations[p_animation->getName()] = std::move( p_animation );
	}

	//*************************************************************************************************
}
