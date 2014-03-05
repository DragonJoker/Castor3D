#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Animable.hpp"
#include "Castor3D/Animation.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

Animable :: Animable()
{
}

Animable :: ~Animable()
{
}

AnimationSPtr Animable :: CreateAnimation( Castor::String const & p_strName )
{
	AnimationSPtr l_pReturn = map::find_or_null( m_mapAnimations, p_strName );

	if( !l_pReturn )
	{
		l_pReturn = std::make_shared< Animation >( p_strName );
		m_mapAnimations.insert( std::make_pair( p_strName, l_pReturn ) );
	}

	return l_pReturn;
}

AnimationSPtr Animable :: GetAnimation( Castor::String const & p_strName )
{
	return map::find_or_null( m_mapAnimations, p_strName );
}

//*************************************************************************************************
