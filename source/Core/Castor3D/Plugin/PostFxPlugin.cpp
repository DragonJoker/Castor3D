#include "Castor3D/Plugin/PostFxPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementCUSmartPtr( castor3d, PostFxPlugin )

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
