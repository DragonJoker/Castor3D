#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
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

	SpecularBRDFPtr SpecularBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUnique< SpecularBRDF >( writer, brdfHelpers );
	}

	void SpecularBRDF::doGenerate( BlendComponents const & pcomponents )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeSpecularBRDF"
			, [this]( BlendComponents const & components
				, sdw::Vec3 const & N
				, sdw::Vec3 const & /*L*/
				, sdw::Vec3 const & /*H*/
				, sdw::Vec3 const & V
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto NdotV = m_writer.declLocale( "NdotV"
					, clamp( dot( N, V ), 0.0_f, 1.0_f ) );

				// GGX Specular BRDF
				auto D = m_writer.declLocale( "D"
					, m_brdfHelpers.distributionGGX( NdotH
						, components.alphaRoughness ) );
				auto Vis = m_writer.declLocale( "Vis"
					, m_brdfHelpers.visibilityGGX( NdotV
						, NdotL
						, components.alphaRoughness ) );

				m_writer.returnStmt( vec3( D * Vis ) );
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
