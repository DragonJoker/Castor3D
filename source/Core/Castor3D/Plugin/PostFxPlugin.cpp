#include "Castor3D/Plugin/PostFxPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	PostFxPlugin::PostFxPlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::ePostEffect, library, *engine )
	{
		load();
	}

	PostFxPlugin::~PostFxPlugin()
	{
		unload();
	}
}
