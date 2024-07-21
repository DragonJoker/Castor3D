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

		static sdw::expr::ExprList getZeroInit()
		{
			sdw::expr::ExprList result;
			result.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			result.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			result.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			result.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			result.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			result.emplace_back( makeExpr( vec3( 0.0_f ) ) );
			return result;
		}
	}

	//************************************************************************************************

	SingleScatteringResult::SingleScatteringResult( sdw::ShaderWriter & writer )
		: SingleScatteringResult{ writer
			, sdw::makeAggrInit( makeType( writer.getTypesCache() ), atmodel::getZeroInit() )
			, true }
	{
	}

	//************************************************************************************************

	AtmosphereModel::AtmosphereModel( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData
		, Settings psettings )
		: AtmosphereModel{ pwriter, patmosphereData, psettings, {} }
	{
	}

	AtmosphereModel::AtmosphereModel( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData
		, Settings psettings
		, VkExtent2D ptransmittanceExtent )
		: writer{ pwriter }
		, atmosphereData{ patmosphereData }
		, settings{ psettings }
		, transmittanceExtent{ castor::move( ptransmittanceExtent ) }
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
						, settings.cameraData->objProjToWorld( vec4( clipSpace, 1.0f ) ) );
					depthBufferWorldPos /= depthBufferWorldPos.w();
					writer.returnStmt( getPositionToPlanet( depthBufferWorldPos.xyz() * settings.length.kilometres() ) );
				}
				, sdw::InFloat{ writer, "depth" }
				, sdw::InVec2{ writer, "pixPos" }
				, sdw::InVec2{ writer, "texSize" } );
		}

		return m_getWorldPos( pdepth, ppixPos, ptexSize );
	}

	sdw::RetVec3 AtmosphereModel::getMultipleScattering( sdw::Float const & pworldPosLength
		, sdw::Float const & pviewZenithCosAngle )
	{
		if ( !m_getMultipleScattering )
		{
			m_getMultipleScattering = writer.implementFunction< sdw::Vec3 >( "getMultipleScattering"
				, [&]( sdw::Float const & worldPosLength
					, sdw::Float const & viewZenithCosAngle )
				{
					auto uv = writer.declLocale( "uv"
						, clamp( vec2( viewZenithCosAngle * 0.5_f + 0.5_f
								, ( worldPosLength - getPlanetRadius() ) / ( getAtmosphereThickness() ) )
							, vec2( 0.0_f )
							, vec2( 1.0_f ) ) );
					uv = vec2( fromUnitToSubUvs( uv.x(), atmosphereData.multiScatteringLUTRes() )
						, fromUnitToSubUvs( uv.y(), atmosphereData.multiScatteringLUTRes() ) );

					writer.returnStmt( multiScatTexture->lod( uv, 0.0_f ).rgb() );
				}
				, sdw::InFloat{ writer, "worldPosLength" }
				, sdw::InFloat{ writer, "viewZenithCosAngle" } );
		}

		return m_getMultipleScattering( pworldPosLength
			, pviewZenithCosAngle );
	}

	sdw::Vec3 AtmosphereModel::getPositionToPlanet( sdw::Vec3 const & position )const
	{
		return position - getPlanetPosition();
	}

	RetRay AtmosphereModel::castRay( sdw::Vec2 const & puv )
	{
		if ( !m_castRay )
		{
			m_castRay = writer.implementFunction< Ray >( "atm_castRay"
				, [&]( sdw::Vec2 const & uv )
				{
					auto clipSpace = writer.declLocale( "clipSpace"
						, atmodel::getClipSpace( uv, 0.0_f ) );
					auto hPos = writer.declLocale( "hPos"
						, camProjToWorld( vec4( clipSpace, 1.0_f ) ) );

					auto result = writer.declLocale< Ray >( "result" );
					result.origin = getCameraPosition();
					result.direction = normalize( hPos.xyz() / hPos.w() - ( getPlanetPosition() + result.origin ) );

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
					auto result = writer.declLocale( "result"
						, SingleScatteringResult{ writer } );

					// Compute next intersection with atmosphere or ground 
					auto planetO = writer.declLocale( "planetO", vec3( 0.0_f, 0.0f, 0.0f ) );
					auto tBottom = writer.declLocale( "tBottom", raySphereIntersectNearest( ray, planetO, getPlanetRadius() ) );
					auto tTop = writer.declLocale( "tTop", raySphereIntersectNearest( ray, planetO, getAtmosphereRadius() ) );
					auto tMax = writer.declLocale( "tMax", 0.0_f );

					IF( writer, !tBottom.valid() )
					{
						IF( writer, !tTop.valid() )
						{
							tMax = 0.0f; // No intersection with planet nor atmosphere: stop right away  
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

					if ( settings.cameraData )
					{
						IF( writer, depthBufferValue <= 1.0_f
							&& depthBufferValue > 0.0f )
						{
							auto transmittanceLutExtent = sdw::vec2( float( transmittanceExtent.width ), float( transmittanceExtent.height ) );
							auto depthBufferWorldPos = writer.declLocale( "depthBufferWorldPos"
								, getWorldPos( depthBufferValue
									, pixPos
									, transmittanceLutExtent ) );
							auto tDepth = writer.declLocale( "tDepth"
								, length( depthBufferWorldPos - ray.origin ) ); // apply planet offset to go back to origin as top of planet mode. 

							IF( writer, tDepth < tMax )
							{
								tMax = tDepth;
							}
							FI;
						}
						FI;
					}

					tMax = min( tMax, tMaxMax );

					// Sample count 
					auto sampleCount = writer.declLocale( "sampleCount"
						, ( settings.variableSampleCount
							? mix( atmosphereData.rayMarchMinMaxSPP().x()
								, atmosphereData.rayMarchMinMaxSPP().y()
								, clamp( tMax * 0.01_f, 0.0_f, 1.0_f ) )
							: sampleCountIni ) );
					auto sampleCountFloor = writer.declLocale( "sampleCountFloor"
						, ( settings.variableSampleCount
							? floor( sampleCount )
							: sampleCountIni ) );
					auto tMaxFloor = writer.declLocale( "tMaxFloor"
						, ( settings.variableSampleCount
							? tMax * sampleCountFloor / sampleCount		// rescale tMax to map to the last entire step segment.
							: tMax ) );
					auto dt = writer.declLocale( "dt", tMax / sampleCount );

					// Phase functions
					auto wi = writer.declLocale( "wi", sunDir );
					auto wo = writer.declLocale( "wo", ray.direction );
					auto cosTheta = writer.declLocale( "cosTheta", dot( wi, wo ) );
					auto miePhaseValue = writer.declLocale( "miePhaseValue", hgPhase( atmosphereData.miePhaseFunctionG(), -cosTheta ) ); 	// negate cosTheta because due to worldDir being a "in" direction. 
					auto rayleighPhaseValue = writer.declLocale( "rayleighPhaseValue", rayleighPhase( cosTheta ) );

					auto globalL = writer.declLocale( "globalL"
						, ( settings.illuminanceIsOne
							// When building the scattering factor, we assume light illuminance is 1 to compute a transfert function relative to identity illuminance of 1.
							// This make the scattering factor independent of the light. It is now only linked to the atmosphere properties.
							? vec3( 1.0_f )
							: atmosphereData.sunIlluminance() ) );

					// Ray march the atmosphere to integrate optical depth
					auto L = writer.declLocale( "L", vec3( 0.0_f ) );
					auto throughput = writer.declLocale( "throughput", vec3( 1.0_f ) );
					auto opticalDepth = writer.declLocale( "opticalDepth", vec3( 0.0_f ) );
					auto t = writer.declLocale( "t", 0.0_f );
					auto tPrev = writer.declLocale( "tPrev", 0.0_f );

					auto sampleSegmentT = 0.3_f;
					auto uniformPhase = 1.0_f / ( 4.0_f * sdw::Float{ castor::Pi< float > } );

					FOR( writer, sdw::Float, s, 0.0_f, s < sampleCount, s += 1.0_f )
					{
						if ( settings.variableSampleCount )
						{
							// More expensive but artifact free
							auto t0 = writer.declLocale( "t0", ( s ) / sampleCountFloor );
							auto t1 = writer.declLocale( "t1", ( s + 1.0_f ) / sampleCountFloor );
							// Non linear distribution of sample within the range.
							t0 = t0 * t0;
							t1 = t1 * t1;
							// Make t0 and t1 world space distances.
							t0 = tMaxFloor * t0;

							IF( writer, t1 > 1.0_f )
							{
								dt = tMax - t0;
								//	t1 = tMaxFloor;	// this reveal depth slices
							}
							ELSE
							{
								dt = sdw::fma( tMaxFloor, t1, -t0 );
							}
							FI;

							//t = t0 + (t1 - t0) * (whangHashNoise(pixPos.x, pixPos.y, gFrameId * 1920 * 1080)); // With dithering required to hide some sampling artefact relying on TAA later? This may even allow volumetric shadow?
							t = sdw::fma( dt, sampleSegmentT, t0 );
						}
						else
						{
							//t = tMax * (s + sampleSegmentT) / sampleCount;
							// Exact difference, important for accuracy of multiple scattering
							auto newT = writer.declLocale( "newT", tMax * ( s + sampleSegmentT ) / sampleCount );
							dt = newT - t;
							t = newT;
						}

						auto rayToSun = writer.declLocale< Ray >( "rayToSun" );
						rayToSun.direction = sunDir;
						rayToSun.origin = ray.step( t );
						auto medium = writer.declLocale( "medium", sampleMediumRGB( rayToSun.origin ) );
						auto sampleOpticalDepth = writer.declLocale( "sampleOpticalDepth", medium.extinction() * dt );
						auto sampleTransmittance = writer.declLocale( "sampleTransmittance", exp( -sampleOpticalDepth ) );
						opticalDepth += sampleOpticalDepth;

						auto pHeight = writer.declLocale( "pHeight", length( rayToSun.origin ) );
						auto upVector = writer.declLocale( "upVector", rayToSun.origin / pHeight );
						auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle", dot( rayToSun.direction, upVector ) );
						auto trUv = writer.declLocale< sdw::Vec2 >( "trUv" );
						lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle, trUv );
						auto transmittanceToSun = writer.declLocale( "transmittanceToSun"
							, ( transmittanceTexture
								? transmittanceTexture->lod( trUv, 0.0_f ).rgb()
								: vec3( 0.0_f ) ) );

						auto planetShadow = writer.declLocale( "planetShadow", doGetPlanetShadow( rayToSun, planetO, upVector ) );
						auto phaseTimesScattering = writer.declLocale( "phaseTimesScattering"
							, ( settings.mieRayPhase
								? medium.scatteringMie() * miePhaseValue + medium.scatteringRay() * rayleighPhaseValue
								: medium.scattering() * uniformPhase ) );
						// Dual scattering for multi scattering
						auto multiScatteredLuminance = writer.declLocale( "multiScatteredLuminance"
							, vec3( 0.0_f ) );

						if ( settings.multiScatApproxEnabled && multiScatTexture )
						{
							multiScatteredLuminance = getMultipleScattering( pHeight, sunZenithCosAngle );
						}

						auto S = writer.declLocale( "S", globalL * ( planetShadow * transmittanceToSun * phaseTimesScattering + multiScatteredLuminance * medium.scattering() ) );

						// When using the power serie to accumulate all scattering order, serie r must be <1 for a serie to converge.
						// Under extreme coefficient, MultiScatAs1 can grow larger and thus result in broken visuals.
						// The way to fix that is to use a proper analytical integration as proposed in slide 28 of http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
						// However, it is possible to disable as it can also work using simple power serie sum unroll up to 5th order. The rest of the orders has a really low contribution.
						if ( !settings.multiScatteringPowerSerie )
						{
							// 1 is the integration of luminance over the 4pi of a sphere, and assuming an isotropic phase function of 1.0/(4*PI)
							result.multiScatAs1() += throughput * medium.scattering() /** 1 */ * dt;
						}
						else
						{
							auto MS = writer.declLocale( "MS", medium.scattering() );
							auto MSint = writer.declLocale( "MSint", ( MS - MS * sampleTransmittance ) / medium.extinction() );
							result.multiScatAs1() += throughput * MSint;
						}

						// Evaluate input to multi scattering 
						{
							auto newMS = writer.declLocale( "newMS", planetShadow * transmittanceToSun * medium.scattering() * uniformPhase );
							result.newMultiScatStep0Out() += throughput * ( newMS - newMS * sampleTransmittance ) / medium.extinction();
							//result.newMultiScatStep0Out += sampleTransmittance * throughput * newMS * dt;

							newMS = medium.scattering() * uniformPhase * multiScatteredLuminance;
							result.newMultiScatStep1Out() += throughput * ( newMS - newMS * sampleTransmittance ) / medium.extinction();
							//result.newMultiScatStep1Out += sampleTransmittance * throughput * newMS * dt;
						}

						// See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/ 
						auto Sint = writer.declLocale( "Sint", ( S - S * sampleTransmittance ) / medium.extinction() );	// integrate along the current step segment 
						L += throughput * Sint;											// accumulate and also take into account the transmittance from previous steps
						throughput *= sampleTransmittance;

						tPrev = t;
					}
					ROF;

					if ( settings.useGround )
					{
						IF( writer, tMax == tBottom.t() && tBottom.t() > 0.0_f )
						{
							// Account for bounced light off the planet
							auto P = writer.declLocale( "P", tBottom.point() );
							auto pHeight = writer.declLocale( "pHeight", length( P ) );

							auto upVector = writer.declLocale( "upVector", P / pHeight );
							auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle", dot( sunDir, upVector ) );
							auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
							lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle, uv );
							auto transmittanceToSun = writer.declLocale( "transmittanceToSun"
								, ( transmittanceTexture
									? transmittanceTexture->lod( uv, 0.0_f ).rgb()
									: vec3( 0.0_f ) ) );

							auto NdotL = writer.declLocale( "NdotL", clamp( dot( normalize( upVector ), normalize( sunDir ) ), 0.0_f, 1.0_f ) );
							L += globalL * transmittanceToSun * throughput * NdotL * atmosphereData.groundAlbedo() / castor::Pi< float >;
						}
						FI;
					}

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
							writer.returnStmt( 0_b );
						}
						FI;
					}
					FI;

					writer.returnStmt( 1_b ); // ok to start tracing
				}
				, InOutRay{ writer, "ray" } );
		}

		return m_moveToTopAtmosphere( pray );
	}

	sdw::RetVec3 AtmosphereModel::getSunRadiance( sdw::Vec3 const & psunDir
		, sdw::CombinedImage2DRgba32 const & ptransmittanceMap )
	{
		if ( !m_getSunRadiance )
		{
			m_getSunRadiance = writer.implementFunction< sdw::Vec3 >( "atm_getSunRadiance"
				, [&]( sdw::Vec3 const & sunDir
					, sdw::CombinedImage2DRgba32 const & transmittanceMap )
				{
					auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
						, dot( sunDir, normalize( getCameraPosition() ) ) );

					auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
					lutTransmittanceParamsToUv( getPlanetRadius(), sunZenithCosAngle, uv );
					writer.returnStmt( transmittanceMap.lod( uv, 0.0_f ).rgb() );
				}
				, sdw::InVec3{ writer, "sunDir" }
				, sdw::InCombinedImage2DRgba32{ writer, "transmittanceMap" } );
		}

		return m_getSunRadiance( psunDir, ptransmittanceMap );
	}

	sdw::RetFloat AtmosphereModel::getPlanetShadow( sdw::Vec3 const & pplanetO
		, sdw::Vec3 const & pposition )
	{
		if ( !m_getPlanetShadow )
		{
			m_getPlanetShadow = writer.implementFunction< sdw::Float >( "atm_getPlanetShadow"
				, [&]( sdw::Vec3 const & planetOrigin
					, sdw::Vec3 const & position )
				{
					auto rayToSun = writer.declLocale< Ray >( "rayToSun" );
					rayToSun.direction = getSunDirection();
					rayToSun.origin = position;
					writer.returnStmt( doGetPlanetShadow( rayToSun
						, planetOrigin
						, normalize( rayToSun.origin ) ) );
				}
				, sdw::InVec3{ writer, "planetOrigin" }
				, sdw::InVec3{ writer, "position" } );
		}

		return m_getPlanetShadow( pplanetO, pposition );
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
					auto result = writer.declLocale( "result"
						, Intersection{ writer } );
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

					auto t0 = writer.declLocale( "t0"
						, ( -b - sqrt( delta ) ) / ( 2.0_f * a ) );
					auto t1 = writer.declLocale( "t1"
						, ( -b + sqrt( delta ) ) / ( 2.0_f * a ) );

					IF( writer, t0 < 0.0_f && t1 < 0.0_f )
					{
						writer.returnStmt( result );
					}
					FI;

					IF( writer, t0 < 0.0_f )
					{
						result.t() = max( 0.0_f, t1 );
					}
					ELSEIF( t1 < 0.0_f )
					{
						result.t() = max( 0.0_f, t0 );
					}
					ELSE
					{
						result.t() = max( 0.0_f, min( t0, t1 ) );
					}
					FI;

					result.valid() = 1_b;
					result.point() = ray.step( result.t() );
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

	sdw::RetInt AtmosphereModel::raySphereIntersect( Ray const & pray
		, sdw::Float const & psphereRadius
		, Intersection const & pground
		, sdw::Boolean const & pclampToGround
		, Intersection & pnearest
		, Intersection & pfarthest )
	{
		if ( !m_raySphereIntersect )
		{
			m_raySphereIntersect = writer.implementFunction< sdw::Int >( "atm_raySphereIntersect"
				, [&]( Ray const & ray
					, sdw::Float const & sphereRadius
					, Intersection const & ground
					, sdw::Boolean const & clampToGround
					, Intersection nearest
					, Intersection farthest )
				{
					auto s0_r0 = writer.declLocale( "s0_r0"
						, ray.origin );

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
						writer.returnStmt( 0_i );
					}
					FI;

					auto t0 = writer.declLocale( "t0"
						, ( -b - sqrt( delta ) ) / ( 2.0_f * a ) );
					auto t1 = writer.declLocale( "t1"
						, ( -b + sqrt( delta ) ) / ( 2.0_f * a ) );

					IF( writer, t0 < 0.0_f && t1 < 0.0_f )
					{
						writer.returnStmt( 0_i );
					}
					FI;

					auto minSol = writer.declLocale( "minSol"
						, writer.ternary( ground.valid() 
							, min( min( t0, t1 ), ground.t() )
							, min( t0, t1 ) ) );
					auto maxSol = writer.declLocale( "maxSol"
						, writer.ternary( ground.valid() 
							, min( max( t0, t1 ), ground.t() )
							, max( t0, t1 ) ) );

					IF( writer, minSol < 0.0_f || minSol == maxSol )
					{
						nearest.t() = maxSol;
						nearest.point() = ray.step( maxSol );
						nearest.valid() = clampToGround || maxSol != ground.t();
						writer.returnStmt( writer.ternary( nearest.valid(), 1_i, 0_i ) );
					}
					FI;

					nearest.t() = minSol;
					nearest.point() = ray.step( minSol );
					nearest.valid() = 1_b;

					farthest.t() = maxSol;
					farthest.point() = ray.step( maxSol );
					farthest.valid() = clampToGround || maxSol != ground.t();

					writer.returnStmt( writer.ternary( farthest.valid(), 2_i, 1_i ) );
				}
				, InRay{ writer, "ray" }
				, sdw::InFloat{ writer, "sphereRadius" }
				, InIntersection{ writer, "ground" }
				, sdw::InBoolean{ writer, "clampToGround" }
				, OutIntersection{ writer, "nearest" }
				, OutIntersection{ writer, "farthest" } );
		}

		return m_raySphereIntersect( pray
			, psphereRadius
			, pground
			, pclampToGround
			, pnearest
			, pfarthest );
	}

	RetIntersection AtmosphereModel::raySphereintersectSkyMap( sdw::Vec3 const & prd
		, sdw::Float const & pradius )
	{
		if ( !m_raySphereintersectSkyMap )
		{
			m_raySphereintersectSkyMap = writer.implementFunction< Intersection >( "atm_raySphereintersectSkyMap"
				, [&]( sdw::Vec3 const & rd
					, sdw::Float const & radius )
				{
					auto L = writer.declLocale( "L", -vec3( 0.0_f ) );
					auto a = writer.declLocale( "a", dot( rd, rd ) );
					auto b = writer.declLocale( "b", 2.0_f * dot( rd, L ) );
					auto c = writer.declLocale( "c", dot( L, L ) - ( radius * radius ) );
					auto delta = writer.declLocale( "delta", b * b - 4.0_f * a * c );
					auto t = writer.declLocale( "t", max( 0.0_f, ( -b + sqrt( delta ) ) / 2.0_f ) );
					writer.returnStmt( Intersection{ writer, rd * t, 1_b, t } );
				}
				, sdw::InVec3{ writer, "rd" }
				, sdw::InFloat{ writer, "radius" } );
		}

		return m_raySphereintersectSkyMap( prd
			, pradius );
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
						, length( worldPos ) - getPlanetRadius() );

					auto densityMie = writer.declLocale( "densityMie"
						, exp( atmosphereData.mieDensity1ExpScale() * viewHeight ) );
					auto densityRay = writer.declLocale( "densityRay"
						, exp( atmosphereData.rayleighDensity1ExpScale() * viewHeight ) );
					auto densityOzo = writer.declLocale( "densityOzo"
						, clamp( writer.ternary( viewHeight < atmosphereData.absorptionDensity0LayerWidth()
								, sdw::fma( atmosphereData.absorptionDensity0LinearTerm(), viewHeight, atmosphereData.absorptionDensity0ConstantTerm() )
								, sdw::fma( atmosphereData.absorptionDensity1LinearTerm(), viewHeight, atmosphereData.absorptionDensity1ConstantTerm() ) )
							, 0.0_f
							, 1.0_f ) );

					auto s = writer.declLocale< MediumSampleRGB >( "s" );

					s.scatteringMie() = densityMie * atmosphereData.mieScattering();
					s.absorptionMie() = densityMie * atmosphereData.mieAbsorption();
					s.extinctionMie() = densityMie * atmosphereData.mieExtinction();

					s.scatteringRay() = densityRay * atmosphereData.rayleighScattering();
					s.absorptionRay() = vec3( 0.0_f );
					s.extinctionRay() = s.scatteringRay() + s.absorptionRay();

					s.scatteringOzo() = vec3( 0.0_f );
					s.absorptionOzo() = densityOzo * atmosphereData.absorptionExtinction();
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
						, getAtmosphereH() );
					auto rho = writer.declLocale( "rho"
						, H * x_r );
					viewHeight = sqrt( rho * rho + getPlanetRadius() * getPlanetRadius() );

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
						, getAtmosphereH() );
					auto rho = writer.declLocale( "rho"
						, getAtmosphereH( viewHeight ) );

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
						, getAtmosphereH( viewHeight ) );
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
						, getAtmosphereH( viewHeight ) );
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

	sdw::Float AtmosphereModel::doGetPlanetShadow( Ray const & rayToSun
		, sdw::Vec3 const & planetO
		, sdw::Vec3 const & upVector )
	{
		auto tPlanet = writer.declLocale( "tPlanet"
			, raySphereIntersectNearest( rayToSun
				, sdw::fma( upVector, vec3( planetRadiusOffset ), planetO )
				, getPlanetRadius() ) );
		return writer.ternary( tPlanet.valid(), 0.0_f, 1.0_f );
	}

	//************************************************************************************************
}
