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

	c3d::DiffuseBRDFUPtr DisneyDiffuseBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, DisneyDiffuseBRDF >( writer, brdfHelpers );
	}

	//*********************************************************************************************

	DisneySpecularBRDF::DisneySpecularBRDF( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
		: c3d::SpecularBRDF{ writer, brdfHelpers }
	{
	}

	void DisneySpecularBRDF::doGenerate( c3d::BlendComponents const & pcomponents
		, c3d::LightSurface const & plightSurface )
	{
		auto sqr = []( auto v )
			{
				return v * v;
			};

		m_anisotropicDistribution = m_writer.implementFunction< sdw::Float >( "c3d_disneyAnisotropicDistribution"
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

		m_separableSmithGGX = m_writer.implementFunction< sdw::Float >( "c3d_disneySeparableSmithGGX"
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
				, sdw::Float const & NdotH
				, sdw::Float const & roughness )
			{
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
					, m_anisotropicDistribution( NdotH, TdotH, BdotH, at, ab ) );

				auto g = m_writer.declLocale( "g"
					, m_separableSmithGGX( NdotL, TdotL, BdotL, at, ab ) );
				g *= m_separableSmithGGX( lightSurface.NdotV().value(), TdotV, BdotV, at, ab );

				auto specReflectance = m_writer.declLocale( "specReflectance"
					, lightSurface.spcF().value() * d * g );

				m_writer.returnStmt( max( specReflectance * radiance * intensity, vec3( 0.0_f ) ) );
			}
			, c3d::InBlendComponents{ m_writer, "components", pcomponents }
			, c3d::InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotH" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

		c3d::SpecularBRDFUPtr DisneySpecularBRDF::create( sdw::ShaderWriter & writer
		, c3d::BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< SpecularBRDF, DisneySpecularBRDF >( writer, brdfHelpers );
	}

	//*********************************************************************************************
}
