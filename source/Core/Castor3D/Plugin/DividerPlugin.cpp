#include "Castor3D/Plugin/DividerPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	DividerPlugin::DividerPlugin( DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eDivider, library, *engine )
	{
		load();
	}

	DividerPlugin::~DividerPlugin()
	{
		unload();
	}
}
