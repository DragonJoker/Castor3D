#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	using namespace sdw;

	namespace shader
	{
		CookTorranceBRDF::CookTorranceBRDF( ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void CookTorranceBRDF::declare()
		{
			doDeclareDistribution();
			doDeclareGeometry();
			doDeclareFresnelShlick();
			doDeclareComputeCookTorrance();
		}

		void CookTorranceBRDF::compute( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & albedo
			, sdw::Float const & metallic
			, sdw::Float const & roughness
			, sdw::Float const & shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & output )
		{
			m_computeCookTorrance( light
				, worldEye
				, direction
				, albedo
				, mix( vec3( 0.04_f ), albedo, vec3( metallic ) )
				, metallic
				, roughness
				, shadowFactor
				, FragmentInput{ fragmentIn }
				, output );
		}

		void CookTorranceBRDF::compute( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, sdw::Vec3 const & albedo
			, sdw::Vec3 const & specular
			, sdw::Float const & roughness
			, sdw::Float const & shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & output )
		{
			m_computeCookTorrance( light
				, worldEye
				, direction
				, albedo
				, specular
				, length( specular )
				, roughness
				, shadowFactor
				, FragmentInput{ fragmentIn }
				, output );
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
						, m_writer.paren( r * r ) / 8.0_f );

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
	
		void CookTorranceBRDF::doDeclareFresnelShlick()
		{
			// Fresnel Function
			m_schlickFresnel = m_writer.implementFunction< Vec3 >( "FresnelShlick"
				, [this]( Float const & product
					, Vec3 const & f0 )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					m_writer.returnStmt( sdw::fma( vec3( 1.0_f ) - f0
						, vec3( pow( 1.0_f - product, 5.0_f ) )
						, f0 ) );
				}
				, InFloat( m_writer, "product" )
				, InVec3( m_writer, "f0" ) );
		}

		void CookTorranceBRDF::doDeclareComputeCookTorrance()
		{
			OutputComponents output{ m_writer };
			m_computeCookTorrance = m_writer.implementFunction< Void >( "doComputeCookTorrance"
				, [this]( Light const & light
					, Vec3 const & worldEye
					, Vec3 const & direction
					, Vec3 const & albedo
					, Vec3 const & f0
					, Float const & metallic
					, Float const & roughness
					, Float const & shadowFactor
					, FragmentInput const & fragmentIn
					, OutputComponents & output )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto L = m_writer.declLocale( "L"
						, normalize( direction ) );
					auto V = m_writer.declLocale( "V"
						, normalize( worldEye - fragmentIn.m_worldVertex ) );
					auto H = m_writer.declLocale( "H"
						, normalize( L + V ) );
					auto N = m_writer.declLocale( "N"
						, normalize( fragmentIn.m_worldNormal ) );
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
						, m_schlickFresnel( HdotV, f0 ) );
					auto NDF = m_writer.declLocale( "NDF"
						, m_distributionGGX( NdotH, roughness ) );
					auto G = m_writer.declLocale( "G"
						, m_geometrySmith( NdotV, NdotL, roughness ) );

					auto numerator = m_writer.declLocale( "numerator"
						, F * NDF * G );
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

					kD *= 1.0_f - metallic;

					output.m_diffuse = shadowFactor * m_writer.paren( radiance * NdotL * kD / Float{ castor::Pi< float > } );
					output.m_specular = shadowFactor * m_writer.paren( specReflectance * radiance * NdotL );
				}
				, InLight( m_writer, "light" )
				, InVec3( m_writer, "worldEye" )
				, InVec3( m_writer, "direction" )
				, InVec3( m_writer, "albedo" )
				, InVec3( m_writer, "f0" )
				, InFloat( m_writer, "metallic" )
				, InFloat( m_writer, "roughness" )
				, InFloat( m_writer, "shadowFactor" )
				, FragmentInput{ m_writer }
				, output );
		}

		//***********************************************************************************************
	}
}
