#include "Castor3D/Plugin/ParticlePlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

namespace castor3d
{
	ParticlePlugin::ParticlePlugin( castor::DynamicLibrarySPtr library, Engine * engine )
		: Plugin( PluginType::eParticle, library, *engine )
	{
		load();
	}

	ParticlePlugin::~ParticlePlugin()
	{
		unload();
	}
}
