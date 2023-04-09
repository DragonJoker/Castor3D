#include "Castor3D/Plugin/ToneMappingPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( castor3d, ToneMappingPlugin )

namespace castor3d
{
	ToneMappingPlugin::ToneMappingPlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin{ PluginType::eToneMapping, std::move( library ), *engine }
	{
		load();
	}

	ToneMappingPlugin::~ToneMappingPlugin()
	{
		unload();
	}
}
