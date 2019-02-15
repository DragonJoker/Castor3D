#include "Castor3D/Plugin/GeneratorPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

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
