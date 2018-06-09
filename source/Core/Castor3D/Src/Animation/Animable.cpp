#include "Animable.hpp"

#include "Animation.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	Animable::Animable( Scene & scene )
		: OwnedBy< Scene >{ scene }
	{
	}

	Animable::~Animable()
	{
	}

	void Animable::cleanupAnimations()
	{
		m_animations.clear();
	}

	bool Animable::hasAnimation( castor::String const & name )const
	{
		return m_animations.find( name ) != m_animations.end();
	}

	Animation const & Animable::getAnimation( castor::String const & name )const
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + name );
		}

		return *it->second;
	}

	Animation & Animable::getAnimation( castor::String const & name )
	{
		auto it = m_animations.find( name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + name );
		}

		return *it->second;
	}

	void Animable::doAddAnimation( AnimationSPtr && animation )
	{
		m_animations[animation->getName()] = std::move( animation );
	}

	//*************************************************************************************************
}
