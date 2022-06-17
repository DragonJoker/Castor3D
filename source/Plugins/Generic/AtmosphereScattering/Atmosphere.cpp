#include "AtmosphereScattering/Atmosphere.hpp"

#include <Castor3D/Shader/Ubos/MatrixUbo.hpp>

#include <ShaderWriter/Source.hpp>

#define NONLINEARSKYVIEWLUT 1

namespace atmosphere_scattering
{
	//*********************************************************************************************

	SingleScatteringResult::SingleScatteringResult( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, luminance{ getMember< sdw::Vec3 >( "luminance" ) }
		, opticalDepth{ getMember< sdw::Vec3 >( "opticalDepth" ) }
		, transmittance{ getMember< sdw::Vec3 >( "transmittance" ) }
		, multiScatAs1{ getMember< sdw::Vec3 >( "multiScatAs1" ) }
		, newMultiScatStep0Out{ getMember< sdw::Vec3 >( "newMultiScatStep0Out" ) }
		, newMultiScatStep1Out{ getMember< sdw::Vec3 >( "newMultiScatStep1Out" ) }
	{
	}

	ast::type::BaseStructPtr SingleScatteringResult::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eC
			, "SingleScatteringResult" );

		if ( result->empty() )
		{
			result->declMember( "luminance", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "opticalDepth", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "transmittance", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "multiScatAs1", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "newMultiScatStep0Out", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "newMultiScatStep1Out", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->end();
		}

		return result;
	}

	//*********************************************************************************************

	MediumSampleRGB::MediumSampleRGB( sdw::ShaderWriter & writer
		, ast::expr::ExprPtr expr
		, bool enabled )
		: sdw::StructInstance{ writer, std::move( expr ), enabled }
		, scattering{ getMember< sdw::Vec3 >( "scattering" ) }
		, absorption{ getMember< sdw::Vec3 >( "absorption" ) }
		, extinction{ getMember< sdw::Vec3 >( "extinction" ) }
		, scatteringMie{ getMember< sdw::Vec3 >( "scatteringMie" ) }
		, absorptionMie{ getMember< sdw::Vec3 >( "absorptionMie" ) }
		, extinctionMie{ getMember< sdw::Vec3 >( "extinctionMie" ) }
		, scatteringRay{ getMember< sdw::Vec3 >( "scatteringRay" ) }
		, absorptionRay{ getMember< sdw::Vec3 >( "absorptionRay" ) }
		, extinctionRay{ getMember< sdw::Vec3 >( "extinctionRay" ) }
		, scatteringOzo{ getMember< sdw::Vec3 >( "scatteringOzo" ) }
		, absorptionOzo{ getMember< sdw::Vec3 >( "absorptionOzo" ) }
		, extinctionOzo{ getMember< sdw::Vec3 >( "extinctionOzo" ) }
		, albedo{ getMember< sdw::Vec3 >( "albedo" ) }
	{
	}

