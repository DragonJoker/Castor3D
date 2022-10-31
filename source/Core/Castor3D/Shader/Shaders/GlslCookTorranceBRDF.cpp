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
		, Utils & utils
		, BRDFHelpers & brdf )
		: m_writer{ writer }
		, m_utils{ utils }
		, m_brdf{ brdf }
	{
	}

	sdw::RetVec3 CookTorranceBRDF::compute( sdw::Vec3 const & radiance
		, sdw::Vec2 const & intensity
		, sdw::Float const & HdotV
		, sdw::Float const & NdotH
		, sdw::Float const & NdotV
		, sdw::Float const & NdotL
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, Surface surface
		, OutputComponents & output )
	{
		declareComputeCookTorrance();
		return m_computeCookTorrance( radiance
			, intensity
			, HdotV
			, NdotH
			, NdotV
			, NdotL
			, specular
			, metalness
			, roughness
			, surface
			, output );
	}

	sdw::RetVec3 CookTorranceBRDF::compute( Light const & light
		, sdw::Float const & HdotV
		, sdw::Float const & NdotH
		, sdw::Float const & NdotV
		, sdw::Float const & NdotL
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, Surface surface
		, OutputComponents & output )
	{
		return compute( light.colour
			, light.intensity
			, HdotV
			, NdotH
			, NdotV
			, NdotL
			, specular
			, metalness
			, roughness
			, surface
			, output );
	}

	void CookTorranceBRDF::computeAON( Light const & light
		, sdw::Float const & HdotV
		, sdw::Float const & NdotH
		, sdw::Float const & NdotV
		, sdw::Float const & NdotL
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, sdw::Float const & smoothBand
		, Surface surface
		, OutputComponents & output )
	{
		declareComputeCookTorranceAON();
		m_computeCookTorranceAON( light.colour
			, light.intensity
			, HdotV
			, NdotH
			, NdotV
			, NdotL
			, specular
			, metalness
			, roughness
			, smoothBand
			, surface
			, output );
	}

	sdw::RetVec3 CookTorranceBRDF::computeSpecular( Light const & light
		, sdw::Float const & HdotV
		, sdw::Float const & NdotH
		, sdw::Float const & NdotV
		, sdw::Float const & NdotL
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, sdw::Vec3 const & position
		, sdw::Vec3 const & normal )
	{
		declareComputeCookTorranceSpecular();
		return m_computeCookTorranceSpecular( light.colour
			, light.intensity
			, HdotV
			, NdotH
			, NdotV
			, NdotL
			, specular
			, metalness
			, roughness
			, position
			, normal );
	}

	sdw::Vec3 CookTorranceBRDF::computeDiffuse( sdw::Vec3 const & colour
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, Surface surface )
	{
		declareComputeCookTorranceDiffuse();
		return m_computeCookTorranceDiffuse( normalize( colour )
			, length( colour )
			, worldEye
			, direction
			, specular
			, metalness
			, surface );
	}

	sdw::Vec3 CookTorranceBRDF::computeDiffuse( Light const & light
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, Surface surface )
	{
		declareComputeCookTorranceDiffuse();
		return m_computeCookTorranceDiffuse( light.colour
			, light.intensity.r()
			, worldEye
			, direction
			, specular
			, metalness
			, surface );
	}

	sdw::Vec3 CookTorranceBRDF::computeDiffuseAON( sdw::Vec3 const & colour
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & smoothBand
		, Surface surface )
	{
		declareComputeCookTorranceDiffuseAON();
		return m_computeCookTorranceDiffuseAON( normalize( colour )
			, length( colour )
			, worldEye
			, direction
			, specular
			, metalness
			, smoothBand
			, surface );
	}

	sdw::Vec3 CookTorranceBRDF::computeDiffuseAON( Light const & light
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & smoothBand
		, Surface surface )
	{
		declareComputeCookTorranceDiffuseAON();
		return m_computeCookTorranceDiffuseAON( light.colour
			, light.intensity.r()
			, worldEye
			, direction
			, specular
			, metalness
			, smoothBand
			, surface );
	}

	void CookTorranceBRDF::declareComputeCookTorrance()
	{
		if ( m_computeCookTorrance )
		{
			return;
		}

		OutputComponents outputs{ m_writer };
		m_computeCookTorrance = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorrance"
			, [this]( sdw::Vec3 const & radiance
				, sdw::Vec2 const & intensity
				, sdw::Float const & HdotV
				, sdw::Float const & NdotH
				, sdw::Float const & NdotV
				, sdw::Float const & NdotL
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness
				, Surface const & surface
				, OutputComponents & output )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto F = m_writer.declLocale( "F"
					, m_utils.conductorFresnel( HdotV, specular ) );
				auto D = m_writer.declLocale( "D"
					, m_brdf.distributionGGX( NdotH, roughness * roughness ) );
				auto G = m_writer.declLocale( "G"
					, m_brdf.visibilitySmithGGXCorrelated( NdotV, NdotL, roughness ) );

				auto numerator = m_writer.declLocale( "numerator"
					, F * D * G );
				auto denominator = m_writer.declLocale( "denominator"
					, sdw::fma( 4.0_f
						, NdotV * NdotL
						, 0.001_f ) );
				auto specReflectance = m_writer.declLocale( "specReflectance"
					, numerator / denominator );
				auto kS = m_writer.declLocale( "kS"
					, F );
				auto kD = m_writer.declLocale( "kD"
					, vec3( 1.0_f ) - kS );

				kD *= 1.0_f - metalness;

				auto result = m_writer.declLocale( "result"
					, max( radiance * intensity.r() * kD, vec3( 0.0_f ) ) / sdw::Float{ castor::Pi< float > } );
				output.m_diffuse = NdotL * result;
				output.m_specular = max( specReflectance * radiance * intensity.g() * NdotL, vec3( 0.0_f ) );
				m_writer.returnStmt( result );
			}
			, sdw::InVec3( m_writer, "radiance" )
			, sdw::InVec2( m_writer, "intensity" )
			, sdw::InFloat( m_writer, "HdotV" )
			, sdw::InFloat( m_writer, "NdotH" )
			, sdw::InFloat( m_writer, "NdotV" )
			, sdw::InFloat( m_writer, "NdotL" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, sdw::InFloat{ m_writer, "roughness" }
			, InSurface{ m_writer, "surface" }
			, outputs );
	}

	void CookTorranceBRDF::declareComputeCookTorranceAON()
	{
		if ( m_computeCookTorranceAON )
		{
			return;
		}

		OutputComponents outputs{ m_writer };
		m_computeCookTorranceAON = m_writer.implementFunction< sdw::Void >( "c3d_computeCookTorranceAON"
			, [this]( sdw::Vec3 const & radiance
				, sdw::Vec2 const & intensity
				, sdw::Float const & HdotV
				, sdw::Float const & NdotH
				, sdw::Float const & NdotV
				, sdw::Float const & NdotL
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness
				, sdw::Float const & smoothBand
				, Surface const & surface
				, OutputComponents & output )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto F = m_writer.declLocale( "F"
					, m_utils.conductorFresnel( HdotV, specular ) );
				auto D = m_writer.declLocale( "D"
					, m_brdf.distributionGGX( NdotH, roughness * roughness ) );
				auto G = m_writer.declLocale( "G"
					, m_brdf.visibilitySmithGGXCorrelated( NdotV, NdotL, roughness ) );

				auto delta = m_writer.declLocale( "delta"
					, fwidth( NdotL ) * smoothBand );
				auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
					, smoothStep( 0.0_f, delta, NdotL ) );

				auto numerator = m_writer.declLocale( "numerator"
					, F * D * G );
				auto denominator = m_writer.declLocale( "denominator"
					, sdw::fma( 4.0_f
						, NdotV * NdotL
						, 0.001_f ) );
				auto specReflectance = m_writer.declLocale( "specReflectance"
					, numerator / denominator );
				auto kS = m_writer.declLocale( "kS"
					, F );
				auto kD = m_writer.declLocale( "kD"
					, vec3( 1.0_f ) - kS );

				kD *= 1.0_f - metalness;

				output.m_diffuse = max( radiance * intensity.r() * diffuseFactor * kD, vec3( 0.0_f ) ) / sdw::Float{ castor::Pi< float > };

				specReflectance = smoothStep( vec3( 0.0_f ), vec3( 0.01_f * smoothBand ), specReflectance );
				output.m_specular = max( specReflectance * radiance * intensity.g() * NdotL, vec3( 0.0_f ) );
			}
			, sdw::InVec3( m_writer, "radiance" )
			, sdw::InVec2( m_writer, "intensity" )
			, sdw::InFloat( m_writer, "HdotV" )
			, sdw::InFloat( m_writer, "NdotH" )
			, sdw::InFloat( m_writer, "NdotV" )
			, sdw::InFloat( m_writer, "NdotL" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InFloat{ m_writer, "smoothBand" }
			, InSurface{ m_writer, "surface" }
			, outputs );
	}

	void CookTorranceBRDF::declareComputeCookTorranceSpecular()
	{
		if ( m_computeCookTorranceSpecular )
		{
			return;
		}

		OutputComponents outputs{ m_writer };
		m_computeCookTorranceSpecular = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorranceSpecular"
			, [this]( sdw::Vec3 const & radiance
				, sdw::Vec2 const & intensity
				, sdw::Float const & HdotV
				, sdw::Float const & NdotH
				, sdw::Float const & NdotV
				, sdw::Float const & NdotL
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness
				, sdw::Vec3 const & position
				, sdw::Vec3 const & normal )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto F = m_writer.declLocale( "F"
					, m_utils.conductorFresnel( HdotV, specular ) );
				auto D = m_writer.declLocale( "D"
					, m_brdf.distributionGGX( NdotH, roughness * roughness ) );
				auto G = m_writer.declLocale( "G"
					, m_brdf.visibilitySmithGGXCorrelated( NdotV, NdotL, roughness ) );

				auto numerator = m_writer.declLocale( "numerator"
					, F * D * G );
				auto denominator = m_writer.declLocale( "denominator"
					, sdw::fma( 4.0_f
						, NdotV * NdotL
						, 0.001_f ) );
				auto specReflectance = m_writer.declLocale( "specReflectance"
					, numerator / denominator );

				m_writer.returnStmt( max( specReflectance * radiance * intensity.g() * NdotL, vec3( 0.0_f ) ) );
			}
			, sdw::InVec3( m_writer, "radiance" )
			, sdw::InVec2( m_writer, "intensity" )
			, sdw::InFloat( m_writer, "HdotV" )
			, sdw::InFloat( m_writer, "NdotH" )
			, sdw::InFloat( m_writer, "NdotV" )
			, sdw::InFloat( m_writer, "NdotL" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InVec3{ m_writer, "position" }
			, sdw::InVec3{ m_writer, "normal" } );
	}

	void CookTorranceBRDF::declareComputeCookTorranceDiffuse()
	{
		if ( m_computeCookTorranceDiffuse )
		{
			return;
		}

		m_computeCookTorranceDiffuse = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorranceDiffuse"
			, [this]( sdw::Vec3 const & colour
				, sdw::Float const intensity
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, Surface const & surface )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto L = m_writer.declLocale( "L"
					, normalize( direction ) );
				auto V = m_writer.declLocale( "V"
					, normalize( worldEye - surface.worldPosition.xyz() ) );
				auto H = m_writer.declLocale( "H"
					, normalize( L + V ) );
				auto N = m_writer.declLocale( "N"
					, normalize( surface.normal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, colour );

				auto NdotL = m_writer.declLocale( "NdotL"
					, max( dot( N, L ), 0.0_f ) );
				auto HdotV = m_writer.declLocale( "HdotV"
					, max( dot( H, V ), 0.0_f ) );

				auto F = m_writer.declLocale( "F"
					, m_utils.conductorFresnel( HdotV, specular ) );
				auto kS = m_writer.declLocale( "kS"
					, F );
				auto kD = m_writer.declLocale( "kD"
					, vec3( 1.0_f ) - kS );

				kD *= 1.0_f - metalness;

				m_writer.returnStmt( ( max( radiance * intensity * NdotL * kD, vec3( 0.0_f ) ) / sdw::Float{ castor::Pi< float > } ) );
			}
			, sdw::InVec3( m_writer, "colour" )
			, sdw::InFloat( m_writer, "intensity" )
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "direction" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, InSurface{ m_writer, "surface" } );
	}

	void CookTorranceBRDF::declareComputeCookTorranceDiffuseAON()
	{
		if ( m_computeCookTorranceDiffuseAON )
		{
			return;
		}

		m_computeCookTorranceDiffuseAON = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeCookTorranceDiffuseAON"
			, [this]( sdw::Vec3 const & colour
				, sdw::Float const intensity
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & smoothBand
				, Surface const & surface )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto L = m_writer.declLocale( "L"
					, normalize( direction ) );
				auto V = m_writer.declLocale( "V"
					, normalize( worldEye - surface.worldPosition.xyz() ) );
				auto H = m_writer.declLocale( "H"
					, normalize( L + V ) );
				auto N = m_writer.declLocale( "N"
					, normalize( surface.normal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, colour );

				auto NdotL = m_writer.declLocale( "NdotL"
					, max( dot( N, L ), 0.0_f ) );
				auto HdotV = m_writer.declLocale( "HdotV"
					, max( dot( H, V ), 0.0_f ) );

				auto F = m_writer.declLocale( "F"
					, m_utils.conductorFresnel( HdotV, specular ) );
				auto kS = m_writer.declLocale( "kS"
					, F );
				auto kD = m_writer.declLocale( "kD"
					, vec3( 1.0_f ) - kS );

				kD *= 1.0_f - metalness;

				auto delta = m_writer.declLocale( "delta"
					, fwidth( NdotL ) * smoothBand );
				auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
					, smoothStep( 0.0_f, delta, NdotL ) );
				m_writer.returnStmt( ( max( radiance * intensity * diffuseFactor * kD, vec3( 0.0_f ) ) / sdw::Float{ castor::Pi< float > } ) );
			}
			, sdw::InVec3( m_writer, "colour" )
			, sdw::InFloat( m_writer, "intensity" )
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "direction" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, sdw::InFloat{ m_writer, "smoothBand" }
			, InSurface{ m_writer, "surface" } );
	}

	//***********************************************************************************************
}
