#include "GlslDisneyBRDF.hpp"

#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslLightSurface.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

namespace disney::shader
{
	//*********************************************************************************************

	DisneyDiffuseBRDF::DisneyDiffuseBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::DiffuseBRDF{ writer, brdfHelpers }
	{
	}

	c3d::DiffuseBRDFUPtr DisneyDiffuseBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, DisneyDiffuseBRDF >( writer, brdfHelpers );
	}

	void DisneyDiffuseBRDF::doGenerate( c3d::BlendComponents const & pcomponents
		, c3d::LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeDisneyDiffuse"
			, [this]( c3d::BlendComponents const & components
				, c3d::LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL )
			{
				auto fl = m_writer.declLocale< sdw::Float >( "fl"
					, pow( 1.0_f - NdotL, 5.0_f ) );
				auto fv = m_writer.declLocale< sdw::Float >( "fv"
					, pow( 1.0_f - lightSurface.NdotV().value(), 5.0_f ) );
				auto rr = m_writer.declLocale< sdw::Float >( "rr"
					, 2.0_f * components.roughness * lightSurface.HdotL().value() * lightSurface.HdotL().value() );
				auto retro = m_writer.declLocale( "retro"
					, radiance * rr * ( fl + fv + fl * fv * ( rr - 1.0_f ) ) );

				auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
					, vec3( 1.0_f ) - lightSurface.difF().value() );
				auto diffuseReflectance = m_writer.declLocale( "diffuseReflectance"
					, ( radiance * ( 1.0_f - 0.5_f * fl ) * ( 1.0_f - 0.5_f * fv ) + retro ) / sdw::Float{ castor::Pi< float > } );
				m_writer.returnStmt( max( diffuseReflectance * intensity * diffuseFactor, vec3( 0.0_f ) ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, c3d::InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" } );
	}

	//*********************************************************************************************

	DisneySpecularBRDF::DisneySpecularBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::SpecularBRDF{ writer, brdfHelpers }
	{
	}

	c3d::SpecularBRDFUPtr DisneySpecularBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< SpecularBRDF, DisneySpecularBRDF >( writer, brdfHelpers );
	}

	void DisneySpecularBRDF::doGenerate( c3d::BlendComponents const & pcomponents
		, c3d::LightSurface const & plightSurface )
	{
		auto sqr = []( auto v )
			{
				return v * v;
			};

		m_distribution = m_writer.implementFunction< sdw::Float >( "c3d_disneyAnisotropicDistribution"
			, [this, &sqr]( sdw::Float const & NdotH
				, sdw::Float const & TdotH
				, sdw::Float const & BdotH
				, sdw::Float const & at
				, sdw::Float const & ab )
			{
				m_writer.returnStmt( 1.0_f
					/ ( castor::Pi< float >
						* at * ab
						* sqr( sqr( TdotH / at ) + sqr( BdotH / ab ) + sqr( NdotH ) ) ) );
			}
			, sdw::InFloat{ m_writer, "NdotH" }
			, sdw::InFloat{ m_writer, "TdotH" }
			, sdw::InFloat{ m_writer, "BdotH" }
			, sdw::InFloat{ m_writer, "at" }
			, sdw::InFloat{ m_writer, "ab" } );

		m_smithGGX = m_writer.implementFunction< sdw::Float >( "c3d_disneyAnisotropicSmithGGX"
			, [this, &sqr]( sdw::Float const & NdotW
				, sdw::Float const & WdotT
				, sdw::Float const & WdotB
				, sdw::Float const & at
				, sdw::Float const & ab )
			{
				m_writer.returnStmt( 1.0_f / ( NdotW + sqrt( sqr( WdotT * at ) + sqr( WdotB * ab ) + sqr( NdotW ) ) ) );
			}
			, sdw::InFloat{ m_writer, "NdotW" }
			, sdw::InFloat{ m_writer, "WdotX" }
			, sdw::InFloat{ m_writer, "WdotY" }
			, sdw::InFloat{ m_writer, "at" }
			, sdw::InFloat{ m_writer, "ab" } );

		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeDisneySpecular"
			, [this, &sqr]( c3d::BlendComponents const & components
				, c3d::LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto const & roughness = components.roughness;
				auto anisotropicT = components.getMember< sdw::Vec3 >( "anisotropicT"
					, ( components.usesDerivativeValues()
						? components.getMember< c3d::DerivVec4 >( "tangent" ).value()
						: components.getMember< sdw::Vec4 >( "tangent" ) ).xyz() );
				auto anisotropicB = components.getMember< sdw::Vec3 >( "anisotropicB"
					, ( components.usesDerivativeValues()
						? components.getMember< c3d::DerivVec3 >( "bitangent" ).value()
						: components.getMember< sdw::Vec3 >( "bitangent" ) ) );
				auto anisotropyStrength = components.getMember< sdw::Float >( "anisotropyStrength", 0.0_f );

				auto aspect = m_writer.declLocale( "aspect"
					, sqrt( 1.0_f - 0.9_f * anisotropyStrength ) );
				auto at = m_writer.declLocale( "at"
					, max( 0.001_f, sqr( roughness ) / aspect ) );
				auto ab = m_writer.declLocale( "ab"
					, max( 0.001_f, sqr( roughness ) * aspect ) );

				auto TdotL = m_writer.declLocale( "TdotL"
					, dot( anisotropicT, lightSurface.L().value() ) );
				auto BdotL = m_writer.declLocale( "BdotL"
					, dot( anisotropicB, lightSurface.L().value() ) );
				auto TdotH = m_writer.declLocale( "TdotH"
					, dot( anisotropicT, lightSurface.H().value() ) );
				auto BdotH = m_writer.declLocale( "BdotH"
					, dot( anisotropicB, lightSurface.H().value() ) );
				auto TdotV = m_writer.declLocale( "TdotV"
					, dot( anisotropicT, lightSurface.V().value() ) );
				auto BdotV = m_writer.declLocale( "BdotV"
					, dot( anisotropicB, lightSurface.V().value() ) );

				auto d = m_writer.declLocale( "d"
					, m_distribution( NdotH, TdotH, BdotH, at, ab ) );

				auto g = m_writer.declLocale( "g"
					, m_smithGGX( NdotL, TdotL, BdotL, at, ab ) );
				g *= m_smithGGX( lightSurface.NdotV().value(), TdotV, BdotV, at, ab );

				auto reflectance = m_writer.declLocale( "reflectance"
					, lightSurface.spcF().value() * d * g );

				m_writer.returnStmt( max( reflectance * radiance * intensity, vec3( 0.0_f ) ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, c3d::InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}

	//*********************************************************************************************

	DisneyClearcoatBRDF::DisneyClearcoatBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::ClearcoatBRDF{ writer, brdfHelpers }
	{
	}

	c3d::ClearcoatBRDFUPtr DisneyClearcoatBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< ClearcoatBRDF, DisneyClearcoatBRDF >( writer, brdfHelpers );
	}

	void DisneyClearcoatBRDF::doGenerate( c3d::BlendComponents const & pcomponents
		, c3d::LightSurface const & plightSurface )
	{
		m_distribution = m_writer.implementFunction< sdw::Float >( "c3d_disneyDistribution"
			, [this]( sdw::Float const & NdotH
				, sdw::Float const & a )
			{
				IF( m_writer, a >= 1.0_f )
				{
					m_writer.returnStmt( 1.0_f / castor::Pi< float > );
				}
				FI

				auto a2 = m_writer.declLocale( "a2"
					, a * a );
				auto t = m_writer.declLocale( "t"
					, 1.0_f + ( a2 - 1.0_f ) * NdotH * NdotH );
				m_writer.returnStmt( ( a2 - 1.0_f ) / ( log( a2 ) * t * castor::Pi< float > ) );
			}
			, sdw::InFloat{ m_writer, "NdotW" }
			, sdw::InFloat{ m_writer, "alphaG" } );

		m_smithGGX = m_writer.implementFunction< sdw::Float >( "c3d_disneySmithGGX"
			, [this]( sdw::Float const & NdotW
				, sdw::Float const & alphaG )
			{
				auto a = m_writer.declLocale( "a", alphaG * alphaG );
				auto b = m_writer.declLocale( "b", NdotW * NdotW );
				m_writer.returnStmt( 1.0_f / ( NdotW + sqrt( a + b - a * b ) ) );
			}
			, sdw::InFloat{ m_writer, "NdotW" }
			, sdw::InFloat{ m_writer, "alphaG" } );

		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeDisneyClearcoat"
			, [this]( c3d::BlendComponents const & components
				, c3d::LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto const & roughness = components.clearcoatRoughness;

				// clearcoat (ior = 1.5 -> F0 = 0.04)
				auto FH = m_writer.declLocale< sdw::Float >( "FH"
					, pow( 1.0_f - lightSurface.HdotL().value(), 5.0_f ) );
				auto Dr = m_writer.declLocale( "Dr"
					, m_distribution( NdotH, mix( 0.1_f, 0.001_f, roughness ) ) );
				auto Fr = m_writer.declLocale( "Fr"
					, mix( 0.04_f, 1.0_f, FH ) );
				auto Gr = m_writer.declLocale( "Gr"
					, m_smithGGX( NdotL, 0.25_f ) * m_smithGGX( lightSurface.NdotV().value(), 0.25_f ) );

				auto reflectance = m_writer.declLocale( "reflectance"
					, 0.25_f * Gr * Fr * Dr );

				m_writer.returnStmt( max( reflectance * radiance * intensity, vec3( 0.0_f ) ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, c3d::InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}

	//*********************************************************************************************

	DisneySheenBRDF::DisneySheenBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::SheenBRDF{ writer, brdfHelpers }
	{
	}

	c3d::SheenBRDFUPtr DisneySheenBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< SheenBRDF, DisneySheenBRDF >( writer, brdfHelpers );
	}

	void DisneySheenBRDF::doGenerate( c3d::Utils & utils
		, c3d::BlendComponents const & pcomponents
		, c3d::LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec4 >( "c3d_computeDisneySheen"
			, [this]( c3d::BlendComponents const & components
				, c3d::LightSurface const & lightSurface
				, sdw::Float const & NdotL
				, sdw::Float const & NdotH )
			{
				auto Cdlum = m_writer.declLocale( "Cdlum"
					, 0.3_f * components.colour.x()
					+ 0.6_f * components.colour.y()
					+ 0.1_f * components.colour.z() ); // luminance approx
				auto Ctint = m_writer.declLocale( "Ctint"
					, m_writer.ternary( Cdlum > 0.0_f, components.colour / Cdlum, vec3( 1.0_f ) ) ); // normalize lum. to isolate hue+sat
				auto Csheen = m_writer.declLocale( "Csheen"
					, mix( vec3( 1.0_f ), Ctint, components.sheenColour ) );
				auto FH = m_writer.declLocale< sdw::Float >( "FH"
					, pow( 1.0_f - lightSurface.HdotL().value(), 5.0_f ) );

				m_writer.returnStmt( vec4( max( FH * Csheen, vec3( 0.0_f ) ), 1.0_f ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, c3d::InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" } );
	}

	//*********************************************************************************************
}
