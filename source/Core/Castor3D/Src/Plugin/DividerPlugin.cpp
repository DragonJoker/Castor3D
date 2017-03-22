#include "DividerPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	DividerPlugin::DividerPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eDivider, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	DividerPlugin::~DividerPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
