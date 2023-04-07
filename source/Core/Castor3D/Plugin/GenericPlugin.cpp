#include "Castor3D/Plugin/GenericPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementCUSmartPtr( castor3d, GenericPlugin )

namespace castor3d
{
	GenericPlugin::GenericPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eGeneric, library, *engine )
	{
		load();
	}

	GenericPlugin::~GenericPlugin()
	{
		unload();
	}
}
