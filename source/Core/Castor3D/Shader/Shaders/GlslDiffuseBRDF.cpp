#include "Castor3D/Shader/Shaders/GlslDiffuseBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

CU_ImplementDeleter( castor3d::shader, DiffuseBRDF )

namespace castor3d::shader
{
	DiffuseBRDF::DiffuseBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: m_writer{ writer }
		, m_brdfHelpers{ brdfHelpers }
	{
	}

	sdw::RetVec3 DiffuseBRDF::compute( BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::Vec3 const & lightIntensity
			, sdw::Float const & NdotL )
	{
		if ( !m_compute )
		{
			doGenerate( components, lightSurface );
		}

		return m_compute( components
			, lightSurface
			, lightIntensity
			, NdotL );
	}
}
