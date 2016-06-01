#include "Animable.hpp"

#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	//*************************************************************************************************

	Animable::Animable( Engine & p_engine )
		: OwnedBy< Engine >{ p_engine }
	{
	}

	Animable::~Animable()
	{
	}

	void Animable::CleanupAnimations()
	{
		m_animations.clear();
	}

	AnimationSPtr Animable::GetAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		auto l_it = m_animations.find( p_name );

		if ( l_it != m_animations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	void Animable::DoAddAnimation( AnimationSPtr p_animation )
	{
		m_animations[p_animation->GetName()] = p_animation;
	}

	//*************************************************************************************************
}
