#include "Castor3D/Plugin/GenericPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( castor3d, GenericPlugin )

namespace castor3d
{
	GenericPlugin::GenericPlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin( PluginType::eGeneric, castor::move( library ), *engine )
	{
		load();
	}

	GenericPlugin::~GenericPlugin()noexcept
	{
		unload();
	}
}
