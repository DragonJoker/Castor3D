#include "Castor3D/Plugin/ToneMappingPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ToneMappingPlugin::ToneMappingPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin{ PluginType::eToneMapping, p_library, *engine }
	{
		load();
	}

	ToneMappingPlugin::~ToneMappingPlugin()
	{
		unload();
	}
}
