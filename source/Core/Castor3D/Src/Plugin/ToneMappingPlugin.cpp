#include "ToneMappingPlugin.hpp"

#if defined( _WIN32 )
#	include <Windows.h>
#else
#	include <dlfcn.h>
#endif

#include <DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	ToneMappingPlugin::ToneMappingPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: PluginBase{ ePLUGIN_TYPE_TONEMAPPING, p_library, *p_engine }
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	ToneMappingPlugin::~ToneMappingPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
