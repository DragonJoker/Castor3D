#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"

#include "Castor3D/Shader/Shaders/GlslBackground.hpp"
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

	sdw::RetFloat BRDFHelpers::visibilitySmithGGXCorrelated( sdw::Float const & pNdotV
		, sdw::Float const & pNdotL
		, sdw::Float const & palphaRoughness )
	{
		if ( !m_visibilitySmithGGXCorrelated )
		{
			m_visibilitySmithGGXCorrelated = m_writer.implementFunction< sdw::Float >( "c3d_visibilitySmithGGXCorrelated"
				, [this]( sdw::Float const & NdotV
					, sdw::Float const & NdotL
					, sdw::Float const & alphaRoughness )
				{
					auto alpha = m_writer.declLocale( "alpha"
						, ( alphaRoughness * alphaRoughness ) / 2.0_f );
					auto ggxV = m_writer.declLocale( "ggxV"
						, NdotV / ( NdotV * ( 1.0_f - alpha ) + alpha ) );
					auto ggxL = m_writer.declLocale( "ggxL"
						, NdotL / ( NdotL * ( 1.0_f - alpha ) + alpha ) );
					m_writer.returnStmt( ggxV * ggxL );
				}
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "alphaRoughness" ) );
		}

		return m_visibilitySmithGGXCorrelated( pNdotV, pNdotL, palphaRoughness );
	}

	sdw::RetFloat BRDFHelpers::visibilityGGX( sdw::Float const & pNdotV
		, sdw::Float const & pNdotL
		, sdw::Float const & palphaRoughness )
	{
		if ( !m_visibilityGGX )
		{
			m_visibilityGGX = m_writer.implementFunction< sdw::Float >( "c3d_visibilityGGX"
				, [this]( sdw::Float const & NdotV
					, sdw::Float const & NdotL
					, sdw::Float const & alphaRoughness )
				{
					auto sqAlphaRoughness = m_writer.declLocale( "sqAlphaRoughness"
						, alphaRoughness * alphaRoughness );
					auto ggxV = m_writer.declLocale( "ggxV"
						, NdotL * sqrt( NdotV * NdotV * ( 1.0_f - sqAlphaRoughness ) + sqAlphaRoughness ) );
					auto ggxL = m_writer.declLocale( "ggxL"
						, NdotV * sqrt( NdotL * NdotL * ( 1.0_f - sqAlphaRoughness ) + sqAlphaRoughness ) );
					auto ggx = m_writer.declLocale( "ggx"
						, ggxV + ggxL );
					m_writer.returnStmt( m_writer.ternary( ggx > 0.0_f
						, 0.5_f / ggx
						, 0.0_f ) );
				}
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "alphaRoughness" ) );
		}

		return m_visibilityGGX( pNdotV, pNdotL, palphaRoughness );
	}

	sdw::RetFloat BRDFHelpers::visibilityBeckmann( sdw::Float const & pNdotL
		, sdw::Float const & pNdotV
		, sdw::Float const & pNdotH
		, sdw::Float const & pVdotH )
	{
		if ( !m_visibilityBeckmann )
		{
			m_visibilityBeckmann = m_writer.implementFunction< sdw::Float >( "c3d_visibilityBeckmann"
				, [this]( sdw::Float const & NdotL
					, sdw::Float const & NdotV
					, sdw::Float const & NdotH
					, sdw::Float const & VdotH )
				{
					auto x = m_writer.declLocale( "x"
						, 2.0_f * NdotH * VdotH );
					m_writer.returnStmt( min( 1.0_f, min( x * NdotV, x * NdotL ) ) );
				}
				, sdw::InFloat{ m_writer, "NdotL" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "NdotH" }
				, sdw::InFloat{ m_writer, "VdotH" } );
		}

		return m_visibilityBeckmann( pNdotL, pNdotV, pNdotH, pVdotH );
	}

	sdw::RetFloat BRDFHelpers::visibilityAshikhmin( sdw::Float const & pNdotL
		, sdw::Float const & pNdotV )
	{
		if ( !m_visibilityAshikhmin )
		{
			m_visibilityAshikhmin = m_writer.implementFunction< sdw::Float >( "c3d_visibilityAshikhmin"
				, [this]( sdw::Float const & NdotL
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
		, sdw::Float const & psheenRoughness )
	{
		if ( !m_visibilitySheen )
		{
			m_visibilitySheen = m_writer.implementFunction< sdw::Float >( "c3d_visibilitySheen"
				, [this]( sdw::Float const & NdotV
					, sdw::Float const & NdotL
					, sdw::Float sheenRoughness )
				{
					sheenRoughness = max( sheenRoughness, 0.000001_f ); //clamp (0,1]
					auto alphaG = m_writer.declLocale( "alphaG"
						, sheenRoughness * sheenRoughness );

					m_writer.returnStmt( clamp( 1.0_f / ( ( 1.0_f + lambdaSheen( NdotV, alphaG ) + lambdaSheen( NdotL, alphaG ) )
						* ( 4.0_f * NdotV * NdotL ) ), 0.0_f, 1.0_f ) );
				}
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "sheenRoughness" ) );
		}

		return m_visibilitySheen( pNdotV, pNdotL, psheenRoughness );
	}

	sdw::RetFloat BRDFHelpers::distributionBeckmann( sdw::Float const & pNdotH
		, sdw::Float const & palpha )
	{
		if ( !m_distributionBeckmann )
		{
			m_distributionBeckmann = m_writer.implementFunction< sdw::Float >( "c3d_distributionBeckmann"
				, [this]( sdw::Float const & NdotH
					, sdw::Float const & alpha )
				{
					auto cos2 = m_writer.declLocale( "cos2"
						, NdotH * NdotH );
					auto tan2 = m_writer.declLocale( "tan2Alpha"
						, ( cos2 - 1.0_f ) / cos2 );
					auto denom = m_writer.declLocale( "denom"
						, castor::Pi< float > * alpha * cos2 * cos2 );
					m_writer.returnStmt( exp( tan2 / alpha ) / denom );
				}
				, sdw::InFloat{ m_writer, "NdotH" }
				, sdw::InFloat{ m_writer, "alpha" } );
		}

		return m_distributionBeckmann( pNdotH, palpha );
	}

	sdw::RetFloat BRDFHelpers::distributionGGX( sdw::Float const & pNdotH
		, sdw::Float const & palphaRoughness )
	{
		if ( !m_distributionGGX )
		{
			m_distributionGGX = m_writer.implementFunction< sdw::Float >( "c3d_distributionGGX"
				, [this]( sdw::Float const & NdotH
					, sdw::Float const & alphaRoughness )
				{
					auto sqAlphaRoughness = m_writer.declLocale( "sqAlphaRoughness"
						, alphaRoughness * alphaRoughness );
					auto f = m_writer.declLocale( "f"
						, ( NdotH * NdotH ) * ( sqAlphaRoughness - 1.0_f ) + 1.0_f );
					m_writer.returnStmt( sqAlphaRoughness / ( f * f * castor::Pi< float > ) );
				}
				, sdw::InFloat{ m_writer, "NdotH" }
				, sdw::InFloat{ m_writer, "alpha" } );
		}

		return m_distributionGGX( pNdotH, palphaRoughness );
	}

	sdw::RetFloat BRDFHelpers::distributionCharlie( sdw::Float const & pNdotH
		, sdw::Float const & psheenRoughness )
	{
		if ( !m_distributionCharlie )
		{
			m_distributionCharlie = m_writer.implementFunction< sdw::Float >( "c3d_distributionCharlie"
				, [this]( sdw::Float const & NdotH
					, sdw::Float sheenRoughness )
				{
					sheenRoughness = max( sheenRoughness, 0.000001_f );
					auto alphaG = m_writer.declLocale( "alphaG"
						, sheenRoughness * sheenRoughness );
					auto invR = m_writer.declLocale( "invR"
						, 1.0_f / alphaG );
					auto cos2h = m_writer.declLocale( "cos2h"
						, NdotH * NdotH );
					auto sin2h = m_writer.declLocale( "sin2h"
						, 1.0_f - cos2h );
					m_writer.returnStmt( ( 2.0_f * invR ) * pow( sin2h, invR * 0.5_f ) / castor::Tau< float > );
				}
				, sdw::InFloat{ m_writer, "NdotH" }
				, sdw::InFloat{ m_writer, "sheenRoughness" } );
		}

		return m_distributionCharlie( pNdotH, psheenRoughness );
	}

	RetMicrofacetDistributionSample BRDFHelpers::importanceSampleGGX( sdw::Vec2 const & pxi
		, sdw::Float const & proughness )
	{
		if ( !m_importanceSampleGGX )
		{
			m_importanceSampleGGX = m_writer.implementFunction< MicrofacetDistributionSample >( "c3d_importanceSampleGGX"
				, [this]( sdw::Vec2 const & xi
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
				, [this]( sdw::Vec2 const & xi
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

	sdw::RetVec4 BRDFHelpers::getImportanceSample( MicrofacetDistributionSample const & pis
		, sdw::Vec3 const & pn )
	{
		if ( !m_getImportanceSample )
		{
			m_getImportanceSample = m_writer.implementFunction< sdw::Vec4 >( "c3d_getImportanceSample"
				, [this]( MicrofacetDistributionSample const & is
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

	sdw::RetVec2 BRDFHelpers::hammersley( sdw::UInt const & pi
		, sdw::UInt const & pn )
	{
		if ( !m_hammersley )
		{
			m_hammersley = m_writer.implementFunction< sdw::Vec2 >( "c3d_hammersley"
				, [this]( sdw::UInt const & i
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

	sdw::RetVec3 BRDFHelpers::computeFresnel( sdw::Float const & pNdotV
		, sdw::Float const & proughness
		, sdw::Vec3 const & pF0
		, sdw::Float const & pspecularWeight )
	{
		if ( !m_computeFresnel )
		{
			m_computeFresnel = m_writer.implementFunction< sdw::Vec3 >( "c3d_bgComputeFresnel"
				, [this]( sdw::Float const & NdotV
					, sdw::Float const & roughness
					, sdw::Vec3 const & F0
					, sdw::Float const & specularWeight )
				{
					auto brdf = m_writer.getVariable< sdw::CombinedImage2DRgba32 >( "c3d_mapBrdf" );
					// see https://bruop.github.io/ibl/#single_scattering_results at Single Scattering Results
					// Roughness dependent fresnel, from Fdez-Aguera
					auto f_ab = m_writer.declLocale( "f_ab"
						, BackgroundModel::getBrdf( brdf, NdotV, roughness ) );
					auto Fr = m_writer.declLocale( "Fr"
						, max( vec3( 1.0_f - roughness ), F0 ) - F0 );
					auto k_S = m_writer.declLocale( "k_S"
						, F0 + Fr * pow( 1.0_f - NdotV, 5.0_f ) );
					auto FssEss = m_writer.declLocale( "FssEss"
						, specularWeight * ( k_S * f_ab.x() + f_ab.y() ) );

					// Multiple scattering, from Fdez-Aguera
					auto Ems = m_writer.declLocale( "Ems"
						, ( 1.0_f - ( f_ab.x() + f_ab.y() ) ) );
					auto F_avg = m_writer.declLocale( "F_avg"
						, specularWeight * ( F0 + ( 1.0_f - F0 ) / 21.0_f ) );
					auto FmsEms = m_writer.declLocale( "FmsEms"
						, Ems * FssEss * F_avg / ( 1.0_f - F_avg * Ems ) );

					m_writer.returnStmt( FssEss + FmsEms );
				}
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InVec3{ m_writer, "F0" }
				, sdw::InFloat{ m_writer, "specularWeight" } );
		}

		return m_computeFresnel( pNdotV, proughness, pF0, pspecularWeight );
	}

	void BRDFHelpers::computeSpecularBrdfs( DebugOutputCategory const & debugOutput
		, BlendComponents const & components
		, sdw::Vec3 const & reflectedDiffuse
		, sdw::Vec3 const & reflectedSpecular
		, sdw::Vec3 const & metalFresnel
		, sdw::Vec3 const & dielectricFresnel
		, sdw::Vec3 & metal
		, sdw::Vec3 & dielectric )
	{
		metal = reflectedSpecular * metalFresnel;
		debugOutput.registerOutput( "Raw Metal BRDF", metal );
		dielectric = mix( reflectedDiffuse, reflectedSpecular, dielectricFresnel );
		debugOutput.registerOutput( "Raw Dielectric BRDF", dielectric );

		if ( components.hasMember( "iridescenceFactor" ) )
		{
			metal = mix( metal
				, reflectedSpecular * components.getMember< sdw::Vec3 >( "iridescenceMetallicFresnel" )
				, vec3( components.iridescenceFactor ) );
			debugOutput.registerOutput( "Iridescent Metal BRDF", metal );
			dielectric = mix( dielectric
				, Utils::rgbMix( reflectedDiffuse, reflectedSpecular, components.getMember< sdw::Vec3 >( "iridescenceDielectricFresnel" ) )
				, vec3( components.iridescenceFactor ) );
			debugOutput.registerOutput( "Iridescent Dielectric BRDF", dielectric );
		}
		else
		{
			debugOutput.registerOutput( "Iridescent Metal BRDF", metal );
			debugOutput.registerOutput( "Iridescent Dielectric BRDF", dielectric );
		}
	}

	sdw::RetFloat BRDFHelpers::radicalInverse( sdw::UInt const & pinBits )
	{
		if ( !m_radicalInverse )
		{
			m_radicalInverse = m_writer.implementFunction< sdw::Float >( "c3d_radicalInverse"
				, [this]( sdw::UInt const & inBits )
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
				, [this]( sdw::Float const & x
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
				, sdw::InFloat{ m_writer, "alphaG" } );
		}

		return m_lambdaSheenNumericHelper( px, palphaG );
	}

	sdw::RetFloat BRDFHelpers::lambdaSheen( sdw::Float const & pcosTheta
		, sdw::Float const & palphaG )
	{
		if ( !m_lambdaSheen )
		{
			m_lambdaSheen = m_writer.implementFunction< sdw::Float >( "c3d_sheenLambda"
				, [this]( sdw::Float const & cosTheta
					, sdw::Float const & alphaG )
				{
					sdwIF( m_writer, abs( cosTheta ) < 0.5_f )
					{
						m_writer.returnStmt( exp( lambdaSheenNumericHelper( cosTheta, alphaG ) ) );
					}
					sdwFI

					m_writer.returnStmt( exp( 2.0_f * lambdaSheenNumericHelper( 0.5_f, alphaG )
						- lambdaSheenNumericHelper( 1.0_f - cosTheta, alphaG ) ) );
				}
				, sdw::InFloat{ m_writer, "cosTheta" }
				, sdw::InFloat{ m_writer, "alphaG" } );
		}

		return m_lambdaSheen( pcosTheta, palphaG );
	}

	//***********************************************************************************************
}
