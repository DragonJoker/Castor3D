#include "GenericPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	GenericPlugin::GenericPlugin( DynamicLibrarySPtr p_pLibrary, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_GENERIC, p_pLibrary, p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( m_engine );
		}
	}

	GenericPlugin::~GenericPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( m_engine );
		}
	}
}
