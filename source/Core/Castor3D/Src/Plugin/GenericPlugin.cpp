#include "GenericPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	GenericPlugin::GenericPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eGeneric, p_library, *engine )
	{
		load();
	}

	GenericPlugin::~GenericPlugin()
	{
		unload();
	}
}
