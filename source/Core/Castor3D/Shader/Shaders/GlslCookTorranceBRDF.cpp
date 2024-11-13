#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

namespace castor3d::shader
{
	CookTorranceBRDF::CookTorranceBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: SpecularBRDF{ writer, brdfHelpers }
	{
	}

	SpecularBRDFPtr CookTorranceBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< SpecularBRDF, CookTorranceBRDF >( writer, brdfHelpers );
	}

	void CookTorranceBRDF::doGenerate( BlendComponents const & pcomponents )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorranceSpecular"
			, [this]( BlendComponents const & components
				, sdw::Vec3 const & N
				, sdw::Vec3 const & /*L*/
				, sdw::Vec3 const & /*H*/
				, sdw::Vec3 const & V
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto NdotV = m_writer.declLocale( "NdotV"
					, max( 0.0_f, dot( N, V ) ) );

				// GGX Specular BRDF
				auto D = m_writer.declLocale( "D"
					, m_brdfHelpers.distributionGGX( NdotH
						, components.alphaRoughness * components.alphaRoughness ) );
				auto Vis = m_writer.declLocale( "Vis"
					, m_brdfHelpers.visibilitySmithGGXCorrelated( NdotV
						, NdotL
						, components.alphaRoughness ) );

				auto numerator = m_writer.declLocale( "numerator"
					, D * Vis );
				auto denominator = m_writer.declLocale( "denominator"
					, sdw::fma( 4.0_f
						, NdotV * NdotL
						, 0.001_f ) );
				auto specReflectance = m_writer.declLocale( "specReflectance"
					, numerator / denominator );

				m_writer.returnStmt( max( vec3( specReflectance ), vec3( 0.0_f ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, sdw::InVec3{ m_writer, "N" }
			, sdw::InVec3{ m_writer, "L" }
			, sdw::InVec3{ m_writer, "H" }
			, sdw::InVec3{ m_writer, "V" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}
}
