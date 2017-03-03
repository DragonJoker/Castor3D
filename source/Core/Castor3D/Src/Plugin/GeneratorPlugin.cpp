#include "GeneratorPlugin.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	GeneratorPlugin::GeneratorPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eGenerator, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
