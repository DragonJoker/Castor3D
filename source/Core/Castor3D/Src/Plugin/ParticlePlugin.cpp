#include "ParticlePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace castor;

namespace castor3d
{
	ParticlePlugin::ParticlePlugin( DynamicLibrarySPtr p_library, Engine * engine )
		: Plugin( PluginType::eParticle, p_library, *engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( getEngine(), this );
		}
	}

	ParticlePlugin::~ParticlePlugin()
	{
		if ( m_pfnOnUnload )
		{
			m_pfnOnUnload( getEngine() );
		}
	}
}
