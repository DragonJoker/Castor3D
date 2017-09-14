#include "GlslSubsurfaceScattering.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		SubsurfaceScattering::SubsurfaceScattering( GlslWriter & writer )
			: m_writer{ writer }
		{
		}

		void SubsurfaceScattering::declare()
		{
		}
	}
}
