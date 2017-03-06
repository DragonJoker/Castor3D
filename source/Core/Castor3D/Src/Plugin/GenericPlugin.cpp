#include "GenericPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	GenericPlugin::GenericPlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eGeneric, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
		}
	}

	GenericPlugin::~GenericPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
