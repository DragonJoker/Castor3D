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
		enum Bindings : uint32_t
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
				writer.implementEntryPointT< c3d::Position2FT, sdw::VoidT >( [&]( sdw::VertexInT< c3d::Position2FT > in
					, sdw::VertexOut out )
					{
						out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					} );

				auto outSky = writer.declOutput< sdw::Vec4 >( "outSky", sdw::EntryPoint::eFragment, 0u );
				auto outSun = writer.declOutput< sdw::Vec4 >( "outSun", sdw::EntryPoint::eFragment, 1u );
				auto outClouds = writer.declOutput< sdw::Vec4 >( "outClouds", sdw::EntryPoint::eFragment, 2u );

				writer.implementEntryPoint( [&]( sdw::FragmentIn in
					, sdw::FragmentOut out )
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

				writer.implementMain( [&]( sdw::ComputeIn in )
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

		static castor3d::ShaderPtr getProgram( castor3d::Engine & engine
			, VkExtent3D renderSize
			, VkExtent3D const & transmittanceExtent
			, bool hasDepth )
		{
			ShaderWriter< useCompute >::Type writer{ &engine.getShaderAllocator() };

			C3D_AtmosphereScattering( writer
				, uint32_t( Bindings::eAtmosphere )
				, 0u );
			C3D_Clouds( writer
				, uint32_t( Bindings::eClouds )
				, 0u );
			ATM_Camera( writer
				, uint32_t( Bindings::eCamera )
				, 0u );

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

			auto depthBufferValue = 0.0_f;

			castor3d::shader::Utils utils{ writer };
			AtmosphereModel atmosphere{ writer
				, c3d_atmosphereData
				, AtmosphereModel::Settings{ castor::Length::fromUnit( 1.0f, engine.getLengthUnit() ) }
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
				, [&]( sdw::Vec2 const & fragCoord
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

					IF( writer, c3d_cloudsData.coverage() > 0.0_f )
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
					FI;

					skyColor = vec4( skyLuminance, skyBlendFactor );
					sunColor = vec4( sunLuminance, 1.0_f );
					return ifragCoord;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	CloudsVolumePass::CloudsVolumePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, CameraUbo const & cameraUbo
		, CloudsUbo const & cloudsUbo
		, crg::ImageViewId const & transmittance
		, crg::ImageViewId const & multiscatter
		, crg::ImageViewId const & skyview
		, crg::ImageViewId const & volume
		, crg::ImageViewId const & perlinWorley
		, crg::ImageViewId const & worley
		, crg::ImageViewId const & curl
		, crg::ImageViewId const & weather
		, crg::ImageViewId const * depthObj
		, crg::ImageViewId const & skyResult
		, crg::ImageViewId const & sunResult
		, crg::ImageViewId const & cloudsResult
		, uint32_t index )
		: castor::Named{ "Clouds/VolumePass" + castor::string::toString( index ) }
		, m_shader{ getName(), volclouds::getProgram( *device.renderSystem.getEngine(), getExtent( skyResult ), getExtent( transmittance ), depthObj != nullptr ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto renderSize = getExtent( skyResult );
		auto & pass = graph.createPass( getName()
			, [this, &device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				crg::RunnablePassPtr result;

				if constexpr ( volclouds::useCompute )
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
			, volclouds::eAtmosphere );
		cloudsUbo.createPassBinding( pass
			, volclouds::eClouds );
		cameraUbo.createPassBinding( pass
			, volclouds::eCamera );
		crg::SamplerDesc linearClampSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR };
		crg::SamplerDesc linearRepeatSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		crg::SamplerDesc mipLinearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT };
		pass.addSampledView( transmittance
			, volclouds::eTransmittance
			, linearClampSampler );
		pass.addSampledView( multiscatter
			, volclouds::eMultiScatter
			, linearClampSampler );
		pass.addSampledView( skyview
			, volclouds::eSkyView
			, linearClampSampler );
		pass.addSampledView( volume
			, volclouds::eVolume
			, linearClampSampler );
		pass.addSampledView( perlinWorley
			, volclouds::ePerlinWorley
			, mipLinearSampler );
		pass.addSampledView( worley
			, volclouds::eWorley
			, mipLinearSampler );
		pass.addSampledView( curl
			, volclouds::eCurl
			, linearRepeatSampler );
		pass.addSampledView( weather
			, volclouds::eWeatherMap
			, linearRepeatSampler );

		if ( depthObj )
		{
			pass.addSampledView( *depthObj
				, volclouds::eDepthMap
				, linearClampSampler );
		}

		if constexpr ( volclouds::useCompute )
		{
			pass.addOutputStorageView( skyResult
				, volclouds::eOutSky );
			pass.addOutputStorageView( sunResult
				, volclouds::eOutSun );
			pass.addOutputStorageView( cloudsResult
				, volclouds::eOutClouds );
		}
		else
		{
			pass.addOutputColourView( skyResult );
			pass.addOutputColourView( sunResult );
			pass.addOutputColourView( cloudsResult );
		}

		m_lastPass = &pass;
	}

	void CloudsVolumePass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}

	//************************************************************************************************
}
