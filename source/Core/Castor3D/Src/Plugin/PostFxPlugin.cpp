#include "PostFxPlugin.hpp"

#if defined( CASTOR_PLATFORM_WINDOWS )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	PostFxPlugin::PostFxPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::ePostEffect, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	PostFxPlugin::~PostFxPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
