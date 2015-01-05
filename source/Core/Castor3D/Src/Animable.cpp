#include "Animable.hpp"
#include "Animation.hpp"

using namespace Castor;

namespace Castor3D
{
	Animable::Animable()
	{
	}

	Animable::~Animable()
	{
	}

	AnimationSPtr Animable::CreateAnimation( Castor::String const & p_strName )
	{
		AnimationSPtr l_return;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_strName );

		if ( l_it == m_mapAnimations.end() )
		{
			l_return = std::make_shared< Animation >( p_strName );
			m_mapAnimations.insert( std::make_pair( p_strName, l_return ) );
		}
		else
		{
			l_return = l_it->second;
		}

		return l_return;
	}

	AnimationSPtr Animable::GetAnimation( Castor::String const & p_strName )
	{
		AnimationSPtr l_return;
		AnimationPtrStrMapIt l_it = m_mapAnimations.find( p_strName );

		if ( l_it != m_mapAnimations.end() )
		{
			l_return = l_it->second;
		}

		return l_return;
	}
}
