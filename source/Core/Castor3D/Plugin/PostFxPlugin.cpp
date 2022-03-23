#include "Castor3D/Plugin/PostFxPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	PostFxPlugin::PostFxPlugin( castor::DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::ePostEffect, p_library, *engine )
	{
		load();
	}

	PostFxPlugin::~PostFxPlugin()
	{
		unload();
	}
}
