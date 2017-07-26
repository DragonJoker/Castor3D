#include "GenericPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	GenericPlugin::GenericPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eGeneric, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
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
