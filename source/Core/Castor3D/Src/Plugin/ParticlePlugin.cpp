#include "ParticlePlugin.hpp"

#include <Miscellaneous/DynamicLibrary.hpp>

using namespace Castor;

namespace Castor3D
{
	ParticlePlugin::ParticlePlugin( DynamicLibrarySPtr p_library, Engine * p_engine )
		: Plugin( PluginType::eParticle, p_library, *p_engine )
	{
		if ( m_pfnOnLoad )
		{
			m_pfnOnLoad( GetEngine() );
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
