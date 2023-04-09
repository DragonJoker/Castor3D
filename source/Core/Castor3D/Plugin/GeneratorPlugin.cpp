#include "Castor3D/Plugin/GeneratorPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( castor3d, GeneratorPlugin )

namespace castor3d
{
	GeneratorPlugin::GeneratorPlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin( PluginType::eGenerator, std::move( library ), *engine )
	{
		load();
	}

	GeneratorPlugin::~GeneratorPlugin()
	{
		unload();
	}
}
