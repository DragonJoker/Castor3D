#include "PostFxPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	PostFxPlugin::PostFxPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::ePostEffect, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
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
