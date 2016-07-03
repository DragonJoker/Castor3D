#include "PostFxPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	PostFxPlugin::PostFxPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: PluginBase( ePLUGIN_TYPE_POSTFX, p_library, *p_engine )
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
