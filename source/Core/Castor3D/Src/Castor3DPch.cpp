#include "Castor3DPch.hpp"

#include <Point.hpp>
#include <SquareMatrix.hpp>

#if defined( VLD_AVAILABLE )
#	include <vld.h>
#elif defined( _MSC_VER )
#	define _CRTDBG_MAP_ALLOC
#	include <cstdlib>
#	include <crtdbg.h>
#endif

namespace Castor
{
	OwnedBy< Castor3D::Engine >::OwnedBy( Castor3D::Engine & p_owner )
		: m_owner( p_owner )
	{
	}

	OwnedBy< Castor3D::Engine >::~OwnedBy()
	{
	}

	Castor3D::Engine * OwnedBy< Castor3D::Engine >::GetOwner()const
	{
		return &m_owner;
	}
}