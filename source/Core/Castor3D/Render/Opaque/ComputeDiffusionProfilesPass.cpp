#include "Castor3D/Render/Opaque/ComputeDiffusionProfilesPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/SssProfileBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslSssProfile.hpp"

#include <RenderGraph/FramePassGroup.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/ComputeWriter.hpp>

namespace c3d
{
	namespace difpfl
	{
		namespace c3ds = c3d::shader;

		enum class Idx : uint32_t
		{
			SssProfilesIdx,
			DiffusionProfileTexIdx,
		};

		static ShaderPtr getProgram( Engine & engine )
		{
			sdw::ComputeWriter writer{ &engine.getShaderAllocator() };

			shader::SssProfiles sssProfiles{ writer, uint32_t( Idx::SssProfilesIdx ), 0u };
			auto c3d_mapDiffusionProfiles = writer.declStorageImg< WFImg1DArrayRgba16 >( "c3d_mapDiffusionProfiles", uint32_t( Idx::DiffusionProfileTexIdx ), 0u );

			writer.implementMain( 32u, 32u, [&writer, &sssProfiles, c3d_mapDiffusionProfiles]( sdw::ComputeIn const & in )
				{
					sdwIF( writer, in.globalInvocationID.y() < sssProfiles.getCount() )
					{
						auto profile = writer.declLocale( "profile"
							, sssProfiles.getData( in.globalInvocationID.y() ) );
						auto result = writer.declLocale( "result"
							, vec3( 0.0_f ) );
						auto s = writer.declLocale( "s"
							, 4.0_f * writer.cast< sdw::Float >( in.globalInvocationID.x() ) / 511.0f );
						auto ss = writer.declLocale( "ss"
							, -s * s );

						sdwFOR( writer, sdw::UInt32, i, 0_u, i < profile.transmittanceProfileSize(), ++i )
						{
							auto profileFactors = writer.declLocale( "profileFactors"
								, profile.transmittanceProfile()[i] );
							result += profileFactors.rgb() * exp( ss / profileFactors.a() );
						}
						sdwROF

						c3d_mapDiffusionProfiles.store( i32vec2( in.globalInvocationID.xy() ), vec4( result, 1.0_f ) );
					}
					sdwFI
				} );
			return writer.getBuilder().releaseShader();
		}

		class FramePass
			: public crg::ComputePass
		{
		public:
			FramePass( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, RenderDevice const & device
				, SssProfileBuffer const & buffer
				, uint32_t imageHeight
				, crg::cp::Config config )
				: crg::ComputePass{ framePass
					, context
					, graph
					, crg::ru::Config{ 1u }
					, config
						.isEnabled( RunnablePass::IsEnabledCallback( [this](){ return doIsEnabled(); } ) )
						.programCreator( { 1u, [this]( uint32_t ){ return doCreateProgram(); } } )
						.getGroupCountY( crg::cp::GetGroupCountCallback( [this]() { return doGetGroupCountY(); } ) )
						.end( RecordCallback{ [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t idx ) { doPostRecord( ctx, cb, idx ); } } ) }
				, m_device{ device }
				, m_buffer{ buffer }
				, m_imageHeight{ imageHeight }
			{
			}

		private:
			crg::VkPipelineShaderStageCreateInfoArray doCreateProgram()
			{
				if ( m_stages.empty() )
				{
					m_shaderModule = ShaderModule{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "ComputeDiffusionProfiles" ), getProgram( *m_device.renderSystem.getEngine() ) };
					m_stages = ashes::PipelineShaderStageCreateInfoArray{ makeShaderState( m_device, m_shaderModule ) };
				}

				return ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages );
			}

			bool doIsEnabled()const
			{
				return m_buffer.getCount() > 0;
			}

			uint32_t doGetGroupCountY()const
			{
				return std::max( 1u
					, uint32_t( ashes::getAlignedSize( std::min( m_buffer.getCount(), m_imageHeight ), 64u ) / 64u ) );
			}

			void doPostRecord( crg::RecordContext & context
				, VkCommandBuffer commandBuffer
				, uint32_t index )const
			{
				for ( auto const & [binding, attach] : getPass().getOutputs() )
				{
					context.memoryBarrier( commandBuffer
						, attach->view( index )
						, { ImageLayout::eShaderReadOnly, ComputeShaderReadState } );
				}
			}

		private:
			RenderDevice const & m_device;
			SssProfileBuffer const & m_buffer;
			uint32_t m_imageHeight;
			ShaderModule m_shaderModule;
			ashes::PipelineShaderStageCreateInfoArray m_stages;
		};
	}

	//*********************************************************************************************

	void createComputeDiffusionProfilesPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, SssProfileBuffer & buffer
		, Texture & result )
	{
		uint32_t imageWidth = result.getExtent().width;
		uint32_t imageHeight = result.getExtent().height;
		auto & pass = graph.createPass( "ComputeDiffusionProfiles"
			, [&device, &buffer, imageHeight, imageWidth]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runGraph )
			{
				auto result = makeRawUnique< difpfl::FramePass >( framePass
					, context
					, runGraph
					, device
					, buffer
					, imageHeight
					, crg::cp::Config{}
						.groupCountX( imageWidth / 32u )
						.groupCountY( imageHeight / 32u )
						.groupCountZ( 1u ) );
				device.renderSystem.getEngine()->registerTimer( makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		buffer.createPassBinding( pass, uint32_t( difpfl::Idx::SssProfilesIdx ) );
		result.setLastAttach( pass.addOutputStorageImage( result.getTargetViewId(), uint32_t( difpfl::Idx::DiffusionProfileTexIdx ) ) );
	}
}
