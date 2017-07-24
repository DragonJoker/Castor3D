#include "ParticlePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	ParticlePlugin::ParticlePlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eParticle, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine(), this );
		}
	}

	ParticlePlugin::~ParticlePlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( GetEngine() );
		}
	}
}
