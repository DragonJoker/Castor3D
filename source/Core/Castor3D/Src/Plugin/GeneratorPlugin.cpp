#include "GeneratorPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	GeneratorPlugin::GeneratorPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eGenerator, p_library, *engine )
	{
		load();
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
		unload();
	}
}
