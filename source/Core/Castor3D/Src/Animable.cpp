#include "Animable.hpp"
#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	Animable::Animable( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
	{
	}

	Animable::~Animable()
	{
	}

	AnimationSPtr Animable::CreateAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_name );

		if ( l_it == m_mapAnimations.end() )
		{
			l_return = std::make_shared< Animation >( p_name );
			m_mapAnimations.insert( std::make_pair( p_name, l_return ) );
		}
		else
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	AnimationSPtr Animable::GetAnimation( Castor::String const & p_name )
	{
		AnimationSPtr l_return;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_name );

		if ( l_it != m_mapAnimations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}
}
