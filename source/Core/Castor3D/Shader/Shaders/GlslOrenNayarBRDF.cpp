#include "Castor3D/Shader/Shaders/GlslOrenNayarBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	namespace fujii
	{
		static float constexpr constant1 = float( 0.5 - 2.0 / ( 3.0 * castor::Pi< double > ) );
		static float constexpr constant2 = float( 2.0 / 3.0 - 28.0 / ( 15.0 * castor::Pi< double > ) );
	}

	//*********************************************************************************************

	QualitativeOrenNayarBRDF::QualitativeOrenNayarBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: DiffuseBRDF{ writer, brdfHelpers }
	{
	}

	void QualitativeOrenNayarBRDF::doGenerate( BlendComponents const & pcomponents
		, LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeQualitativeOrenNayar"
			, [this]( BlendComponents const & components
				, LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL )
			{
				auto NdotV = m_writer.declLocale( "NdotV"
					, lightSurface.NdotV().value() );
				auto LdotV = m_writer.declLocale( "LdotV"
					, lightSurface.LdotV().value() );
				auto roughness = m_writer.declLocale( "roughness"
					, components.perceptualRoughness );

				auto sigma = m_writer.declLocale( "sigma"
					, roughness * castor::PiDiv2< float > );
				auto s = m_writer.declLocale( "s"
					, LdotV - NdotL * NdotV );
				auto oneOverT = m_writer.declLocale( "oneOverT"
					, mix( 1.0_f / max( NdotL, NdotV ), 0.0_f, step( 0.0_f, -s ) ) );

				auto sqSigma = m_writer.declLocale( "sigma"
					, sigma * sigma );
				auto A = m_writer.declLocale( "A"
					, 1.0_f - 0.5_f * ( sqSigma / ( sqSigma + 0.57_f ) ) );
				auto B = m_writer.declLocale( "B"
					, 0.45_f * ( sqSigma / ( sqSigma + 0.09_f ) ) );

				auto diffuseReflectance = m_writer.declLocale( "diffuseReflectance"
					, ( A + B * s * oneOverT ) * radiance / sdw::Float{ castor::Pi< float > } );
				m_writer.returnStmt( max( diffuseReflectance * intensity, vec3( 0.0_f ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" } );
	}

	DiffuseBRDFPtr QualitativeOrenNayarBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, QualitativeOrenNayarBRDF >( writer, brdfHelpers );
	}

	//*********************************************************************************************

	FujiiOrenNayarBRDF::FujiiOrenNayarBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: DiffuseBRDF{ writer, brdfHelpers }
	{
	}

	void FujiiOrenNayarBRDF::doGenerate( BlendComponents const & pcomponents
		, LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeFujiiOrenNayar"
			, [this]( BlendComponents const & components
				, LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL )
			{
				auto NdotV = m_writer.declLocale( "NdotV"
					, lightSurface.NdotV().value() );
				auto LdotV = m_writer.declLocale( "LdotV"
					, lightSurface.LdotV().value() );
				auto roughness = m_writer.declLocale( "roughness"
					, components.perceptualRoughness );

				auto s = m_writer.declLocale( "s"
					, LdotV - NdotL * NdotV );
				auto sOverT = m_writer.declLocale( "oneOverT"
					, m_writer.ternary( s > 0.0_f, s / max( NdotL, NdotV ), s ) );
				auto A = m_writer.declLocale( "A"
					, 1.0_f / ( 1.0_f + roughness * sdw::Float{ fujii::constant1 } ) );

				auto diffuseReflectance = m_writer.declLocale( "diffuseReflectance"
					, A * ( 1.0_f + roughness * sOverT ) * radiance / sdw::Float{ castor::Pi< float > } );
				m_writer.returnStmt( max( diffuseReflectance * intensity, vec3( 0.0_f ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" } );
	}

	DiffuseBRDFPtr FujiiOrenNayarBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, FujiiOrenNayarBRDF >( writer, brdfHelpers );
	}

	//*********************************************************************************************

	EnergyConservativeOrenNayarBRDF::EnergyConservativeOrenNayarBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: DiffuseBRDF{ writer, brdfHelpers }
	{
	}

	void EnergyConservativeOrenNayarBRDF::doGenerate( BlendComponents const & pcomponents
		, LightSurface const & plightSurface )
	{
		m_fujiiOrenNayarAlbedo = m_writer.implementFunction< sdw::Float >( "c3d_fujiiOrenNayarAlbedo"
			, [this]( sdw::Float const & roughness
				, sdw::Float const & mu )
			{
				//// Exact
				//auto A = m_writer.declLocale( "A"
				//	, 1.0f / ( 1.0f + roughness * sdw::Float{ fujii::constant1 } ) ); // FON A coeff.
				//auto B = m_writer.declLocale( "B"
				//	, roughness * A ); // FON B coeff.
				//auto Si = m_writer.declLocale( "Si"
				//	, sqrt( 1.0f - ( mu * mu ) ) );
				//auto G = m_writer.declLocale( "G"
				//	, Si * ( acos( mu ) - Si * mu )
				//		+ ( 2.0_f / 3.0_f ) * ( ( Si / mu ) * ( 1.0_f - ( Si * Si * Si ) ) - Si ) );
				//m_writer.returnStmt( A + ( B / castor::Pi< float > ) * G );

				// Approximate
				auto Gcoeffs = m_writer.declConstant( "Gcoeffs"
					, mat2( vec2( 0.0571085289_f, -0.332181442_f )
						, vec2( 0.491881867_f, 0.0714429953_f ) ) );

				auto mucomp = m_writer.declLocale( "mucomp"
					, 1.0_f - mu );
				auto mucomp2 = m_writer.declLocale( "mucomp2"
					, mucomp  * mucomp );

				auto Goverpi = m_writer.declLocale( "Goverpi"
					, dot( Gcoeffs * vec2( mucomp, mucomp2 ), vec2( 1.0_f, mucomp2 ) ) );

				m_writer.returnStmt( ( 1.0_f + roughness * Goverpi ) / ( 1.0_f + sdw::Float{ fujii::constant1 } * roughness ) );
			}
			, sdw::InFloat{ m_writer, "roughness" }
			, sdw::InFloat{ m_writer, "mu" } );

		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeEnergyConservativeOrenNayar"
			, [this]( BlendComponents const & components
				, LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL )
			{
				auto NdotV = m_writer.declLocale( "NdotV"
					, lightSurface.NdotV().value() );
				auto LdotV = m_writer.declLocale( "LdotV"
					, lightSurface.LdotV().value() );
				auto roughness = m_writer.declLocale( "roughness"
					, components.perceptualRoughness );

				auto s = m_writer.declLocale( "s"
					, LdotV - NdotL * NdotV );
				auto sOverT = m_writer.declLocale( "sOverT"
					, m_writer.ternary( s > 0.0_f, s / max( NdotL, NdotV ), s ) );
				auto A = m_writer.declLocale( "A"
					, 1.0_f / ( 1.0_f + roughness * sdw::Float{ fujii::constant1 } ) );

				auto singleScatter = m_writer.declLocale( "singleScatter"
					, A * ( 1.0_f + roughness * sOverT ) * radiance / sdw::Float{ castor::Pi< float > } );

				auto Eo = m_writer.declLocale( "Eo"
					, m_fujiiOrenNayarAlbedo( roughness, NdotV ) );
				auto Ei = m_writer.declLocale( "Ei"
					, m_fujiiOrenNayarAlbedo( roughness, NdotL ) );
				auto avgE = m_writer.declLocale( "avgE"
					, A * ( 1.0_f + roughness * sdw::Float{ fujii::constant2 } ) );
				auto msRadiance = m_writer.declLocale( "msRadiance"
					, ( radiance * radiance ) * avgE / ( vec3( 1.0_f ) - radiance * ( 1.0_f - avgE ) ) );
				const auto eps = 1.0e-7_f;
				auto multiScatter = m_writer.declLocale( "multiScatter"
					, ( msRadiance / sdw::Float{ castor::Pi< float > } )
						* sdw::max( eps, 1.0_f - Eo ) // multi-scatter lobe
						* sdw::max( eps, 1.0_f - Ei )
						/ sdw::max( eps, 1.0_f - avgE ) );
				auto diffuseReflectance = m_writer.declLocale( "diffuseReflectance"
					, singleScatter + multiScatter );

				m_writer.returnStmt( max( diffuseReflectance * intensity, vec3( 0.0_f ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" } );
	}

	DiffuseBRDFPtr EnergyConservativeOrenNayarBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, EnergyConservativeOrenNayarBRDF >( writer, brdfHelpers );
	}

	//*********************************************************************************************
}
