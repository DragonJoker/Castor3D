#include "AtmosphereScattering/AtmosphereBackgroundModel.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Shader/Shaders/GlslBlendComponents.hpp>
#include <Castor3D/Shader/Shaders/GlslDebugOutput.hpp>

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace atmosphere_scattering
{
	namespace model
	{
		template< typename DataT >
		DataT getData( sdw::UniformBuffer & ubo
			, castor::MbString const & name
			, bool enabled = true )
		{
			auto result = ubo.declMember< DataT >( name, enabled );
			ubo.end();
			return result;
		}
	}

	castor::String const AtmosphereBackgroundModel::Name = cuT( "c3d.atmosphere" );
	castor::String const AtmosphereBackgroundModel::PluginType = cuT( "atmospheric_scattering" );
	castor::MbString const AtmosphereBackgroundModel::PluginName = "Atmospheric Scattering";

	AtmosphereBackgroundModel::AtmosphereBackgroundModel( castor3d::Engine const & engine
		, sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, VkExtent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
		: castor3d::shader::BackgroundModel{ writer, utils, castor::move( targetSize ), true, false, false }
		, cameraBuffer{ writer.declUniformBuffer<>( CameraUbo::Buffer
			, binding++
			, set ) }
		, cameraData{ model::getData< CameraData >( cameraBuffer, CameraUbo::Data ) }
		, atmosphereBuffer{ writer.declUniformBuffer<>( AtmosphereScatteringUbo::Buffer
			, binding++
			, set ) }
		, atmosphereData{ model::getData< AtmosphereData >( atmosphereBuffer, AtmosphereScatteringUbo::Data ) }
		, cloudsBuffer{ writer.declUniformBuffer<>( CloudsUbo::Buffer
			, binding++
			, set
			, sdw::type::MemoryLayout::eStd140
			, needsForeground ) }
		, cloudsData{ model::getData< CloudsData >( cloudsBuffer, CloudsUbo::Data, needsForeground ) }
		, atmosphere{ m_writer
			, atmosphereData
			, AtmosphereModel::Settings{ castor::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
				.setCameraData( &cameraData )
				.setVariableSampleCount( true )
				.setMieRayPhase( true )
				.setMultiScatApprox( true )
			, { 256u, 64u } }
		, scattering{ m_writer
			, atmosphere
			, ScatteringModel::Settings{}
				.setNeedsMultiscatter( true )
				.setFastAerialPerspective( true )
			, binding
			, set }
		, cloudsResult{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_atmbg_cloudsResult"
			, binding++
			, set
			, needsForeground ) }
	{
	}

	castor3d::shader::BackgroundModelPtr AtmosphereBackgroundModel::create( castor3d::Engine const & engine
		, sdw::ShaderWriter & writer
		, castor3d::shader::Utils & utils
		, VkExtent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set )
	{
		return castor::make_unique< AtmosphereBackgroundModel >( engine
			, writer
			, utils
			, castor::move( targetSize )
			, needsForeground
			, binding
			, set );
	}

	sdw::RetVec3 AtmosphereBackgroundModel::computeSpecularReflections( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pwsPosition
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, sdw::Vec3 const & pfresnel
		, sdw::Float const & proughness
		, castor3d::shader::BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & brdf
		, castor3d::shader::DebugOutputCategory & debugOutput )
	{
		if ( !m_computeSpecularReflections )
		{
			m_computeSpecularReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_atmbg_computeSpecularReflections"
				, [this, &debugOutput]( sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & fresnel
					, sdw::Vec3 const & V
					, sdw::Vec2 const & gradient )
				{
					auto Ty = m_writer.declLocale( "Ty"
						, vec3( 0.0_f, wsNormal.z(), -wsNormal.y() ) );
					auto Lty = m_writer.declLocale( "Lty"
						, length( Ty ) );
					Ty = m_writer.ternary( ( Lty == 0.0_f )
						, Ty
						, normalize( Ty ) );
					auto Tx = m_writer.declLocale( "Tx"
						, cross( Ty, wsNormal ) );
					auto sunContrib = m_writer.declLocale( "sunContrib"
						, getSunRadiance( atmosphereData.sunDirection() )
							* reflectedSunRadiance( atmosphereData.sunDirection(), V, wsNormal, Tx, Ty, vec2( 1.0_f ) ) );
					debugOutput.registerOutput( cuT( "Atmosphere Reflections" ), cuT( "Sun Contribution" ), sunContrib );
					auto skyContrib = m_writer.declLocale( "skyContrib"
						, meanSkyRadiance( V, wsNormal, Tx, Ty, gradient ) );
					debugOutput.registerOutput( cuT( "Atmosphere Reflections" ), cuT( "Sky Contribution" ), skyContrib );
					m_writer.returnStmt( fresnel * ( sunContrib + skyContrib ) );
				}
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "fresnel" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec2{ m_writer, "gradient" } );
		}
		return m_computeSpecularReflections( pwsNormal
			, pfresnel
			, pV
			, components.getMember< sdw::Vec2 >( "noiseGradient", vec2( 1.0_f ) ) );
	}

	void AtmosphereBackgroundModel::applyVolume( sdw::Vec2 const pfragCoord
		, sdw::Float const plinearDepth
		, sdw::Vec2 const ptargetSize
		, sdw::Vec2 const pcameraPlanes
		, sdw::Vec4 & poutput )
	{
		if ( !cloudsBuffer.isEnabled() )
		{
			return;
		}

		if ( !m_computeVolume )
		{
			m_computeVolume = m_writer.implementFunction< sdw::Void >( "c3d_atmbg_computeVolume"
				, [&]( sdw::Vec2 const & fragCoord
					, sdw::Float linearDepth
					, sdw::Vec2 const & targetSize
					, sdw::Vec4 output )
				{
					auto uv = m_writer.declLocale( "uv"
						, fragCoord / targetSize );
					auto ray = m_writer.declLocale( "ray"
						, atmosphere.castRay( vec2( uv.x(), 1.0_f - uv.y() ) ) );
					linearDepth = linearDepth * atmosphere.settings.length.kilometres();

					auto clampOuter = m_writer.declLocale( "clampOuter"
						, 0_b );
					auto interGround = m_writer.declLocale( "interGround"
						, atmosphere.raySphereIntersectNearest( ray
							, atmosphere.getPlanetRadius() ) );
					auto wasHittingGround = m_writer.declLocale( "wasHittingGround"
						, interGround.valid() );

					IF( m_writer, ( ( !interGround.valid() ) || linearDepth < interGround.t() ) )
					{
						interGround.t() = linearDepth;
						interGround.point() = ray.step( linearDepth );
						interGround.valid() = 1_b;
						clampOuter = 1_b;
					}
					FI;

					auto interInnerN = m_writer.declLocale( "interInnerN", Intersection{ m_writer } );
					auto interInnerF = m_writer.declLocale( "interInnerF", Intersection{ m_writer } );
					auto interInnerCount = m_writer.declLocale( "interInnerCount"
						, atmosphere.raySphereIntersect( ray
							, cloudsData.innerRadius() + atmosphere.getPlanetRadius()
							, interGround
							, 0_b
							, interInnerN
							, interInnerF ) );
					auto interOuterN = m_writer.declLocale( "interOuterN", Intersection{ m_writer } );
					auto interOuterF = m_writer.declLocale( "interOuterF", Intersection{ m_writer } );
					auto interOuterCount = m_writer.declLocale( "interOuterCount"
						, atmosphere.raySphereIntersect( ray
							, cloudsData.outerRadius() + atmosphere.getPlanetRadius()
							, interGround
							, clampOuter
							, interOuterN
							, interOuterF ) );
					auto crossesInner = m_writer.declLocale( "crossesInner"
						, interInnerN.valid() && linearDepth > interInnerN.t() );
					auto crossesOuter = m_writer.declLocale( "crossesOuter"
						, interOuterN.valid() && linearDepth >= interOuterN.t() );

					IF( m_writer, crossesInner || crossesOuter )
					{
						auto clouds = m_writer.declLocale( "clouds"
							, cloudsResult.lod( uv, 0.0_f ) );
						output = mix( output, clouds, vec4( clouds.a() ) );
					}
					FI;
				}
				, sdw::InVec2{ m_writer, "fragCoord" }
				, sdw::InFloat{ m_writer, "linearDepth" }
				, sdw::InVec2{ m_writer, "targetSize" }
				, sdw::InOutVec4{ m_writer, "output" } );
		}

		IF( m_writer, cloudsData.coverage() > 0.0_f
			&& plinearDepth < pcameraPlanes.y() )
		{
			m_computeVolume( pfragCoord
				, plinearDepth
				, ptargetSize
				, poutput );
		}
		FI
	}

	sdw::Vec3 AtmosphereBackgroundModel::getSunRadiance( sdw::Vec3 const & psunDir )
	{
		return scattering.getSunRadiance( psunDir );
	}

	sdw::Float AtmosphereBackgroundModel::erfc( sdw::Float const x )
	{
		return 2.0_f * exp( -x * x ) / ( 2.319_f * x + sqrt( 4.0_f + 1.52_f * x * x ) );
	}

	sdw::RetFloat AtmosphereBackgroundModel::lambda( sdw::Float const pcosTheta
		, sdw::Float const psigmaSq )
	{
		if ( !m_lambda )
		{
			m_lambda = m_writer.implementFunction< sdw::Float >( "Lambda"
				, [&]( sdw::Float const & cosTheta
					, sdw::Float const & sigmaSq )
				{
					auto sqrtPi = float( sqrt( castor::Pi< double > ) );
					auto v = m_writer.declLocale( "v"
						, cosTheta / sqrt( max( 0.0_f, ( 1.0_f - cosTheta * cosTheta ) * ( 2.0_f * sigmaSq ) ) ) );
					m_writer.returnStmt( max( 0.0_f, ( exp( -v * v ) - v * sdw::Float{ sqrtPi } * erfc( v ) ) / ( 2.0_f * v * sdw::Float{ sqrtPi } ) ) );
				}
				, sdw::InFloat{ m_writer, "cosTheta" }
				, sdw::InFloat{ m_writer, "sigmaSq" } );
		}

		return m_lambda( pcosTheta, psigmaSq );
	}

	sdw::RetFloat AtmosphereBackgroundModel::reflectedSunRadiance( sdw::Vec3 const pL
		, sdw::Vec3 const pV
		, sdw::Vec3 const pN
		, sdw::Vec3 const pTx
		, sdw::Vec3 const pTy
		, sdw::Vec2 const psigmaSq )
	{
		if ( !m_reflectedSunRadiance )
		{
			// L, V, N, Tx, Ty in world space
			m_reflectedSunRadiance = m_writer.implementFunction< sdw::Float >( "reflectedSunRadiance"
				, [&]( sdw::Vec3 const & L
					, sdw::Vec3 const & V
					, sdw::Vec3 const & N
					, sdw::Vec3 const & Tx
					, sdw::Vec3 const & Ty
					, sdw::Vec2 const & sigmaSq )
				{
					auto H = m_writer.declLocale( "H"
						, normalize( L + V ) );
					auto zetax = m_writer.declLocale( "zetax"
						, dot( H, Tx ) / dot( H, N ) );
					auto zetay = m_writer.declLocale( "zetay"
						, dot( H, Ty ) / dot( H, N ) );

					auto zL = m_writer.declLocale( "zL"
						, dot( L, N ) ); // cos of source zenith angle
					auto zV = m_writer.declLocale( "zV"
						, dot( V, N ) ); // cos of receiver zenith angle
					auto zH = m_writer.declLocale( "zH"
						, dot( H, N ) ); // cos of facet normal zenith angle
					auto zH2 = m_writer.declLocale( "zH2"
						, zH * zH );

					auto p = m_writer.declLocale( "p"
						, exp( -0.5_f * ( zetax * zetax / sigmaSq.x() + zetay * zetay / sigmaSq.y() ) ) / ( 2.0_f * sdw::Float{ castor::Pi< float > } * sqrt( max( 0.0_f, sigmaSq.x() * sigmaSq.y() ) ) ) );

					auto tanV = m_writer.declLocale( "tanV"
						, atan( dot( V, Ty ) / dot( V, Tx ) ) );
					auto cosV2 = m_writer.declLocale( "cosV2"
						, 1.0_f / ( 1.0_f + tanV * tanV ) );
					auto sigmaV2 = m_writer.declLocale( "sigmaV2"
						, sigmaSq.x() * cosV2 + sigmaSq.y() * ( 1.0_f - cosV2 ) );

					auto tanL = m_writer.declLocale( "tanL"
						, atan( dot( L, Ty ) / dot( L, Tx ) ) );
					auto cosL2 = m_writer.declLocale( "cosL2"
						, 1.0_f / ( 1.0_f + tanL * tanL ) );
					auto sigmaL2 = m_writer.declLocale( "sigmaL2"
						, sigmaSq.x() * cosL2 + sigmaSq.y() * ( 1.0_f - cosL2 ) );

					auto fresnel = m_writer.declLocale( "fresnel"
						, 0.02_f + 0.98_f * pow( 1.0_f - dot( V, H ), 5.0_f ) );

					zL = max( zL, 0.01_f );
					zV = max( zV, 0.01_f );

					m_writer.returnStmt( fresnel * p / ( ( 1.0_f + lambda( zL, sigmaL2 ) + lambda( zV, sigmaV2 ) ) * zV * zH2 * zH2 * 4.0_f ) );
				}
				, sdw::InVec3{ m_writer, "L" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "Tx" }
				, sdw::InVec3{ m_writer, "Ty" }
				, sdw::InVec2{ m_writer, "sigmaSq" } );
		}

		return m_reflectedSunRadiance( pL, pV, pN, pTx, pTy, psigmaSq );
	}

	sdw::RetVec2 AtmosphereBackgroundModel::U( sdw::Vec2 const pzeta
		, sdw::Vec3 const pV
		, sdw::Vec3 const pN
		, sdw::Vec3 const pTx
		, sdw::Vec3 const pTy )
	{
		if ( !m_U )
		{
			// V, N, Tx, Ty in world space
			m_U = m_writer.implementFunction< sdw::Vec2 >( "U"
				, [&]( sdw::Vec2 const & zeta
					, sdw::Vec3 const & V
					, sdw::Vec3 const & N
					, sdw::Vec3 const & Tx
					, sdw::Vec3 const & Ty )
				{
					auto f = m_writer.declLocale( "f"
						, normalize( vec3( -zeta, 1.0 ) ) ); // tangent space
					auto F = m_writer.declLocale( "F"
						, f.x() * Tx + f.y() * Ty + f.z() * N ); // world space
					auto R = m_writer.declLocale( "R"
						, 2.0_f * dot( F, V ) * F - V );
					m_writer.returnStmt( R.xy() / ( 1.0_f + R.z() ) );
				}
				, sdw::InVec2{ m_writer, "zeta" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "Tx" }
				, sdw::InVec3{ m_writer, "Ty" } );
		}

		return m_U( pzeta, pV, pN, pTx, pTy );
	}

	sdw::RetFloat AtmosphereBackgroundModel::meanFresnel1F( sdw::Float const pcosThetaV
		, sdw::Float const psigmaV )
	{
		if ( !m_meanFresnel1F )
		{
			m_meanFresnel1F = m_writer.implementFunction< sdw::Float >( "meanFresnel1F"
				, [&]( sdw::Float const & cosThetaV
					, sdw::Float const & sigmaV )
				{
					m_writer.returnStmt( pow( 1.0_f - cosThetaV
						, 5.0_f * exp( -2.69_f * sigmaV ) ) / ( 1.0_f + 22.7_f * pow( sigmaV, 1.5_f ) ) );
				}
				, sdw::InFloat{ m_writer, "cosThetaV" }
				, sdw::InFloat{ m_writer, "sigmaV" } );
		}

		return m_meanFresnel1F( pcosThetaV, psigmaV );
	}

	sdw::RetFloat AtmosphereBackgroundModel::meanFresnel2F( sdw::Vec3 const pV
		, sdw::Vec3 const pN
		, sdw::Vec2 const psigmaSq )
	{
		if ( !m_meanFresnel2F )
		{
			// V, N in world space
			m_meanFresnel2F = m_writer.implementFunction< sdw::Float >( "meanFresnel2F"
				, [&]( sdw::Vec3 const & V
					, sdw::Vec3 const & N
					, sdw::Vec2 const & sigmaSq )
				{
					auto v = m_writer.declLocale( "v"
						, V.xy() ); // view direction in wind space
					auto t = m_writer.declLocale( "t"
						, v * v / ( 1.0_f - V.z() * V.z() ) ); // cos^2 and sin^2 of view direction
					auto sigmaV2 = m_writer.declLocale( "sigmaV2"
						, dot( t, sigmaSq ) ); // slope variance in view direction
					m_writer.returnStmt( meanFresnel1F( dot( V, N ), sqrt( max( 0.0_f, sigmaV2 ) ) ) );
				}
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec2{ m_writer, "sigmaSq" } );
		}

		return m_meanFresnel2F( pV, pN, psigmaSq );
	}

	sdw::RetVec3 AtmosphereBackgroundModel::meanSkyRadiance( sdw::Vec3 const pV
		, sdw::Vec3 const pN
		, sdw::Vec3 const pTx
		, sdw::Vec3 const pTy
		, sdw::Vec2 const psigmaSq )
	{
		if ( !m_meanSkyRadiance )
		{
			// V, N, Tx, Ty in world space;
			m_meanSkyRadiance = m_writer.implementFunction< sdw::Vec3 >( "meanSkyRadiance"
				, [&]( sdw::Vec3 const & V
					, sdw::Vec3 const & N
					, sdw::Vec3 const & Tx
					, sdw::Vec3 const & Ty
					, sdw::Vec2 const & sigmaSq )
				{
					auto result = m_writer.declLocale( "result"
						, vec4( 0.0_f ) );

					const auto eps = 0.001_f;
					auto u0 = m_writer.declLocale( "u0"
						, U( vec2( 0.0_f ), V, N, Tx, Ty ) );
					auto dux = m_writer.declLocale( "dux"
						, 2.0_f * ( U( vec2( eps, 0.0_f ), V, N, Tx, Ty ) - u0 ) / eps * sqrt( max( 0.0_f, sigmaSq.x() ) ) );
					auto duy = m_writer.declLocale( "duy"
						, 2.0_f * ( U( vec2( 0.0_f, eps ), V, N, Tx, Ty ) - u0 ) / eps * sqrt( max( 0.0_f, sigmaSq.y() ) ) );
					result = scattering.gradSkyView( u0 * ( 0.5_f / 1.1_f ) + 0.5_f
						, dux * ( 0.5_f / 1.1_f )
						, duy * ( 0.5_f / 1.1_f ) );

					m_writer.returnStmt( result.rgb() );
				}
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "Tx" }
				, sdw::InVec3{ m_writer, "Ty" }
				, sdw::InVec2{ m_writer, "sigmaSq" } );
		}

		return m_meanSkyRadiance( pV, pN, pTx, pTy, psigmaSq );
	}
}
