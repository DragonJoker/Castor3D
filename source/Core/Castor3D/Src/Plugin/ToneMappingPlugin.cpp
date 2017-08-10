#include "ToneMappingPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ToneMappingPlugin::ToneMappingPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin{ PluginType::eToneMapping, p_library, *engine }
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( getEngine(), this );
		}
	}

	ToneMappingPlugin::~ToneMappingPlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( getEngine() );
		}
	}
}
