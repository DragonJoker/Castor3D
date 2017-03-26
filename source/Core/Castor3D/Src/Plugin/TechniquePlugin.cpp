#include "TechniquePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	TechniquePlugin::TechniquePlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eTechnique, p_library, *p_engine )
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
