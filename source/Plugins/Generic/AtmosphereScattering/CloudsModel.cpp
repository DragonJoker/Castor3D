#include "AtmosphereScattering/CloudsModel.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsUbo.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

#define NONLINEARSKYVIEWLUT 1

namespace atmosphere_scattering
{
	//************************************************************************************************

	CloudsModel::CloudsModel( sdw::ShaderWriter & pwriter
		, castor3d::shader::Utils & putils
		, AtmosphereModel & patmosphere
		, ScatteringModel & pscattering
		, CloudsData const & pcloudsData
		, uint32_t binding )
		: writer{ pwriter }
		, utils{ putils }
		, atmosphere{ patmosphere }
		, scattering{ pscattering }
		, cloudsData{ pcloudsData }
		, perlinWorleyNoiseMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "perlinWorleyNoiseMap"
			, binding++
			, 0u ) }
		, worleyNoiseMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "worleyNoiseMap"
			, binding++
			, 0u ) }
		, curlNoiseMap{ writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "curlNoiseMap"
			, binding++
			, 0u ) }
		, weatherMap{ writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "weatherMap"
			, binding++
			, 0u ) }
		, sphereInnerRadius{ cloudsData.sphereInnerRadius() + atmosphere.getEarthRadius() }
		, sphereOuterRadius{ cloudsData.sphereOuterRadius() + atmosphere.getEarthRadius() }
		, sphereDelta{ cloudsData.sphereOuterRadius() - cloudsData.sphereInnerRadius() }
	{
	}

	sdw::Void CloudsModel::applyClouds( sdw::IVec2 const & pfragCoord
		, sdw::Vec2 const & ptargetSize
		, sdw::Vec4 & pbg
		, sdw::Vec4 & pemission )
	{
		if ( !m_applyClouds )
		{
			m_applyClouds = writer.implementFunction< sdw::Void >( "clouds_apply"
				, [&]( sdw::IVec2 const & fragCoord
					, sdw::Vec2 const & targetSize
					, sdw::Vec4 bg
					, sdw::Vec4 emission )
				{
					auto ray = writer.declLocale( "ray"
						, atmosphere.castRay( vec2( fragCoord ), targetSize ) );
					auto startPos = writer.declLocale( "startPos"
						, vec3( 0.0_f ) );
					auto endPos = writer.declLocale( "endPos"
						, vec3( 0.0_f ) );

					//compute raymarching starting and ending point
					auto fogRay = writer.declLocale( "fogRay"
						, vec3( 0.0_f ) );

					IF( writer, ray.origin.y() < sphereInnerRadius )
					{
						// Ray below clouds boundaries.
						startPos = atmosphere.raySphereIntersectNearest( ray, sphereInnerRadius ).point();
						endPos = atmosphere.raySphereIntersectNearest( ray, sphereOuterRadius ).point();
						fogRay = startPos;
					}
					ELSEIF( ray.origin.y() > sphereInnerRadius
						&& ray.origin.y() < sphereOuterRadius )
					{
						// Ray inside clouds boundaries.
						startPos = ray.origin;
						endPos = atmosphere.raySphereIntersectNearest( ray, sphereOuterRadius ).point();
						auto ifog = writer.declLocale( "ifog"
							, atmosphere.raySphereIntersectNearest( ray, sphereInnerRadius ) );

						IF( writer, ifog.valid() )
						{
							fogRay = ifog.point();
						}
						ELSE
						{
							fogRay = startPos;
						}
						FI;
					}
					ELSE
					{
						// Ray over clouds.
						startPos = atmosphere.raySphereIntersectNearest( ray, sphereOuterRadius ).point();
						endPos = atmosphere.raySphereIntersectNearest( ray, sphereInnerRadius ).point();
						fogRay = atmosphere.raySphereIntersectNearest( ray, sphereOuterRadius ).point();
					}
					FI;

					auto sunColor = writer.declLocale( "sunColor"
						, 3.0_f * scattering.getSunRadiance( atmosphere.getSunDirection() ) );

					//compute fog amount and early exit if over a certain value
					auto fogAmount = writer.declLocale( "fogAmount"
						, computeFogAmount( fogRay, ray.origin, 0.006_f ) );

					IF( writer, fogAmount > 0.965_f )
					{
						emission = bg;
						writer.returnStmt(); //early exit
					}
					FI;

					auto cloudStartPos = writer.declLocale( "cloudStartPos"
						, vec3( 0.0_f ) );
					auto cloudEndPos = writer.declLocale( "cloudEndPos"
						, vec3( 0.0_f ) );
					auto accumDensity = writer.declLocale( "accumDensity"
						, 0.0_f );
					auto rayMarchResult = writer.declLocale( "rayMarchResult"
						, raymarchToCloud( ray
							, startPos
							, endPos
							, bg.rgb()
							, fragCoord
							, sunColor
							, accumDensity ) );

					auto cloudAlphaness = writer.declLocale( "cloudAlphaness"
						, utils.threshold( accumDensity, 0.2_f ) );
					rayMarchResult = rayMarchResult * 1.8_f - 0.1_f; // contrast-illumination tuning

					// use current position distance to center as action radius
					rayMarchResult = mix( rayMarchResult
						, bg.rgb() * accumDensity
						, vec3( clamp( fogAmount, 0.0_f, 1.0_f ) ) );

					// add sun glare to clouds
					auto sunIntensity = writer.declLocale( "sunIntensity"
						, clamp( dot( atmosphere.getSunDirection(), normalize( endPos - startPos ) )
							, 0.0_f
							, 1.0_f ) );
					auto sunGlare = writer.declLocale( "sunGlare"
						, 0.8_f * sunColor * pow( sunIntensity, 256.0_f ) );
					rayMarchResult += sunGlare * accumDensity;

					// Blend and fade out clouds into the horizon
					auto cubeMapEndPos = writer.declLocale( "cubeMapEndPos"
						, raySphereintersectSkyMap( ray.direction, 0.5_f ).point() );
					rayMarchResult = mix( vec3( 1.0_f )
						, rayMarchResult
						, vec3( pow( max( cubeMapEndPos.y() + 0.1_f, 0.0_f ), 0.2_f ) ) );
					auto cloudFadeOutPoint = 0.06_f;
					accumDensity *= smoothStep( 0.0_f
						, 1.0_f
						, min( 1.0_f, utils.remap( ray.direction.y(), cloudFadeOutPoint, 0.2_f, 0.0_f, 1.0_f ) ) );
					bg = vec4( mix( bg.rgb(), rayMarchResult, vec3( accumDensity ) ), cloudAlphaness );

					auto bloom = writer.declLocale( "bloom"
						, vec4( sunColor * 1.3_f, 1.0_f ) );

					IF( writer, cloudAlphaness > 0.1_f )
					{
						//apply fog to bloom buffer
						fogAmount = computeFogAmount( startPos, ray.origin, 0.003_f );
						auto cloud = writer.declLocale( "cloud"
							, mix( vec3( 0.0_f ), bloom.rgb(), vec3( clamp( fogAmount, 0.0_f, 1.0_f ) ) ) );
						bloom.rgb() = bloom.rgb() * ( 1.0_f - cloudAlphaness ) + cloud.rgb();
					}
					FI;

					emission = bloom;
				}
				, sdw::InIVec2{ writer, "fragCoord" }
				, sdw::InVec2{ writer, "targetSize" }
				, sdw::InOutVec4{ writer, "bg" }
				, sdw::OutVec4{ writer, "emission" } );
		}

		return m_applyClouds( pfragCoord
			, ptargetSize
			, pbg
			, pemission );
	}

	sdw::Vec2 CloudsModel::getUVProjection( sdw::Vec3 const & p )
	{
		return p.xz() / sphereInnerRadius + 0.5_f;
	}

	sdw::Float CloudsModel::getHeightFraction( sdw::Vec3 const & inPos )
	{
		return ( length( inPos ) - sphereInnerRadius ) / sphereDelta;
	}

	sdw::RetFloat CloudsModel::getRelativeHeightInAtmosphere( sdw::Vec3 const & ppos
		, sdw::Vec3 const & pstartPos
		, Ray const & pray )
	{
		if ( !m_getRelativeHeightInAtmosphere )
		{
			m_getRelativeHeightInAtmosphere = writer.implementFunction< sdw::Float >( "getRelativeHeightInAtmosphere"
				, [&]( sdw::Vec3 const & point
					, sdw::Vec3 const & startPosOnInnerShell
					, Ray const & ray )
				{
					auto lengthOfRayfromCamera = writer.declLocale( "lengthOfRayfromCamera"
						, length( point - ray.origin ) );
					auto lengthOfRayToInnerShell = writer.declLocale( "lengthOfRayToInnerShell"
						, length( startPosOnInnerShell - ray.origin ) );
					auto pointToEarthDir = writer.declLocale( "pointToEarthDir"
						, normalize( point ) );
					// assuming RayDir is normalised
					auto cosTheta = writer.declLocale( "cosTheta"
						, dot( ray.direction, pointToEarthDir ) );

					// CosTheta is an approximation whose error gets relatively big near the horizon and could lead to problems.
					// However, the actual calculationis involve a lot of trig and thats expensive;
					// No longer drawing clouds that close to the horizon and so the cosTheta Approximation is fine

					auto numerator = writer.declLocale( "numerator"
						, abs( cosTheta * ( lengthOfRayfromCamera - lengthOfRayToInnerShell ) ) );
					writer.returnStmt( numerator / sphereDelta );
					// return clamp( length(point.y - projectedPos.y) / sphereDelta, 0.0, 1.0);
				}
				, sdw::InVec3{ writer, "point" }
				, sdw::InVec3{ writer, "startPosOnInnerShell" }
				, InRay{ writer, "ray" } );
		}

		return m_getRelativeHeightInAtmosphere( ppos
			, pstartPos
			, pray );
	}

	sdw::RetFloat CloudsModel::getDensityHeightGradientForPoint( sdw::Float const & pheightFraction
		, sdw::Float const & pcloudType )
	{
		if ( !m_getDensityHeightGradientForPoint )
		{
			m_getDensityHeightGradientForPoint = writer.implementFunction< sdw::Float >( "clouds_getDensityHeightGradientForPoint"
				, [&]( sdw::Float const & heightFraction
					, sdw::Float const & cloudType )
				{
					auto stratusGradient = vec4( 0.0_f, 0.1_f, 0.2_f, 0.3_f );
					auto stratocumulusGradient = vec4( 0.02_f, 0.2_f, 0.48_f, 0.625_f );
					auto cumulusGradient = vec4( 0.0_f, 0.1625_f, 0.88_f, 0.98_f );

					auto stratusFactor = writer.declLocale( "stratusFactor"
						, 1.0_f - clamp( cloudType * 2.0_f, 0.0_f, 1.0_f ) );
					auto stratoCumulusFactor = writer.declLocale( "stratoCumulusFactor"
						, 1.0_f - abs( cloudType - 0.5_f ) * 2.0_f );
					auto cumulusFactor = writer.declLocale( "cumulusFactor"
						, clamp( cloudType - 0.5_f, 0.0_f, 1.0_f ) * 2.0_f );

					auto baseGradient = writer.declLocale( "baseGradient"
						, stratusFactor * stratusGradient + stratoCumulusFactor * stratocumulusGradient + cumulusFactor * cumulusGradient );

					// gradient computation (see Siggraph 2017 Nubis-Decima talk)
					//writer.returnStmt( utils.remap( heightFraction, baseGradient.x(), baseGradient.y(), 0.0_f, 1.0_f )
					//	* utils.remap( heightFraction, baseGradient.z(), baseGradient.w(), 1.0_f, 0.0_f ) );
					writer.returnStmt( smoothStep( baseGradient.x(), baseGradient.y(), heightFraction )
						- smoothStep( baseGradient.z(), baseGradient.w(), heightFraction ) );
				}
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "cloudType" } );
		}

		return m_getDensityHeightGradientForPoint( pheightFraction
			, pcloudType );
	}

	sdw::RetVec3 CloudsModel::skewSamplePointWithWind( sdw::Vec3 const & ppoint
		, sdw::Float const & pheightFraction )
	{
		if ( !m_skewSamplePointWithWind )
		{
			m_skewSamplePointWithWind = writer.implementFunction< sdw::Vec3 >( "clouds_skewSamplePointWithWind"
				, [&]( sdw::Vec3 point
					, sdw::Float const & heightFraction )
				{
					//skew in wind direction
					point += heightFraction * cloudsData.windDirection() * cloudsData.cloudTopOffset();

					//Animate clouds in wind direction and add a small upward bias to the wind direction
					point += cloudsData.windDirection() * cloudsData.time() * cloudsData.cloudSpeed();
					writer.returnStmt( point );
				}
				, sdw::InVec3{ writer, "point" }
				, sdw::InFloat{ writer, "heightFraction" } );
		}

		return m_skewSamplePointWithWind( ppoint
			, pheightFraction );
	}

	sdw::RetFloat CloudsModel::sampleLowFrequency( sdw::Vec3 const & pskewedSamplePoint
		, sdw::Vec3 const & punskewedSamplePoint
		, sdw::Float const & pheightFraction
		, sdw::Float const & plod )
	{
		if ( !m_sampleLowFrequency )
		{
			m_sampleLowFrequency = writer.implementFunction< sdw::Float >( "clouds_sampleLowFrequency"
				, [&]( sdw::Vec3 const & skewedSamplePoint
					, sdw::Vec3 const & unskewedSamplePoint
					, sdw::Float const & heightFraction
					, sdw::Float const & lod )
				{
					auto uv = writer.declLocale( "uv"
						, getUVProjection( unskewedSamplePoint ) );
					//Read in the low-frequency Perlin-Worley noises and Worley noises
					auto lowFrequencyNoise = writer.declLocale( "lowFrequencyNoise"
						, perlinWorleyNoiseMap.lod( vec3( uv * cloudsData.crispiness(), heightFraction ), lod ) );

					//Build an FBM out of the low-frequency Worley Noises that are used to add detail to the Low-frequency Perlin Worley noise
					auto lowFrequencyFBM = writer.declLocale( "lowFrequencyFBM"
						, dot( lowFrequencyNoise.gba(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );

					// Define the base cloud shape by dilating it with the low-frequency FBM
					auto baseCloud = writer.declLocale( "baseCloud"
						, utils.remap( lowFrequencyNoise.r(), ( lowFrequencyFBM - 1.0_f ), 1.0_f, 0.0_f, 1.0_f ) );

					// Use weather map for cloud types and blend between them and their densities
					uv = getUVProjection( skewedSamplePoint );
					auto weather = writer.declLocale( "weatherData"
						, weatherMap.lod( uv, 0.0_f ) );
					auto densityHeightGradient = writer.declLocale( "densityHeightGradient"
						, getDensityHeightGradientForPoint( heightFraction, weather.g() ) );
					// Apply Height function to the base cloud shape
					baseCloud *= ( densityHeightGradient / heightFraction );

					// Cloud coverage is stored in weather data’s red channel.
					auto cloudCoverage = writer.declLocale( "cloudCoverage"
						, weather.r() * cloudsData.coverage() );
					// Use remap to apply the cloud coverage attribute.
					auto baseCloudWithCoverage = writer.declLocale( "baseCloudWithCoverage"
						, utils.remap( baseCloud, cloudCoverage, 1.0_f, 0.0_f, 1.0_f ) );
					// To ensure that the density increases with coverage in an aesthetically pleasing manner
					// Multiply the result by the cloud coverage attribute so that smaller clouds are lighter 
					// and more aesthetically pleasing
					baseCloudWithCoverage *= cloudCoverage;

					writer.returnStmt( baseCloudWithCoverage );
				}
				, sdw::InVec3{ writer, "skewedSamplePoint" }
				, sdw::InVec3{ writer, "unskewedSamplePoint" }
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "lod" } );
		}

		return m_sampleLowFrequency( pskewedSamplePoint
			, punskewedSamplePoint
			, pheightFraction
			, plod );
	}

	sdw::RetFloat CloudsModel::erodeWithHighFrequency( sdw::Float const & pbaseDensity
		, sdw::Vec3 const & pskewedSamplePoint
		, sdw::Float const & pheightFraction
		, sdw::Float const & plod )
	{
		if ( !m_erodeWithHighFrequency )
		{
			m_erodeWithHighFrequency = writer.implementFunction< sdw::Float >( "clouds_erodeWithHighFrequency"
				, [&]( sdw::Float const & baseDensity
					, sdw::Vec3 skewedSamplePoint
					, sdw::Float const & heightFraction
					, sdw::Float const & lod )
				{
					// Add turbulence to the bottom of the clouds
					auto curlNoise = writer.declLocale( "curlNoise"
						, curlNoiseMap.sample( skewedSamplePoint.xy(), 0.0_f ) );
					skewedSamplePoint.xy() += curlNoise * ( 1.0_f - heightFraction );
					auto uv = writer.declLocale( "uv"
						, getUVProjection( skewedSamplePoint ) );

					// Sample High Frequency Noises
					auto highFrequencyNoise = writer.declLocale( "highFrequencyNoise"
						, worleyNoiseMap.lod( vec3( uv * cloudsData.crispiness(), heightFraction ) * cloudsData.curliness(), lod ).rgb() );

					// Build High Frequency FBM
					auto highFrequencyFBM = writer.declLocale( "highFrequencyFBM"
						, dot( highFrequencyNoise.rgb(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );

					//Erode the base shape of the cloud with the distorted high frequency worley noises
					auto highFreqNoiseModifier = writer.declLocale( "highFreqNoiseModifier"
						, clamp( mix( highFrequencyFBM, 1.0_f - highFrequencyFBM, clamp( heightFraction * 10.0_f, 0.0_f, 1.0_f ) ), 0.0_f, 1.0_f ) );

					auto finalCloud = writer.declLocale( "finalCloud"
						, baseDensity - highFreqNoiseModifier * ( 1.0_f - baseDensity ) );
					writer.returnStmt( utils.remap( finalCloud * 2.0_f, highFreqNoiseModifier * 0.2_f, 1.0_f, 0.0_f, 1.0_f ) );
				}
				, sdw::InFloat{ writer, "baseCloud" }
				, sdw::InVec3{ writer, "skewedSamplePoint" }
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "lod" } );
		}

		return m_erodeWithHighFrequency( pbaseDensity
			, pskewedSamplePoint
			, pheightFraction
			, plod );
	}

	sdw::RetFloat CloudsModel::sampleCloudDensity( sdw::Vec3 const & psamplePoint
		, sdw::Boolean const & pexpensive
		, sdw::Float const & pheightFraction
		, sdw::Float const & plod )
	{
		if ( !m_sampleCloudDensity )
		{
			m_sampleCloudDensity = writer.implementFunction< sdw::Float >( "clouds_sampleDensity"
				, [&]( sdw::Vec3 const & samplePoint
					, sdw::Boolean const & expensive
					, sdw::Float const & heightFraction
					, sdw::Float const & lod )
				{
					auto skewedSamplePoint = writer.declLocale( "skewedSamplePoint"
						, skewSamplePointWithWind( samplePoint, heightFraction ) );
					auto baseDensity = writer.declLocale( "baseDensity"
						, sampleLowFrequency( skewedSamplePoint
							, samplePoint
							, heightFraction
							, lod ) );

					IF( writer, baseDensity > 0.0_f && expensive )
					{
						baseDensity = erodeWithHighFrequency( baseDensity
							, skewedSamplePoint
							, heightFraction
							, lod );
					}
					FI;

					writer.returnStmt( clamp( baseDensity, 0.0_f, 1.0_f ) );
				}
				, sdw::InVec3{ writer, "samplePoint" }
				, sdw::InBoolean{ writer, "expensive" }
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "lod" } );
		}

		return m_sampleCloudDensity( psamplePoint
			, pexpensive
			, pheightFraction
			, plod );
	}

	sdw::RetFloat CloudsModel::raymarchToLight( Ray const & pray
		, sdw::Vec3 const & po
		, sdw::Float const & pstepSize
		, sdw::Vec3 const & plightDir
		, sdw::Float const & poriginalDensity
		, sdw::Float const & plightDotEye
		, sdw::Float & pdensityAlongLight )
	{
		if ( !m_raymarchToLight )
		{
			m_raymarchToLight = writer.implementFunction< sdw::Float >( "clouds_raymarchToLight"
				, [&]( Ray const & ray
					, sdw::Vec3 const & o
					, sdw::Float const & stepSize
					, sdw::Vec3 const & lightDir
					, sdw::Float const & originalDensity
					, sdw::Float const & lightDotEye
					, sdw::Float densityAlongLight )
				{
					auto coneStep = 1.0_f / 6.0_f;
					auto noiseKernel = writer.declConstantArray( "noiseKernel"
						, std::vector< sdw::Vec3 >{ vec3( 0.38051305_f, 0.92453449_f, -0.02111345_f )
						, vec3( -0.50625799_f, -0.03590792_f, -0.86163418_f )
						, vec3( -0.32509218_f, -0.94557439_f, 0.01428793_f )
						, vec3( 0.09026238_f, -0.27376545_f, 0.95755165_f )
						, vec3( 0.28128598_f, 0.42443639_f, -0.86065785_f )
						, vec3( -0.16852403_f, 0.14748697_f, 0.97460106_f ) } );

					auto startPos = writer.declLocale( "startPos"
						, o );
					auto ds = writer.declLocale( "ds"
						, stepSize * 6.0_f );
					auto rayStep = writer.declLocale( "rayStep"
						, lightDir * ds );
					auto coneRadius = writer.declLocale( "coneRadius"
						, 1.0_f );
					auto coneDensity = writer.declLocale( "coneDensity"
						, 0.0_f );
					auto invDepth = writer.declLocale( "invDepth"
						, 1.0_f / ds );
					auto sigmaDs = writer.declLocale( "sigmaDs"
						, -ds * cloudsData.absorption() );
					auto transmittance = writer.declLocale( "transmittance"
						, 1.0_f );
					densityAlongLight = 0.0_f;

					FOR( writer, sdw::Int, i, 0_i, i < 6_i, ++i )
					{
						auto pos = writer.declLocale( "pos"
							, startPos + noiseKernel[i] * coneRadius * writer.cast< sdw::Float >( i ) );
						auto heightFraction = writer.declLocale( "heightFraction"
							, getHeightFraction( pos ) );

						IF( writer, heightFraction >= 0.0_f )
						{
							auto cloudDensity = writer.declLocale( "cloudDensity"
								, sampleCloudDensity( pos
									, densityAlongLight > 0.3_f
									, heightFraction
									, writer.cast< sdw::Float >( i ) / 16.0_f ) );

							IF( writer, cloudDensity > 0.0_f )
							{
								transmittance *= exp( cloudDensity * sigmaDs );
								densityAlongLight += cloudDensity;
							}
							FI;
						}
						FI;

						startPos += rayStep;
						coneRadius += coneStep;
					}
					ROF;

					writer.returnStmt( transmittance );
				}
				, InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "o" }
				, sdw::InFloat{ writer, "stepSize" }
				, sdw::InVec3{ writer, "lightDir" }
				, sdw::InFloat{ writer, "originalDensity" }
				, sdw::InFloat{ writer, "lightDotEye" }
				, sdw::OutFloat{ writer, "densityAlongLight" } );
		}

		return m_raymarchToLight( pray
			, po
			, pstepSize
			, plightDir
			, poriginalDensity
			, plightDotEye
			, pdensityAlongLight );
	}

	sdw::RetVec3 CloudsModel::raymarchToCloud( Ray const & pray
		, sdw::Vec3 const & pstartPos
		, sdw::Vec3 const & pendPos
		, sdw::Vec3 const & pbg
		, sdw::IVec2 const & pfragCoord
		, sdw::Vec3 const & psunColor
		, sdw::Float & paccumDensity )
	{
		if ( !m_raymarchToCloud )
		{
			m_raymarchToCloud = writer.implementFunction< sdw::Vec3 >( "clouds_raymarch"
				, [&]( Ray const & ray
					, sdw::Vec3 startPos
					, sdw::Vec3 const & endPos
					, sdw::Vec3 const & bg
					, sdw::IVec2 const & fragCoord
					, sdw::Vec3 const & sunColor
					, sdw::Float accumDensity )
				{
					auto eccentricity = 0.6_f;
					auto silverIntensity = 0.7_f;
					auto silverSpread = 0.1_f;
					auto cloudsMinTransmittance = 1e-1_f;
					auto bayerFactor = 1.0_f / 16.0_f;
					auto bayerFilter = writer.declConstantArray( "bayerFilter"
						, std::vector< sdw::Float >{ 0.0_f * bayerFactor, 8.0_f * bayerFactor, 2.0_f * bayerFactor, 10.0_f * bayerFactor
							, 12.0_f * bayerFactor, 4.0_f * bayerFactor, 14.0_f * bayerFactor, 6.0_f * bayerFactor
							, 3.0_f * bayerFactor, 11.0_f * bayerFactor, 1.0_f * bayerFactor, 9.0_f * bayerFactor
							, 15.0_f * bayerFactor, 7.0_f * bayerFactor, 13.0_f * bayerFactor, 5.0_f * bayerFactor } );

					auto path = writer.declLocale( "path"
						, endPos - startPos );
					auto len = writer.declLocale( "len"
						, length( path ) );

					auto nSteps = 64_i;//int(mix(48.0, 96.0, clamp( len/SPHERE_DELTA - 1.0,0.0,1.0) ));

					auto ds = writer.declLocale( "ds"
						, len / writer.cast< sdw::Float >( nSteps ) );
					auto dir = writer.declLocale( "dir"
						, ray.direction );
					dir *= ds;
					auto result = writer.declLocale( "result"
						, vec3( 0.0_f ) );
					auto a = fragCoord.x() % 4_i;
					auto b = fragCoord.y() % 4_i;
					auto pos = writer.declLocale( "pos"
						, startPos + dir * bayerFilter[a * 4 + b] );

					auto lightDotEye = writer.declLocale( "lightDotEye"
						, dot( normalize( atmosphere.getSunDirection() ), normalize( dir ) ) );
					auto scattering = writer.declLocale( "scattering"
						, henyeyGreenstein( eccentricity, lightDotEye, silverIntensity, silverSpread ) );
					scattering = max( scattering, 1.0_f );

					auto transmittance = writer.declLocale( "transmittance"
						, 1.0_f );
					auto sigmaDs = writer.declLocale( "sigmaDs"
						, -ds * cloudsData.density() );
					auto entered = writer.declLocale( "entered"
						, 0_b );

					FOR( writer, sdw::Int, i, 0_i, i < nSteps, ++i )
					{
						//IF( writer, pos.y() < atmosphere.getCameraPosition().y() - sphereDelta * 1.5_f )
						//{
						//	writer.loopContinueStmt();
						//}
						//FI;

						auto relativeHeight = writer.declLocale( "relativeHeight"
							, getHeightFraction( pos ) );

						IF( writer, relativeHeight >= 0.0_f && relativeHeight <= 1.0_f )
						{
							auto densitySample = writer.declLocale( "densitySample"
								, sampleCloudDensity( pos
									, 1_b
									, relativeHeight
									, writer.cast< sdw::Float >( i ) / 16.0_f ) );

							IF( writer, densitySample > 0.0_f )
							{
								accumDensity += densitySample;
								auto ambientLight = writer.declLocale( "ambientLight"
									, cloudsData.cloudColorBottom() ); //mix( CLOUDS_AMBIENT_COLOR_BOTTOM, CLOUDS_AMBIENT_COLOR_TOP, height );
								auto densityAlongLight = writer.declLocale( "densityAlongLight"
									, 0.0_f );
								auto lightDensity = writer.declLocale( "lightDensity"
									, raymarchToLight( ray
										, pos
										, ds * 0.1_f
										, atmosphere.getSunDirection()
										, densitySample
										, lightDotEye
										, densityAlongLight ) );
								auto powderTerm = writer.declLocale( "powderTerm"
									, utils.powder( densitySample ) );

								IF( writer, cloudsData.enablePowder() == 0_i )
								{
									powderTerm = 1.0_f;
								}
								FI;

								auto scattered = writer.declLocale( "scattered"
									, 0.6_f * densitySample * mix( mix( ambientLight * 1.8_f, bg, vec3( 0.2_f ) )
										, scattering * sunColor
										, vec3( powderTerm * lightDensity ) ) );
								auto dTrans = writer.declLocale( "dTrans"
									, exp( densitySample * sigmaDs ) );
								auto Sint = writer.declLocale( "Sint"
									, ( scattered - scattered * dTrans ) * ( 1.0_f / densitySample ) );
								result += transmittance * Sint;
								transmittance *= dTrans;
							}
							FI;
						}
						FI;

						IF( writer, accumDensity >= 1.0_f )
						{
							accumDensity = 1.0_f;
							writer.loopBreakStmt();
						}
						FI;

						pos += dir;
					}
					ROF;

					writer.returnStmt( result );
				}
				, InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "startPos" }
				, sdw::InVec3{ writer, "endPos" }
				, sdw::InVec3{ writer, "bg" }
				, sdw::InIVec2{ writer, "fragCoord" }
				, sdw::InVec3{ writer, "sunColor" }
				, sdw::OutFloat{ writer, "accumDensity" } );
		}

		return m_raymarchToCloud( pray
			, pstartPos
			, pendPos
			, pbg
			, pfragCoord
			, psunColor
			, paccumDensity );
	}

	sdw::RetFloat CloudsModel::computeFogAmount( sdw::Vec3 const & pstartPos
		, sdw::Vec3 const & pworldPos
		, sdw::Float const & pfactor )
	{
		if ( !m_computeFogAmount )
		{
			m_computeFogAmount = writer.implementFunction< sdw::Float >( "clouds_computeFogAmount"
				, [&]( sdw::Vec3 const & startPos
					, sdw::Vec3 const & worldPos
					, sdw::Float const & factor )
				{
					auto dist = writer.declLocale( "dist"
						, length( startPos - worldPos ) );
					auto radius = writer.declLocale( "radius"
						, worldPos.y() * 0.3_f );
					auto alpha = writer.declLocale( "alpha"
						, ( dist / radius ) );

					writer.returnStmt( ( 1.0_f - exp( -dist * alpha * factor ) ) );
				}
				, sdw::InVec3{ writer, "startPos" }
				, sdw::InVec3{ writer, "worldPos" }
				, sdw::InFloat{ writer, "factor" } );
		}

		return m_computeFogAmount( pstartPos, pworldPos, pfactor );
	}

	RetIntersection CloudsModel::raySphereintersectSkyMap( sdw::Vec3 const & prd
		, sdw::Float const & pradius )
	{
		if ( !m_raySphereintersectSkyMap )
		{
			m_raySphereintersectSkyMap = writer.implementFunction< Intersection >( "clouds_raySphereintersectSkyMap"
				, [&]( sdw::Vec3 const & rd
					, sdw::Float const & radius )
				{
					auto L = writer.declLocale( "L", -vec3( 0.0_f ) );
					auto a = writer.declLocale( "a", dot( rd, rd ) );
					auto b = writer.declLocale( "b", 2.0_f * dot( rd, L ) );
					auto c = writer.declLocale( "c", dot( L, L ) - ( radius * radius ) );
					auto delta = writer.declLocale( "delta", b * b - 4.0_f * a * c );
					auto t = writer.declLocale( "t", max( 0.0_f, ( -b + sqrt( delta ) ) / 2.0_f ) );
					auto result = writer.declLocale< Intersection >( "result" );
					result.point() = rd * t;
					result.valid() = 1_b;
					writer.returnStmt( result );
				}
				, sdw::InVec3{ writer, "rd" }
				, sdw::InFloat{ writer, "radius" } );
		}

		return m_raySphereintersectSkyMap( prd
			, pradius );
	}

	sdw::RetFloat CloudsModel::henyeyGreenstein( sdw::Float const & pg
		, sdw::Float const & pcosTheta )
	{
		if ( !m_henyeyGreenstein )
		{
			m_henyeyGreenstein = writer.implementFunction< sdw::Float >( "clouds_henyeyGreenstein"
				, [&]( sdw::Float const & g
					, sdw::Float const & cosTheta )
				{
					auto numer = writer.declLocale( "numer"
						, 1.0f - g * g );
					auto denom = writer.declLocale( "denom"
						, 1.0f + g * g + 2.0f * g * cosTheta );
					writer.returnStmt( numer / pow( denom, 1.0_f ) );
				}
				, sdw::InFloat{ writer, "g" }
				, sdw::InFloat{ writer, "cosTheta" } );
		}

		return m_henyeyGreenstein( pg, pcosTheta );
	}

	sdw::Float CloudsModel::henyeyGreenstein( sdw::Float const & g
		, sdw::Float const & cosTheta
		, sdw::Float const & silverIntensity
		, sdw::Float const & silverSpread )
	{
		return max( henyeyGreenstein( g, cosTheta )
			, silverIntensity * henyeyGreenstein( 0.99_f - silverSpread, cosTheta ) );
	}

	//************************************************************************************************
}
