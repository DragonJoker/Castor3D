#include "Castor3D/Render/Passes/ComputeDepthRange.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace passcompdr
	{
		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBinding( ComputeDepthRange::eInput
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( ComputeDepthRange::eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( "ComputeDepthRange"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			auto input = pass.images.front();
			auto output = pass.buffers.front();
			ashes::WriteDescriptorSetArray writes;
			writes.push_back( ashes::WriteDescriptorSet{ input.binding
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( input.view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );
			auto write = output.getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;
			auto descriptorSet = pool.createDescriptorSet( "ComputeDepthRange" );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "ComputeDepthRange"
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		static ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ShaderModule & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( "ComputeDepthRange"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		static ShaderPtr createShader( RenderDevice const & device )
		{
			using namespace sdw;
			ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			// Inputs
			auto input( writer.declStorageImg< RFImg2DRgba32 >( "input"
				, ComputeDepthRange::eInput
				, 0u ) );

			// Outputs
			auto output( writer.declStorageBuffer( "c3d_output"
				, ComputeDepthRange::eOutput
				, 0u ) );
			auto minmax = output.declMember< sdw::Int >( "minmax", 2u );
			output.end();

			shader::Utils utils{ writer };

			writer.implementMainT< VoidT >( 32u, 32u, [&]( ComputeIn in )
				{
					auto size = writer.declLocale( "size"
						, uvec2( input.getSize() ) );

					IF( writer, in.globalInvocationID.x() >= size.x()
						|| in.globalInvocationID.y() >= size.y() )
					{
						writer.returnStmt();
					}
					FI;

					auto fragColor = writer.declLocale( "fragColor"
						, input.load( ivec2( in.globalInvocationID.xy() ) ).y() );

					IF( writer, fragColor > 0.0_f )
					{
						atomicMin( minmax[0], floatBitsToInt( fragColor ) );
						atomicMax( minmax[1], floatBitsToInt( fragColor ) );
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}
	
	//*********************************************************************************************

	ComputeDepthRange::ComputeDepthRange( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, bool & enabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t index ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, IsEnabledCallback( [&enabled](){ return enabled; } )
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 2u, false } }
		, m_device{ device }
		, m_descriptorSetLayout{ passcompdr::createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ passcompdr::createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, "ComputeDepthRange", passcompdr::createShader( device ) }
		, m_pipeline{ passcompdr::createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ passcompdr::createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
	{
	}

	void ComputeDepthRange::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void ComputeDepthRange::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		auto view = m_pass.images.front().view();
		auto extent = getExtent( view );

		m_context.vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		m_context.vkCmdDispatch( commandBuffer
			, extent.width / 32u + 1u
			, extent.height / 32u + 1u
			, 1u );
	}

	uint32_t ComputeDepthRange::doGetPassIndex()const
	{
		return 0u;
	}

	bool ComputeDepthRange::doIsComputePass()const
	{
		return true;
	}

	//*********************************************************************************************
}
