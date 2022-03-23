#include "Castor3D/Plugin/DividerPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	DividerPlugin::DividerPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eDivider, library, *engine )
	{
		load();
	}

	DividerPlugin::~DividerPlugin()
	{
		unload();
	}
}
