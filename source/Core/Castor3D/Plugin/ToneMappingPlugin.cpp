#include "Castor3D/Plugin/ToneMappingPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementCUSmartPtr( castor3d, ToneMappingPlugin )

namespace castor3d
{
	ToneMappingPlugin::ToneMappingPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin{ PluginType::eToneMapping, library, *engine }
	{
		load();
	}

	ToneMappingPlugin::~ToneMappingPlugin()
	{
		unload();
	}
}
