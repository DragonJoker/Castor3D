#include "ParticlePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ParticlePlugin::ParticlePlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eParticle, p_library, *engine )
	{
		load();
	}

	ParticlePlugin::~ParticlePlugin()
	{
		unload();
	}
}
