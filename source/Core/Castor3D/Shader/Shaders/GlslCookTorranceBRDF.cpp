#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"

namespace castor3d::shader
{
	CookTorranceBRDF::CookTorranceBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: SpecularBRDF{ writer, brdfHelpers }
	{
	}

	sdw::RetVec3 CookTorranceBRDF::compute( sdw::Vec3 const & pradiance
		, sdw::Float const & pintensity
		, sdw::Float const & pNdotL
		, sdw::Float const & pNdotH
		, sdw::Float const & pNdotV
		, sdw::Vec3 const & pF
		, sdw::Float const & proughness )
	{
		if ( !m_compute )
		{
			m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorrance"
				, [this]( sdw::Vec3 const & radiance
					, sdw::Float const & intensity
					, sdw::Float const & NdotL
					, sdw::Float const & NdotH
					, sdw::Float const & NdotV
					, sdw::Vec3 const & F
					, sdw::Float const & roughness )
				{
					// GGX Specular BRDF
					auto D = m_writer.declLocale( "D"
						, m_brdfHelpers.distributionGGX( NdotH
							, roughness * roughness ) );
					auto V = m_writer.declLocale( "V"
						, m_brdfHelpers.visibilitySmithGGXCorrelated( NdotV
							, NdotL
							, roughness ) );

					auto numerator = m_writer.declLocale( "numerator"
						, F * D * V );
					auto denominator = m_writer.declLocale( "denominator"
						, sdw::fma( 4.0_f
							, NdotV * NdotL
							, 0.001_f ) );
					auto specReflectance = m_writer.declLocale( "specReflectance"
						, numerator / denominator );

					m_writer.returnStmt( max( specReflectance * radiance * intensity, vec3( 0.0_f ) ) );
				}
				, sdw::InVec3( m_writer, "radiance" )
				, sdw::InFloat( m_writer, "intensity" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "NdotH" )
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InVec3{ m_writer, "F" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_compute( pradiance
			, pintensity
			, pNdotL
			, pNdotH
			, pNdotV
			, pF
			, proughness );
	}

	SpecularBRDFUPtr CookTorranceBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< SpecularBRDF, CookTorranceBRDF >( writer, brdfHelpers );
	}
}
