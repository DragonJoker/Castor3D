#include "AtmosphereScattering/CloudsModel.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/AtmosphereWeatherUbo.hpp"
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
		, CameraData const & pcameraData
		, AtmosphereData const & patmosphereData
		, WeatherData const & pweatherData
		, uint32_t binding )
		: writer{ pwriter }
		, utils{ putils }
		, atmosphere{ patmosphere }
		, scattering{ pscattering }
		, cameraData{ pcameraData }
		, atmosphereData{ patmosphereData }
		, weatherData{ pweatherData }
		, perlinWorleyNoiseMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "perlinWorleyNoiseMap"
			, binding + 0u
			, 0u ) }
		, worleyNoiseMap{ writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "worleyNoiseMap"
			, binding + 1u
			, 0u ) }
		, weatherMap{ writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "weatherMap"
			, binding + 2u
			, 0u ) }
		, sphereInnerRadius{ weatherData.sphereInnerRadius() + atmosphereData.bottomRadius }
		, sphereOuterRadius{ weatherData.sphereOuterRadius() + atmosphereData.bottomRadius }
		, sphereDelta{ weatherData.sphereOuterRadius() - weatherData.sphereInnerRadius() }
		, sphereCenter{ vec3( 0.0_f, -atmosphereData.bottomRadius, 0.0_f ) }
	{
	}

	sdw::Vec2 CloudsModel::getUVProjection( sdw::Vec3 const & p )
	{
		return p.xz() / sphereInnerRadius + 0.5_f;
	}

	sdw::Float CloudsModel::getHeightFraction( sdw::Vec3 const & inPos )
	{
		return ( length( inPos - sphereCenter ) - sphereInnerRadius ) / sphereDelta;
	}

	sdw::Void CloudsModel::applyClouds( sdw::IVec2 const & pfragCoord
		, sdw::Vec2 const & ptargetSize
		, sdw::CombinedImage2DRgba32 const & ptransmittance
		, sdw::Vec4 & pbg
		, sdw::Vec4 & pemission
		, sdw::Vec4 & pdistance )
	{
		if ( !m_applyClouds )
		{
			m_applyClouds = writer.implementFunction< sdw::Void >( "applyClouds"
				, [&]( sdw::IVec2 const & fragCoord
					, sdw::Vec2 const & targetSize
					, sdw::CombinedImage2DRgba32 const & transmittanceMap
					, sdw::Vec4 bg
					, sdw::Vec4 emission
					, sdw::Vec4 distance )
				{
					auto clipDir = writer.declLocale( "clipDir"
						, atmosphere.getClipSpace( vec2( fragCoord.xy() ), targetSize, 1.0_f ) );
					auto viewDir = writer.declLocale( "viewDir"
						, cameraData.camInvProj() * vec4( clipDir, 1.0_f ) );
					viewDir = vec4( viewDir.xy(), -1.0_f, 0.0_f );
					auto ray = writer.declLocale< Ray >( "ray" );
					ray.direction = normalize( ( cameraData.camInvView() * viewDir ).xyz() );
					ray.origin = cameraData.position() + vec3( 0.0_f, atmosphereData.bottomRadius, 0.0_f );
					auto startPos = writer.declLocale( "startPos"
						, vec3( 0.0_f ) );
					auto endPos = writer.declLocale( "endPos"
						, vec3( 0.0_f ) );
					auto stub = writer.declLocale( "stub"
						, vec3( 0.0_f ) );
					auto v = writer.declLocale( "v"
						, vec4( 0.0_f ) );
					auto sunColor = writer.declLocale( "sunColor"
						, scattering.getSunLuminance( ray, transmittanceMap ) );

					//compute background color
					auto cubeMapEndPos = writer.declLocale( "cubeMapEndPos"
						, vec3( 0.0_f ) );
					raySphereintersectSkyMap( ray.direction
						, 0.5_f
						, cubeMapEndPos );

					//bg = mix( vec4( 1.0_f )
					//	, bg
					//	, vec4( pow( max( cubeMapEndPos.y() + 0.1_f, 0.0_f ), 0.2_f ) ) );

					//compute raymarching starting and ending point
					auto fogRay = writer.declLocale( "fogRay"
						, vec3( 0.0_f ) );

					IF( writer, ray.origin.y() < sphereInnerRadius )
					{
						raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereInnerRadius
							, startPos );
						raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereOuterRadius
							, endPos );
						fogRay = startPos;
					}
					ELSEIF( ray.origin.y() > sphereInnerRadius
						&& ray.origin.y() < sphereOuterRadius )
					{
						startPos = cameraData.position();
						raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereOuterRadius
							, endPos );

						IF( writer, !raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereInnerRadius
							, fogRay ) )
						{
							fogRay = startPos;
						}
						FI;
					}
					ELSE
					{
						raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereOuterRadius
							, startPos );
						raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereInnerRadius
							, endPos );
						raySphereIntersect( cameraData.position()
							, ray.direction
							, sphereOuterRadius
							, fogRay );
					}
					FI;

					//compute fog amount and early exit if over a certain value
					auto fogAmount = writer.declLocale( "fogAmount"
						, computeFogAmount( fogRay, 0.006_f ) );

					auto bloom = writer.declLocale( "bloom"
						, vec4( sunColor * 1.3_f, 1.0_f ) );

					IF( writer, fogAmount > 0.965_f )
					{
						emission = bg;
						distance = vec4( -1.0_f );
						writer.returnStmt(); //early exit
					}
					FI;

					auto cloudDistance = writer.declLocale( "cloudDistance"
						, vec4( 0.0_f ) );
					v = raymarchToCloud( startPos
						, endPos
						, bg.rgb()
						, fragCoord
						, sunColor
						, cloudDistance );
					cloudDistance = vec4( sdw::distance( cameraData.position(), cloudDistance.xyz() ), 0.0_f, 0.0_f, 0.0_f );

					auto cloudAlphaness = writer.declLocale( "cloudAlphaness"
						, utils.threshold( v.a(), 0.2_f ) );
					v.rgb() = v.rgb() * 1.8_f - 0.1_f; // contrast-illumination tuning

					// apply atmospheric fog to far away clouds
					auto ambientColor = writer.declLocale( "ambientColor"
						, bg.rgb() );

					// use current position distance to center as action radius
					v.rgb() = mix( v.rgb()
						, bg.rgb() * v.a()
						, vec3( clamp( fogAmount, 0.0_f, 1.0_f ) ) );

					// add sun glare to clouds
					auto sun = writer.declLocale( "sun"
						, clamp( dot( atmosphereData.sunDirection, normalize( endPos - startPos ) )
							, 0.0_f
							, 1.0_f ) );
					auto s = writer.declLocale( "s"
						, 0.8_f * vec3( 1.0_f, 0.4_f, 0.2_f ) * pow( sun, 256.0_f ) );
					v.rgb() += s * v.a();

					// blend clouds and background
					bg.rgb() = bg.rgb() * ( 1.0_f - v.a() ) + v.rgb();
					bg.a() = 1.0_f;

					auto alphaness = writer.declLocale( "alphaness"
						, vec4( cloudAlphaness, 0.0_f, 0.0_f, 1.0_f ) ); // alphaness buffer

					IF( writer, cloudAlphaness > 0.1_f )
					{
						//apply fog to bloom buffer
						auto fogAmount = writer.declLocale( "fogAmount"
							, computeFogAmount( startPos, 0.003_f ) );

						auto cloud = writer.declLocale( "cloud"
							, mix( vec3( 0.0_f ), bloom.rgb(), vec3( clamp( fogAmount, 0.0_f, 1.0_f ) ) ) );
						bloom.rgb() = bloom.rgb() * ( 1.0_f - cloudAlphaness ) + cloud.rgb();

					}
					FI;

					bg.a() = alphaness.r();
					emission = bloom;
					distance = cloudDistance;
				}
				, sdw::InIVec2{ writer, "fragCoord" }
				, sdw::InVec2{ writer, "targetSize" }
				, sdw::InCombinedImage2DRgba32{ writer, "transmittanceMap" }
				, sdw::InOutVec4{ writer, "bg" }
				, sdw::OutVec4{ writer, "emission" }
				, sdw::OutVec4{ writer, "distance" } );
		}

		return m_applyClouds( pfragCoord
			, ptargetSize
			, ptransmittance
			, pbg
			, pemission
			, pdistance );
	}

	sdw::RetFloat CloudsModel::getDensityForCloud( sdw::Float const & pheightFraction
		, sdw::Float const & pcloudType )
	{
		if ( !m_getDensityForCloud )
		{
			m_getDensityForCloud = writer.implementFunction< sdw::Float >( "getDensityForCloud"
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

					// gradicent computation (see Siggraph 2017 Nubis-Decima talk)
					//return remap(heightFraction, baseGradient.x, baseGradient.y, 0.0, 1.0) * remap(heightFraction, baseGradient.z, baseGradient.w, 1.0, 0.0);
					writer.returnStmt( smoothStep( baseGradient.x(), baseGradient.y(), heightFraction ) - smoothStep( baseGradient.z(), baseGradient.w(), heightFraction ) );
				}
				, sdw::InFloat{ writer, "heightFraction" }
				, sdw::InFloat{ writer, "cloudType" } );
		}

		return m_getDensityForCloud( pheightFraction
			, pcloudType );
	}

	sdw::RetFloat CloudsModel::sampleCloudDensity( sdw::Vec3 const & pp
		, sdw::Boolean const & pexpensive
		, sdw::Float const & plod )
	{
		if ( !m_sampleCloudDensity )
		{
			m_sampleCloudDensity = writer.implementFunction< sdw::Float >( "sampleCloudDensity"
				, [&]( sdw::Vec3 const & p
					, sdw::Boolean const & expensive
					, sdw::Float const & lod )
				{
					auto cloudTopOffset = 0.75_f;

					auto heightFraction = writer.declLocale( "heightFraction"
						, getHeightFraction( p ) );
					auto animation = writer.declLocale( "animation"
						, heightFraction * weatherData.windDirection() * cloudTopOffset
							+ weatherData.windDirection() * weatherData.time() * weatherData.cloudSpeed() );
					auto uv = writer.declLocale( "uv"
						, getUVProjection( p ) );
					auto movingUv = writer.declLocale( "movingUv"
						, getUVProjection( p + animation ) );

					IF( writer, heightFraction < 0.0_f || heightFraction > 1.0_f )
					{
						writer.returnStmt( 0.0_f );
					}
					FI;

					auto lowFrequencyNoise = writer.declLocale( "lowFrequencyNoise"
						, perlinWorleyNoiseMap.lod( vec3( uv * weatherData.crispiness(), heightFraction ), lod ) );
					auto lowFreqFBM = writer.declLocale( "lowFreqFBM"
						, dot( lowFrequencyNoise.gba(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );
					auto baseCloud = writer.declLocale( "baseCloud"
						, utils.remap( lowFrequencyNoise.r(), -( 1.0_f - lowFreqFBM ), 1.0_f, 0.0_f, 1.0_f ) );

					auto density = writer.declLocale( "density"
						, getDensityForCloud( heightFraction, 1.0_f ) );
					baseCloud *= ( density / heightFraction );

					auto weather = writer.declLocale( "weatherData"
						, weatherMap.lod( movingUv, 0.0_f ) );
					auto cloudCoverage = writer.declLocale( "cloudCoverage"
						, weather.r() * weatherData.coverage() );
					auto baseCloudWithCoverage = writer.declLocale( "baseCloudWithCoverage"
						, utils.remap( baseCloud, cloudCoverage, 1.0_f, 0.0_f, 1.0_f ) );
					baseCloudWithCoverage *= cloudCoverage;

					IF( writer, expensive )
					{
						auto erodeCloudNoise = writer.declLocale( "erodeCloudNoise"
							, worleyNoiseMap.lod( vec3( movingUv * weatherData.crispiness(), heightFraction ) * weatherData.curliness(), lod ).rgb() );
						auto highFreqFBM = writer.declLocale( "highFreqFBM"
							, dot( erodeCloudNoise.rgb(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );//(erodeCloudNoise.r * 0.625) + (erodeCloudNoise.g * 0.25) + (erodeCloudNoise.b * 0.125);
						auto highFreqNoiseModifier = writer.declLocale( "highFreqNoiseModifier"
							, mix( highFreqFBM, 1.0_f - highFreqFBM, clamp( heightFraction * 10.0_f, 0.0_f, 1.0_f ) ) );

						baseCloudWithCoverage = baseCloudWithCoverage - highFreqNoiseModifier * ( 1.0_f - baseCloudWithCoverage );
						baseCloudWithCoverage = utils.remap( baseCloudWithCoverage * 2.0_f, highFreqNoiseModifier * 0.2_f, 1.0_f, 0.0_f, 1.0_f );
					}
					FI;

					writer.returnStmt( clamp( baseCloudWithCoverage, 0.0_f, 1.0_f ) );
				}
				, sdw::InVec3{ writer, "p" }
				, sdw::InBoolean{ writer, "expensive" }
				, sdw::InFloat{ writer, "lod" } );
		}

		return m_sampleCloudDensity( pp
			, pexpensive
			, plod );
	}

	sdw::RetFloat CloudsModel::raymarchToLight( sdw::Vec3 const & po
		, sdw::Float const & pstepSize
		, sdw::Vec3 const & plightDir
		, sdw::Float const & poriginalDensity
		, sdw::Float const & plightDotEye )
	{
		if ( !m_raymarchToLight )
		{
			m_raymarchToLight = writer.implementFunction< sdw::Float >( "raymarchToLight"
				, [&]( sdw::Vec3 const & o
					, sdw::Float const & stepSize
					, sdw::Vec3 const & lightDir
					, sdw::Float const & originalDensity
					, sdw::Float const & lightDotEye )
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
					auto density = writer.declLocale( "density"
						, 0.0_f );
					auto coneDensity = writer.declLocale( "coneDensity"
						, 0.0_f );
					auto invDepth = writer.declLocale( "invDepth"
						, 1.0_f / ds );
					auto sigmaDs = writer.declLocale( "sigmaDs"
						, -ds * weatherData.absorption() );
					auto pos = writer.declLocale( "pos"
						, vec3( 0.0_f ) );

					auto T = writer.declLocale( "T"
						, 1.0_f );

					FOR( writer, sdw::Int, i, 0_i, i < 6_i, ++i )
					{
						pos = startPos + coneRadius * noiseKernel[i] * writer.cast< sdw::Float >( i );
						auto heightFraction = writer.declLocale( "heightFraction"
							, getHeightFraction( pos ) );

						IF( writer, heightFraction >= 0.0_f )
						{
							auto cloudDensity = writer.declLocale( "cloudDensity"
								, sampleCloudDensity( pos
									, density > 0.3_f
									, writer.cast< sdw::Float >( i ) / 16.0_f ) );

							IF( writer, cloudDensity > 0.0_f )
							{
								auto Ti = writer.declLocale( "Ti"
									, exp( cloudDensity * sigmaDs ) );
								T *= Ti;
								density += cloudDensity;
							}
							FI;
						}
						FI;

						startPos += rayStep;
						coneRadius += coneStep;
					}
					ROF;

					//return 2.0*T*powder((originalDensity));//*powder(originalDensity, 0.0);
					writer.returnStmt( T );
				}
				, sdw::InVec3{ writer, "o" }
				, sdw::InFloat{ writer, "stepSize" }
				, sdw::InVec3{ writer, "lightDir" }
				, sdw::InFloat{ writer, "originalDensity" }
				, sdw::InFloat{ writer, "lightDotEye" } );
		}

		return m_raymarchToLight( po
			, pstepSize
			, plightDir
			, poriginalDensity
			, plightDotEye );
	}

	sdw::RetVec4 CloudsModel::raymarchToCloud( sdw::Vec3 const & pstartPos
		, sdw::Vec3 const & pendPos
		, sdw::Vec3 const & pbg
		, sdw::IVec2 const & pfragCoord
		, sdw::Vec3 const & psunColor
		, sdw::Vec4 & pcloudPos )
	{
		if ( !m_raymarchToCloud )
		{
			m_raymarchToCloud = writer.implementFunction< sdw::Vec4 >( "raymarchToCloud"
				, [&]( sdw::Vec3 startPos
					, sdw::Vec3 const & endPos
					, sdw::Vec3 const & bg
					, sdw::IVec2 const & fragCoord
					, sdw::Vec3 const & sunColor
					, sdw::Vec4 cloudPos )
				{
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
						, path / len );
					dir *= ds;
					auto col = writer.declLocale( "col"
						, vec4( 0.0_f ) );
					auto a = writer.declLocale( "a"
						, fragCoord.x() % 4_i );
					auto b = writer.declLocale( "b"
						, fragCoord.y() % 4_i );
					auto pos = writer.declLocale( "pos"
						, startPos + dir * bayerFilter[a * 4 + b] );

					auto density = writer.declLocale( "density"
						, 0.0_f );

					auto lightDotEye = writer.declLocale( "lightDotEye"
						, dot( normalize( atmosphereData.sunDirection ), normalize( dir ) ) );

					auto T = writer.declLocale( "T"
						, 1.0_f );
					auto sigmaDs = writer.declLocale( "sigmaDs"
						, -ds * weatherData.density() );
					auto expensive = writer.declLocale( "expensive"
						, 1_b );
					auto entered = writer.declLocale( "entered"
						, 0_b );

					auto zeroDensitySample = writer.declLocale( "zeroDensitySample"
						, 0_i );

					FOR( writer, sdw::Int, i, 0_i, i < nSteps, ++i )
					{
						//IF( writer, pos.y() >= c3d_camera.position().y() - SPHERE_DELTA*1.5 ){

						auto densitySample = writer.declLocale( "densitySample"
							, sampleCloudDensity( pos
								, 1_b
								, writer.cast< sdw::Float >( i ) / 16.0_f ) );

						IF( writer, densitySample > 0.0_f )
						{
							IF( writer, !entered )
							{
								cloudPos = vec4( pos, 1.0_f );
								entered = true;
							}
							FI;

							auto height = writer.declLocale( "height"
								, getHeightFraction( pos ) );
							auto ambientLight = writer.declLocale( "ambientLight"
								, weatherData.cloudColorBottom() ); //mix( CLOUDS_AMBIENT_COLOR_BOTTOM, CLOUDS_AMBIENT_COLOR_TOP, height );
							auto lightDensity = writer.declLocale( "lightDensity"
								, raymarchToLight( pos
									, ds * 0.1_f
									, atmosphereData.sunDirection
									, densitySample
									, lightDotEye ) );
							auto scattering = writer.declLocale( "scattering"
								, mix( atmosphere.hgPhase( -0.08_f, lightDotEye )
									, atmosphere.hgPhase( 0.08_f, lightDotEye )
									, clamp( sdw::fma( lightDotEye, 0.5_f, 0.5_f ), 0.0_f, 1.0_f ) ) );
							scattering = max( scattering, 1.0_f );
							auto powderTerm = writer.declLocale( "powderTerm"
								, utils.powder( densitySample ) );

							IF( writer, weatherData.enablePowder() == 0_i )
							{
								powderTerm = 1.0_f;
							}
							FI;

							auto S = writer.declLocale( "S"
								, 0.6_f * densitySample * mix( mix( ambientLight * 1.8_f, bg, vec3( 0.2_f ) )
									, scattering * sunColor
									, vec3( powderTerm * lightDensity ) ) );
							auto dTrans = writer.declLocale( "dTrans"
								, exp( densitySample * sigmaDs ) );
							auto Sint = writer.declLocale( "Sint"
								, ( S - S * dTrans ) * ( 1.0_f / densitySample ) );
							col.rgb() += T * Sint;
							T *= dTrans;
						}
						FI;

						IF( writer, T <= cloudsMinTransmittance )
						{
							writer.loopBreakStmt();
						}
						FI;

						pos += dir;
						//}
						//FI;
					}
					ROF;

					col.a() = 1.0_f - T;

					writer.returnStmt( col );
				}
				, sdw::InVec3{ writer, "startPos" }
				, sdw::InVec3{ writer, "endPos" }
				, sdw::InVec3{ writer, "bg" }
				, sdw::InIVec2{ writer, "fragCoord" }
				, sdw::InVec3{ writer, "sunColor" }
				, sdw::OutVec4{ writer, "cloudPos" } );
		}

		return m_raymarchToCloud( pstartPos
			, pendPos
			, pbg
			, pfragCoord
			, psunColor
			, pcloudPos );
	}

	sdw::RetFloat CloudsModel::computeFogAmount( sdw::Vec3 const & pstartPos
		, sdw::Float const & pfactor )
	{
		if ( !m_computeFogAmount )
		{
			m_computeFogAmount = writer.implementFunction< sdw::Float >( "computeFogAmount"
				, [&]( sdw::Vec3 const & startPos
					, sdw::Float const & factor )
				{
					auto dist = writer.declLocale( "dist"
						, length( startPos - cameraData.position() ) );
					auto radius = writer.declLocale( "radius"
						, ( cameraData.position().y() + atmosphereData.bottomRadius ) * 0.3_f );
					auto alpha = writer.declLocale( "alpha"
						, ( dist / radius ) );

					writer.returnStmt( ( 1.0_f - exp( -dist * alpha * factor ) ) );
				}
				, sdw::InVec3{ writer, "startPos" }
				, sdw::InFloat{ writer, "factor" } );
		}

		return m_computeFogAmount( pstartPos, pfactor );
	}

	sdw::RetBoolean CloudsModel::raySphereintersectSkyMap( sdw::Vec3 const & prd
		, sdw::Float const & pradius
		, sdw::Vec3 & pstartPos )
	{
		if ( !m_raySphereintersectSkyMap )
		{
			m_raySphereintersectSkyMap = writer.implementFunction< sdw::Boolean >( "raySphereintersectSkyMap"
				, [&]( sdw::Vec3 const & rd
					, sdw::Float const & radius
					, sdw::Vec3 startPos )
				{
					auto sphereCenter = writer.declLocale( "sphereCenter", vec3( 0.0_f ) );
					auto L = writer.declLocale( "L", -sphereCenter );
					auto a = writer.declLocale( "a", dot( rd, rd ) );
					auto b = writer.declLocale( "b", 2.0_f * dot( rd, L ) );
					auto c = writer.declLocale( "c", dot( L, L ) - ( radius * radius ) );
					auto delta = writer.declLocale( "delta", b * b - 4.0_f * a * c );
					auto t = writer.declLocale( "t", max( 0.0_f, ( -b + sqrt( delta ) ) / 2.0_f ) );
					startPos = rd * t;
					writer.returnStmt( sdw::Boolean{ true } );
				}
				, sdw::InVec3{ writer, "rd" }
				, sdw::InFloat{ writer, "radius" }
				, sdw::OutVec3{ writer, "startPos" } );
		}

		return m_raySphereintersectSkyMap( prd
			, pradius
			, pstartPos );
	}

	sdw::RetBoolean CloudsModel::raySphereIntersect( sdw::Vec3 const & pr0
		, sdw::Vec3 const & prd
		, sdw::Float const & pradius
		, sdw::Vec3 & pstartPos )
	{
		if ( !m_raySphereIntersect )
		{
			m_raySphereIntersect = writer.implementFunction< sdw::Boolean >( "raySphereIntersect"
				, [&]( sdw::Vec3 const & r0
					, sdw::Vec3 const & rd
					, sdw::Float const & radius
					, sdw::Vec3 startPos )
				{
					auto sphereCenter = writer.declLocale( "sphereCenter", vec3( cameraData.position().x()
						, -atmosphereData.bottomRadius
						, cameraData.position().z() ) );
					auto L = writer.declLocale( "L", r0 - sphereCenter );
					auto a = writer.declLocale( "a", dot( rd, rd ) );
					auto b = writer.declLocale( "b", 2.0_f * dot( rd, L ) );
					auto c = writer.declLocale( "c", dot( L, L ) - ( radius * radius ) );
					auto delta = writer.declLocale( "delta", b * b - 4.0_f * a * c );

					IF( writer, delta < 0.0_f )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					FI;

					auto t = writer.declLocale( "t", max( 0.0_f, ( -b + sqrt( delta ) ) / 2.0_f ) );

					IF( writer, t == 0.0_f )
					{
						writer.returnStmt( sdw::Boolean{ false } );
					}
					FI;

					startPos = r0 + rd * t;
					writer.returnStmt( sdw::Boolean{ true } );
				}
				, sdw::InVec3{ writer, "r0" }
				, sdw::InVec3{ writer, "rd" }
				, sdw::InFloat{ writer, "radius" }
				, sdw::OutVec3{ writer, "startPos" } );
		}

		return m_raySphereIntersect( pr0
			, prd
			, pradius
			, pstartPos );
	}

	//************************************************************************************************
}
