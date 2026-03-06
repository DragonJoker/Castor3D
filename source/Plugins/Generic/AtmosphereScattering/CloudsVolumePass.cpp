#include "AtmosphereScattering/CloudsVolumePass.hpp"

#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/CloudsModel.hpp"
#include "AtmosphereScattering/CloudsVolumePlugin.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Render/Volumetric/VolumeComponentRegister.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>

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
			eOutScattering,
			eOutTransmittance,
			eMainCamera,
			eDepthMap,
			eAtmosphere,
			eClouds,
			eKmCamera,
			eTransmittance,
			eMultiScatter,
			eSkyView,
			eVolume,
			ePerlinWorley,
			eWorley,
			eCurl,
			eWeatherMap,
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

				auto outScattering = writer.declOutput< sdw::Vec4 >( "outScattering", sdw::EntryPoint::eFragment, 0u );
				auto outTransmittance = writer.declOutput< sdw::Vec4 >( "outTransmittance", sdw::EntryPoint::eFragment, 1u );

				writer.implementEntryPoint( [&writer, &func, &outScattering, &outTransmittance]( sdw::FragmentIn const & in
					, sdw::FragmentOut const & )
					{
						auto pixelCoord = writer.declLocale( "pixelCoord"
							, in.fragCoord.xy() );
						auto scattering = writer.declLocale( "scattering"
							, vec4( 0.0_f ) );
						auto transmittance = writer.declLocale( "transmittance"
							, vec4( 0.0_f ) );
						func( pixelCoord, scattering, transmittance );

						outScattering = scattering;
						outTransmittance = transmittance;
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
				auto outScattering = writer.declStorageImg< sdw::WImage2DRgba32 >( "outScattering"
					, uint32_t( Bindings::eOutScattering )
					, 0u );
				auto outTransmittance = writer.declStorageImg< sdw::WImage2DRgba32 >( "outTransmittance"
					, uint32_t( Bindings::eOutTransmittance )
					, 0u );

				writer.implementMain( [&writer, &func, &outScattering, &outTransmittance]( sdw::ComputeIn const & in )
					{
						auto pixelCoord = writer.declLocale( "pixelCoord"
							, vec2( in.globalInvocationID.xy() ) );
						auto scattering = writer.declLocale( "scattering"
							, vec4( 0.0_f ) );
						auto transmittance = writer.declLocale( "transmittance"
							, vec4( 0.0_f ) );
						func( pixelCoord, scattering, transmittance );

						auto ifragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.globalInvocationID.xy() ) );
						outScattering.store( ifragCoord, scattering );
						outTransmittance.store( ifragCoord, transmittance );
					} );
			}
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D renderSize
			, c3d::Extent3D const & transmittanceExtent
			, bool hasDepth )
		{
			ShaderWriter< useCompute >::Type writer{ &engine.getShaderAllocator() };

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
			C3D_Camera( writer
				, Bindings::eMainCamera
				, 0u );

			if constexpr ( useUnified )
			{
				auto binding = uint32_t( Bindings::eDepthMap );
				c3ds::VolumeShaders volumeShaders{ writer
					, engine.getVolumeComponentsRegister(), { renderSize.width, renderSize.height }, hasDepth
					, ( 0x00000001u << engine.getVolumeComponentsRegister().getNameId( CloudsVolumePlugin::TypeName ) )
					, binding };

				ShaderWriter< useCompute >::implementMain( writer
					, [&writer, &volumeShaders, &targetSize, &c3d_cameraData]( sdw::Vec2 const & fragCoord
						, sdw::Vec4 & outScattering
						, sdw::Vec4 & outTransmittance )
					{
						c3ds::Volumes volumes{ writer, volumeShaders };
						volumeShaders.registerVolumeTypes( volumes );

						auto traversalResult = writer.declLocale( "traversalResult"
							, c3ds::VolumesTraversalResult{ writer, volumeShaders, targetSize } );

						auto ray = writer.declLocale( "ray"
							, c3d_cameraData.castRay( fragCoord, targetSize ) );
						volumeShaders.initialise( fragCoord, ray, volumes, traversalResult );

						auto i = writer.declLocale( "i", 0_u );
						auto dt = writer.declLocale( "dt", 0.0_f );
						sdwWHILE( writer, i < volumes.size() && traversalResult.transmittanceAboveThreshold )
						{
							auto volume = writer.declLocale( "volume", volumes[i] );
							auto t = writer.declLocale( "t", volume.begin );

							sdwFOR( writer, sdw::Float, sample, 0.0_f, sample < volume.sampleCount && traversalResult.transmittanceAboveThreshold, sample += 1.0_f )
							{
								volumes.step( volume, ray, sample, t, dt, traversalResult );
								volumes.traverse( volume, ray, sample, t, dt, traversalResult );
							}
							sdwROF

							++i;
						}
						sdwELIHW

						volumeShaders.finalise( ray, traversalResult );

						outScattering = vec4( traversalResult.inscatter, 1.0_f );
						outTransmittance = vec4( traversalResult.transmittance, 1.0_f );
					} );
			}
			else
			{
				auto transmittanceThreshold = writer.declConstant( "transmittanceThreshold"
					, 0.01_f );
				auto depthMap{ writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "depthMap"
					, Bindings::eDepthMap
					, 0u
					, hasDepth ) };
				C3D_AtmosphereScattering( writer
					, Bindings::eAtmosphere
					, 0u );
				C3D_Clouds( writer
					, Bindings::eClouds
					, 0u );
				ATM_Camera( writer
					, Bindings::eKmCamera
					, 0u );

				c3d::shader::Utils utils{ writer };
				AtmosphereModel atmosphere{ writer, c3d_atmosphereData
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

				auto depthBufferValue = 0.0_f;

				// For reference
				ShaderWriter< useCompute >::implementMain( writer
					, [&writer, &scattering, &targetSize, &depthBufferValue, &c3d_cloudsData, &depthMap, &atmosphere, &clouds
						, &hasDepth]( sdw::Vec2 const & fragCoord
							, sdw::Vec4 & outScattering
							, sdw::Vec4 & outTransmittance )
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
								, outScattering ) );

						auto skyLuminance = writer.declLocale( "skyLuminance"
							, scattering.rescaleLuminance( outScattering ).rgb() );
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
								depthObj.g() *= atmosphere.settings.length.kilometres();
							outTransmittance = clouds.applyClouds( ray
								, depthObj.b()
								, depthObj.g()
								, ifragCoord
								, sunLuminance
								, skyLuminance
								, skyBlendFactor );
						}
						sdwFI

						outScattering = vec4( skyLuminance, skyBlendFactor );
						return ifragCoord;
					} );
			}

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	CloudsVolumePass::CloudsVolumePass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, AtmosphereScatteringUbo const & atmosphereUbo
		, c3d::CameraUbo const & mainCameraUbo
		, CameraUbo const & kmCameraUbo
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
		, c3d::Texture & scatteringResult
		, c3d::Texture & transmittanceResult
		, uint32_t index )
		: c3d::Named{ cuT( "Clouds/VolumePass" ) + c3d::string::toString( index ) }
		, m_shader{ getName(), volclouds::getProgram( c3d::getEngine( device ), scatteringResult.getExtent(), transmittance.getExtent(), depthObj != nullptr ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto renderSize = scatteringResult.getExtent();
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

				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		atmosphereUbo.createPassBinding( pass, volclouds::Bindings::eAtmosphere );
		cloudsUbo.createPassBinding( pass, volclouds::Bindings::eClouds );
		mainCameraUbo.createPassBinding( pass, volclouds::Bindings::eMainCamera );
		kmCameraUbo.createPassBinding( pass, volclouds::Bindings::eKmCamera );
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
			scatteringResult.setLastAttach( pass.addOutputStorageImageT( scatteringResult.getTargetViewId(), volclouds::Bindings::eOutScattering ) );
			transmittanceResult.setLastAttach( pass.addOutputStorageImageT( transmittanceResult.getTargetViewId(), volclouds::Bindings::eOutTransmittance ) );
		}
		else
		{
			scatteringResult.setLastAttach( pass.addOutputColourTarget( scatteringResult.getTargetViewId() ) );
			transmittanceResult.setLastAttach( pass.addOutputColourTarget( transmittanceResult.getTargetViewId() ) );
		}
	}

	void CloudsVolumePass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}

	//************************************************************************************************
}
