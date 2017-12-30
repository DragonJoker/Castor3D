#include "PostFxPlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	PostFxPlugin::PostFxPlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::ePostEffect, p_library, *engine )
	{
		load();
	}

	PostFxPlugin::~PostFxPlugin()
	{
		unload();
	}
}
