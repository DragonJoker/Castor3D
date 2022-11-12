#include "AtmosphereScattering/AtmosphereModel.hpp"

#include <Castor3D/Shader/Shaders/GlslLight.hpp>
#include <Castor3D/Shader/Shaders/GlslShadow.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

namespace atmosphere_scattering
{
	//************************************************************************************************

	namespace atmodel
	{
		static sdw::Vec3 getClipSpace( sdw::Vec2 const & uv
			, sdw::Float const & fragDepth )
		{
			return vec3( sdw::fma( uv, vec2( 2.0_f, -2.0_f ), vec2( -1.0_f, 1.0_f ) ), fragDepth );
		}
	}

	//************************************************************************************************

	AtmosphereModel::AtmosphereModel( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData
		, LuminanceSettings pluminanceSettings )
		: AtmosphereModel{ pwriter, patmosphereData, pluminanceSettings, {} }
	{
	}

	AtmosphereModel::AtmosphereModel( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData
		, LuminanceSettings pluminanceSettings
		, VkExtent2D ptransmittanceExtent )
		: writer{ pwriter }
		, atmosphereData{ patmosphereData }
		, luminanceSettings{ pluminanceSettings }
		, transmittanceExtent{ std::move( ptransmittanceExtent ) }
		, planetRadiusOffset{ 0.01_f }
	{
	}

	sdw::RetVec3 AtmosphereModel::getWorldPos( sdw::Float const & pdepth
		, sdw::Vec2 const & ppixPos
		, sdw::Vec2 const & ptexSize )
	{
		if ( !m_getWorldPos )
		{
			m_getWorldPos = writer.implementFunction< sdw::Vec3 >( "atm_getWorldPos"
				, [&]( sdw::Float const & depth
					, sdw::Vec2 const & pixPos
					, sdw::Vec2 const & texSize )
				{
					auto clipSpace = writer.declLocale( "clipSpace"
						, atmodel::getClipSpace( pixPos / texSize, depth ) );
					auto depthBufferWorldPos = writer.declLocale( "depthBufferWorldPos"
						, luminanceSettings.cameraData->objProjToWorld( vec4( clipSpace, 1.0f ) ) );
					depthBufferWorldPos /= depthBufferWorldPos.w();
					writer.returnStmt( depthBufferWorldPos.xyz() / 1000.0_f );
				}
				, sdw::InFloat{ writer, "depth" }
				, sdw::InVec2{ writer, "pixPos" }
				, sdw::InVec2{ writer, "texSize" } );
		}

		return m_getWorldPos( pdepth, ppixPos, ptexSize );
	}

	sdw::Vec3 AtmosphereModel::getPositionFromEarth( sdw::Vec3 const & position )const
	{
		return position + vec3( 0.0_f, getEarthRadius(), 0.0_f );
	}

	sdw::Vec3 AtmosphereModel::getCameraPositionFromEarth()const
	{
		return getPositionFromEarth( getCameraPosition() );
	}

