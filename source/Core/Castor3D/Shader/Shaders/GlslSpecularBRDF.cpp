#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

CU_ImplementDeleter( castor3d::shader, SpecularBRDF )

namespace castor3d::shader
{
	SpecularBRDF::SpecularBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: m_writer{ writer }
		, m_brdfHelpers{ brdfHelpers }
	{
	}

	sdw::RetVec3 SpecularBRDF::compute( BlendComponents const & components
		, sdw::Vec3 const & N
		, sdw::Vec3 const & L
		, sdw::Vec3 const & H
		, sdw::Vec3 const & V
		, sdw::Float const & NdotL
		, sdw::Float const & NdotH )
	{
		if ( !m_compute )
		{
			doGenerate( components );
		}

		return m_compute( components
			, N
			, L
			, H
			, V
			, NdotL
			, NdotH );
	}
}
