#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"

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
		, Utils & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}

	void CookTorranceBRDF::declare()
	{
		doDeclareDistribution();
		doDeclareGeometry();
		m_utils.declareFresnelSchlick();
		doDeclareComputeCookTorrance();
	}

	void CookTorranceBRDF::declareAON()
	{
		doDeclareDistribution();
		doDeclareGeometry();
		m_utils.declareFresnelSchlick();
		doDeclareComputeCookTorranceAON();
	}

	void CookTorranceBRDF::declareDiffuse()
	{
		m_utils.declareFresnelSchlick();
		doDeclareComputeCookTorranceDiffuse();
	}

	void CookTorranceBRDF::declareDiffuseAON()
	{
		m_utils.declareFresnelSchlick();
		doDeclareComputeCookTorranceDiffuseAON();
	}

	void CookTorranceBRDF::compute( Light const & light
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, Surface surface
		, OutputComponents & output )const
	{
		m_computeCookTorrance( light
			, worldEye
			, direction
			, specular
			, metalness
			, roughness
			, surface
			, output );
	}

	void CookTorranceBRDF::computeAON( Light const & light
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, sdw::Float const & smoothBand
		, Surface surface
		, OutputComponents & output )const
	{
		m_computeCookTorranceAON( light
			, worldEye
			, direction
			, specular
			, metalness
			, roughness
			, smoothBand
			, surface
			, output );
	}

	sdw::Vec3 CookTorranceBRDF::computeDiffuse( sdw::Vec3 const & colour
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, Surface surface )const
	{
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
		, Surface surface )const
	{
		return m_computeCookTorranceDiffuse( light.m_colour
			, light.m_intensity.r()
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
		, Surface surface )const
	{
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
		, Surface surface )const
	{
		return m_computeCookTorranceDiffuseAON( light.m_colour
			, light.m_intensity.r()
			, worldEye
			, direction
			, specular
			, metalness
			, smoothBand
			, surface );
	}

	void CookTorranceBRDF::doDeclareDistribution()
	{
		// Distribution Function
		m_distributionGGX = m_writer.implementFunction< sdw::Float >( "c3d_distribution"
			, [this]( sdw::Float const & product
				, sdw::Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto a = m_writer.declLocale( "a"
					, roughness * roughness );
				auto a2 = m_writer.declLocale( "a2"
					, a * a );
				auto NdotH2 = m_writer.declLocale( "NdotH2"
					, product * product );

				auto numerator = m_writer.declLocale( "num"
					, a2 );
				auto denominator = m_writer.declLocale( "denom"
					, sdw::fma( NdotH2
						, a2 - 1.0_f
						, 1.0_f ) );
				denominator = sdw::Float{ castor::Pi< float > } * denominator * denominator;

				m_writer.returnStmt( numerator / denominator );
			}
			, sdw::InFloat( m_writer, "product" )
			, sdw::InFloat( m_writer, "roughness" ) );
	}
	
	void CookTorranceBRDF::doDeclareGeometry()
	{
		// Geometry Functions
		m_geometrySchlickGGX = m_writer.implementFunction< sdw::Float >( "c3d_geometrySchlickGGX"
			, [this]( sdw::Float const & product
				, sdw::Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto r = m_writer.declLocale( "r"
					, roughness + 1.0_f );
				auto k = m_writer.declLocale( "k"
					, ( r * r ) / 8.0_f );

				auto numerator = m_writer.declLocale( "num"
					, product );
				auto denominator = m_writer.declLocale( "denom"
					, sdw::fma( product
						, 1.0_f - k
						, k ) );

				m_writer.returnStmt( numerator / denominator );
			}
			, sdw::InFloat( m_writer, "product" )
			, sdw::InFloat( m_writer, "roughness" ) );

		m_geometrySmith = m_writer.implementFunction< sdw::Float >( "c3d_geometrySmith"
			, [this]( sdw::Float const & NdotV
				, sdw::Float const & NdotL
				, sdw::Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto ggx2 = m_writer.declLocale( "ggx2"
					, m_geometrySchlickGGX( NdotV, roughness ) );
				auto ggx1 = m_writer.declLocale( "ggx1"
					, m_geometrySchlickGGX( NdotL, roughness ) );

				m_writer.returnStmt( ggx1 * ggx2 );
			}
			, sdw::InFloat( m_writer, "NdotV" )
			, sdw::InFloat( m_writer, "NdotL" )
			, sdw::InFloat( m_writer, "roughness" ) );
	}

	void CookTorranceBRDF::doDeclareComputeCookTorrance()
	{
		OutputComponents outputs{ m_writer };
		m_computeCookTorrance = m_writer.implementFunction< sdw::Void >( "c3d_computeCookTorrance"
			, [this]( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness
				, Surface const & surface
				, OutputComponents & output )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto L = m_writer.declLocale( "L"
					, normalize( direction ) );
				auto V = m_writer.declLocale( "V"
					, normalize( worldEye - surface.worldPosition ) );
				auto H = m_writer.declLocale( "H"
					, normalize( L + V ) );
				auto N = m_writer.declLocale( "N"
					, normalize( surface.worldNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, light.m_colour );

				auto NdotL = m_writer.declLocale( "NdotL"
					, max( 0.0_f, dot( N, L ) ) );
				auto NdotV = m_writer.declLocale( "NdotV"
					, max( 0.0_f, dot( N, V ) ) );
				auto NdotH = m_writer.declLocale( "NdotH"
					, max( 0.0_f, dot( N, H ) ) );
				auto HdotV = m_writer.declLocale( "HdotV"
					, max( 0.0_f, dot( H, V ) ) );
				auto LdotV = m_writer.declLocale( "LdotV"
					, max( 0.0_f, dot( L, V ) ) );

				auto F = m_writer.declLocale( "F"
					, m_utils.fresnelSchlick( HdotV, specular ) );
				auto D = m_writer.declLocale( "D"
					, m_distributionGGX( NdotH, roughness ) );
				auto G = m_writer.declLocale( "G"
					, m_geometrySmith( NdotV, NdotL, roughness ) );

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

				output.m_diffuse = max( radiance * light.m_intensity.r() * NdotL * kD, vec3( 0.0_f ) ) / sdw::Float{ castor::Pi< float > };
				output.m_specular = max( specReflectance * radiance * light.m_intensity.g() * NdotL, vec3( 0.0_f ) );
			}
			, InLight( m_writer, "light" )
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "direction" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, sdw::InFloat{ m_writer, "roughness" }
			, InSurface{ m_writer, "surface" }
			, outputs );
	}

	void CookTorranceBRDF::doDeclareComputeCookTorranceAON()
	{
		OutputComponents outputs{ m_writer };
		m_computeCookTorranceAON = m_writer.implementFunction< sdw::Void >( "c3d_computeCookTorranceAON"
			, [this]( Light const & light
				, sdw::Vec3 const & worldEye
				, sdw::Vec3 const & direction
				, sdw::Vec3 const & specular
				, sdw::Float const & metalness
				, sdw::Float const & roughness
				, sdw::Float const & smoothBand
				, Surface const & surface
				, OutputComponents & output )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto L = m_writer.declLocale( "L"
					, normalize( direction ) );
				auto V = m_writer.declLocale( "V"
					, normalize( worldEye - surface.worldPosition ) );
				auto H = m_writer.declLocale( "H"
					, normalize( L + V ) );
				auto N = m_writer.declLocale( "N"
					, normalize( surface.worldNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, light.m_colour );

				auto NdotL = m_writer.declLocale( "NdotL"
					, max( 0.0_f, dot( N, L ) ) );
				auto NdotV = m_writer.declLocale( "NdotV"
					, max( 0.0_f, dot( N, V ) ) );
				auto NdotH = m_writer.declLocale( "NdotH"
					, max( 0.0_f, dot( N, H ) ) );
				auto HdotV = m_writer.declLocale( "HdotV"
					, max( 0.0_f, dot( H, V ) ) );
				auto LdotV = m_writer.declLocale( "LdotV"
					, max( 0.0_f, dot( L, V ) ) );

				auto F = m_writer.declLocale( "F"
					, m_utils.fresnelSchlick( HdotV, specular ) );
				auto D = m_writer.declLocale( "D"
					, m_distributionGGX( NdotH, roughness ) );
				auto G = m_writer.declLocale( "G"
					, m_geometrySmith( NdotV, NdotL, roughness ) );

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

				output.m_diffuse = max( radiance * light.m_intensity.r() * diffuseFactor * kD, vec3( 0.0_f ) ) / sdw::Float{ castor::Pi< float > };

				specReflectance = smoothStep( vec3( 0.0_f ), vec3( 0.01_f * smoothBand ), specReflectance );
				output.m_specular = max( specReflectance * radiance * light.m_intensity.g() * NdotL, vec3( 0.0_f ) );
			}
			, InLight( m_writer, "light" )
			, sdw::InVec3( m_writer, "worldEye" )
			, sdw::InVec3( m_writer, "direction" )
			, sdw::InVec3{ m_writer, "specular" }
			, sdw::InFloat{ m_writer, "metalness" }
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InFloat{ m_writer, "smoothBand" }
			, InSurface{ m_writer, "surface" }
			, outputs );
	}

	void CookTorranceBRDF::doDeclareComputeCookTorranceDiffuse()
	{
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
					, normalize( worldEye - surface.worldPosition ) );
				auto H = m_writer.declLocale( "H"
					, normalize( L + V ) );
				auto N = m_writer.declLocale( "N"
					, normalize( surface.worldNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, colour );

				auto NdotL = m_writer.declLocale( "NdotL"
					, max( dot( N, L ), 0.0_f ) );
				auto HdotV = m_writer.declLocale( "HdotV"
					, max( dot( H, V ), 0.0_f ) );

				auto F = m_writer.declLocale( "F"
					, m_utils.fresnelSchlick( HdotV, specular ) );
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

	void CookTorranceBRDF::doDeclareComputeCookTorranceDiffuseAON()
	{
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
					, normalize( worldEye - surface.worldPosition ) );
				auto H = m_writer.declLocale( "H"
					, normalize( L + V ) );
				auto N = m_writer.declLocale( "N"
					, normalize( surface.worldNormal ) );
				auto radiance = m_writer.declLocale( "radiance"
					, colour );

				auto NdotL = m_writer.declLocale( "NdotL"
					, max( dot( N, L ), 0.0_f ) );
				auto HdotV = m_writer.declLocale( "HdotV"
					, max( dot( H, V ), 0.0_f ) );

				auto F = m_writer.declLocale( "F"
					, m_utils.fresnelSchlick( HdotV, specular ) );
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
