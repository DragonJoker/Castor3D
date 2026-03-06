#include "Castor3D/Render/Volumetric/VolumesTraversal.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/Volumetric/VolumetricRendering.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslRay.hpp>
#include <Castor3D/Shader/Shaders/GlslVolumeShaders.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/ComputeWriter.hpp>

namespace c3d
{
	//*********************************************************************************************

	namespace voltrv
	{
		enum class Bindings : uint32_t
		{
			eOutTransmittance,
			eOutInscatter,
			eCamera,
			eDepthMap,
			eCount,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, c3d::Extent3D renderSize
			, bool hasDepth )
		{
			sdw::ComputeWriter writer{ &engine.getShaderAllocator() };

			auto targetSize = writer.declConstant( "targetSize"
				, vec2( sdw::Float{ float( renderSize.width ) }, float( renderSize.height ) ) );

			auto outTransmittance = writer.declStorageImg< sdw::WImage2DRgba16 >( "outTransmittance", Bindings::eOutTransmittance, 0u );
			auto outInscatter = writer.declStorageImg< sdw::WImage2DRgba16 >( "outSun", Bindings::eOutInscatter, 0u );

			C3D_Camera( writer, Bindings::eCamera, 0u );
			auto binding = uint32_t( Bindings::eDepthMap );
			shader::VolumeShaders volumeShaders{ writer, engine.getVolumeComponentsRegister()
				, { renderSize.width, renderSize.height }, hasDepth, 0xFFFFFFFFu, binding };

			writer.implementMain( 16u, 16u
				, [&writer, &volumeShaders, &targetSize, &c3d_cameraData
					, &outTransmittance, &outInscatter]( sdw::ComputeIn const & in )
				{
					auto fragCoord = writer.declLocale( "fragCoord"
						, vec2( in.globalInvocationID.xy() ) );

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

					auto ifragCoord = writer.declLocale( "ifragCoord"
						, ivec2( fragCoord ) );
					outTransmittance.store( ifragCoord, vec4( traversalResult.transmittance, 1.0_f ) );
					outInscatter.store( ifragCoord, vec4( traversalResult.inscatter, 1.0_f ) );
				} );

			return writer.getBuilder().releaseShader();
		}

		//************************************************************************************************

		class FramePass
			: public crg::ComputePass
		{
		public:
			FramePass( crg::FramePass const & framePass
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
	}

	//************************************************************************************************

	void createVolumesTraversalPass( crg::FramePassGroup & graph, RenderDevice const & device
		, VolumetricRendering const & volumetric, CameraUbo const & cameraUbo
		, Texture & transmittance, Texture & inscatter )
	{
		auto renderSize = transmittance.getExtent();
		auto & pass = graph.createPass( "TraverseVolumes"
			, [&device, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = c3d::makeRawUnique< voltrv::FramePass >( framePass, context, graph
					, device, renderSize
					, crg::cp::Config{}
						.groupCountX( renderSize.width / 16u )
						.groupCountY( renderSize.height / 16u ) );
				c3d::getEngine( device ).registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		crg::SamplerDesc linearClampSampler{ c3d::FilterMode::eLinear, c3d::FilterMode::eLinear };

		transmittance.setLastAttach( pass.addOutputStorageImageT( transmittance.getTargetViewId(), voltrv::Bindings::eOutTransmittance ) );
		inscatter.setLastAttach( pass.addOutputStorageImageT( inscatter.getTargetViewId(), voltrv::Bindings::eOutInscatter ) );
		cameraUbo.createPassBinding( pass, voltrv::Bindings::eCamera );
		pass.addInputSampledT( *volumetric.getColour().getLastAttach(), voltrv::Bindings::eDepthMap, linearClampSampler );
	}

	//************************************************************************************************
}
