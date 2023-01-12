#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	CookTorranceBRDF::CookTorranceBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdf )
		: m_writer{ writer }
		, m_brdf{ brdf }
	{
	}

	sdw::RetVec3 CookTorranceBRDF::computeSpecular( sdw::Vec3 const & pradiance
		, sdw::Float const & pintensity
		, sdw::Float const & pNdotL
		, sdw::Float const & pNdotH
		, sdw::Float const & pNdotV
		, sdw::Vec3 const & pF
		, sdw::Float const & proughness )
	{
		if ( !m_computeCookTorranceSpecular )
		{
			OutputComponents outputs{ m_writer };
			m_computeCookTorranceSpecular = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorranceSpecular"
				, [this]( sdw::Vec3 const & radiance
					, sdw::Float const & intensity
					, sdw::Float const & NdotL
					, sdw::Float const & NdotH
					, sdw::Float const & NdotV
					, sdw::Vec3 const & F
					, sdw::Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto D = m_writer.declLocale( "D"
						, m_brdf.distributionGGX( NdotH
							, roughness * roughness ) );
					auto G = m_writer.declLocale( "G"
						, m_brdf.visibilitySmithGGXCorrelated( NdotV
							, NdotL
							, roughness ) );

					auto numerator = m_writer.declLocale( "numerator"
						, F * D * G );
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

		return m_computeCookTorranceSpecular( pradiance
			, pintensity
			, pNdotL
			, pNdotH
			, pNdotV
			, pF
			, proughness );
	}

	sdw::RetVec3 CookTorranceBRDF::computeDiffuse( sdw::Vec3 const & pradiance
		, sdw::Float const & pintensity
		, sdw::Vec3 const & pF
		, sdw::Float const & pmetalness )
	{
		if ( !m_computeCookTorranceDiffuse )
		{
			m_computeCookTorranceDiffuse = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorranceDiffuse"
				, [this]( sdw::Vec3 radiance
					, sdw::Float const intensity
					, sdw::Vec3 const & F
					, sdw::Float const & metalness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto kS = m_writer.declLocale( "kS"
						, F );
					auto kD = m_writer.declLocale( "kD"
						, vec3( 1.0_f ) - kS );
					kD *= 1.0_f - metalness;

					radiance = max( radiance * intensity * kD, vec3( 0.0_f ) );
					m_writer.returnStmt( ( radiance / sdw::Float{ castor::Pi< float > } ) );
				}
				, sdw::InVec3( m_writer, "radiance" )
				, sdw::InFloat( m_writer, "intensity" )
				, sdw::InVec3{ m_writer, "F" }
				, sdw::InFloat{ m_writer, "metalness" } );
		}

		return m_computeCookTorranceDiffuse( pradiance
			, pintensity
			, pF
			, pmetalness );
	}

	//***********************************************************************************************
}
