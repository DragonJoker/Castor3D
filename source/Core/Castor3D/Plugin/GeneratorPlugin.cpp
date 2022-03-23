#include "Castor3D/Plugin/GeneratorPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	GeneratorPlugin::GeneratorPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eGenerator, library, *engine )
	{
		load();
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
		unload();
	}
}
