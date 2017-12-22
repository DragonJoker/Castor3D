#include "DividerPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	DividerPlugin::DividerPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eDivider, p_library, *engine )
	{
		load();
	}

	DividerPlugin::~DividerPlugin()
	{
		unload();
	}
}
