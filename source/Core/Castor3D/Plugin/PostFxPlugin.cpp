#include "Castor3D/Plugin/PostFxPlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( castor3d, PostFxPlugin )

namespace castor3d
{
	PostFxPlugin::PostFxPlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin( PluginType::ePostEffect, castor::move( library ), *engine )
	{
		load();
	}

	PostFxPlugin::~PostFxPlugin()noexcept
	{
		unload();
	}
}
