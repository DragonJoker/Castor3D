#include "Castor3D/Plugin/DividerPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementCUSmartPtr( castor3d, DividerPlugin )

namespace castor3d
{
	DividerPlugin::DividerPlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin( PluginType::eDivider, std::move( library ), *engine )
	{
		load();
	}

	DividerPlugin::~DividerPlugin()
	{
		unload();
	}
}
