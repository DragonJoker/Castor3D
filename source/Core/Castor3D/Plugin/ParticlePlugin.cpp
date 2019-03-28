#include "Castor3D/Plugin/ParticlePlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ParticlePlugin::ParticlePlugin( DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eParticle, library, *engine )
	{
		load();
	}

	ParticlePlugin::~ParticlePlugin()
	{
		unload();
	}
}
