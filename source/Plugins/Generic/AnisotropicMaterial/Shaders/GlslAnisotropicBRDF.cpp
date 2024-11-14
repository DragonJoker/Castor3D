#include "GlslAnisotropicBRDF.hpp"

#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>

#include <ShaderWriter/Source.hpp>

namespace anisotropy::shader
{
	AnisotropicBRDF::AnisotropicBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::SpecularBRDF{ writer, brdfHelpers }
	{
	}

	c3d::SpecularBRDFPtr AnisotropicBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< c3d::SpecularBRDF, AnisotropicBRDF >( writer, brdfHelpers );
	}

	void AnisotropicBRDF::doGenerate( c3d::BlendComponents const & pcomponents )
	{
		m_anisotropicGGXDistribution = m_writer.implementFunction< sdw::Float >( "c3dam_anisotropicGGXDistribution"
			, [this]( sdw::Float const & NdotH
				, sdw::Float const & TdotH
				, sdw::Float const & BdotH
				, sdw::Float const & at
				, sdw::Float const & ab )
			{
				auto a2 = m_writer.declLocale( "a2"
					, at * ab );
				auto f = m_writer.declLocale( "f"
					, vec3( ab * TdotH, at * BdotH, a2 * NdotH ) );
				auto w2 = m_writer.declLocale( "w2"
					, a2 / dot( f, f ) );
				m_writer.returnStmt( a2 * w2 * w2 / castor::Pi< float > );
			}
			, sdw::InFloat{ m_writer, "NdotH" }
			, sdw::InFloat{ m_writer, "TdotH" }
			, sdw::InFloat{ m_writer, "BdotH" }
			, sdw::InFloat{ m_writer, "at" }
			, sdw::InFloat{ m_writer, "ab" } );

		m_anisotropicGGXVisibility = m_writer.implementFunction< sdw::Float >( "c3dam_anisotropicGGXVisibility"
			, [this]( sdw::Float const & NdotL
				, sdw::Float const & NdotV
				, sdw::Float const & BdotV
				, sdw::Float const & TdotV
				, sdw::Float const & TdotL
				, sdw::Float const & BdotL
				, sdw::Float const & at
				, sdw::Float const & ab )
			{
				auto GGXV = m_writer.declLocale( "GGXV"
					, NdotL * length( vec3( at * TdotV, ab * BdotV, NdotV ) ) );
				auto GGXL = m_writer.declLocale( "GGXL"
					, NdotV * length( vec3( at * TdotL, ab * BdotL, NdotL ) ) );
				auto v = m_writer.declLocale( "v"
					, 0.5_f / ( GGXV + GGXL ) );
				m_writer.returnStmt( clamp( v, 0.0_f, 1.0_f ) );
			}
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotV" }
			, sdw::InFloat{ m_writer, "BdotV" }
			, sdw::InFloat{ m_writer, "TdotV" }
			, sdw::InFloat{ m_writer, "TdotL" }
			, sdw::InFloat{ m_writer, "BdotL" }
			, sdw::InFloat{ m_writer, "at" }
			, sdw::InFloat{ m_writer, "ab" } );

		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3dam_computeAnisotropicSpecular"
			, [this]( c3d::BlendComponents const & components
				, sdw::Vec3 const & N
				, sdw::Vec3 const & L
				, sdw::Vec3 const & H
				, sdw::Vec3 const & V
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto NdotV = m_writer.declLocale( "NdotV"
					, clamp( dot( N, V ), 0.0_f, 1.0_f ) );

				auto anisotropicT = components.getMember< sdw::Vec3 >( "anisotropicT" );
				auto anisotropicB = components.getMember< sdw::Vec3 >( "anisotropicB" );
				auto anisotropyStrength = components.getMember< sdw::Float >( "anisotropyStrength" );

				auto TdotL = m_writer.declLocale( "TdotL"
					, dot( anisotropicT, L ) );
				auto BdotL = m_writer.declLocale( "BdotL"
					, dot( anisotropicB, L ) );
				auto TdotH = m_writer.declLocale( "TdotH"
					, dot( anisotropicT, H ) );
				auto BdotH = m_writer.declLocale( "BdotH"
					, dot( anisotropicB, H ) );
				auto TdotV = m_writer.declLocale( "TdotV"
					, dot( anisotropicT, V ) );
				auto BdotV = m_writer.declLocale( "BdotV"
					, dot( anisotropicB, V ) );

				auto at = m_writer.declLocale( "at"
					, mix( components.alphaRoughness, 1.0_f, anisotropyStrength * anisotropyStrength ) );
				auto ab = m_writer.declLocale( "ab"
					, components.alphaRoughness );

				auto Vis = m_writer.declLocale( "Vis"
					, m_anisotropicGGXVisibility( NdotL, NdotV, BdotV, TdotV, TdotL, BdotL, at, ab ) );
				auto D = m_writer.declLocale( "D"
					, m_anisotropicGGXDistribution( NdotH, TdotH, BdotH, at, ab ) );

				auto specReflectance = m_writer.declLocale( "specReflectance"
					, Vis * D );

				m_writer.returnStmt( max( vec3( specReflectance ), vec3( 0.0_f ) ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, sdw::InVec3{ m_writer, "N" }
			, sdw::InVec3{ m_writer, "L" }
			, sdw::InVec3{ m_writer, "H" }
			, sdw::InVec3{ m_writer, "V" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}
}