	RetRay AtmosphereModel::castRay( sdw::Vec2 const & puv )
	{
		if ( !m_castRay )
		{
			m_castRay = writer.implementFunction< Ray >( "atm_castRay"
				, [&]( sdw::Vec2 const & uv )
				{
					auto clipSpace = writer.declLocale( "clipSpace"
						, atmodel::getClipSpace( uv, 1.0_f ) );
					auto hPos = writer.declLocale( "hPos"
						, camProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto result = writer.declLocale< Ray >( "result" );
					result.origin = getCameraPositionFromEarth();
					result.direction = normalize( hPos.xyz() / hPos.w() - getCameraPosition() );

					writer.returnStmt( result );
				}
				, sdw::InVec2{ writer, "uv" } );
		}

		return m_castRay( puv );
	}

	RetRay AtmosphereModel::castRay( sdw::Vec2 const & pscreenPoint
		, sdw::Vec2 const & pscreenSize )
	{
		return castRay( pscreenPoint / pscreenSize );
	}

	sdw::RetVec3 AtmosphereModel::getMultipleScattering( sdw::Vec3 const & pscattering
		, sdw::Vec3 const & pextinction
		, sdw::Vec3 const & pworldPos
		, sdw::Float const & pviewZenithCosAngle )
	{
		if ( !m_getMultipleScattering )
		{
			m_getMultipleScattering = writer.implementFunction< sdw::Vec3 >( "atm_getMultipleScattering"
				, [&]( sdw::Vec3 const & scattering
					, sdw::Vec3 const & extinction
					, sdw::Vec3 const & worldPos
					, sdw::Float const & viewZenithCosAngle )
				{
					auto uv = writer.declLocale( "uv"
						, clamp( vec2( viewZenithCosAngle * 0.5_f + 0.5_f
								, ( length( worldPos ) - getEarthRadius() ) / ( getAtmosphereThickness() ) )
							, vec2( 0.0_f )
							, vec2( 1.0_f ) ) );
					uv = vec2( fromUnitToSubUvs( uv.x(), atmosphereData.multiScatteringLUTRes )
						, fromUnitToSubUvs( uv.y(), atmosphereData.multiScatteringLUTRes ) );

					writer.returnStmt( multiScatTexture->lod( uv, 0.0_f ).rgb() );
				}
				, sdw::InVec3{ writer, "scattering" }
				, sdw::InVec3{ writer, "extinction" }
				, sdw::InVec3{ writer, "worldPos" }
				, sdw::InFloat{ writer, "viewZenithCosAngle" } );
		}

		return m_getMultipleScattering( pscattering
			, pextinction
			, pworldPos
			, pviewZenithCosAngle );
	}

	RetSingleScatteringResult AtmosphereModel::integrateScatteredLuminance( sdw::Vec2 const & ppixPos
		, Ray const & pray
		, sdw::Vec3 const & psunDir
		, sdw::Float const & psampleCountIni
		, sdw::Float const & pdepthBufferValue
		, sdw::Float const & ptMaxMax )
	{
		if ( !m_integrateScatteredLuminance )
		{
			m_integrateScatteredLuminance = writer.implementFunction< SingleScatteringResult >( "atm_integrateScatteredLuminance"
				, [&]( sdw::Vec2 const & pixPos
					, Ray const & ray
					, sdw::Vec3 const & sunDir
					, sdw::Float const & sampleCountIni
					, sdw::Float const & depthBufferValue
					, sdw::Float const & tMaxMax )
				{
					auto result = writer.declLocale< SingleScatteringResult >( "result" );
					result.luminance() = vec3( 0.0_f );
					result.opticalDepth() = vec3( 0.0_f );
					result.transmittance() = vec3( 0.0_f );
					result.multiScatAs1() = vec3( 0.0_f );
					result.newMultiScatStep0Out() = vec3( 0.0_f );
					result.newMultiScatStep1Out() = vec3( 0.0_f );

					// Compute next intersection with atmosphere or ground 
					auto earthO = writer.declLocale( "earthO"
						, vec3( 0.0_f, 0.0f, 0.0f ) );
					auto tBottom = writer.declLocale( "tBottom"
						, raySphereIntersectNearest( ray, earthO, getEarthRadius() ) );
					auto tTop = writer.declLocale( "tTop"
						, raySphereIntersectNearest( ray, earthO, getAtmosphereRadius() ) );
					auto tMax = writer.declLocale( "tMax"
						, 0.0_f );
					doInitRay( depthBufferValue, pixPos, ray, tMaxMax, earthO, tTop, tBottom, result, tMax );

					// Sample count 
					auto sampleCount = writer.declLocale( "sampleCount"
						, ( luminanceSettings.variableSampleCount
							? mix( atmosphereData.rayMarchMinMaxSPP.x()
								, atmosphereData.rayMarchMinMaxSPP.y()
								, clamp( tMax * 0.01_f, 0.0_f, 1.0_f ) )
							: sampleCountIni ) );
					auto sampleCountFloor = writer.declLocale( "sampleCountFloor"
						, ( luminanceSettings.variableSampleCount
							? floor( sampleCount )
							: sampleCountIni ) );
					auto tMaxFloor = writer.declLocale( "tMaxFloor"
						, ( luminanceSettings.variableSampleCount
							? tMax * sampleCountFloor / sampleCount		// rescale tMax to map to the last entire step segment.
							: tMax ) );
					auto dt = writer.declLocale( "dt"
						, tMax / sampleCount );

					// Phase functions
					auto wi = writer.declLocale( "wi"
						, sunDir );
					auto wo = writer.declLocale( "wo"
						, ray.direction );
					auto cosTheta = writer.declLocale( "cosTheta"
						, dot( wi, wo ) );
					auto miePhaseValue = writer.declLocale( "miePhaseValue"
						, hgPhase( atmosphereData.miePhaseFunctionG, -cosTheta ) ); 	// negate cosTheta because due to worldDir being a "in" direction. 
					auto rayleighPhaseValue = writer.declLocale( "rayleighPhaseValue"
						, rayleighPhase( cosTheta ) );

					auto globalL = writer.declLocale( "globalL"
						, ( luminanceSettings.illuminanceIsOne
							// When building the scattering factor, we assume light illuminance is 1 to compute a transfert function relative to identity illuminance of 1.
							// This make the scattering factor independent of the light. It is now only linked to the atmosphere properties.
							? vec3( 1.0_f )
							: atmosphereData.sunIlluminance ) );

					// Ray march the atmosphere to integrate optical depth
					auto L = writer.declLocale( "L"
						, vec3( 0.0_f ) );
					auto throughput = writer.declLocale( "throughput"
						, vec3( 1.0_f ) );
					auto opticalDepth = writer.declLocale( "opticalDepth"
						, vec3( 0.0_f ) );
					auto t = writer.declLocale( "t"
						, 0.0_f );
					auto tPrev = writer.declLocale( "tPrev"
						, 0.0_f );
					auto sampleSegmentT = 0.3_f;
					auto uniformPhase = 1.0_f / ( 4.0_f * sdw::Float{ castor::Pi< float > } );

					FOR( writer, sdw::Float, s, 0.0_f, s < sampleCount, s += 1.0_f )
					{
						doStepRay( s, sampleCount, sampleCountFloor, tMaxFloor, tMax, sampleSegmentT, t, dt );
						auto rayToSun = writer.declLocale< Ray >( "rayToSun" );
						rayToSun.direction = sunDir;
						rayToSun.origin = ray.step( t );
						auto medium = writer.declLocale( "medium", sampleMediumRGB( rayToSun.origin ) );
						auto [upVector, sunZenithCosAngle, uv, sampleTransmittance, transmittanceToSun] = doGetSunTransmittance( rayToSun
							, medium
							, dt
							, opticalDepth );
						auto earthShadow = writer.declLocale( "earthShadow"
							, doGetEarthShadow( rayToSun, earthO, upVector ) );
						auto phaseTimesScattering = writer.declLocale< sdw::Vec3 >( "phaseTimesScattering"
							, ( luminanceSettings.mieRayPhase
								? medium.scatteringMie() * miePhaseValue + medium.scatteringRay() * rayleighPhaseValue
								: medium.scattering() * uniformPhase ) );
						// Dual scattering for multi scattering
						auto multiScatteredLuminance = writer.declLocale( "multiScatteredLuminance"
							, ( ( luminanceSettings.multiScatApproxEnabled && multiScatTexture )
								? getMultipleScattering( medium.scattering(), medium.extinction(), rayToSun.origin, sunZenithCosAngle )
								: vec3( 0.0_f ) ) );
						auto S = writer.declLocale( "S"
							, globalL * ( earthShadow * transmittanceToSun * phaseTimesScattering + multiScatteredLuminance * medium.scattering() ) );
						doComputeStep( medium
							, dt
							, sampleTransmittance
							, earthShadow
							, transmittanceToSun
							, multiScatteredLuminance
							, S
							, t
							, tPrev
							, throughput
							, L
							, result );
					}
					ROF;

					//doProcessGround( sunDir, tMax , tBottom, globalL, throughput, L );
					result.luminance() = L;
					result.opticalDepth() = opticalDepth;
					result.transmittance() = throughput;
					writer.returnStmt( result );
				}
				, sdw::InVec2{ writer, "pixPos" }
				, InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "sunDir" }
				, sdw::InFloat{ writer, "sampleCountIni" }
				, sdw::InFloat{ writer, "depthBufferValue" }
				, sdw::InFloat{ writer, "tMaxMax" } );
		}

		return m_integrateScatteredLuminance( ppixPos
			, pray
			, psunDir
			, psampleCountIni
			, pdepthBufferValue
			, ptMaxMax );
	}

