#include "AtmosphereScattering/AtmosphereVolumetricCloudsPass.hpp"

#include "AtmosphereScattering/Atmosphere.hpp"
#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/Scattering.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace weather
	{
		static constexpr bool useCompute = false;

		template< bool ComputeT >
		struct ShaderWriter;

		template<>
		struct ShaderWriter< false >
		{
			using Type = sdw::FragmentWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementMain( [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.fragCoord.xy() ) );
						func( fragCoord );
					} );
			}
		};

		template<>
		struct ShaderWriter< true >
		{
			using Type = sdw::ComputeWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementMain( [&]( sdw::ComputeIn in )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.globalInvocationID.xy() ) );
						func( fragCoord );
					} );
			}
		};

		enum Bindings : uint32_t
		{
			eAtmosphere,
			eWeather,
			eCamera,
			eTransmittance,
			eMultiScatter,
			eSkyView,
			eVolume,
			ePerlinWorley,
			eWorley,
			eWeatherMap,
			eOutColour,
			eOutEmission,
			eOutDistance,
			eCount,
		};

		static castor3d::ShaderPtr getVertexProgram()
		{
			sdw::VertexWriter writer;
			sdw::Vec2 position = writer.declInput< sdw::Vec2 >( "position", 0u );

			writer.implementMainT< sdw::VoidT, sdw::VoidT >( sdw::VertexIn{ writer }
				, sdw::VertexOut{ writer }
				, [&]( sdw::VertexIn in
					, sdw::VertexOut out )
				{
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static castor3d::ShaderPtr getProgram( VkExtent3D renderSize
			, VkExtent3D const & transmittanceExtent )
		{
			ShaderWriter< useCompute >::Type writer;

			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			C3D_AtmosphereWeather( writer
				, uint32_t( Bindings::eWeather )
				, 0u );
			C3D_Camera( writer
				, uint32_t( Bindings::eCamera )
				, 0u );
			auto transmittanceMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "transmittanceMap"
				, uint32_t( Bindings::eTransmittance )
				, 0u );
			auto multiScatterMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "multiScatterMap"
				, uint32_t( Bindings::eMultiScatter )
				, 0u );
			auto skyViewMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "skyViewMap"
				, uint32_t( Bindings::eSkyView )
				, 0u );
			auto volumeMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >( "volumeMap"
				, uint32_t( Bindings::eVolume )
				, 0u );
			auto cloudsMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >("cloudsMap"
				, uint32_t( Bindings::ePerlinWorley )
				, 0u );
			auto worleyMap = writer.declCombinedImg< sdw::CombinedImage3DRgba32 >("worleyMap"
				, uint32_t( Bindings::eWorley )
				, 0u );
			auto weatherMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >("weatherMap"
				, uint32_t( Bindings::eWeatherMap )
				, 0u );

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

			auto depthBufferValue = 1.0_f;
			auto sphereInnerRadius = c3d_atmosphereData.bottomRadius + c3d_weatherData.sphereInnerRadius();
			auto sphereOuterRadius = sphereInnerRadius + c3d_weatherData.sphereOuterRadius();
			auto sphereDelta = sphereOuterRadius - sphereInnerRadius;
			auto sphereCenter = vec3( 0.0_f, -c3d_atmosphereData.bottomRadius, 0.0_f );

			castor3d::shader::Utils utils{ writer };
			AtmosphereConfig atmosphere{ writer
				, utils
				, c3d_atmosphereData
				, { false, &c3d_cameraData, true, true }
				, { transmittanceExtent.width, transmittanceExtent.height }
				, &transmittanceMap };
			ScatteringConfig scattering{ writer
				, atmosphere
				, c3d_cameraData
				, c3d_atmosphereData
				, c3d_weatherData
				, false /*colorTransmittance*/
				, true /*fastSky*/
				, true /*fastAerialPerspective*/
				, true /*renderSunDisk*/
				, false /*bloomSunDisk*/ };

			auto getUVProjection = [&]( sdw::Vec3 const & p )
			{
				return p.xz() / sphereInnerRadius + 0.5_f;
			};

			auto getHeightFraction = [&]( sdw::Vec3 const & inPos )
			{
				return ( length( inPos - sphereCenter ) - sphereInnerRadius ) / sphereDelta;
			};

			auto getDensityForCloud = writer.implementFunction< sdw::Float >( "getDensityForCloud"
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

			auto sampleCloudDensity = writer.implementFunction< sdw::Float >( "sampleCloudDensity"
				, [&]( sdw::Vec3 const & p
					, sdw::Boolean const & expensive
					, sdw::Float const & lod )
				{
					auto cloudTopOffset = 0.75_f;

					auto heightFraction = writer.declLocale( "heightFraction"
						, getHeightFraction( p ) );
					auto animation = writer.declLocale( "animation"
						, heightFraction * c3d_weatherData.windDirection() * cloudTopOffset
							+ c3d_weatherData.windDirection() * c3d_weatherData.time() * c3d_weatherData.cloudSpeed() );
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
						, cloudsMap.lod( vec3( uv * c3d_weatherData.crispiness(), heightFraction ), lod ) );
					auto lowFreqFBM = writer.declLocale( "lowFreqFBM"
						, dot( lowFrequencyNoise.gba(), vec3( 0.625_f, 0.25_f, 0.125_f ) ) );
					auto baseCloud = writer.declLocale( "baseCloud"
						, utils.remap( lowFrequencyNoise.r(), -( 1.0_f - lowFreqFBM ), 1.0_f, 0.0_f, 1.0_f ) );

					auto density = writer.declLocale( "density"
						, getDensityForCloud( heightFraction, 1.0_f ) );
					baseCloud *= ( density / heightFraction );

					auto weatherData = writer.declLocale( "weatherData"
						, weatherMap.lod( movingUv, 0.0_f ).rgb() );
					auto cloudCoverage = writer.declLocale( "cloudCoverage"
						, weatherData.r() * c3d_weatherData.coverage() );
					auto baseCloudWithCoverage = writer.declLocale( "baseCloudWithCoverage"
						, utils.remap( baseCloud, cloudCoverage, 1.0_f, 0.0_f, 1.0_f ) );
					baseCloudWithCoverage *= cloudCoverage;

					IF( writer, expensive )
					{
						auto erodeCloudNoise = writer.declLocale( "erodeCloudNoise"
							, worleyMap.lod( vec3( movingUv * c3d_weatherData.crispiness(), heightFraction ) * c3d_weatherData.curliness(), lod ).rgb() );
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
			
			auto raymarchToLight = writer.implementFunction< sdw::Float >( "raymarchToLight"
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
						, -ds * c3d_weatherData.absorption() );
					auto pos = writer.declLocale( "pos"
						, vec3( 0.0_f ) );

					auto T = writer.declLocale( "erodeCloudNoise"
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
			
			auto raymarchToCloud = writer.implementFunction< sdw::Vec4 >( "raymarchToCloud"
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
						, dot( normalize( c3d_atmosphereData.sunDirection ), normalize( dir ) ) );

					auto T = writer.declLocale( "T"
						, 1.0_f );
					auto sigmaDs = writer.declLocale( "sigmaDs"
						, -ds * c3d_weatherData.density() );
					auto expensive = writer.declLocale( "expensive"
						, sdw::Boolean{ true } );
					auto entered = writer.declLocale( "entered"
						, sdw::Boolean{ false } );

					auto zeroDensitySample = writer.declLocale( "zeroDensitySample"
						, 0_i );

					FOR( writer, sdw::Int, i, 0_i, i < nSteps, ++i )
					{
						//IF( writer, pos.y() >= c3d_camera.position().y() - SPHERE_DELTA*1.5 ){

						auto densitySample = writer.declLocale( "densitySample"
							, sampleCloudDensity( pos
								, sdw::Boolean{ true }
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
								, c3d_weatherData.cloudColorBottom() ); //mix( CLOUDS_AMBIENT_COLOR_BOTTOM, CLOUDS_AMBIENT_COLOR_TOP, height );
							auto lightDensity = writer.declLocale( "lightDensity"
								, raymarchToLight( pos
									, ds * 0.1_f
									, c3d_atmosphereData.sunDirection
									, densitySample
									, lightDotEye ) );
							auto scattering = writer.declLocale( "scattering"
								, mix( atmosphere.hgPhase( -0.08_f, lightDotEye )
									, atmosphere.hgPhase( 0.08_f, lightDotEye )
									, clamp( sdw::fma( lightDotEye, 0.5_f, 0.5_f ), 0.0_f, 1.0_f ) ) );
							scattering = max( scattering, 1.0_f );
							auto powderTerm = writer.declLocale( "powderTerm"
								, utils.powder( densitySample ) );

							IF( writer, c3d_weatherData.enablePowder() == 0_i )
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
				
			auto computeFogAmount = writer.implementFunction< sdw::Float >( "computeFogAmount"
				, [&]( sdw::Vec3 const & startPos
					, sdw::Float const & factor )
				{
					auto dist = writer.declLocale( "dist"
						, length( startPos - c3d_cameraData.position() ) );
					auto radius = writer.declLocale( "radius"
						, ( c3d_cameraData.position().y() + c3d_atmosphereData.bottomRadius ) * 0.3_f );
					auto alpha = writer.declLocale( "alpha"
						, ( dist / radius ) );

					writer.returnStmt( ( 1.0_f - exp( -dist * alpha * factor ) ) );
				}
				, sdw::InVec3{ writer, "startPos" }
				, sdw::InFloat{ writer, "factor" } );

			auto raySphereintersectSkyMap = writer.implementFunction< sdw::Boolean >( "raySphereintersectSkyMap"
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

			auto raySphereIntersect = writer.implementFunction< sdw::Boolean >( "raySphereIntersect"
				, [&]( sdw::Vec3 const & r0
					, sdw::Vec3 const & rd
					, sdw::Float const & radius
					, sdw::Vec3 startPos )
				{
					auto sphereCenter = writer.declLocale( "sphereCenter", vec3( c3d_cameraData.position().x()
						, -c3d_atmosphereData.bottomRadius
						, c3d_cameraData.position().z() ) );
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

			auto applyClouds = writer.implementFunction< sdw::Void >( "applyClouds"
					, [&]( sdw::IVec2 const & fragCoord
						, sdw::Vec4 bg
						, sdw::Vec4 emission
						, sdw::Vec4 distance )
					{
						auto clipDir = writer.declLocale( "clipDir"
							, atmosphere.getClipSpace( vec2( fragCoord.xy() ), targetSize, 1.0_f ) );
						auto viewDir = writer.declLocale( "viewDir"
							, c3d_cameraData.camInvProj() * vec4( clipDir, 1.0_f ) );
						viewDir = vec4( viewDir.xy(), -1.0_f, 0.0_f );
						auto ray = writer.declLocale< Ray >( "ray" );
						ray.direction = normalize( ( c3d_cameraData.camInvView() * viewDir ).xyz() );
						ray.origin = c3d_cameraData.position() + vec3( 0.0_f, c3d_atmosphereData.bottomRadius, 0.0_f );
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
							raySphereIntersect( c3d_cameraData.position()
								, ray.direction
								, sphereInnerRadius
								, startPos );
							raySphereIntersect( c3d_cameraData.position()
								, ray.direction
								, sphereOuterRadius
								, endPos );
							fogRay = startPos;
						}
						ELSEIF( ray.origin.y() > sphereInnerRadius
							&& ray.origin.y() < sphereOuterRadius )
						{
							startPos = c3d_cameraData.position();
							raySphereIntersect( c3d_cameraData.position()
								, ray.direction
								, sphereOuterRadius
								, endPos );
							auto hit = writer.declLocale( "hit"
								, raySphereIntersect( c3d_cameraData.position()
									, ray.direction
									, sphereInnerRadius
									, fogRay ) );

							IF( writer, !hit )
							{
								fogRay = startPos;
							}
							FI;
						}
						ELSE
						{
							raySphereIntersect( c3d_cameraData.position()
								, ray.direction
								, sphereOuterRadius
								, startPos );
							raySphereIntersect( c3d_cameraData.position()
								, ray.direction
								, sphereInnerRadius
								, endPos );
							raySphereIntersect( c3d_cameraData.position()
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
						cloudDistance = vec4( sdw::distance( c3d_cameraData.position(), cloudDistance.xyz() ), 0.0_f, 0.0_f, 0.0_f );

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
							, clamp( dot( c3d_atmosphereData.sunDirection, normalize( endPos - startPos ) )
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
					, sdw::InOutVec4{ writer, "bg" }
					, sdw::OutVec4{ writer, "emission" }
					, sdw::OutVec4{ writer, "distance" } );

			if constexpr ( useCompute )
			{
				auto outColour = writer.declStorageImg< sdw::WImage2DRgba32 >( "outColour"
					, uint32_t( Bindings::eOutColour )
					, 0u );
				auto outEmission = writer.declStorageImg< sdw::WImage2DRgba32 >("outEmission"
					, uint32_t( Bindings::eOutEmission )
					, 0u );
				auto outDistance = writer.declStorageImg< sdw::WImage2DRgba32 >("outDistance"
					, uint32_t( Bindings::eOutDistance )
					, 0u );

				ShaderWriter< useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & fragCoord )
					{
						auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
						auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
						scattering.getPixelTransLum( vec2( fragCoord.xy() )
							, targetSize
							, depthBufferValue
							, transmittanceMap
							, skyViewMap
							, volumeMap
							, transmittance
							, luminance );
						luminance = scattering.rescaleLuminance( luminance );
						auto distance = writer.declLocale( "distance", vec4( 0.0_f ) );
						auto emission = writer.declLocale( "emission", vec4( 0.0_f ) );
						applyClouds( fragCoord
							, luminance
							, emission
							, distance );
						outColour.store( fragCoord, luminance );
						outDistance.store( fragCoord, distance );
						outEmission.store( fragCoord, emission );
					} );
			}
			else
			{
				auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", 0u );
				auto outEmission = writer.declOutput< sdw::Vec4 >( "outEmission", 1u );
				auto outDistance = writer.declOutput< sdw::Vec4 >( "outDistance", 2u );

				ShaderWriter< useCompute >::implementMain( writer
					, [&]( sdw::IVec2 const & fragCoord )
					{
						auto luminance = writer.declLocale< sdw::Vec4 >( "luminance" );
						auto transmittance = writer.declLocale< sdw::Vec4 >( "transmittance" );
						scattering.getPixelTransLum( vec2( fragCoord.xy() )
							, targetSize
							, depthBufferValue
							, transmittanceMap
							, skyViewMap
							, volumeMap
							, transmittance
							, luminance );
						luminance = scattering.rescaleLuminance( luminance );
						auto distance = writer.declLocale( "distance", vec4( 0.0_f ) );
						auto emission = writer.declLocale( "emission", vec4( 0.0_f ) );
						applyClouds( fragCoord
							, luminance
							, emission
							, distance );
						outColour = luminance;
						outDistance = distance;
						outEmission = emission;
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereVolumetricCloudsPass::AtmosphereVolumetricCloudsPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CameraUbo const & cameraUbo
		, AtmosphereWeatherUbo const & weatherUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & multiscatter
		, crg::ImageViewId const & skyview
		, crg::ImageViewId const & volume
		, crg::ImageViewId const & perlinWorley
		, crg::ImageViewId const & worley
		, crg::ImageViewId const & weather
		, crg::ImageViewId const & colourResult
		, crg::ImageViewId const & emissionResult
		, crg::ImageViewId const & distanceResult
		, uint32_t index )
		: castor::Named{ "VolumetricCloudsPass" + castor::string::toString( index ) }
		, m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT
			, getName()
			, ( weather::useCompute 
				? weather::getProgram( getExtent( colourResult ), getExtent( transmittance ) )
				: nullptr) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, getName()
			, ( weather::useCompute 
				? nullptr
				: weather::getVertexProgram() ) }
		, m_fragmentShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, getName()
			, ( weather::useCompute
				? nullptr
				: weather::getProgram( getExtent( colourResult ), getExtent( transmittance ) ) ) }
		, m_stages{ ( weather::useCompute
			? ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_computeShader ) }
			: ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( device, m_vertexShader )
				, makeShaderState( device, m_fragmentShader ) } ) }
	{
		auto renderSize = getExtent( colourResult );
		auto & pass = graph.createPass( getName()
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				crg::RunnablePassPtr result;

				if constexpr ( weather::useCompute )
				{
					result = std::make_unique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.groupCountX( renderSize.width / 16u )
							.groupCountY( renderSize.height / 16u )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				}
				else
				{
					result = crg::RenderQuadBuilder{}
						.renderSize( { renderSize.width, renderSize.height } )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
						.build( framePass, context, graph );
				}

				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		atmosphereUbo.createPassBinding( pass
			, weather::eAtmosphere );
		weatherUbo.createPassBinding( pass
			, weather::eWeather );
		cameraUbo.createPassBinding( pass
			, weather::eCamera );
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		crg::SamplerDesc mipLinearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		pass.addSampledView( transmittance
			, weather::eTransmittance
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( multiscatter
			, weather::eMultiScatter
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( skyview
			, weather::eSkyView
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( volume
			, weather::eVolume
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, linearSampler );
		pass.addSampledView( perlinWorley
			, weather::ePerlinWorley
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, mipLinearSampler );
		pass.addSampledView( worley
			, weather::eWorley
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, mipLinearSampler );
		pass.addSampledView( weather
			, weather::eWeatherMap
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, crg::SamplerDesc{ VK_FILTER_LINEAR
				, VK_FILTER_LINEAR
				, VK_SAMPLER_MIPMAP_MODE_NEAREST
				, VK_SAMPLER_ADDRESS_MODE_REPEAT
				, VK_SAMPLER_ADDRESS_MODE_REPEAT } );

		if constexpr ( weather::useCompute )
		{
			pass.addOutputStorageView( colourResult
				, weather::eOutColour );
			pass.addOutputStorageView( emissionResult
				, weather::eOutEmission );
			pass.addOutputStorageView( distanceResult
				, weather::eOutDistance );
		}
		else
		{
			pass.addOutputColourView( colourResult );
			pass.addOutputColourView( emissionResult );
			pass.addOutputColourView( distanceResult );
		}

		m_lastPass = &pass;
	}

	void AtmosphereVolumetricCloudsPass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
