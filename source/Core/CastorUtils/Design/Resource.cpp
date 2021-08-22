/*
See LICENSE file in root folder
*/
#include "CastorUtils/Design/Resource.hpp"

CU_ImplementCUSmartPtr( castor, Resource )

namespace castor
{
	Resource::Resource( String const & name )
		: Named{ name }
	{
	}

	void Resource::initialise()
	{
		if ( !m_initialised )
		{
			doInitialise();
			m_initialised = true;
			onInitialised( *this );
		}
	}

	void Resource::cleanup()
	{
		if ( !m_initialised.exchange( false ) )
		{
			doCleanup();
		}
	}

	void Resource::reinitialise()
	{
		if ( m_initialised )
		{
			cleanup();
			initialise();
		}
	}
}