	ast::type::BaseStructPtr MediumSampleRGB::makeType( ast::type::TypesCache & cache )
	{
		auto result = cache.getStruct( sdw::type::MemoryLayout::eC
			, "MediumSampleRGB" );

		if ( result->empty() )
		{
			result->declMember( "scattering", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "absorption", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "extinction", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "scatteringMie", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "absorptionMie", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "extinctionMie", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "scatteringRay", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "absorptionRay", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "extinctionRay", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "scatteringOzo", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "absorptionOzo", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "extinctionOzo", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->declMember( "albedo", sdw::type::Kind::eVec3F, sdw::type::NotArray );
			result->end();
		}
		return result;
	}

	//************************************************************************************************

	AtmosphereConfig::AtmosphereConfig( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData
		, LuminanceSettings pluminanceSettings )
		: AtmosphereConfig{ pwriter, patmosphereData, pluminanceSettings, {}, nullptr }
	{
	}

	AtmosphereConfig::AtmosphereConfig( sdw::ShaderWriter & pwriter
		, AtmosphereData const & patmosphereData
		, LuminanceSettings pluminanceSettings
		, VkExtent2D ptransmittanceExtent
		, sdw::CombinedImage2DRgba32 const * transmittanceLut )
		: writer{ pwriter }
		, atmosphereData{ patmosphereData }
		, luminanceSettings{ pluminanceSettings }
		, transmittanceExtent{ std::move( ptransmittanceExtent ) }
		, transmittanceTexture{ transmittanceLut }
	{
	}

	SingleScatteringResult AtmosphereConfig::integrateScatteredLuminance( sdw::Vec2 const & ppixPos
		, sdw::Vec3 const & pworldPos
		, sdw::Vec3 const & pworldDir
		, sdw::Vec3 const & psunDir
		, sdw::Float const & psampleCountIni
		, sdw::Float const & pdepthBufferValue
		, sdw::Float const & ptMaxMax )
	{
		if ( !m_integrateScatteredLuminance )
		{
			if ( luminanceSettings.multiScatApproxEnabled )
			{
				CU_Require( multiScatTexture );
				m_getMultipleScattering = writer.implementFunction< sdw::Vec3 >( "getMultipleScattering"
					, [&]( sdw::Vec3 const & scattering
						, sdw::Vec3 const & extinction
						, sdw::Vec3 const & worldPos
						, sdw::Float const & viewZenithCosAngle )
					{
						auto uv = writer.declLocale( "uv"
							, clamp( vec2( viewZenithCosAngle * 0.5_f + 0.5_f
									, ( length( worldPos ) - atmosphereData.bottomRadius ) / ( atmosphereData.topRadius - atmosphereData.bottomRadius ) )
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

			if ( luminanceSettings.shadowMapEnabled )
			{
				CU_Require( shadowMapTexture );
				CU_Require( shadowmapViewProjMat );
				m_getShadow = writer.implementFunction< sdw::Float >( "getShadow"
					, [&]( sdw::Vec3 const & P )
					{
						// First evaluate opaque shadow
						auto shadowUv = writer.declLocale( "shadowUv"
							, ( *shadowmapViewProjMat ) * vec4( P + vec3( 0.0_f, 0.0_f, -atmosphereData.bottomRadius ), 1.0_f ) );
						//shadowUv /= shadowUv.w;	// not be needed as it is an ortho projection
						shadowUv.x() = shadowUv.x() * 0.5_f + 0.5_f;
						shadowUv.y() = -shadowUv.y() * 0.5_f + 0.5_f;

						IF( writer, all( shadowUv.xyz() >= vec3( 0.0_f ) ) && all( shadowUv.xyz() < vec3( 1.0_f ) ) )
						{
							writer.returnStmt( shadowMapTexture->lod( shadowUv.xy(), shadowUv.z(), 0.0_f ) );
						}
						FI;

						writer.returnStmt( 1.0_f );
					}
					, sdw::InVec3{ writer, "P" } );
			}

			m_integrateScatteredLuminance = writer.implementFunction< SingleScatteringResult >( "integrateScatteredLuminance"
				, [&]( sdw::Vec2 const & pixPos
					, sdw::Vec3 const & worldPos
					, sdw::Vec3 const & worldDir
					, sdw::Vec3 const & sunDir
					, sdw::Float const & sampleCountIni
					, sdw::Float const & depthBufferValue
					, sdw::Float const & tMaxMax )
				{
					auto planetRadiusOffset = 0.01_f;
					auto transmittanceLutExtent = vec2( sdw::Float{ float( transmittanceExtent.width ) }, float( transmittanceExtent.height ) );

					auto result = writer.declLocale< SingleScatteringResult >( "result" );
					result.luminance = vec3( 0.0_f );
					result.opticalDepth = vec3( 0.0_f );
					result.transmittance = vec3( 0.0_f );
					result.multiScatAs1 = vec3( 0.0_f );
					result.newMultiScatStep0Out = vec3( 0.0_f );
					result.newMultiScatStep1Out = vec3( 0.0_f );

					// Compute next intersection with atmosphere or ground 
					auto earthO = writer.declLocale( "earthO"
						, vec3( 0.0_f, 0.0f, 0.0f ) );
					auto tBottom = writer.declLocale( "tBottom"
						, raySphereIntersectNearest( worldPos, worldDir, earthO, atmosphereData.bottomRadius ) );
					auto tTop = writer.declLocale( "tTop"
						, raySphereIntersectNearest( worldPos, worldDir, earthO, atmosphereData.topRadius ) );
					auto tMax = writer.declLocale( "tMax"
						, 0.0_f );

					IF( writer, tBottom < 0.0f )
					{
						IF( writer, tTop < 0.0f )
						{
							tMax = 0.0f; // No intersection with earth nor atmosphere: stop right away  
							writer.returnStmt( result );
						}
						ELSE
						{
							tMax = tTop;
						}
						FI;
					}
					ELSE
					{
						IF( writer, tTop > 0.0f )
						{
							tMax = min( tTop, tBottom );
						}
						FI;
					}
					FI;

					if ( luminanceSettings.cameraData )
					{
						IF( writer, depthBufferValue >= 0.0f )
						{
							auto clipSpace = writer.declLocale( "clipSpace"
								, vec3( ( pixPos / transmittanceLutExtent ) * vec2( 2.0_f, -2.0_f ) - vec2( 1.0_f, -1.0_f )
									, depthBufferValue ) );

							IF( writer, clipSpace.z() < 1.0f )
							{
								auto depthBufferWorldPos = writer.declLocale( "depthBufferWorldPos"
									, luminanceSettings.cameraData->invViewProj * vec4( clipSpace, 1.0f ) );
								depthBufferWorldPos /= depthBufferWorldPos.w();

								auto tDepth = writer.declLocale( "tDepth"
									, length( depthBufferWorldPos.xyz() - ( worldPos + vec3( 0.0_f, 0.0_f, -atmosphereData.bottomRadius ) ) ) ); // apply earth offset to go back to origin as top of earth mode. 

								IF( writer, tDepth < tMax )
								{
									tMax = tDepth;
								}
								FI;
							}
							FI;
							//if (VariableSampleCount && clipSpace.z() == 1.0f)
							//	return result;
						}
						FI;
					}

					tMax = min( tMax, tMaxMax );

					// Sample count 
					auto sampleCount = writer.declLocale( "sampleCount"
						, sampleCountIni );
					auto sampleCountFloor = writer.declLocale( "sampleCountFloor"
						, sampleCountIni );
					auto tMaxFloor = writer.declLocale( "tMaxFloor"
						, tMax );

					if ( luminanceSettings.variableSampleCount )
					{
						sampleCount = mix( atmosphereData.rayMarchMinMaxSPP.x()
							, atmosphereData.rayMarchMinMaxSPP.y()
							, clamp( tMax * 0.01_f, 0.0_f, 1.0_f ) );
						sampleCountFloor = floor( sampleCount );
						tMaxFloor = tMax * sampleCountFloor / sampleCount;	// rescale tMax to map to the last entire step segment.
					}

					auto dt = writer.declLocale( "dt"
						, tMax / sampleCount );

					// Phase functions
					auto uniformPhase = writer.declLocale( "uniformPhase"
						, 1.0_f / ( 4.0_f * sdw::Float{ castor::Pi< float > } ) );
					auto wi = writer.declLocale( "wi"
						, sunDir );
					auto wo = writer.declLocale( "wo"
						, worldDir );
					auto cosTheta = writer.declLocale( "cosTheta"
						, dot( wi, wo ) );
					auto miePhaseValue = writer.declLocale( "miePhaseValue"
						, hgPhase( atmosphereData.miePhaseFunctionG, -cosTheta ) );	// negate cosTheta because due to worldDir being a "in" direction. 
					auto rayleighPhaseValue = writer.declLocale( "rayleighPhaseValue"
						, rayleighPhase( cosTheta ) );

					if ( luminanceSettings.illuminanceIsOne )
					{
						// When building the scattering factor, we assume light illuminance is 1 to compute a transfert function relative to identity illuminance of 1.
						// This make the scattering factor independent of the light. It is now only linked to the atmosphere properties.
						auto globalL = writer.declLocale( "globalL"
							, vec3( 1.0_f ) );
					}
					else
					{
						auto globalL = writer.declLocale( "globalL"
							, atmosphereData.sunIlluminance );
					}

					auto globalL = writer.getVariable< sdw::Vec3 >( "globalL" );
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
					auto sampleSegmentT = writer.declLocale( "sampleSegmentT"
						, 0.3_f );

					FOR( writer, sdw::Float, s, 0.0_f, s < sampleCount, s += 1.0_f )
					{
						if ( luminanceSettings.variableSampleCount )
						{
							// More expenssive but artefact free
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

						auto P = writer.declLocale( "P"
							, worldPos + t * worldDir );

						auto medium = writer.declLocale( "medium"
							, sampleMediumRGB( P ) );
						auto sampleOpticalDepth = writer.declLocale( "sampleOpticalDepth"
							, medium.extinction * dt );
						auto sampleTransmittance = writer.declLocale( "sampleTransmittance"
							, exp( -sampleOpticalDepth ) );
						opticalDepth += sampleOpticalDepth;

						auto pHeight = writer.declLocale( "pHeight"
							, length( P ) );
						auto upVector = writer.declLocale( "upVector"
							, P / pHeight );
						auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
							, dot( sunDir, upVector ) );
						auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
						lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle, uv );
						auto transmittanceToSun = writer.declLocale( "transmittanceToSun"
							, vec3( 0.0_f ) );

						if ( transmittanceTexture )
						{
							transmittanceToSun = transmittanceTexture->lod( uv, 0.0_f ).rgb();
						}

						auto phaseTimesScattering = writer.declLocale< sdw::Vec3 >( "phaseTimesScattering" );

						if ( luminanceSettings.mieRayPhase )
						{
							phaseTimesScattering = medium.scatteringMie * miePhaseValue + medium.scatteringRay * rayleighPhaseValue;
						}
						else
						{
							phaseTimesScattering = medium.scattering * uniformPhase;
						}

						// Earth shadow 
						auto tEarth = writer.declLocale( "tEarth"
							, raySphereIntersectNearest( P, sunDir, earthO + planetRadiusOffset * upVector, atmosphereData.bottomRadius ) );
						auto earthShadow = writer.declLocale( "earthShadow"
							, writer.ternary( tEarth >= 0.0_f, 0.0_f, 1.0_f ) );

						// Dual scattering for multi scattering 

						auto multiScatteredLuminance = writer.declLocale( "multiScatteredLuminance"
							, vec3( 0.0_f ) );

						if ( luminanceSettings.multiScatApproxEnabled )
						{
							multiScatteredLuminance = m_getMultipleScattering( medium.scattering, medium.extinction, P, sunZenithCosAngle );
						}

						auto shadow = writer.declLocale( "shadow"
							, 1.0_f );

						if ( luminanceSettings.shadowMapEnabled )
						{
							// First evaluate opaque shadow
							shadow = m_getShadow( P );
						}

						auto S = writer.declLocale( "S"
							, globalL * ( earthShadow * shadow * transmittanceToSun * phaseTimesScattering + multiScatteredLuminance * medium.scattering ) );

						// When using the power serie to accumulate all sattering order, serie r must be <1 for a serie to converge.
						// Under extreme coefficient, MultiScatAs1 can grow larger and thus result in broken visuals.
						// The way to fix that is to use a proper analytical integration as proposed in slide 28 of http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
						// However, it is possible to disable as it can also work using simple power serie sum unroll up to 5th order. The rest of the orders has a really low contribution.
						if ( luminanceSettings.multiScatteringPowerSerie == 0u )
						{
							// 1 is the integration of luminance over the 4pi of a sphere, and assuming an isotropic phase function of 1.0/(4*PI)
							result.multiScatAs1 += throughput * medium.scattering /** 1 */* dt;
						}
						else
						{
							auto MS = writer.declLocale( "MS"
								, medium.scattering );
							auto MSint = writer.declLocale( "MSint"
								, ( MS - MS * sampleTransmittance ) / medium.extinction );
							result.multiScatAs1 += throughput * MSint;
						}

						// Evaluate input to multi scattering 
						{
							auto newMS = writer.declLocale< sdw::Vec3 >( "newMS" );

							newMS = earthShadow * transmittanceToSun * medium.scattering * uniformPhase;
							result.newMultiScatStep0Out += throughput * ( newMS - newMS * sampleTransmittance ) / medium.extinction;
							//result.newMultiScatStep0Out += sampleTransmittance * throughput * newMS * dt;

							newMS = medium.scattering * uniformPhase * multiScatteredLuminance;
							result.newMultiScatStep1Out += throughput * ( newMS - newMS * sampleTransmittance ) / medium.extinction;
							//result.newMultiScatStep1Out += sampleTransmittance * throughput * newMS * dt;
						}

						// See slide 28 at http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/ 
						auto Sint = writer.declLocale( "Sint"
							, ( S - S * sampleTransmittance ) / medium.extinction );	// integrate along the current step segment 
						L += throughput * Sint;											// accumulate and also take into account the transmittance from previous steps
						throughput *= sampleTransmittance;

						tPrev = t;
					}
					ROF;

					if ( luminanceSettings.useGround )
					{
						IF( writer, tMax == tBottom && tBottom > 0.0_f )
						{
							// Account for bounced light off the earth
							auto P = writer.declLocale( "P"
								, worldPos + tBottom * worldDir );
							auto pHeight = writer.declLocale( "pHeight"
								, length( P ) );

							auto upVector = writer.declLocale( "upVector"
								, P / pHeight );
							auto sunZenithCosAngle = writer.declLocale( "sunZenithCosAngle"
								, dot( sunDir, upVector ) );
							auto uv = writer.declLocale< sdw::Vec2 >( "uv" );
							lutTransmittanceParamsToUv( pHeight, sunZenithCosAngle, uv );
							auto transmittanceToSun = writer.declLocale( "transmittanceToSun"
								, vec3( 0.0_f ) );

							if ( transmittanceTexture )
							{
								transmittanceToSun = transmittanceTexture->lod( uv, 0.0_f ).rgb();
							}

							auto NdotL = writer.declLocale( "NdotL"
								, clamp( dot( normalize( upVector ), normalize( sunDir ) ), 0.0_f, 1.0_f ) );
							L += globalL * transmittanceToSun * throughput * NdotL * atmosphereData.groundAlbedo / castor::Pi< float >;
						}
						FI;
					}

					result.luminance = L;
					result.opticalDepth = opticalDepth;
					result.transmittance = throughput;
					writer.returnStmt( result );
				}
				, sdw::InVec2{ writer, "pixPos" }
				, sdw::InVec3{ writer, "worldPos" }
				, sdw::InVec3{ writer, "worldDir" }
				, sdw::InVec3{ writer, "sunDir" }
				, sdw::InFloat{ writer, "sampleCountIni" }
				, sdw::InFloat{ writer, "depthBufferValue" }
				, sdw::InFloat{ writer, "tMaxMax" } );
		}

		return m_integrateScatteredLuminance( ppixPos
			, pworldPos
			, pworldDir
			, psunDir
			, psampleCountIni
			, pdepthBufferValue
			, ptMaxMax );
	}

	sdw::Boolean AtmosphereConfig::moveToTopAtmosphere( sdw::Vec3 & pworldPos
		, sdw::Vec3 const & pworldDir )
	{
		if ( !m_moveToTopAtmosphere )
		{
			m_moveToTopAtmosphere = writer.implementFunction< sdw::Boolean >( "moveToTopAtmosphere"
				, [&]( sdw::Vec3 worldPos
					, sdw::Vec3 const & worldDir )
				{
					auto planetRadiusOffset = 0.01_f;
					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) );

					IF( writer, viewHeight > atmosphereData.topRadius )
					{
						auto tTop = writer.declLocale( "tTop"
							, raySphereIntersectNearest( worldPos
								, worldDir
								, vec3( 0.0_f, 0.0_f, 0.0_f )
								, atmosphereData.topRadius ) );

						IF( writer, tTop >= 0.0_f )
						{
							auto upVector = writer.declLocale( "upVector"
								, worldPos / viewHeight );
							auto upOffset = writer.declLocale( "upOffset"
								, upVector * -planetRadiusOffset );
							worldPos = worldPos + worldDir * tTop + upOffset;
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
				, sdw::InOutVec3{ writer, "worldPos" }
				, sdw::InVec3{ writer, "worldDir" } );
		}

		return m_moveToTopAtmosphere( pworldPos, pworldDir );
	}

	sdw::Float AtmosphereConfig::raySphereIntersectNearest( sdw::Vec3 const & pr0
		, sdw::Vec3 const & prd
		, sdw::Vec3 const & ps0
		, sdw::Float const & psR )
	{
		if ( !m_raySphereIntersectNearest )
		{
			m_raySphereIntersectNearest = writer.implementFunction< sdw::Float >( "raySphereIntersectNearest"
				, [&]( sdw::Vec3 const & r0
					, sdw::Vec3 const & rd
					, sdw::Vec3 const & s0
					, sdw::Float const & sR )
				{
					auto a = writer.declLocale( "a"
						, dot( rd, rd ) );
					auto s0_r0 = writer.declLocale( "s0_r0"
						, r0 - s0 );
					auto b = writer.declLocale( "b"
						, 2.0_f * dot( rd, s0_r0 ) );
					auto c = writer.declLocale( "c"
						, dot( s0_r0, s0_r0 ) - ( sR * sR ) );
					auto delta = writer.declLocale( "delta"
						, b * b - 4.0_f * a * c );

					IF( writer, delta < 0.0_f || a == 0.0_f )
					{
						writer.returnStmt( -1.0_f );
					}
					FI;

					auto sol0 = writer.declLocale( "sol0"
						, ( -b - sqrt( delta ) ) / ( 2.0_f * a ) );
					auto sol1 = writer.declLocale( "sol1"
						, ( -b + sqrt( delta ) ) / ( 2.0_f * a ) );

					IF( writer, sol0 < 0.0_f && sol1 < 0.0_f )
					{
						writer.returnStmt( -1.0_f );
					}
					FI;

					IF( writer, sol0 < 0.0_f )
					{
						writer.returnStmt( max( 0.0_f, sol1 ) );
					}
					ELSEIF( sol1 < 0.0_f )
					{
						writer.returnStmt( max( 0.0_f, sol0 ) );
					}
					FI;

					writer.returnStmt( max( 0.0_f, min( sol0, sol1 ) ) );
				}
				, sdw::InVec3{ writer, "r0" }
				, sdw::InVec3{ writer, "rd" }
				, sdw::InVec3{ writer, "s0" }
				, sdw::InFloat{ writer, "sR" } );
		}

		return m_raySphereIntersectNearest( pr0, prd, ps0, psR );
	}

	sdw::Float AtmosphereConfig::hgPhase( sdw::Float const & pg
		, sdw::Float const & pcosTheta )
	{
		if ( !m_hgPhase )
		{
			m_hgPhase = writer.implementFunction< sdw::Float >( "hgPhase"
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

	MediumSampleRGB AtmosphereConfig::sampleMediumRGB( sdw::Vec3 const & pworldPos )
	{
		if ( !m_sampleMediumRGB )
		{
			m_sampleMediumRGB = writer.implementFunction< MediumSampleRGB >( "sampleMediumRGB"
				, [&]( sdw::Vec3 const & worldPos )
				{
					auto viewHeight = writer.declLocale( "viewHeight"
						, length( worldPos ) - atmosphereData.bottomRadius );

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

					s.scatteringMie = densityMie * atmosphereData.mieScattering;
					s.absorptionMie = densityMie * atmosphereData.mieAbsorption;
					s.extinctionMie = densityMie * atmosphereData.mieExtinction;

					s.scatteringRay = densityRay * atmosphereData.rayleighScattering;
					s.absorptionRay = vec3( 0.0_f );
					s.extinctionRay = s.scatteringRay + s.absorptionRay;

					s.scatteringOzo = vec3( 0.0_f );
					s.absorptionOzo = densityOzo * atmosphereData.absorptionExtinction;
					s.extinctionOzo = s.scatteringOzo + s.absorptionOzo;

					s.scattering = s.scatteringMie + s.scatteringRay + s.scatteringOzo;
					s.absorption = s.absorptionMie + s.absorptionRay + s.absorptionOzo;
					s.extinction = s.extinctionMie + s.extinctionRay + s.extinctionOzo;
					s.albedo = s.scattering / max( vec3( 0.001_f ), s.extinction );

					writer.returnStmt( s );
				}
				, sdw::InVec3{ writer, "worldPos" } );
		}

		return m_sampleMediumRGB( pworldPos );
	}

	sdw::Float AtmosphereConfig::rayleighPhase( sdw::Float const & cosTheta )
	{
		return 3.0_f / ( 16.0_f * castor::Pi< float > ) * ( 1.0_f + cosTheta * cosTheta );
	}

	sdw::Float AtmosphereConfig::fromUnitToSubUvs( sdw::Float u, sdw::Float resolution )
	{
		return ( u + 0.5f / resolution ) * ( resolution / ( resolution + 1.0f ) );
	}

	sdw::Float AtmosphereConfig::fromSubUvsToUnit( sdw::Float u, sdw::Float resolution )
	{
		return ( u - 0.5f / resolution ) * ( resolution / ( resolution - 1.0f ) );
	}

	sdw::Void AtmosphereConfig::uvToLutTransmittanceParams( sdw::Float & pviewHeight
		, sdw::Float & pviewZenithCosAngle
		, sdw::Vec2 const & puv )
	{
		if ( !m_uvToLutTransmittanceParams )
		{
			m_uvToLutTransmittanceParams = writer.implementFunction< sdw::Void >( "uvToLutTransmittanceParams"
				, [&]( sdw::Float viewHeight
					, sdw::Float viewZenithCosAngle
					, sdw::Vec2 const & uv )
				{
					auto x_mu = writer.declLocale( "x_mu"
						, uv.x() );
					auto x_r = writer.declLocale( "x_r"
						, uv.y() );

					auto H = writer.declLocale( "H"
						, sqrt( atmosphereData.topRadius * atmosphereData.topRadius - atmosphereData.bottomRadius * atmosphereData.bottomRadius ) );
					auto rho = writer.declLocale( "rho"
						, H * x_r );
					viewHeight = sqrt( rho * rho + atmosphereData.bottomRadius * atmosphereData.bottomRadius );

					auto d_min = writer.declLocale( "d_min"
						, atmosphereData.topRadius - viewHeight );
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

	sdw::Void AtmosphereConfig::lutTransmittanceParamsToUv( sdw::Float const & pviewHeight
		, sdw::Float const & pviewZenithCosAngle
		, sdw::Vec2 & puv )
	{
		if ( !m_lutTransmittanceParamsToUv )
		{
			m_lutTransmittanceParamsToUv = writer.implementFunction< sdw::Void >( "lutTransmittanceParamsToUv"
				, [&]( sdw::Float const & viewHeight
					, sdw::Float const & viewZenithCosAngle
					, sdw::Vec2 uv )
				{
					auto H = writer.declLocale( "H"
						, sqrt( max( 0.0_f, atmosphereData.topRadius * atmosphereData.topRadius - atmosphereData.bottomRadius * atmosphereData.bottomRadius ) ) );
					auto rho = writer.declLocale( "rho"
						, sqrt( max( 0.0_f, viewHeight * viewHeight - atmosphereData.bottomRadius * atmosphereData.bottomRadius ) ) );

					auto discriminant = writer.declLocale( "discriminant"
						, viewHeight * viewHeight * ( viewZenithCosAngle * viewZenithCosAngle - 1.0_f ) + atmosphereData.topRadius * atmosphereData.topRadius );
					auto d = writer.declLocale( "d"
						, max( 0.0_f, ( -viewHeight * viewZenithCosAngle + sqrt( discriminant ) ) ) ); // Distance to atmosphere boundary

					auto d_min = writer.declLocale( "d_min"
						, atmosphereData.topRadius - viewHeight );
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

	sdw::Void AtmosphereConfig::uvToSkyViewLutParams( sdw::Float & pviewZenithCosAngle
		, sdw::Float & plightViewCosAngle
		, sdw::Float const & pviewHeight
		, sdw::Vec2 const & puv
		, sdw::Vec2 const & psize )
	{
		if ( !m_uvToSkyViewLutParams )
		{
			m_uvToSkyViewLutParams = writer.implementFunction< sdw::Void >( "uvToSkyViewLutParams"
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
						, sqrt( viewHeight * viewHeight - atmosphereData.bottomRadius * atmosphereData.bottomRadius ) );
					auto cosBeta = writer.declLocale( "cosBeta"
						, vHorizon / viewHeight );				// GroundToHorizonCos
					auto beta = writer.declLocale( "beta"
						, acos( cosBeta ) );
					auto zenithHorizonAngle = writer.declLocale( "zenithHorizonAngle"
						, sdw::Float{ castor::Pi< float > } - beta );

					IF( writer, uv.y() < 0.5_f )
					{
						auto coord = writer.declLocale( "coord"
							, 2.0_f * uv.y() );
						coord = 1.0_f - coord;
#if NONLINEARSKYVIEWLUT
						coord *= coord;
#endif
						coord = 1.0_f - coord;
						viewZenithCosAngle = cos( zenithHorizonAngle * coord );
					}
					ELSE
					{
						auto coord = writer.declLocale( "coord"
							, uv.y() * 2.0_f - 1.0_f );
#if NONLINEARSKYVIEWLUT
						coord *= coord;
#endif
						viewZenithCosAngle = cos( zenithHorizonAngle + beta * coord );
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

	sdw::Void AtmosphereConfig::skyViewLutParamsToUv( sdw::Boolean const & pintersectGround
		, sdw::Float const & pviewZenithCosAngle
		, sdw::Float const & plightViewCosAngle
		, sdw::Float const & pviewHeight
		, sdw::Vec2 & puv
		, sdw::Vec2 const & psize )
	{
		if ( !m_skyViewLutParamsToUv )
		{
			m_skyViewLutParamsToUv = writer.implementFunction< sdw::Void >( "skyViewLutParamsToUv"
				, [&]( sdw::Boolean const & intersectGround
					,  sdw::Float const & viewZenithCosAngle
					, sdw::Float const & lightViewCosAngle
					, sdw::Float const & viewHeight
					, sdw::Vec2 uv
					, sdw::Vec2 const & size )
				{
					auto Vhorizon = writer.declLocale( "Vhorizon"
						, sqrt( viewHeight * viewHeight - atmosphereData.bottomRadius * atmosphereData.bottomRadius ) );
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
#if NONLINEARSKYVIEWLUT
						coord = sqrt( coord );
#endif
						coord = 1.0_f - coord;
						uv.y() = coord * 0.5_f;
					}
					ELSE
					{
						auto coord = writer.declLocale( "coord"
						, ( acos( viewZenithCosAngle ) - zenithHorizonAngle ) / beta );
#if NONLINEARSKYVIEWLUT
						coord = sqrt( coord );
#endif
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

	//************************************************************************************************
}
