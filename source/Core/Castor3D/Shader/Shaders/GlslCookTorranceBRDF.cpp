#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	using namespace sdw;

	CookTorranceBRDF::CookTorranceBRDF( ShaderWriter & writer
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

	void CookTorranceBRDF::declareDiffuse()
	{
		m_utils.declareFresnelSchlick();
		doDeclareComputeCookTorranceDiffuse();
	}

	void CookTorranceBRDF::compute( Light const & light
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, sdw::Float const & roughness
		, Surface surface
		, OutputComponents & output )
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

	sdw::Vec3 CookTorranceBRDF::computeDiffuse( Light const & light
		, sdw::Vec3 const & worldEye
		, sdw::Vec3 const & direction
		, sdw::Vec3 const & specular
		, sdw::Float const & metalness
		, Surface surface )
	{
		return m_computeCookTorranceDiffuse( light
			, worldEye
			, direction
			, specular
			, metalness
			, surface );
	}

	void CookTorranceBRDF::doDeclareDistribution()
	{
		// Distribution Function
		m_distributionGGX = m_writer.implementFunction< Float >( "Distribution"
			, [this]( Float const & product
				, Float const & roughness )
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
				denominator = Float{ castor::Pi< float > } * denominator * denominator;

				m_writer.returnStmt( numerator / denominator );
			}
			, InFloat( m_writer, "product" )
			, InFloat( m_writer, "roughness" ) );
	}
	
	void CookTorranceBRDF::doDeclareGeometry()
	{
		// Geometry Functions
		m_geometrySchlickGGX = m_writer.implementFunction< Float >( "GeometrySchlickGGX"
			, [this]( Float const & product
				, Float const & roughness )
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
			, InFloat( m_writer, "product" )
			, InFloat( m_writer, "roughness" ) );

		m_geometrySmith = m_writer.implementFunction< Float >( "GeometrySmith"
			, [this]( Float const & NdotV
				, Float const & NdotL
				, Float const & roughness )
			{
				// From https://learnopengl.com/#!PBR/Lighting
				auto ggx2 = m_writer.declLocale( "ggx2"
					, m_geometrySchlickGGX( NdotV, roughness ) );
				auto ggx1 = m_writer.declLocale( "ggx1"
					, m_geometrySchlickGGX( NdotL, roughness ) );

				m_writer.returnStmt( ggx1 * ggx2 );
			}
			, InFloat( m_writer, "NdotV" )
			, InFloat( m_writer, "NdotL" )
			, InFloat( m_writer, "roughness" ) );
	}

	void CookTorranceBRDF::doDeclareComputeCookTorrance()
	{
		OutputComponents output{ m_writer };
		m_computeCookTorrance = m_writer.implementFunction< Void >( "doComputeCookTorrance"
			, [this]( Light const & light
				, Vec3 const & worldEye
				, Vec3 const & direction
				, Vec3 const & specular
				, Float const & metalness
				, Float const & roughness
				, Surface surface
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

				output.m_diffuse = ( radiance * light.m_intensity.r() * NdotL * kD / Float{ castor::Pi< float > } );
				output.m_specular = ( specReflectance * radiance * light.m_intensity.g() * NdotL );
			}
			, InLight( m_writer, "light" )
			, InVec3( m_writer, "worldEye" )
			, InVec3( m_writer, "direction" )
			, InVec3( m_writer, "specular" )
			, InFloat( m_writer, "metalness" )
			, InFloat( m_writer, "roughness" )
			, InSurface{ m_writer, "surface" }
			, output );
	}

	void CookTorranceBRDF::doDeclareComputeCookTorranceDiffuse()
	{
		m_computeCookTorranceDiffuse = m_writer.implementFunction< Vec3 >( "doComputeCookTorrance"
			, [this]( Light const & light
				, Vec3 const & worldEye
				, Vec3 const & direction
				, Vec3 const & specular
				, Float const & metalness
				, Surface surface )
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

				m_writer.returnStmt( ( radiance * light.m_intensity.r() * NdotL * kD / Float{ castor::Pi< float > } ) );
			}
			, InLight( m_writer, "light" )
			, InVec3( m_writer, "worldEye" )
			, InVec3( m_writer, "direction" )
			, InVec3( m_writer, "specular" )
			, InFloat( m_writer, "metalness" )
			, InSurface{ m_writer, "surface" } );
	}

	//***********************************************************************************************
}
