#include "Castor3D/Plugin/ParticlePlugin.hpp"

#include <CastorUtils/Miscellaneous/DynamicLibrary.hpp>

CU_ImplementSmartPtr( castor3d, ParticlePlugin )

namespace castor3d
{
	ParticlePlugin::ParticlePlugin( castor::DynamicLibraryUPtr library, Engine * engine )
		: Plugin( PluginType::eParticle, std::move( library ), *engine )
	{
		load();
	}

	ParticlePlugin::~ParticlePlugin()noexcept
	{
		unload();
	}
}
