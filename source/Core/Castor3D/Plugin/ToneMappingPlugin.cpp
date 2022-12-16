#include "Castor3D/Plugin/ToneMappingPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	ToneMappingPlugin::ToneMappingPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin{ PluginType::eToneMapping, library, *engine }
	{
		load();
	}

	ToneMappingPlugin::~ToneMappingPlugin()
	{
		unload();
	}
}
