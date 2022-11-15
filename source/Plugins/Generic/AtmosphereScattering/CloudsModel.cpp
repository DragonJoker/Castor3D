#include "AtmosphereScattering/CloudsModel.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsUbo.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

#define Debug_CloudsRadius 0

namespace atmosphere_scattering
{
	//************************************************************************************************

	CloudsModel::CloudsModel( sdw::ShaderWriter & pwriter
		, castor3d::shader::Utils & putils
		, AtmosphereModel & patmosphere
		, ScatteringModel & pscattering
		, CloudsData const & pclouds
		, uint32_t & binding
		, uint32_t set )
		: writer{ pwriter }
		, utils{ putils }
		, atmosphere{ patmosphere }
		, scattering{ pscattering }
		, clouds{ pclouds }
		, perlinWorleyNoiseMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "perlinWorleyNoiseMap"
			, binding++
			, set ) }
		, worleyNoiseMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "worleyNoiseMap"
			, binding++
			, set ) }
		, curlNoiseMap{ writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "curlNoiseMap"
			, binding++
			, set ) }
		, weatherMap{ writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "weatherMap"
			, binding++
			, set ) }
		, cloudsInnerRadius{ clouds.innerRadius() + atmosphere.getEarthRadius() }
		, cloudsOuterRadius{ clouds.outerRadius() + atmosphere.getEarthRadius() }
		, cloudsThickness{ clouds.outerRadius() - clouds.innerRadius() }
	{
	}

	sdw::RetVec4 CloudsModel::applyClouds( castor3d::shader::Ray const & pray
		, sdw::IVec2 const & pfragCoord
		, sdw::Vec3 const & psunLuminance
		, sdw::Vec3 & pskyLuminance
		, sdw::Float & pskyBlendFactor )
	{
		if ( !m_applyClouds )
		{
			m_applyClouds = writer.implementFunction< sdw::Vec4 >( "clouds_apply"
				, [&]( castor3d::shader::Ray const & ray
					, sdw::IVec2 const & fragCoord
					, sdw::Vec3 const & sunLuminance
					, sdw::Vec3 skyLuminance
					, sdw::Float skyBlendFactor )
				{
					auto startPos0 = writer.declLocale( "startPos0"
						, vec3( 0.0_f ) );
					auto endPos0 = writer.declLocale( "endPos0"
						, vec3( 0.0_f ) );
					auto startPos1 = writer.declLocale( "startPos1"
						, vec3( 0.0_f ) );
					auto endPos1 = writer.declLocale( "endPos1"
						, vec3( 0.0_f ) );
					auto fogRay0 = writer.declLocale( "fogRay0"
						, vec3( 0.0_f ) );
					auto fogRay1 = writer.declLocale( "fogRay1"
						, vec3( 0.0_f ) );
					auto secondRay = writer.declLocale( "secondRay"
						, 0_b );
					auto interGround = writer.declLocale( "interGround"
						, atmosphere.raySphereIntersectNearest( ray, atmosphere.getEarthRadius() ) );

					auto interInnerN = writer.declLocale( "interInnerN", Intersection{ writer } );
					auto interInnerF = writer.declLocale( "interInnerF", Intersection{ writer } );
					auto interInnerCount = writer.declLocale( "interInnerCount"
						, atmosphere.raySphereIntersect( ray, cloudsInnerRadius, interGround, interInnerN, interInnerF ) );

					auto interOuterN = writer.declLocale( "interOuterN", Intersection{ writer } );
					auto interOuterF = writer.declLocale( "interOuterF", Intersection{ writer } );
					auto interOuterCount = writer.declLocale( "interOuterCount"
						, atmosphere.raySphereIntersect( ray, cloudsOuterRadius, interGround, interOuterN, interOuterF ) );

					//compute raymarching starting and ending point
					IF( writer, ray.origin.y() < cloudsInnerRadius )
					{
						// Ray below clouds boundaries.
						IF( writer, interGround.valid() )
						{
							writer.returnStmt( vec4( 0.0_f ) );
						}
						FI;

						startPos0 = interInnerN.point();
						endPos0 = interOuterN.point();
						fogRay0 = startPos0;
					}
					ELSEIF( ray.origin.y() >= cloudsInnerRadius
						&& ray.origin.y() < cloudsOuterRadius )
					{
						// Ray inside clouds boundaries.
						startPos0 = ray.origin;
						endPos0 = interOuterN.point();
						fogRay0 = startPos0;

						IF( writer, interInnerCount > 1_i )
						{
							// Ray intersects clouds layer a second time
							// Hence, for precision's sake, trace a second ray.
							startPos1 = interInnerF.point();
							endPos1 = endPos0;
							fogRay1 = startPos1;
							endPos0 = interInnerN.point();
							secondRay = 1_b;
						}
						FI;
					}
					ELSE
					{
						// Ray over clouds.
						startPos0 = interOuterN.point();
						endPos0 = interInnerN.point();
						fogRay0 = startPos0;

						IF( writer, interInnerCount > 1_i && interOuterCount > 1_i )
						{
							// Ray intersects clouds layer a second time
							// Hence, for precision's sake, trace a second ray.
							startPos1 = interInnerF.point();
							endPos1 = interOuterF.point();
							secondRay = 1_b;
						}
						ELSEIF( interOuterCount > 1_i )
						{
							// Ray remains between clouds layer bounds before getting out.
							endPos0 = interOuterF.point();
						}
						FI;
					}
					FI;

					auto sunRadiance = writer.declLocale( "sunRadiance"
						, 3.0_f * scattering.getSunRadiance( atmosphere.getSunDirection() ) );

					// Compute fog amount.
					auto fogAmount0 = writer.declLocale( "fogAmount0"
						, computeFogAmount( fogRay0, ray.origin, 2.0_f ) );
					auto earthShadow0 = writer.declLocale( "earthShadow"
						, 0.0_f );
					auto rayMarchResult = writer.declLocale( "rayMarchResult"
						, raymarchToCloud( ray
							, startPos0
							, endPos0
							, fragCoord
							, sunRadiance
							, earthShadow0 ) );
					auto skyLuminance0 = writer.declLocale( "skyLuminance0"
						, skyLuminance );
					auto result = writer.declLocale( "result"
						, computeLighting( ray
							, sunRadiance
							, sunLuminance
							, skyLuminance0
							, 1.0_f - fogAmount0
							, earthShadow0
							, rayMarchResult ) );

					IF( writer, secondRay )
					{
						// Compute fog amount.
						auto fogAmount1 = writer.declLocale( "fogAmount1"
							, computeFogAmount( fogRay1, ray.origin, 0.006_f ) );

						IF( writer, fogAmount1 <= 0.965_f )
						{
							auto earthShadow1 = writer.declLocale( "earthShadow1"
								, 0.0_f );
							rayMarchResult = raymarchToCloud( ray
								, startPos1
								, endPos1
								, fragCoord
								, sunRadiance
								, earthShadow1 );
							auto skyLuminance1 = writer.declLocale( "skyLuminance1"
								, skyLuminance );
							auto lightingResult = writer.declLocale( "lightingResult"
								, computeLighting( ray
									, sunRadiance
									, sunLuminance
									, skyLuminance1
									, 1.0_f - fogAmount1
									, earthShadow1
									, rayMarchResult ) );
							result = max( result, lightingResult );
							earthShadow0 = max( earthShadow0, earthShadow1 );
							fogAmount0 = max( fogAmount0, fogAmount1 );
							skyLuminance0 = max( skyLuminance0, skyLuminance1 );
						}
						FI;
					}
					FI;

					skyLuminance = skyLuminance0;
					skyBlendFactor = earthShadow0 * fogAmount0;
					writer.returnStmt( result );
				}
				, castor3d::shader::InRay{ writer, "ray" }
				, sdw::InIVec2{ writer, "fragCoord" }
				, sdw::InVec3{ writer, "sunLuminance" }
				, sdw::InOutVec3{ writer, "skyLuminance" }
				, sdw::OutFloat{ writer, "skyBlendFactor" } );
		}

		return m_applyClouds( pray
			, pfragCoord
			, psunLuminance
			, pskyLuminance
			, pskyBlendFactor );
	}

	sdw::Vec2 CloudsModel::getSphericalProjection( sdw::Vec3 const & p )
	{
		return normalize( p ).xz() * vec2( 0.5_f );
	}

	sdw::Float CloudsModel::getHeightFraction( sdw::Vec3 const & inPos )
	{
		return ( length( inPos.y() ) - cloudsInnerRadius ) / cloudsThickness;
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
					point += heightFraction * clouds.windDirection() * clouds.topOffset();

					//Animate clouds in wind direction and add a small upward bias to the wind direction
					point += clouds.windDirection() * clouds.time() * clouds.cloudSpeed();
					writer.returnStmt( point );
				}
				, sdw::InVec3{ writer, "point" }
				, sdw::InFloat{ writer, "heightFraction" } );
		}

		return m_skewSamplePointWithWind( ppoint
			, pheightFraction );
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
					writer.returnStmt( numerator / cloudsThickness );
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

	sdw::RetFloat CloudsModel::sampleLowFrequency( sdw::Vec2 const & punskewedUV
		, sdw::Vec2 const & pskewedUV
		, sdw::Float const & pheightFraction
		, sdw::Float const & plod )
	{
		if ( !m_sampleLowFrequency )
		{
			m_sampleLowFrequency = writer.implementFunction< sdw::Float >( "clouds_sampleLowFrequency"
				, [&]( sdw::Vec2 const & unskewedUV
					, sdw::Vec2 const & skewedUV
					, sdw::Float const & heightFraction
					, sdw::Float const & lod )
				{
					//Read in the low-frequency Perlin-Worley noises and Worley noises
					auto lowFrequencyNoise = writer.declLocale( "lowFrequencyNoise"
						, perlinWorleyNoiseMap.lod( vec3( unskewedUV * clouds.crispiness(), heightFraction ), lod ) );

					//Build an FBM out of the low-frequency Worley Noises that are used to add detail to the Low-frequency Perlin Worley noise
					auto lowFrequencyFBM = writer.declLocale( "lowFrequencyFBM"
						, dot( lowFrequencyNoise.gba(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );

					// Define the base cloud shape by dilating it with the low-frequency FBM
					auto baseCloudShape = writer.declLocale( "baseCloudShape"
						, utils.remap( lowFrequencyNoise.r(), ( lowFrequencyFBM - 1.0_f ), 1.0_f, 0.0_f, 1.0_f ) );

					// Use weather map for cloud types and blend between them and their densities
					auto weather = writer.declLocale( "weatherData"
						, weatherMap.lod( skewedUV, 0.0_f ) );
					auto densityHeightGradient = writer.declLocale( "densityHeightGradient"
						, getDensityHeightGradientForPoint( heightFraction, weather.g() ) );
					// Apply Height function to the base cloud shape
					baseCloudShape *= densityHeightGradient / heightFraction;

					// Cloud coverage is stored in weather data’s red channel.
					auto cloudCoverage = writer.declLocale( "cloudCoverage"
						, weather.r() * clouds.coverage() );
					// Use remap to apply the cloud coverage attribute.
					auto baseCloudWithCoverage = writer.declLocale( "baseCloudWithCoverage"
						, utils.remap( baseCloudShape, cloudCoverage, 1.0_f, 0.0_f, 1.0_f ) );
					// To ensure that the density increases with coverage in an aesthetically pleasing manner
					// Multiply the result by the cloud coverage attribute so that smaller clouds are lighter 
					// and more aesthetically pleasing
					baseCloudWithCoverage *= cloudCoverage;

					writer.returnStmt( baseCloudWithCoverage );
				}
				, sdw::InVec2{ writer, "unskewedUV" }
				, sdw::InVec2{ writer, "skewedUV" }
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "lod" } );
		}

		return m_sampleLowFrequency( punskewedUV
			, pskewedUV
			, pheightFraction
			, plod );
	}

	sdw::RetFloat CloudsModel::erodeWithHighFrequency( sdw::Float const & pbaseDensity
		, sdw::Vec3 const & pskewedSamplePoint
		, sdw::Vec2 const & pskewedUV
		, sdw::Float const & pheightFraction
		, sdw::Float const & plod )
	{
		if ( !m_erodeWithHighFrequency )
		{
			m_erodeWithHighFrequency = writer.implementFunction< sdw::Float >( "clouds_erodeWithHighFrequency"
				, [&]( sdw::Float const & baseDensity
					, sdw::Vec3 skewedSamplePoint
					, sdw::Vec2 const & skewedUV
					, sdw::Float heightFraction
					, sdw::Float const & lod )
				{
					// Add turbulence to the bottom of the clouds
					auto curlNoise = writer.declLocale( "curlNoise"
						, curlNoiseMap.sample( skewedUV, 0.0_f ) );
					skewedSamplePoint.xy() += curlNoise * ( 1.0_f - heightFraction );

					// Sample High Frequency Noises
					auto highFrequencyNoise = writer.declLocale( "highFrequencyNoise"
						, worleyNoiseMap.lod( vec3( skewedUV * clouds.crispiness(), heightFraction ) * clouds.curliness(), lod ).rgb() );

					// Build High Frequency FBM
					auto highFrequencyFBM = writer.declLocale( "highFrequencyFBM"
						, dot( highFrequencyNoise.rgb(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );

					// Recompute height fraction after applying wind and top offset
					heightFraction = getHeightFraction( skewedSamplePoint );

					//Erode the base shape of the cloud with the distorted high frequency worley noises
					auto highFreqNoiseModifier = writer.declLocale( "highFreqNoiseModifier"
						, clamp( mix( highFrequencyFBM, 1.0_f - highFrequencyFBM, clamp( heightFraction * 10.0_f, 0.0_f, 1.0_f ) ), 0.0_f, 1.0_f ) );

					auto finalCloud = writer.declLocale( "finalCloud"
						, baseDensity - highFreqNoiseModifier * ( 1.0_f - baseDensity ) );
					writer.returnStmt( utils.remap( finalCloud * 2.0_f, highFreqNoiseModifier * 0.2_f, 1.0_f, 0.0_f, 1.0_f ) );
				}
				, sdw::InFloat{ writer, "baseCloud" }
				, sdw::InVec3{ writer, "skewedSamplePoint" }
				, sdw::InVec2{ writer, "skewedUV" }
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "lod" } );
		}

		return m_erodeWithHighFrequency( pbaseDensity
			, pskewedSamplePoint
			, pskewedUV
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
					auto unskewedUV = writer.declLocale( "unskewedUV"
						, getSphericalProjection( samplePoint ) );
					auto skewedUV = writer.declLocale( "skewedUV"
						, getSphericalProjection( skewedSamplePoint ) );
					auto baseDensity = writer.declLocale( "baseDensity"
						, sampleLowFrequency( unskewedUV
							, skewedUV
							, heightFraction
							, lod ) );

					IF( writer, baseDensity > 0.0_f && expensive )
					{
						baseDensity = erodeWithHighFrequency( baseDensity
							, skewedSamplePoint
							, skewedUV
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

	sdw::RetFloat CloudsModel::raymarchToLight( sdw::Vec3 const & pviewDir
		, sdw::Vec3 const & ppos
		, sdw::Float const & pstepSize
		, sdw::Vec3 const & plightDir )
	{
		if ( !m_raymarchToLight )
		{
			m_raymarchToLight = writer.implementFunction< sdw::Float >( "clouds_raymarchToLight"
				, [&]( sdw::Vec3 const & viewDir
					, sdw::Vec3 pos
					, sdw::Float const & stepSize
					, sdw::Vec3 const & lightDir )
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
						, pos );
					auto rayStep = writer.declLocale( "rayStep"
						, lightDir * stepSize * 0.7_f );
					auto coneRadius = writer.declLocale( "coneRadius"
						, 1.0_f );
					auto coneDensity = writer.declLocale( "coneDensity"
						, 0.0_f );
					auto invDepth = writer.declLocale( "invDepth"
						, 1.0_f / ( stepSize * 6.0_f ) );
					auto density = writer.declLocale( "density"
						, 0.0_f );

					FOR( writer, sdw::Int, i, 0_i, i < 6_i, ++i )
					{
						auto posInCone = writer.declLocale( "posInCone"
							, startPos + lightDir + coneRadius * noiseKernel[i] * writer.cast< sdw::Float >( i ) );
						auto heightFraction = writer.declLocale( "heightFraction"
							, getHeightFraction( posInCone ) );

						IF( writer, heightFraction <= 1.0_f )
						{
							auto cloudDensity = writer.declLocale( "cloudDensity"
								, sampleCloudDensity( posInCone
									, density < 0.3_f
									, heightFraction
									, writer.cast< sdw::Float >( i + 1_i ) ) );

							IF( writer, cloudDensity > 0.0_f )
							{
								density += cloudDensity;
								auto transmittance = writer.declLocale( "transmittance"
									, 1.0_f - ( density * invDepth * clouds.absorption() ) );
								coneDensity += ( cloudDensity * transmittance );
							}
							FI;
						}
						FI;

						startPos += rayStep;
						coneRadius += coneStep;
					}
					ROF;

					// 1 far sample for shadowing
					pos += rayStep * 8.0_f;
					auto heightFraction = writer.declLocale( "heightFraction"
						, getHeightFraction( pos ) );
					auto cloudDensity = writer.declLocale( "cloudDensity"
						, sampleCloudDensity( pos
							, 0_b
							, heightFraction
							, 6.0_f ) );

					IF( writer, cloudDensity > 0.0_f )
					{
						density += cloudDensity;
						auto transmittance = writer.declLocale( "transmittance"
							, 1.0_f - ( density * invDepth * clouds.absorption() ) );
						coneDensity += ( cloudDensity * transmittance );
					}
					FI;

					writer.returnStmt( getLightEnergy( dot( lightDir, viewDir )
						, coneDensity ) );
				}
				, sdw::InVec3{ writer, "viewDir" }
				, sdw::InVec3{ writer, "pos" }
				, sdw::InFloat{ writer, "stepSize" }
				, sdw::InVec3{ writer, "lightDir" } );
		}

		return m_raymarchToLight( pviewDir
			, ppos
			, pstepSize
			, plightDir );
	}

	sdw::RetVec4 CloudsModel::raymarchToCloud( Ray const & pray
		, sdw::Vec3 const & pstartPos
		, sdw::Vec3 const & pendPos
		, sdw::IVec2 const & pfragCoord
		, sdw::Vec3 const & psunColor
		, sdw::Float & pearthShadow )
	{
		if ( !m_raymarchToCloud )
		{
			m_raymarchToCloud = writer.implementFunction< sdw::Vec4 >( "clouds_raymarch"
				, [&]( Ray const & ray
					, sdw::Vec3 startPos
					, sdw::Vec3 const & endPos
					, sdw::IVec2 const & fragCoord
					, sdw::Vec3 const & sunColor
					, sdw::Float earthShadow )
				{
					auto lightFactor = 1.0_f;
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

					auto sampleCount = writer.declLocale( "sampleCount"
						, writer.cast< sdw::Int >( ceil( mix( 48.0_f
							, 96.0_f
							, clamp( length( path ) / cloudsThickness, 0.0_f, 1.0_f ) ) ) ) );

					auto stepVector = writer.declLocale( "stepVector"
						, path / writer.cast< sdw::Float >( sampleCount - 1_i ) );
					auto stepSize = writer.declLocale( "stepSize"
						, length( stepVector ) );
					auto viewDir = writer.declLocale( "viewDir"
						, normalize( path ) );

					auto pos = writer.declLocale( "pos"
						, startPos );
					auto result = writer.declLocale( "result"
						, vec4( 0.0_f ) );

					// Dithering on the starting ray position to reduce banding artifacts
					auto a = fragCoord.x() % 4_i;
					auto b = fragCoord.y() % 4_i;
					pos += stepVector * bayerFilter[a * 4 + b];

					auto lightColor = writer.declLocale( "lightColor"
						, sunColor * lightFactor * clouds.bottomColor() );
					auto ambientLight = writer.declLocale( "ambientLight"
						, lightFactor * 0.65_f * mix( sunColor
							, mix( clouds.topColor(), clouds.bottomColor(), vec3( 0.15_f ) )
							, vec3( 0.65_f ) ) );
					auto accumDensity = writer.declLocale( "accumDensity"
						, 0.0_f );
					auto i = writer.declLocale( "i"
						, 0_i );

					earthShadow = 0.0_f;

					WHILE( writer, i < sampleCount )
					{
						auto relativeHeight = writer.declLocale( "relativeHeight"
							, getHeightFraction( pos ) );
						earthShadow += atmosphere.getEarthShadow( vec3( 0.0_f ), pos );

						IF( writer, relativeHeight >= 0.0_f
							&& relativeHeight <= 1.0_f
							&& result.a() < 0.95_f )
						{
							auto cloudDensity = writer.declLocale( "cloudDensity"
								, sampleCloudDensity( pos
									, 1_b
									, relativeHeight
									, writer.cast< sdw::Float >( i ) / 16.0_f ) );
							cloudDensity *= clouds.density();

							IF( writer, cloudDensity > 0.0_f )
							{
								accumDensity += cloudDensity;
								auto lightEnergy = writer.declLocale( "lightEnergy"
									, raymarchToLight( viewDir
										, pos
										, stepSize
										, atmosphere.getSunDirection() ) );
								auto src = writer.declLocale( "src"
									, vec4( sunColor * lightEnergy, cloudDensity ) );
								src.rgb() *= cloudDensity;
								result = ( 1.0_f - result.a() ) * src + result;
							}
							FI;
						}
						FI;

						pos += stepVector;
						++i;
					}
					ELIHW;

					earthShadow /= writer.cast< sdw::Float >( max( 1_i, i ) );
					earthShadow = min( 1.0_f, earthShadow );
					result.a() = accumDensity;
					writer.returnStmt( result );
				}
				, InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "startPos" }
				, sdw::InVec3{ writer, "endPos" }
				, sdw::InIVec2{ writer, "fragCoord" }
				, sdw::InVec3{ writer, "sunColor" }
				, sdw::OutFloat{ writer, "earthShadow" } );
		}

		return m_raymarchToCloud( pray
			, pstartPos
			, pendPos
			, pfragCoord
			, psunColor
			, pearthShadow );
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

					writer.returnStmt( clamp( 1.0_f - exp( -dist * alpha * factor ), 0.0_f, 1.0_f ) );
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

	sdw::RetVec4 CloudsModel::computeLighting( Ray const & pray
		, sdw::Vec3 const & psunRadiance
		, sdw::Vec3 const & psunLuminance
		, sdw::Vec3 & pskyLuminance
		, sdw::Float const & pfadeOut
		, sdw::Float const & pearthShadow
		, sdw::Vec4 const & prayMarchResult )
	{
		if ( !m_computeLighting )
		{
			m_computeLighting = writer.implementFunction< sdw::Vec4 >( "clouds_computeLighting"
				, [&]( Ray const & ray
					, sdw::Vec3 const & sunRadiance
					, sdw::Vec3 const & sunLuminance
					, sdw::Vec3 skyLuminance
					, sdw::Float const & fadeOut
					, sdw::Float const & earthShadow
					, sdw::Vec4 const & rayMarchResult )
				{
					auto cloudsDensity = writer.declLocale( "cloudsDensity"
						, clamp( rayMarchResult.a(), 0.0_f, 1.0_f ) );
					auto cloudsColour = writer.declLocale( "cloudsColour"
						, rayMarchResult.rgb() );

					// add sun glare to clouds
					auto sunIntensity = writer.declLocale( "sunIntensity"
						, clamp( dot( atmosphere.getSunDirection(), ray.direction )
							, 0.0_f
							, 1.0_f ) );
					auto sunGlare = writer.declLocale( "sunGlare"
						, sunRadiance * pow( sunIntensity, 256.0_f ) );
					auto sunGlareIntensity = writer.declLocale( "sunGlareIntensity"
						, ( 1.0_f - cloudsDensity ) );
					auto minGlare = 0.5_f;
					sunGlareIntensity = utils.remap( sunGlareIntensity, 0.0_f, 1.0_f, minGlare, 1.0_f );
					cloudsColour += sunGlare * ( sunGlareIntensity - minGlare );

					// Fade out clouds into the horizon.
					auto cubeMapEndPos = writer.declLocale( "cubeMapEndPos"
						, raySphereintersectSkyMap( ray.direction, 0.5_f ).point() );
					cloudsColour = mix( vec3( 1.0_f )
						, cloudsColour
						, vec3( pow( max( cubeMapEndPos.y() + 0.1_f, 0.0_f ), 0.2_f ) ) );

					skyLuminance += sunLuminance * ( sunGlareIntensity - minGlare );
					cloudsColour = mix( vec3( 0.0_f )
						, cloudsColour
						, vec3( fadeOut ) );
					cloudsDensity = max( cloudsDensity, ( 1.0_f - fadeOut ) );
					writer.returnStmt( vec4( earthShadow * cloudsColour, cloudsDensity ) );
				}
				, InRay{ writer, "ray" }
				, sdw::InVec3{ writer, "sunRadiance" }
				, sdw::InVec3{ writer, "sunLuminance" }
				, sdw::InOutVec3{ writer, "skyLuminance" }
				, sdw::InFloat{ writer, "fadeOut" }
				, sdw::InFloat{ writer, "earthShadow" }
				, sdw::InVec4{ writer, "rayMarchResult" } );
		}

		return m_computeLighting( pray
			, psunRadiance
			, psunLuminance
			, pskyLuminance
			, pfadeOut
			, pearthShadow
			, prayMarchResult );
	}

	sdw::Float CloudsModel::getLightEnergy( sdw::Float cosTheta
		, sdw::Float const & coneDensity )
	{
		return utils.beer( coneDensity )
			* utils.powder( coneDensity, cosTheta )
			* henyeyGreenstein( 0.2_f, cosTheta );
	}

	//************************************************************************************************
}
