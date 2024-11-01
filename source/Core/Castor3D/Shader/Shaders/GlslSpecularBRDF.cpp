#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

CU_ImplementSmartPtr( castor3d::shader, SpecularBRDF )

namespace castor3d::shader
{
	SpecularBRDF::SpecularBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: m_writer{ writer }
		, m_brdfHelpers{ brdfHelpers }
	{
	}

	sdw::RetVec3 SpecularBRDF::compute( BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, sdw::Float const & NdotL
		, sdw::Float const & NdotH
		, sdw::Float const & roughness )
	{
		if ( !m_compute )
		{
			doGenerate( components, lightSurface );
		}

		return m_compute( components
			, lightSurface
			, radiance
			, intensity
			, NdotL
			, NdotH
			, roughness );
	}
}
