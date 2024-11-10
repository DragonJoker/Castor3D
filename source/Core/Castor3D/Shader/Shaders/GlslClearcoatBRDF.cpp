#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

CU_ImplementDeleter( castor3d::shader, ClearcoatBRDF )

namespace castor3d::shader
{
	ClearcoatBRDF::ClearcoatBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: m_writer{ writer }
		, m_brdfHelpers{ brdfHelpers }
	{}

	sdw::RetVec3 ClearcoatBRDF::compute( BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::Vec3 const & radiance
		, sdw::Float const & intensity
		, sdw::Float const & NdotL
		, sdw::Float const & NdotH )
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
			, NdotH );
	}

	ClearcoatBRDFPtr ClearcoatBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUnique< ClearcoatBRDF >( writer, brdfHelpers );
	}

	void ClearcoatBRDF::doGenerate( BlendComponents const & pcomponents
		, LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeClearcoat"
			, [this]( BlendComponents const & components
				, LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto roughness = components.clearcoatRoughness;
				auto NdotV = m_writer.declLocale( "NdotV"
					, lightSurface.NdotV().value() );
				auto F = m_writer.declLocale( "F"
					, lightSurface.spcF().value() );
				auto alphaRoughness = m_writer.declLocale( "alphaRoughness"
					, roughness * roughness );

				// GGX Specular BRDF
				auto D = m_writer.declLocale( "D"
					, m_brdfHelpers.distributionGGX( NdotH
						, alphaRoughness * alphaRoughness ) );
				auto V = m_writer.declLocale( "V"
					, m_brdfHelpers.visibilitySmithGGXCorrelated( NdotV
						, NdotL
						, alphaRoughness ) );

				auto numerator = m_writer.declLocale( "numerator"
					, F * D * V );
				auto denominator = m_writer.declLocale( "denominator"
					, sdw::fma( 4.0_f
						, NdotV * NdotL
						, 0.001_f ) );
				auto reflectance = m_writer.declLocale( "reflectance"
					, numerator / denominator );

				m_writer.returnStmt( max( reflectance * radiance * intensity, vec3( 0.0_f ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}
}
