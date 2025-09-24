#include "AtmosphereScattering/CloudsVolumePass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsModel.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace volclouds
	{
		enum class Bindings : uint32_t
		{
			eAtmosphere,
			eClouds,
			eCamera,
			eTransmittance,
			eMultiScatter,
			eSkyView,
			eVolume,
			ePerlinWorley,
			eWorley,
			eCurl,
			eWeatherMap,
			eDepthMap,
			eOutSky,
			eOutSun,
			eOutClouds,
			eCount,
		};

		static constexpr bool useCompute = false;

		template< bool ComputeT >
		struct ShaderWriter;

		template<>
		struct ShaderWriter< false >
		{
			using Type = sdw::TraditionalGraphicsWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementEntryPointT< c3ds::Position2FT, sdw::VoidT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
					, sdw::VertexOut out )
					{
						out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					} );

				auto outSky = writer.declOutput< sdw::Vec4 >( "outSky", sdw::EntryPoint::eFragment, 0u );
				auto outSun = writer.declOutput< sdw::Vec4 >( "outSun", sdw::EntryPoint::eFragment, 1u );
				auto outClouds = writer.declOutput< sdw::Vec4 >( "outClouds", sdw::EntryPoint::eFragment, 2u );

				writer.implementEntryPoint( [&writer, &func, &outSky, &outSun, &outClouds]( sdw::FragmentIn const & in
					, sdw::FragmentOut const & )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, vec2( in.fragCoord.xy() ) );
						auto sky = writer.declLocale( "sky"
							, vec4( 0.0_f ) );
						auto sun = writer.declLocale( "sun"
							, vec4( 0.0_f ) );
						auto clouds = writer.declLocale( "clouds"
							, vec4( 0.0_f ) );
						func( fragCoord, sky, sun, clouds );

						outSky = sky;
						outSun = sun;
						outClouds = clouds;
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
				auto outSky = writer.declStorageImg< sdw::WImage2DRgba32 >( "outSky"
					, uint32_t( Bindings::eOutSky )
					, 0u );
				auto outSun = writer.declStorageImg< sdw::WImage2DRgba32 >( "outSun"
					, uint32_t( Bindings::eOutSun )
					, 0u );
				auto outClouds = writer.declStorageImg< sdw::WImage2DRgba32 >( "outClouds"
					, uint32_t( Bindings::eOutClouds )
					, 0u );

				writer.implementMain( [&writer, &func, &outSky, &outSun, &outClouds]( sdw::ComputeIn const & in )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, vec2( in.globalInvocationID.xy() ) );
						auto sky = writer.declLocale( "sky"
							, vec4( 0.0_f ) );
						auto sun = writer.declLocale( "sun"
							, vec4( 0.0_f ) );
						auto clouds = writer.declLocale( "clouds"
							, vec4( 0.0_f ) );
						auto ifragCoord = func( fragCoord, sky, sun, clouds );

						outSky.store( ifragCoord, sky );
						outSun.store( ifragCoord, sun );
						outClouds.store( ifragCoord, clouds );
					} );
			}
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D renderSize
			, c3d::Extent3D const & transmittanceExtent
			, bool hasDepth )
		{
			ShaderWriter< useCompute >::Type writer{ &engine.getShaderAllocator() };

			C3D_AtmosphereScattering( writer
				, Bindings::eAtmosphere
				, 0u );
			C3D_Clouds( writer
				, Bindings::eClouds
				, 0u );
			ATM_Camera( writer
				, Bindings::eCamera
				, 0u );

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

			auto depthBufferValue = 0.0_f;

			c3d::shader::Utils utils{ writer };
			AtmosphereModel atmosphere{ writer
				, c3d_atmosphereData
				, AtmosphereModel::Settings{ c3d::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
					.setCameraData( &atm_cameraData )
					.setVariableSampleCount( true )
					.setMieRayPhase( true )
					.setMultiScatApprox( true )
				, { transmittanceExtent.width, transmittanceExtent.height } };
			auto binding = uint32_t( Bindings::eTransmittance );
			ScatteringModel scattering{ writer
				, atmosphere
				, ScatteringModel::Settings{}
					.setNeedsMultiscatter( true )
					.setBloomSunDisk( true )
				, binding
				, 0u };
			binding = uint32_t( Bindings::ePerlinWorley );
			CloudsModel clouds{ writer
				, utils
				, atmosphere
				, scattering
				, c3d_cloudsData
				, binding
				, 0u };
			auto depthMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "depthMap"
				, Bindings::eDepthMap
				, 0u
				, hasDepth ) };

			ShaderWriter< useCompute >::implementMain( writer
				, [&writer, &scattering, &targetSize, &depthBufferValue, &c3d_cloudsData, &depthMap, &atmosphere, &clouds
					, &hasDepth]( sdw::Vec2 const & fragCoord
					, sdw::Vec4 & skyColor
					, sdw::Vec4 & sunColor
					, sdw::Vec4 & cloudsColor )
				{
					auto ifragCoord = writer.declLocale( "ifragCoord"
						, ivec2( fragCoord ) );
					auto transmittance = writer.declLocale( "transmittance"
						, vec4( 0.0_f ) );
					auto ray = writer.declLocale( "ray"
						, scattering.getPixelTransLum( fragCoord
							, targetSize
							, depthBufferValue
							, transmittance
							, skyColor ) );

					auto skyLuminance = writer.declLocale( "skyLuminance"
						, scattering.rescaleLuminance( skyColor ).rgb() );
					auto sunLuminance = writer.declLocale( "sunLuminance"
						, scattering.getSunLuminance( ray ) );
					auto skyBlendFactor = writer.declLocale( "skyBlendFactor"
						, 0.0_f );

					sdwIF( writer, c3d_cloudsData.coverage() > 0.0_f )
					{
						auto sceneUv = writer.declLocale( "sceneUv"
							, fragCoord / targetSize );
						auto depthObj = writer.declLocale( "depthObj"
							, hasDepth ? depthMap.lod( vec2( sceneUv.x(), 1.0_f - sceneUv.y() ), 0.0_f ) : vec4( -1.0_f, -1.0_f, 0.0_f, 0.0_f ) );

						if ( hasDepth )
						{
							depthObj.g() *= atmosphere.settings.length.kilometres();
						}

						cloudsColor = clouds.applyClouds( ray
							, depthObj.b()
							, depthObj.g()
							, ifragCoord
							, sunLuminance
							, skyLuminance
							, skyBlendFactor );
					}
					sdwFI

					skyColor = vec4( skyLuminance, skyBlendFactor );
					sunColor = vec4( sunLuminance, 1.0_f );
					return ifragCoord;
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	CloudsVolumePass::CloudsVolumePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CameraUbo const & cameraUbo
		, CloudsUbo const & cloudsUbo
		, c3d::Texture const & transmittance
		, c3d::Texture const & multiscatter
		, c3d::Texture const & skyview
		, c3d::Texture const & volume
		, c3d::Texture const & perlinWorley
		, c3d::Texture const & worley
		, c3d::Texture const & curl
		, c3d::Texture const & weather
		, c3d::Texture const * depthObj
		, c3d::Texture & skyResult
		, c3d::Texture & sunResult
		, c3d::Texture & cloudsResult
		, uint32_t index )
		: c3d::Named{ cuT( "Clouds/VolumePass" ) + c3d::string::toString( index ) }
		, m_shader{ getName(), volclouds::getProgram( *device.renderSystem.getEngine(), skyResult.getExtent(), transmittance.getExtent(), depthObj != nullptr ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto renderSize = skyResult.getExtent();
		auto & pass = graph.createPass( c3d::toUtf8( getName() )
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				crg::RunnablePassPtr result;

				if constexpr ( volclouds::useCompute )
				{
					result = c3d::makeRawUnique< crg::ComputePass >( framePass
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

				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		atmosphereUbo.createPassBinding( pass, volclouds::Bindings::eAtmosphere );
		cloudsUbo.createPassBinding( pass, volclouds::Bindings::eClouds );
		cameraUbo.createPassBinding( pass, volclouds::Bindings::eCamera );
		crg::SamplerDesc linearClampSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear };
		crg::SamplerDesc linearRepeatSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eNearest
			, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat };
		crg::SamplerDesc mipLinearSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear, c3d::MipmapMode::eLinear
			, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat, c3d::WrapMode::eRepeat };
		pass.addInputSampledT( *transmittance.getSampledLastAttach(), volclouds::Bindings::eTransmittance, linearClampSampler );
		pass.addInputSampledT( *multiscatter.getSampledLastAttach(), volclouds::Bindings::eMultiScatter, linearClampSampler );
		pass.addInputSampledT( *skyview.getSampledLastAttach(), volclouds::Bindings::eSkyView, linearClampSampler );
		pass.addInputSampledT( *volume.getSampledLastAttach(), volclouds::Bindings::eVolume, linearClampSampler );
		pass.addInputSampledT( *perlinWorley.getSampledLastAttach(), volclouds::Bindings::ePerlinWorley, mipLinearSampler );
		pass.addInputSampledT( *worley.getSampledLastAttach(), volclouds::Bindings::eWorley, mipLinearSampler );
		pass.addInputSampledT( *curl.getSampledLastAttach(), volclouds::Bindings::eCurl, linearRepeatSampler );
		pass.addInputSampledT( *weather.getSampledLastAttach(), volclouds::Bindings::eWeatherMap, linearRepeatSampler );

		if ( depthObj )
			pass.addInputSampledT( *depthObj->getSampledLastAttach(), volclouds::Bindings::eDepthMap, linearClampSampler );

		if constexpr ( volclouds::useCompute )
		{
			skyResult.setLastAttach( pass.addOutputStorageImageT( skyResult.getTargetViewId(), volclouds::Bindings::eOutSky ) );
			sunResult.setLastAttach( pass.addOutputStorageImageT( sunResult.getTargetViewId(), volclouds::Bindings::eOutSun ) );
			cloudsResult.setLastAttach( pass.addOutputStorageImageT( cloudsResult.getTargetViewId(), volclouds::Bindings::eOutClouds ) );
		}
		else
		{
			skyResult.setLastAttach( pass.addOutputColourTarget( skyResult.getTargetViewId() ) );
			sunResult.setLastAttach( pass.addOutputColourTarget( sunResult.getTargetViewId() ) );
			cloudsResult.setLastAttach( pass.addOutputColourTarget( cloudsResult.getTargetViewId() ) );
		}
	}

	void CloudsVolumePass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}

	//************************************************************************************************
}