	sdw::RetBoolean AtmosphereModel::moveToTopAtmosphere( Ray & pray )
	{
		if ( !m_moveToTopAtmosphere )
		{
			m_moveToTopAtmosphere = writer.implementFunction< sdw::Boolean >( "atm_moveToTopAtmosphere"
				, [&]( Ray ray )
				{
					auto viewHeight = writer.declLocale( "viewHeight"
						, length( ray.origin ) );

					IF( writer, viewHeight > getAtmosphereRadius() )
					{
						auto tTop = writer.declLocale( "tTop"
							, raySphereIntersectNearest( ray
								, vec3( 0.0_f, 0.0_f, 0.0_f )
								, getAtmosphereRadius() ) );

						IF( writer, tTop.valid() )
						{
							auto upVector = writer.declLocale( "upVector"
								, ray.origin / viewHeight );
							auto upOffset = writer.declLocale( "upOffset"
								, upVector * -planetRadiusOffset );
							ray.origin = tTop.point() + upOffset;
						}
						ELSE
						{
							// Ray is not intersecting the atmosphere
							writer.returnStmt( sdw::Boolean{ false } );
						}
						FI;
					}
					FI;

					writer.returnStmt( sdw::Boolean{ true } ); // ok to start tracing
				}
				, InOutRay{ writer, "ray" } );
		}

		return m_moveToTopAtmosphere( pray );
	}

	sdw::RetVec3 AtmosphereModel::getSunRadiance( sdw::Vec3 const & pcameraPosition
		, sdw::Vec3 const & psunDir
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap )
	{
		if ( !m_getSunRadiance )
		{
			m_getSunRadiance = writer.implementFunction< sdw::Vec3 >( "atm_getSunRadiance"
				, [&]( sdw::Vec3 const & cameraPosition
					, sdw::Vec3 const & sunDir
					, sdw::CombinedImage2DRgba32 const & transmittanceMap )
				{
					auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
						, dot( sunDir, normalize( getCameraPositionFromEarth() ) ) );

					auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
					lutTransmittanceParamsToUv( getEarthRadius(), sunZenithCosAngle, uv );
					writer.returnStmt( transmittanceMap.lod( uv, 0.0_f ).rgb() );
				}
				, sdw::InVec3{ writer, "cameraPosition" }
				, sdw::InVec3{ writer, "sunDir" }
				, sdw::InCombinedImage2DRgba32{ writer, "transmittanceMap" } );
		}

