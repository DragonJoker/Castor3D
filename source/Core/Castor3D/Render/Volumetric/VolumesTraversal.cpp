#include "Castor3D/Render/Volumetric/VolumesTraversal.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/Volumetric/VolumeComponentRegister.hpp>
#include <Castor3D/Render/Volumetric/VolumetricRendering.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslRay.hpp>
#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/ComputeWriter.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace voltrv
	{
		enum class Bindings : uint32_t
		{
			eOutTransmittance,
			eOutScattering,
			eCamera,
			eDepthMap,
			eCount,
		};

		static constexpr bool useCompute = false;

		template< bool ComputeT >
		struct ShaderWriter;

		template<>
		struct ShaderWriter< false >
		{
			using Writer = sdw::TraditionalGraphicsWriter;

			template< typename FuncT >
			static void implementMain( Writer & writer, FuncT func )
			{
				writer.implementEntryPointT< shader::Position2FT, sdw::VoidT >( []( sdw::VertexInT< shader::Position2FT > const & in
					, sdw::VertexOut out )
					{
						out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					} );

				auto outTransmittance = writer.declOutput< sdw::Vec4 >( "outTransmittance", sdw::EntryPoint::eFragment, 0u );
				auto outScattering = writer.declOutput< sdw::Vec4 >( "outScattering", sdw::EntryPoint::eFragment, 1u );

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

						outTransmittance = transmittance;
						outScattering = scattering;
					} );
			}
		};

		template<>
		struct ShaderWriter< true >
		{
			using Writer = sdw::ComputeWriter;

			template< typename FuncT >
			static void implementMain( Writer & writer, FuncT func )
			{
				auto outTransmittance = writer.declStorageImg< sdw::WImage2DRgba32 >( "outTransmittance"
					, uint32_t( Bindings::eOutTransmittance )
					, 0u );
				auto outScattering = writer.declStorageImg< sdw::WImage2DRgba32 >( "outScattering"
					, uint32_t( Bindings::eOutScattering )
					, 0u );

				writer.implementMain( 16u, 16u
					, [&writer, &func, &outScattering, &outTransmittance]( sdw::ComputeIn const & in )
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
						outTransmittance.store( ifragCoord, transmittance );
						outScattering.store( ifragCoord, scattering );
					} );
			}
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D renderSize
			, bool hasDepth )
		{
			ShaderWriter< useCompute >::Writer writer{ &engine.getShaderAllocator() };

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );
			C3D_Camera( writer, Bindings::eCamera, 0u );

			auto binding = uint32_t( Bindings::eDepthMap );
			shader::VolumeShaders volumeShaders{ writer, engine.getVolumeComponentsRegister()
				, { renderSize.width, renderSize.height }, hasDepth, 0xFFFFFFFFu, binding };

			ShaderWriter< useCompute >::implementMain( writer
				, [&writer, &volumeShaders, &targetSize, &c3d_cameraData]( sdw::Vec2 const & fragCoord
					, sdw::Vec4 & outScattering
					, sdw::Vec4 & outTransmittance )
				{
					shader::Volumes volumes{ writer, volumeShaders };
					volumeShaders.registerVolumeTypes( volumes );

					auto traversalResult = writer.declLocale( "traversalResult"
						, shader::VolumesTraversalResult{ writer, volumeShaders, targetSize } );

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

			return writer.getBuilder().releaseShader();
		}

		//************************************************************************************************

		class ComputePass
			: public crg::ComputePass
		{
		public:
			ComputePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, Extent3D const & extent
				, crg::cp::Config config )
				: crg::ComputePass{framePass
					, context
					, graph
					, crg::ru::Config{ 1u }
					, config
						.programCreator( { 1u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } ) }
				, m_device{ device }
				, m_extent{ extent }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ShaderModule shaderModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "TraverseVolumes" ), voltrv::getProgram( getEngine( m_device ), m_extent, true ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, program.shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

		private:
			RenderDevice const & m_device;
			Extent3D m_extent;
			Map< uint32_t, ProgramData > m_programs;
		};

		//************************************************************************************************

		class RenderPass
			: public crg::RenderQuad
		{
		public:
			RenderPass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, Extent3D const & extent )
				: crg::RenderQuad{ framePass, context, graph
					, crg::ru::Config{}
					, crg::rq::Config{}
						.programCreator( { 1u, [this]( uint32_t passIndex ){ return doCreateProgram( passIndex ); } } )
						.renderSize( { extent.width, extent.height } ) }
				, m_device{ device }
				, m_extent{ extent }
			{
			}

		private:
			struct ProgramData
			{
				ProgramData() = default;
				ProgramModule programModule{};
				ashes::PipelineShaderStageCreateInfoArray stages{};
			};

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram( uint32_t passIndex )
			{
				auto [it, res] = m_programs.try_emplace( passIndex );

				if ( res )
				{
					auto & program = it->second;
					program.programModule = ProgramModule{ cuT( "TraverseVolumes" ), voltrv::getProgram( getEngine( m_device ), m_extent, true ) };
					program.stages = ashes::PipelineShaderStageCreateInfoArray{ makeProgramStates( m_device, program.programModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( it->second.stages );
			}

		private:
			RenderDevice const & m_device;
			Extent3D m_extent;
			Map< uint32_t, ProgramData > m_programs;
		};
	}

	//************************************************************************************************

	void createVolumesTraversalPass( crg::FramePassGroup & graph, RenderDevice const & device
		, Camera const & camera, VolumetricRendering const & volumetric, CameraUbo const & cameraUbo
		, Texture & transmittance, Texture & inscatter )
	{
		auto & engine = c3d::getEngine( device );
		auto renderSize = transmittance.getExtent();
		auto & pass = graph.createPass( "TraverseVolumes"
			, [&device, &engine, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				crg::RunnablePassPtr result;

				if constexpr ( voltrv::useCompute )
				{
					result = c3d::makeRawUnique< voltrv::ComputePass >( framePass, context, graph
						, device, renderSize
						, crg::cp::Config{}
							.groupCountX( renderSize.width / 16u )
							.groupCountY( renderSize.height / 16u ) );
				}
				else
				{
					result = c3d::makeRawUnique< voltrv::RenderPass >( framePass, context, graph
						, device, renderSize );
				}

				engine.registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );

		if constexpr ( voltrv::useCompute )
		{
			transmittance.setLastAttach( pass.addOutputStorageImageT( transmittance.getTargetViewId(), voltrv::Bindings::eOutTransmittance ) );
			inscatter.setLastAttach( pass.addOutputStorageImageT( inscatter.getTargetViewId(), voltrv::Bindings::eOutScattering ) );
		}
		else
		{
			transmittance.setLastAttach( pass.addOutputColourTarget( transmittance.getTargetViewId() ) );
			inscatter.setLastAttach( pass.addOutputColourTarget( inscatter.getTargetViewId() ) );
		}
		cameraUbo.createPassBinding( pass, voltrv::Bindings::eCamera );

		auto binding = uint32_t( voltrv::Bindings::eDepthMap );
		engine.getVolumeComponentsRegister().registerBindings( pass, 0xFFFFFFFFu, camera, binding );
	}

	//************************************************************************************************
}
