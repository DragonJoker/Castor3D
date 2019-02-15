#include "TechniquePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	TechniquePlugin::TechniquePlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eTechnique, p_library, *engine )
	{
		load();
	}

	TechniquePlugin::~TechniquePlugin()
	{
		unload();
	}
}
