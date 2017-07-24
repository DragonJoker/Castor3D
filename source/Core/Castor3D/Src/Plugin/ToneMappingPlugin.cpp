#include "ToneMappingPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	ToneMappingPlugin::ToneMappingPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin{ PluginType::eToneMapping, p_library, *engine }
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
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
