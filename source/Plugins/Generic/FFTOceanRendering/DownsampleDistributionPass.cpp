#include "FFTOceanRendering/DownsampleDistributionPass.hpp"

#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/RunnableGraph.hpp>

#include <ShaderWriter/Source.hpp>

namespace ocean_fft
{
	//*********************************************************************************************

	namespace
	{
		ashes::DescriptorSetLayoutPtr createDescriptorLayout( castor3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ castor3d::makeDescriptorSetLayoutBinding( DownsampleDistributionPass::eConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( DownsampleDistributionPass::eInput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( DownsampleDistributionPass::eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( DownsampleDistributionPass::Name 
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			ashes::WriteDescriptorSetArray writes;

			auto write = pass.buffers[DownsampleDistributionPass::eConfig].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[DownsampleDistributionPass::eInput].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			write = pass.buffers[DownsampleDistributionPass::eOutput].getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;

			auto descriptorSet = pool.createDescriptorSet( DownsampleDistributionPass::Name );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( castor3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( DownsampleDistributionPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		ashes::ComputePipelinePtr createPipeline( castor3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, castor3d::ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( DownsampleDistributionPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, castor3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		castor3d::ShaderPtr createShader()
		{
			sdw::ComputeWriter writer;

			UBO_OCEAN( writer, DownsampleDistributionPass::eConfig, 0u );

			auto distributions = writer.declStorageBuffer( "Distribution", DownsampleDistributionPass::eInput, 0u );
			auto distribution = distributions.declMemberArray< sdw::Vec2 >( "distribution" );
			distributions.end();

			auto downs = writer.declStorageBuffer( "Downsampled", DownsampleDistributionPass::eOutput, 0u );
			auto downsampled = downs.declMemberArray< sdw::Vec2 >( "downsampled" );
			downs.end();

			auto alias = writer.implementFunction< sdw::Vec2 >( "alias"
				, [&]( sdw::Vec2 i
					, sdw::Vec2 N )
				{
					writer.returnStmt( mix( i, i - N, vec2( greaterThan( i, 0.5 * N ) ) ) );
				}
				, sdw::InVec2{ writer, "i" }
				, sdw::InVec2{ writer, "n" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 64u, 1u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto outN = writer.declLocale( "outN"
						, in.workGroupSize.xy() * in.numWorkGroups.xy() );
					auto inN = writer.declLocale( "inN"
						, uvec2( outN.x() << writer.cast< sdw::UInt >( c3d_oceanData.displacementDownsample )
							, outN.y() << writer.cast< sdw::UInt >( c3d_oceanData.displacementDownsample ) ) );
					auto i = writer.declLocale( "i"
						, in.globalInvocationID.xy() );

					auto aliased = writer.declLocale( "aliased"
						, ivec2( alias( vec2( i ), vec2( outN ) ) ) );

					IF( writer, aliased.x() < 0_i )
					{
						aliased.x() += writer.cast< sdw::Int >( inN.x() );
					}
					FI;
					IF( writer, aliased.y() < 0_i )
					{
						aliased.y() += writer.cast< sdw::Int >( inN.y() );
					}
					FI;

					downsampled[i.y() * outN.x() + i.x()] = distribution[writer.cast< sdw::UInt >( aliased.y() ) * inN.x() + writer.cast< sdw::UInt >( aliased.x() )];
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	castor::String const DownsampleDistributionPass::Name{ "GenerateDistribution" };

	DownsampleDistributionPass::DownsampleDistributionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, VkExtent2D const & extent
		, uint32_t downsample
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [this](){ return doGetSemaphoreWaitFlags(); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 1u } }
		, m_device{ device }
		, m_descriptorSetLayout{ createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, Name, createShader() }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
		, m_extent{ ashes::getSubresourceDimensions( extent, downsample ) }
	{
	}

	void DownsampleDistributionPass::accept( castor3d::RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void DownsampleDistributionPass::doInitialise()
	{
	}

	void DownsampleDistributionPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		VkDescriptorSet descriptorSet = *m_descriptorSet;
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
			, m_extent.width / 64u
			, m_extent.height
			, 1u );
	}

	VkPipelineStageFlags DownsampleDistributionPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	uint32_t DownsampleDistributionPass::doGetPassIndex()const
	{
		return 0u;
	}

	bool DownsampleDistributionPass::doIsComputePass()const
	{
		return true;
	}

	//************************************************************************************************

	crg::FramePass const & createDownsampleDistributionPass( castor::String const & prefix
		, castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePassArray previousPasses
		, VkExtent2D const & extent
		, uint32_t downsample
		, OceanUbo const & ubo
		, ashes::BufferBase const & input
		, ashes::BufferBase const & output
		, std::shared_ptr< IsRenderPassEnabled > isEnabled )
	{
		auto & result = graph.createPass( "GenerateDistribution" + name
			, [&device, downsample, extent, isEnabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = std::make_unique< DownsampleDistributionPass >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, downsample
					, crg::RunnablePass::IsEnabledCallback( [isEnabled](){ return ( *isEnabled )(); } ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, res->getTimer() );
				return res;
			} );
		result.addDependencies( previousPasses );
		ubo.createPassBinding( result
			, DownsampleDistributionPass::eConfig );
		result.addInputStorageBuffer( { input, name + "Seed" }
			, DownsampleDistributionPass::eInput
			, 0u
			, input.getSize() );
		result.addOutputStorageBuffer( { output, name + "Distribution" }
			, DownsampleDistributionPass::eOutput
			, 0u
			, output.getSize() );
		return result;
	}

	//************************************************************************************************
}
