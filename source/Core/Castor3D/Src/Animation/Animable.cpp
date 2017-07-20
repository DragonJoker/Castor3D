#include "Animable.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Animable::Animable( Scene & p_scene )
		: OwnedBy< Scene >{ p_scene }
	{
	}

	Animable::~Animable()
	{
	}

	void Animable::CleanupAnimations()
	{
		m_animations.clear();
	}

	bool Animable::HasAnimation( Castor::String const & p_name )const
	{
		return m_animations.find( p_name ) != m_animations.end();
	}

	Animation const & Animable::GetAnimation( Castor::String const & p_name )const
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *it->second;
	}

	Animation & Animable::GetAnimation( Castor::String const & p_name )
	{
		auto it = m_animations.find( p_name );

		if ( it == m_animations.end() )
		{
			CASTOR_EXCEPTION( cuT( "No animation named " ) + p_name );
		}

		return *it->second;
	}

	void Animable::DoAddAnimation( AnimationSPtr && p_animation )
	{
		m_animations[p_animation->GetName()] = std::move( p_animation );
	}

	//*************************************************************************************************
}
