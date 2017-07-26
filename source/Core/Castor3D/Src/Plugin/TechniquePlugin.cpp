#include "TechniquePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	TechniquePlugin::TechniquePlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eTechnique, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
		}
	}

	TechniquePlugin::~TechniquePlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
