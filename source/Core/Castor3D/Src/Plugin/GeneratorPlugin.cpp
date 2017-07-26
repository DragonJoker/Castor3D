#include "GeneratorPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	GeneratorPlugin::GeneratorPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eGenerator, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
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