		return m_getSunRadiance( pcameraPosition
			, psunDir
			, ptransmittanceMap );
	}

	sdw::RetFloat AtmosphereModel::getEarthShadow( sdw::Vec3 const & pearthO
		, sdw::Vec3 const & pposition )
	{
		if ( !m_getEarthShadow )
		{
			m_getEarthShadow = writer.implementFunction< sdw::Float >( "atm_getEarthShadow"
				, [&]( sdw::Vec3 const & earthO
					, sdw::Vec3 const & position )
				{
					auto rayToSun = writer.declLocale< Ray >( "rayToSun" );
					rayToSun.direction = getSunDirection();
					rayToSun.origin = position;
					writer.returnStmt( doGetEarthShadow( rayToSun
						, earthO
						, normalize( rayToSun.origin ) ) );
				}
				, sdw::InVec3{ writer, "earthO" }
				, sdw::InVec3{ writer, "position" } );
		}

		return m_getEarthShadow( pearthO, pposition );
	}

	RetIntersection AtmosphereModel::raySphereIntersectNearest( Ray const & pray
		, sdw::Vec3 const & psphereCenter
		, sdw::Float const & psphereRadius )
	{
		if ( !m_raySphereIntersectNearest )
		{
			m_raySphereIntersectNearest = writer.implementFunction< Intersection >( "atm_raySphereIntersectNearest"
				, [&]( Ray const & ray
					, sdw::Vec3 const & sphereCenter
					, sdw::Float const & sphereRadius )
				{
					auto result = writer.declLocale< Intersection >( "result" );
					result.valid() = 0_b;
					result.point() = vec3( 0.0_f );
					auto s0_r0 = writer.declLocale( "s0_r0"
						, ray.origin - sphereCenter );

					auto a = writer.declLocale( "a"
						, dot( ray.direction, ray.direction ) );
					auto b = writer.declLocale( "b"
						, 2.0_f * dot( ray.direction, s0_r0 ) );
					auto c = writer.declLocale( "c"
						, dot( s0_r0, s0_r0 ) - ( sphereRadius * sphereRadius ) );
					auto delta = writer.declLocale( "delta"
						, b * b - 4.0_f * a * c );

					IF( writer, delta < 0.0_f || a == 0.0_f )
					{
						writer.returnStmt( result );
					}
					FI;

					auto t = writer.declLocale( "t"
						, ( -b - sqrt( delta ) ) / ( 2.0_f * a ) );

					IF( writer, t < 0.0_f )
					{
						t = ( -b + sqrt( delta ) ) / ( 2.0_f * a );
					}
					FI;

					IF( writer, t >= 0.0_f )
					{
						result.t() = t;
						result.point() = ray.step( t );
						result.valid() = 1_b;
					}
					FI;

					writer.returnStmt( result );
				}
				, InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "sphereCenter" }
				, sdw::InFloat{ writer, "sphereRadius" } );
		}

		return m_raySphereIntersectNearest( pray
			, psphereCenter
			, psphereRadius );
	}

	RetIntersection AtmosphereModel::raySphereIntersectNearest( Ray const & ray
		, sdw::Float const & sphereRadius )
	{
		return raySphereIntersectNearest( ray, vec3( 0.0_f ), sphereRadius );
	}

	sdw::RetFloat AtmosphereModel::hgPhase( sdw::Float const & pg
		, sdw::Float const & pcosTheta )
	{
		if ( !m_hgPhase )
		{
			m_hgPhase = writer.implementFunction< sdw::Float >( "atm_hgPhase"
				, [&]( sdw::Float const & g
					, sdw::Float const & cosTheta )
				{
					auto numer = writer.declLocale( "numer"
						, 1.0f - g * g );
					auto denom = writer.declLocale( "denom"
						, 1.0f + g * g + 2.0f * g * cosTheta );
					writer.returnStmt( numer / ( 4.0f * castor::Pi< float > * denom * sqrt( denom ) ) );
				}
				, sdw::InFloat{ writer, "g" }
				, sdw::InFloat{ writer, "cosTheta" } );
		}

		return m_hgPhase( pg, pcosTheta );
	}

	RetMediumSampleRGB AtmosphereModel::sampleMediumRGB( sdw::Vec3 const & pworldPos )
	{
		if ( !m_sampleMediumRGB )
		{
			m_sampleMediumRGB = writer.implementFunction< MediumSampleRGB >( "atm_sampleMediumRGB"
				, [&]( sdw::Vec3 const & worldPos )
				{
					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) - getEarthRadius() );

					auto densityMie = writer.declLocale( "densityMie"
						, exp( atmosphereData.mieDensity1ExpScale * viewHeight ) );
					auto densityRay = writer.declLocale( "densityRay"
						, exp( atmosphereData.rayleighDensity1ExpScale * viewHeight ) );
					auto densityOzo = writer.declLocale( "densityOzo"
						, clamp( writer.ternary( viewHeight < atmosphereData.absorptionDensity0LayerWidth
								, atmosphereData.absorptionDensity0LinearTerm * viewHeight + atmosphereData.absorptionDensity0ConstantTerm
								, atmosphereData.absorptionDensity1LinearTerm * viewHeight + atmosphereData.absorptionDensity1ConstantTerm )
							, 0.0_f
							, 1.0_f ) );

					auto s = writer.declLocale< MediumSampleRGB >( "s" );

					s.scatteringMie() = densityMie * atmosphereData.mieScattering;
					s.absorptionMie() = densityMie * atmosphereData.mieAbsorption;
					s.extinctionMie() = densityMie * atmosphereData.mieExtinction;

					s.scatteringRay() = densityRay * atmosphereData.rayleighScattering;
					s.absorptionRay() = vec3( 0.0_f );
					s.extinctionRay() = s.scatteringRay() + s.absorptionRay();

					s.scatteringOzo() = vec3( 0.0_f );
					s.absorptionOzo() = densityOzo * atmosphereData.absorptionExtinction;
					s.extinctionOzo() = s.scatteringOzo() + s.absorptionOzo();

					s.scattering() = s.scatteringMie() + s.scatteringRay() + s.scatteringOzo();
					s.absorption() = s.absorptionMie() + s.absorptionRay() + s.absorptionOzo();
					s.extinction() = s.extinctionMie() + s.extinctionRay() + s.extinctionOzo();
					s.albedo() = s.scattering() / max( vec3( 0.001_f ), s.extinction() );

					writer.returnStmt( s );
				}
				, sdw::InVec3{ writer, "worldPos" } );
		}

		return m_sampleMediumRGB( pworldPos );
	}

	sdw::Float AtmosphereModel::rayleighPhase( sdw::Float const & cosTheta )
	{
		return 3.0_f / ( 16.0_f * castor::Pi< float > ) * ( 1.0_f + cosTheta * cosTheta );
	}

	sdw::Float AtmosphereModel::fromUnitToSubUvs( sdw::Float u, sdw::Float resolution )
	{
		return ( u + 0.5f / resolution ) * ( resolution / ( resolution + 1.0f ) );
	}

	sdw::Float AtmosphereModel::fromSubUvsToUnit( sdw::Float u, sdw::Float resolution )
	{
		return ( u - 0.5f / resolution ) * ( resolution / ( resolution - 1.0f ) );
	}

	sdw::Void AtmosphereModel::uvToLutTransmittanceParams( sdw::Float & pviewHeight
		, sdw::Float & pviewZenithCosAngle
		, sdw::Vec2 const & puv )
	{
		if ( !m_uvToLutTransmittanceParams )
		{
			m_uvToLutTransmittanceParams = writer.implementFunction< sdw::Void >( "atm_uvToLutTransmittanceParams"
				, [&]( sdw::Float viewHeight
					, sdw::Float viewZenithCosAngle
					, sdw::Vec2 const & uv )
				{
					auto x_mu = writer.declLocale( "x_mu"
						, uv.x() );
					auto x_r = writer.declLocale( "x_r"
						, uv.y() );

					auto H = writer.declLocale( "H"
						, sqrt( getAtmosphereRadius() * getAtmosphereRadius() - getEarthRadius() * getEarthRadius() ) );
					auto rho = writer.declLocale( "rho"
						, H * x_r );
					viewHeight = sqrt( rho * rho + getEarthRadius() * getEarthRadius() );

					auto d_min = writer.declLocale( "d_min"
						, getAtmosphereRadius() - viewHeight );
					auto d_max = writer.declLocale( "d_max"
						, rho + H );
					auto d = writer.declLocale( "d"
						, d_min + x_mu * ( d_max - d_min ) );
					viewZenithCosAngle = writer.ternary( d == 0.0_f
						, 1.0_f
						, ( H * H - rho * rho - d * d ) / ( 2.0_f * viewHeight * d ) );
					viewZenithCosAngle = clamp( viewZenithCosAngle, -1.0_f, 1.0_f );
				}
				, sdw::OutFloat{ writer, "viewHeight" }
				, sdw::OutFloat{ writer, "viewZenithCosAngle" }
				, sdw::InVec2{ writer, "uv" } );
		}

		return m_uvToLutTransmittanceParams( pviewHeight, pviewZenithCosAngle, puv );
	}

	sdw::Void AtmosphereModel::lutTransmittanceParamsToUv( sdw::Float const & pviewHeight
		, sdw::Float const & pviewZenithCosAngle
		, sdw::Vec2 & puv )
	{
		if ( !m_lutTransmittanceParamsToUv )
		{
			m_lutTransmittanceParamsToUv = writer.implementFunction< sdw::Void >( "atm_lutTransmittanceParamsToUv"
				, [&]( sdw::Float const & viewHeight
					, sdw::Float const & viewZenithCosAngle
					, sdw::Vec2 uv )
				{
					auto H = writer.declLocale( "H"
						, sqrt( max( 0.0_f, getAtmosphereRadius() * getAtmosphereRadius() - getEarthRadius() * getEarthRadius() ) ) );
					auto rho = writer.declLocale( "rho"
						, sqrt( max( 0.0_f, viewHeight * viewHeight - getEarthRadius() * getEarthRadius() ) ) );

					auto discriminant = writer.declLocale( "discriminant"
						, viewHeight * viewHeight * ( viewZenithCosAngle * viewZenithCosAngle - 1.0_f ) + getAtmosphereRadius() * getAtmosphereRadius() );
					auto d = writer.declLocale( "d"
						, max( 0.0_f, ( -viewHeight * viewZenithCosAngle + sqrt( discriminant ) ) ) ); // Distance to atmosphere boundary

					auto d_min = writer.declLocale( "d_min"
						, getAtmosphereRadius() - viewHeight );
					auto d_max = writer.declLocale( "d_max"
						, rho + H );
					auto x_mu = writer.declLocale( "x_mu"
						, ( d - d_min ) / ( d_max - d_min ) );
					auto x_r = writer.declLocale( "x_r"
						, rho / H );

					uv = vec2( x_mu, x_r );
				}
				, sdw::InFloat{ writer, "viewHeight" }
				, sdw::InFloat{ writer, "viewZenithCosAngle" }
				, sdw::OutVec2{ writer, "uv" } );
		}

		return m_lutTransmittanceParamsToUv( pviewHeight, pviewZenithCosAngle, puv );
	}

	sdw::Void AtmosphereModel::uvToSkyViewLutParams( sdw::Float & pviewZenithCosAngle
		, sdw::Float & plightViewCosAngle
		, sdw::Float const & pviewHeight
		, sdw::Vec2 const & puv
		, sdw::Vec2 const & psize )
	{
		if ( !m_uvToSkyViewLutParams )
		{
			m_uvToSkyViewLutParams = writer.implementFunction< sdw::Void >( "atm_uvToSkyViewLutParams"
				, [&]( sdw::Float viewZenithCosAngle
					, sdw::Float lightViewCosAngle
					, sdw::Float const & viewHeight
					, sdw::Vec2 uv
					, sdw::Vec2 size )
				{
					// Constrain uvs to valid sub texel range (avoid zenith derivative issue making LUT usage visible)
					uv = vec2( fromSubUvsToUnit( uv.x(), size.x() )
						, fromSubUvsToUnit( uv.y(), size.y() ) );

					auto vHorizon = writer.declLocale( "vHorizon"
						, sqrt( viewHeight * viewHeight - getEarthRadius() * getEarthRadius() ) );
					auto cosBeta = writer.declLocale( "cosBeta"
						, vHorizon / viewHeight );				// GroundToHorizonCos
					auto beta = writer.declLocale( "beta"
						, acos( cosBeta ) );
					auto zenithHorizonAngle = writer.declLocale( "zenithHorizonAngle"
						, castor::Pi< float > - beta );

					IF( writer, uv.y() < 0.5_f )
					{
						auto coords = writer.declLocale( "coords"
							, 2.0_f * uv.y() );
						coords = 1.0_f - coords;
						coords *= coords;
						coords = 1.0_f - coords;
						viewZenithCosAngle = cos( zenithHorizonAngle * coords );
					}
					ELSE
					{
						auto coords = writer.declLocale( "coords"
							, uv.y() * 2.0_f - 1.0_f );
						coords *= coords;
						viewZenithCosAngle = cos( zenithHorizonAngle + beta * coords );
					}
					FI;

					auto coord = writer.declLocale( "coord"
						, uv.x() );
					coord *= coord;
					lightViewCosAngle = -( coord * 2.0_f - 1.0_f );
				}
				, sdw::OutFloat{ writer, "viewZenithCosAngle" }
				, sdw::OutFloat{ writer, "lightViewCosAngle" }
				, sdw::InFloat{ writer, "viewHeight" }
				, sdw::InVec2{ writer, "uv" }
				, sdw::InVec2{ writer, "size" } );
		}

		return m_uvToSkyViewLutParams( pviewZenithCosAngle, plightViewCosAngle, pviewHeight, puv, psize );
	}

	sdw::Void AtmosphereModel::skyViewLutParamsToUv( sdw::Boolean const & pintersectGround
		, sdw::Float const & pviewZenithCosAngle
		, sdw::Float const & plightViewCosAngle
		, sdw::Float const & pviewHeight
		, sdw::Vec2 & puv
		, sdw::Vec2 const & psize )
	{
		if ( !m_skyViewLutParamsToUv )
		{
			m_skyViewLutParamsToUv = writer.implementFunction< sdw::Void >( "atm_skyViewLutParamsToUv"
				, [&]( sdw::Boolean const & intersectGround
					,  sdw::Float const & viewZenithCosAngle
					, sdw::Float const & lightViewCosAngle
					, sdw::Float const & viewHeight
					, sdw::Vec2 uv
					, sdw::Vec2 const & size )
				{
					auto Vhorizon = writer.declLocale( "Vhorizon"
						, sqrt( viewHeight * viewHeight - getEarthRadius() * getEarthRadius() ) );
					auto cosBeta = writer.declLocale( "cosBeta"
						, Vhorizon / viewHeight );				// GroundToHorizonCos
					auto beta = writer.declLocale( "beta"
						, acos( cosBeta ) );
					auto zenithHorizonAngle = writer.declLocale( "zenithHorizonAngle"
						, sdw::Float{ castor::Pi< float > } - beta );

					IF( writer, !intersectGround )
					{
						auto coord = writer.declLocale( "coord"
							, acos( viewZenithCosAngle ) / zenithHorizonAngle );
						coord = 1.0_f - coord;
						coord = sqrt( coord );
						coord = 1.0_f - coord;
						uv.y() = coord * 0.5_f;
					}
					ELSE
					{
						auto coord = writer.declLocale( "coord"
						, ( acos( viewZenithCosAngle ) - zenithHorizonAngle ) / beta );
						coord = sqrt( coord );
						uv.y() = coord * 0.5_f + 0.5_f;
					}
					FI;
					{
						auto coord = writer.declLocale( "coord"
							, -lightViewCosAngle * 0.5_f + 0.5_f );
						coord = sqrt( coord );
						uv.x() = coord;
					}

					// Constrain uvs to valid sub texel range (avoid zenith derivative issue making LUT usage visible)
					uv = vec2( fromUnitToSubUvs( uv.x(), size.x() ), fromUnitToSubUvs( uv.y(), size.y() ) );
				}
				, sdw::InBoolean{ writer, "intersectGround" }
				, sdw::InFloat{ writer, "viewZenithCosAngle" }
				, sdw::InFloat{ writer, "lightViewCosAngle" }
				, sdw::InFloat{ writer, "viewHeight" }
				, sdw::OutVec2{ writer, "uv" }
				, sdw::InVec2{ writer, "size" } );
		}

		return m_skyViewLutParamsToUv( pintersectGround
			, pviewZenithCosAngle
			, plightViewCosAngle
			, pviewHeight
			, puv
			, psize );
	}

	void AtmosphereModel::doInitRay( sdw::Float const & depthBufferValue
		, sdw::Vec2 const & pixPos
		, Ray const & ray
		, sdw::Float const & tMaxMax
		, sdw::Vec3 const & earthO
		, Intersection const & tTop
		, Intersection const & tBottom
		, SingleScatteringResult const & result
		, sdw::Float & tMax )
	{
		IF( writer, !tBottom.valid() )
		{
			IF( writer, !tTop.valid() )
			{
				tMax = 0.0f; // No intersection with earth nor atmosphere: stop right away  
				writer.returnStmt( result );
			}
			ELSE
			{
				tMax = tTop.t();
			}
			FI;
		}
		ELSE
		{
			IF( writer, tTop.t() > 0.0_f )
			{
				tMax = min( tTop.t(), tBottom.t() );
			}
			FI;
		}
		FI;

		if ( luminanceSettings.cameraData )
		{
			IF( writer, depthBufferValue >= 0.0_f
				&& depthBufferValue < 1.0f )
			{
				auto transmittanceLutExtent = vec2( sdw::Float{ float( transmittanceExtent.width ) }
				, float( transmittanceExtent.height ) );
				auto depthBufferWorldPos = writer.declLocale( "depthBufferWorldPos"
					, getWorldPos( depthBufferValue
						, pixPos
						, transmittanceLutExtent ) );
				auto tDepth = writer.declLocale( "tDepth"
					, length( depthBufferWorldPos - ( ray.origin + vec3( 0.0_f, -getEarthRadius(), 0.0_f ) ) ) ); // apply earth offset to go back to origin as top of earth mode. 

				IF( writer, tDepth < tMax )
				{
					tMax = tDepth;
				}
				FI;
			}
			FI;
		}

		tMax = min( tMax, tMaxMax );
	}

	void AtmosphereModel::doStepRay( sdw::Float const & s
		, sdw::Float const & sampleCount
		, sdw::Float const & sampleCountFloor
		, sdw::Float const & tMaxFloor
		, sdw::Float const & tMax
		, sdw::Float const & sampleSegmentT
		, sdw::Float & t
		, sdw::Float & dt )
	{
		if ( luminanceSettings.variableSampleCount )
		{
			// More expensive but artifact free
			auto t0 = writer.declLocale( "t0"
				, ( s ) / sampleCountFloor );
			auto t1 = writer.declLocale( "t1"
				, ( s + 1.0_f ) / sampleCountFloor );
			// Non linear distribution of sample within the range.
			t0 = t0 * t0;
			t1 = t1 * t1;
			// Make t0 and t1 world space distances.
			t0 = tMaxFloor * t0;

			IF( writer, t1 > 1.0_f )
			{
				t1 = tMax;
				//	t1 = tMaxFloor;	// this reveal depth slices
			}
			ELSE
			{
				t1 = tMaxFloor * t1;
			}
			FI;

			//t = t0 + (t1 - t0) * (whangHashNoise(pixPos.x, pixPos.y, gFrameId * 1920 * 1080)); // With dithering required to hide some sampling artefact relying on TAA later? This may even allow volumetric shadow?
			t = t0 + ( t1 - t0 ) * sampleSegmentT;
			dt = t1 - t0;
		}
		else
		{
			//t = tMax * (s + sampleSegmentT) / sampleCount;
			// Exact difference, important for accuracy of multiple scattering
			auto newT = writer.declLocale( "newT"
				, tMax * ( s + sampleSegmentT ) / sampleCount );
			dt = newT - t;
			t = newT;
		}
	}

	std::tuple< sdw::Vec3, sdw::Float, sdw::Vec2, sdw::Vec3, sdw::Vec3 > AtmosphereModel::doGetSunTransmittance( Ray const & rayToSun
		, MediumSampleRGB const & medium
		, sdw::Float const & dt
		, sdw::Vec3 & opticalDepth )
	{
		auto sampleOpticalDepth = writer.declLocale( "sampleOpticalDepth"
			, medium.extinction() * dt );
		auto sampleTransmittance = writer.declLocale( "sampleTransmittance"
			, exp( -sampleOpticalDepth ) );
		opticalDepth += sampleOpticalDepth;

		auto pHeight = writer.declLocale( "pHeight"
			, length( rayToSun.origin ) );
		auto upVector = writer.declLocale( "upVector"
			, rayToSun.origin / pHeight );
		auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
			, dot( rayToSun.direction, upVector ) );
		auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
		lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle, uv );
		auto transmittanceToSun = writer.declLocale( "transmittanceToSun"
			, ( transmittanceTexture
				? transmittanceTexture->lod( uv, 0.0_f ).rgb()
				: vec3( 0.0_f ) ) );
		return { upVector, sunZenithCosAngle, uv, sampleTransmittance, transmittanceToSun };
	}

	void AtmosphereModel::doComputeStep( MediumSampleRGB const & medium
		, sdw::Float const & dt
		, sdw::Vec3 const & sampleTransmittance
		, sdw::Float const & earthShadow
		, sdw::Vec3 const & transmittanceToSun
		, sdw::Vec3 const & multiScatteredLuminance
		, sdw::Vec3 const & S
		, sdw::Float const & t
		, sdw::Float & tPrev
		, sdw::Vec3 & throughput
		, sdw::Vec3 & L
		, SingleScatteringResult & result )
	{
		auto uniformPhase = 1.0_f / ( 4.0_f * sdw::Float{ castor::Pi< float > } );
		// When using the power serie to accumulate all scattering order, serie r must be <1 for a serie to converge.
		// Under extreme coefficient, MultiScatAs1 can grow larger and thus result in broken visuals.
		// The way to fix that is to use a proper analytical integration as proposed in slide 28 of http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
		// However, it is possible to disable as it can also work using simple power serie sum unroll up to 5th order. The rest of the orders has a really low contribution.
		if ( !luminanceSettings.multiScatteringPowerSerie )
		{
			// 1 is the integration of luminance over the 4pi of a sphere, and assuming an isotropic phase function of 1.0/(4*PI)
			result.multiScatAs1() += throughput * medium.scattering() /** 1 */ * dt;
		}
		else
		{
			auto MS = writer.declLocale( "MS"
				, medium.scattering() );
			auto MSint = writer.declLocale( "MSint"
				, ( MS - MS * sampleTransmittance ) / medium.extinction() );
			result.multiScatAs1() += throughput * MSint;
		}

		// Evaluate input to multi scattering 
		{
			auto newMS = writer.declLocale( "newMS"
				, earthShadow * transmittanceToSun * medium.scattering() * uniformPhase );
			result.newMultiScatStep0Out() += throughput * ( newMS - newMS * sampleTransmittance ) / medium.extinction();
			//result.newMultiScatStep0Out += sampleTransmittance * throughput * newMS * dt;

			newMS = medium.scattering() * uniformPhase * multiScatteredLuminance;
			result.newMultiScatStep1Out() += throughput * ( newMS - newMS * sampleTransmittance ) / medium.extinction();
			//result.newMultiScatStep1Out += sampleTransmittance * throughput * newMS * dt;
		}

		// See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/ 
		auto Sint = writer.declLocale( "Sint"
			, ( S - S * sampleTransmittance ) / medium.extinction() );	// integrate along the current step segment 
		L += throughput * Sint;											// accumulate and also take into account the transmittance from previous steps
		throughput *= sampleTransmittance;

		tPrev = t;
	}

	void AtmosphereModel::doProcessGround( sdw::Vec3 const & sunDir
		, sdw::Float const & tMax
		, Intersection const & tBottom
		, sdw::Vec3 const & globalL
		, sdw::Vec3 const & throughput
		, sdw::Vec3 & L )
	{
		if ( luminanceSettings.useGround )
		{
			IF( writer, tMax == tBottom.t() && tBottom.t() > 0.0_f )
			{
				// Account for bounced light off the earth
				auto P = writer.declLocale( "P"
					, tBottom.point() );
				auto pHeight = writer.declLocale( "pHeight"
					, length( P ) );

				auto upVector = writer.declLocale( "upVector"
					, P / pHeight );
				auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
					, dot( sunDir, upVector ) );
				auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
				lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle, uv );
				auto transmittanceToSun = writer.declLocale( "transmittanceToSun"
					, ( transmittanceTexture
						? transmittanceTexture->lod( uv, 0.0_f ).rgb()
						: vec3( 0.0_f ) ) );

				auto NdotL = writer.declLocale( "NdotL"
					, clamp( dot( normalize( upVector ), normalize( sunDir ) ), 0.0_f, 1.0_f ) );
				L += globalL * transmittanceToSun * throughput * NdotL * atmosphereData.groundAlbedo / castor::Pi< float >;
			}
			FI;
		}
	}

	sdw::Float AtmosphereModel::doGetEarthShadow( Ray const & rayToSun
		, sdw::Vec3 const & earthO
		, sdw::Vec3 const & upVector )
	{
		auto tEarth = writer.declLocale( "tEarth"
			, raySphereIntersectNearest( rayToSun, earthO + planetRadiusOffset * upVector, getEarthRadius() ) );
		return writer.ternary( tEarth.valid(), 0.0_f, 1.0_f );
	}

	//************************************************************************************************
}
