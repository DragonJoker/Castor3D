#include "GeneratorPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	GeneratorPlugin::GeneratorPlugin( DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eGenerator, library, *engine )
	{
		load();
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
		unload();
	}
}
