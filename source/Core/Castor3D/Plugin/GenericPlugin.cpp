#include "GenericPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	GenericPlugin::GenericPlugin( DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eGeneric, library, *engine )
	{
		load();
	}

	GenericPlugin::~GenericPlugin()
	{
		unload();
	}
}
