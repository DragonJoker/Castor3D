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

	BRDFHelpers::BRDFHelpers( sdw::ShaderWriter & writer )
		: m_writer{ writer }
	{
	}

	sdw::RetVec2 BRDFHelpers::hammersley( sdw::UInt const & pi
		, sdw::UInt const & pn )
	{
		if ( !m_hammersley )
		{
			m_hammersley = m_writer.implementFunction< sdw::Vec2 >( "c3d_hammersley"
				, [&]( sdw::UInt const & i
					, sdw::UInt const & n )
				{
					m_writer.returnStmt( vec2( m_writer.cast< sdw::Float >( i ) / m_writer.cast< sdw::Float >( n )
						, radicalInverse( i ) ) );
				}
				, sdw::InUInt{ m_writer, "i" }
				, sdw::InUInt{ m_writer, "n" } );
		}

		return m_hammersley( pi, pn );
	}

	sdw::RetFloat BRDFHelpers::visibilitySmithGGXCorrelated( sdw::Float const & pNdotV
		, sdw::Float const & pNdotL
		, sdw::Float const & proughness )
	{
		if ( !m_visibilitySmithGGXCorrelated )
		{
			m_visibilitySmithGGXCorrelated = m_writer.implementFunction< sdw::Float >( "c3d_visibilitySmithGGXCorrelated"
				, [&]( sdw::Float const & NdotV
					, sdw::Float const & NdotL
					, sdw::Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto ggxV = m_writer.declLocale( "ggxV"
						, visibilitySchlickGGX( NdotV, roughness ) );
					auto ggxL = m_writer.declLocale( "ggxL"
						, visibilitySchlickGGX( NdotL, roughness ) );

					m_writer.returnStmt( ggxV * ggxL );
				}
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "roughness" ) );
		}

		return m_visibilitySmithGGXCorrelated( pNdotV, pNdotL, proughness );
	}

	sdw::RetFloat BRDFHelpers::visibilityAshikhmin( sdw::Float const & pNdotL
		, sdw::Float const & pNdotV )
	{
		if ( !m_visibilityAshikhmin )
		{
			m_visibilityAshikhmin = m_writer.implementFunction< sdw::Float >( "c3d_visibilityAshikhmin"
				, [&]( sdw::Float const & NdotL
					, sdw::Float const & NdotV )
				{
					m_writer.returnStmt( clamp( 1.0_f / ( 4.0_f * ( NdotL + NdotV - NdotL * NdotV ) )
						, 0.0_f
						, 1.0_f ) );
				}
				, sdw::InFloat{ m_writer, "NdotL" }
				, sdw::InFloat{ m_writer, "NdotV" } );
		}

		return m_visibilityAshikhmin( pNdotL, pNdotV );
	}

	sdw::RetFloat BRDFHelpers::visibilitySheen( sdw::Float const & pNdotV
		, sdw::Float const & pNdotL
		, sdw::Float const & proughness )
	{
		if ( !m_visibilitySheen )
		{
			m_visibilitySheen = m_writer.implementFunction< sdw::Float >( "c3d_visibilitySheen"
				, [&]( sdw::Float const & NdotV
					, sdw::Float const & NdotL
					, sdw::Float roughness )
				{
					roughness = max( roughness, 0.000001_f ); //clamp (0,1]
					auto alphaG = m_writer.declLocale( "alphaG"
						, roughness * roughness );

					m_writer.returnStmt( clamp( 1.0_f / ( ( 1.0_f + lambdaSheen( NdotV, alphaG ) + lambdaSheen( NdotL, alphaG ) )
						* ( 4.0_f * NdotV * NdotL ) ), 0.0_f, 1.0_f ) );
				}
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "roughness" ) );
		}

		return m_visibilitySheen( pNdotV, pNdotL, proughness );
	}

	sdw::RetFloat BRDFHelpers::distributionGGX( sdw::Float const & pNdotH
		, sdw::Float const & palpha )
	{
		if ( !m_distributionGGX )
		{
			m_distributionGGX = m_writer.implementFunction< sdw::Float >( "c3d_distributionGGX"
				, [&]( sdw::Float const & NdotH
					, sdw::Float alpha )
				{
					auto a = m_writer.declLocale( "a"
						, NdotH * alpha );
					auto k = m_writer.declLocale( "k"
						, alpha / ( 1.0_f - NdotH * NdotH + a * a ) );
					m_writer.returnStmt( k * k * 1.0_f / castor::Pi< float > );
				}
				, sdw::InFloat{ m_writer, "NdotH" }
				, sdw::InFloat{ m_writer, "alpha" } );
		}

		return m_distributionGGX( pNdotH, palpha );
	}

	sdw::RetFloat BRDFHelpers::distributionCharlie( sdw::Float const & pNdotH
		, sdw::Float const & palpha )
	{
		if ( !m_distributionCharlie )
		{
			m_distributionCharlie = m_writer.implementFunction< sdw::Float >( "c3d_distributionCharlie"
				, [&]( sdw::Float const & NdotH
					, sdw::Float alpha )
				{
					alpha = max( alpha, 0.000001_f );
					auto invR = m_writer.declLocale( "invR"
						, 1.0_f / alpha );
					auto cos2h = m_writer.declLocale( "cos2h"
						, NdotH * NdotH );
					auto sin2h = m_writer.declLocale( "sin2h"
						, 1.0_f - cos2h );
					m_writer.returnStmt( ( 2.0_f * invR ) * pow( sin2h, invR * 0.5_f ) / castor::Tau< float > );
				}
				, sdw::InFloat{ m_writer, "NdotH" }
				, sdw::InFloat{ m_writer, "alpha" } );
		}

		return m_distributionCharlie( pNdotH, palpha );
	}

	RetMicrofacetDistributionSample BRDFHelpers::importanceSampleGGX( sdw::Vec2 const & pxi
		, sdw::Float const & proughness )
	{
		if ( !m_importanceSampleGGX )
		{
			m_importanceSampleGGX = m_writer.implementFunction< MicrofacetDistributionSample >( "c3d_importanceSampleGGX"
				, [&]( sdw::Vec2 const & xi
					, sdw::Float const & roughness )
				{
					auto result = m_writer.declLocale< MicrofacetDistributionSample >( "result" );

					// Evaluate sampling equations
					auto alpha = m_writer.declLocale( "alpha"
						, roughness * roughness );
					auto a2 = m_writer.declLocale( "a2"
						, alpha * alpha );

					result.phi() = castor::Tau< float > *xi.x();
					result.cosTheta() = sqrt( ( 1.0_f - xi.y() ) / ( 1.0_f + ( a2 - 1.0_f ) * xi.y() ) );
					result.sinTheta() = sqrt( 1.0_f - result.cosTheta() * result.cosTheta() );

					// Evaluate GGX pdf (for half vector)
					result.pdf() = distributionGGX( result.cosTheta(), alpha );

					// Apply the Jacobian to obtain a pdf that is parameterized by l
					// see https://bruop.github.io/ibl/
					// Typically you'd have the following:
					// float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
					// but since V = N => VoH == NoH
					result.pdf() /= 4.0_f;

					m_writer.returnStmt( result );
				}
				, sdw::InVec2{ m_writer, "xi" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_importanceSampleGGX( pxi, proughness );
	}

	RetMicrofacetDistributionSample BRDFHelpers::importanceSampleCharlie( sdw::Vec2 const & pxi
		, sdw::Float const & proughness )
	{
		if ( !m_importanceSampleCharlie )
		{
			m_importanceSampleCharlie = m_writer.implementFunction< MicrofacetDistributionSample >( "c3d_importanceSampleCharlie"
				, [&]( sdw::Vec2 const & xi
					, sdw::Float const & roughness )
				{
					auto result = m_writer.declLocale< shader::MicrofacetDistributionSample >( "result" );

					// Evaluate sampling equations
					auto alpha = m_writer.declLocale( "alpha"
						, roughness * roughness );

					result.phi() = castor::Tau< float > *xi.x();
					result.sinTheta() = m_writer.ternary( alpha == 0.0_f
						, 0.0_f
						, pow( xi.y(), alpha / ( 2.0f * alpha + 1.0_f ) ) );
					result.cosTheta() = sqrt( 1.0_f - result.sinTheta() * result.sinTheta() );

					// Evaluate GGX pdf (for half vector)
					result.pdf() = distributionCharlie( result.cosTheta(), alpha );

					// Apply the Jacobian to obtain a pdf that is parameterized by l
					// see https://bruop.github.io/ibl/
					// Typically you'd have the following:
					// float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
					// but since V = N => VoH == NoH
					result.pdf() /= 4.0_f;

					m_writer.returnStmt( result );
				}
				, sdw::InVec2{ m_writer, "xi" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_importanceSampleCharlie( pxi, proughness );
	}

	sdw::Vec4 BRDFHelpers::getImportanceSample( MicrofacetDistributionSample const & pis
		, sdw::Vec3 const & pn )
	{
		if ( !m_getImportanceSample )
		{
			m_getImportanceSample = m_writer.implementFunction< sdw::Vec4 >( "c3d_getImportanceSample"
				, [&]( MicrofacetDistributionSample const & is
					, sdw::Vec3 const & n )
				{
					// from spherical coordinates to cartesian coordinates
					auto localSpaceDirection = m_writer.declLocale( "localSpaceDirection"
						, normalize( vec3( cos( is.phi() ) * is.sinTheta()
							, sin( is.phi() ) * is.sinTheta()
							, is.cosTheta() ) ) );

					// from tangent-space vector to world-space sample vector
					auto up = m_writer.declLocale( "up"
						, m_writer.ternary( sdw::abs( n.z() ) < 0.999_f
							, vec3( 0.0_f, 0.0_f, 1.0_f )
							, vec3( 1.0_f, 0.0_f, 0.0_f ) ) );
					auto tangent = m_writer.declLocale( "tangent"
						, normalize( cross( up, n ) ) );
					auto bitangent = m_writer.declLocale( "bitangent"
						, cross( n, tangent ) );

					auto sampleVec = m_writer.declLocale( "sampleVec"
						, ( tangent * localSpaceDirection.x()
							+ bitangent * localSpaceDirection.y()
							+ n * localSpaceDirection.z() ) );

					m_writer.returnStmt( vec4( sampleVec, is.pdf() ) );
				}
				, InMicrofacetDistributionSample{ m_writer, "is" }
				, sdw::InVec3{ m_writer, "n" } );
		}

		return m_getImportanceSample( pis, pn );
	}

	sdw::RetFloat BRDFHelpers::radicalInverse( sdw::UInt const & pinBits )
	{
		if ( !m_radicalInverse )
		{
			m_radicalInverse = m_writer.implementFunction< sdw::Float >( "c3d_radicalInverse"
				, [&]( sdw::UInt const & inBits )
				{
					auto bits = m_writer.declLocale( "bits"
						, inBits );
					bits = ( bits << 16u ) | ( bits >> 16u );
					bits = ( ( bits & 0x55555555_u ) << 1u ) | ( ( bits & 0xAAAAAAAA_u ) >> 1u );
					bits = ( ( bits & 0x33333333_u ) << 2u ) | ( ( bits & 0xCCCCCCCC_u ) >> 2u );
					bits = ( ( bits & 0x0F0F0F0F_u ) << 4u ) | ( ( bits & 0xF0F0F0F0_u ) >> 4u );
					bits = ( ( bits & 0x00FF00FF_u ) << 8u ) | ( ( bits & 0xFF00FF00_u ) >> 8u );
					m_writer.returnStmt( m_writer.cast< sdw::Float >( bits ) * 2.3283064365386963e-10_f ); // / 0x100000000
				}
				, sdw::InUInt{ m_writer, "inBits" } );
		}

		return m_radicalInverse( pinBits );
	}

	sdw::RetFloat BRDFHelpers::lambdaSheenNumericHelper( sdw::Float const & px
		, sdw::Float const & palphaG )
	{
		if ( !m_lambdaSheenNumericHelper )
		{
			m_lambdaSheenNumericHelper = m_writer.implementFunction< sdw::Float >( "c3d_sheenLambdaNumericHelper"
				, [&]( sdw::Float const & x
					, sdw::Float const & alphaG )
				{
					auto oneMinusAlphaSq = m_writer.declLocale( "oneMinusAlphaSq"
						, ( 1.0_f - alphaG ) * ( 1.0_f - alphaG ) );
					auto a = m_writer.declLocale( "a"
						, mix( 21.5473_f, 25.3245_f, oneMinusAlphaSq ) );
					auto b = m_writer.declLocale( "b"
						, mix( 3.82987_f, 3.32435_f, oneMinusAlphaSq ) );
					auto c = m_writer.declLocale( "c"
						, mix( 0.19823_f, 0.16801_f, oneMinusAlphaSq ) );
					auto d = m_writer.declLocale( "d"
						, mix( -1.97760_f, -1.27393_f, oneMinusAlphaSq ) );
					auto e = m_writer.declLocale( "e"
						, mix( -4.32054_f, -4.85967_f, oneMinusAlphaSq ) );
					m_writer.returnStmt( a / ( 1.0_f + b * pow( x, c ) ) + d * x + e );
				}
				, sdw::InFloat{ m_writer, "x" }
				, sdw::InFloat{ m_writer, "palphaG" } );
		}

		return m_lambdaSheenNumericHelper( px, palphaG );
	}

	sdw::RetFloat BRDFHelpers::lambdaSheen( sdw::Float const & pcosTheta
		, sdw::Float const & palphaG )
	{
		if ( !m_lambdaSheen )
		{
			m_lambdaSheen = m_writer.implementFunction< sdw::Float >( "c3d_sheenLambda"
				, [&]( sdw::Float const & cosTheta
					, sdw::Float const & alphaG )
				{
					IF( m_writer, abs( cosTheta ) < 0.5_f )
					{
						m_writer.returnStmt( exp( lambdaSheenNumericHelper( cosTheta, alphaG ) ) );
					}
					FI;

					m_writer.returnStmt( exp( 2.0_f * lambdaSheenNumericHelper( 0.5_f, alphaG )
						- lambdaSheenNumericHelper( 1.0_f - cosTheta, alphaG ) ) );
				}
				, sdw::InFloat{ m_writer, "cosTheta" }
				, sdw::InFloat{ m_writer, "alphaG" } );
		}

		return m_lambdaSheen( pcosTheta, palphaG );
	}

	sdw::RetFloat BRDFHelpers::visibilitySchlickGGX( sdw::Float const & pproduct
		, sdw::Float const & proughness )
	{
		if ( !m_visibilitySchlickGGX )
		{
			m_visibilitySchlickGGX = m_writer.implementFunction< sdw::Float >( "c3d_visibilitySchlickGGX"
				, [&]( sdw::Float const & product
					, sdw::Float const & roughness )
				{
					// From https://learnopengl.com/#!PBR/Lighting
					auto k = m_writer.declLocale( "k"
						, ( roughness * roughness ) / 2.0_f );

					auto numerator = m_writer.declLocale( "num"
						, product );
					auto denominator = m_writer.declLocale( "denom"
						, product * ( 1.0_f - k ) + k );

					m_writer.returnStmt( numerator / denominator );
				}
				, sdw::InFloat( m_writer, "product" )
				, sdw::InFloat( m_writer, "roughness" ) );
		}

		return m_visibilitySchlickGGX( pproduct, proughness );
	}

	//***********************************************************************************************
}
