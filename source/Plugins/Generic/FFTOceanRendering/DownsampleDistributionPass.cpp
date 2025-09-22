#include "FFTOceanRendering/DownsampleDistributionPass.hpp"

#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
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

	namespace downdist
	{
		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( c3d::RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ c3d::makeDescriptorSetLayoutBinding( DownsampleDistributionPass::eConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBinding( DownsampleDistributionPass::eInput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, c3d::makeDescriptorSetLayoutBinding( DownsampleDistributionPass::eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( DownsampleDistributionPass::Name 
				, c3d::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			ashes::WriteDescriptorSetArray writes;

			auto write = graph.getDescriptorWrite( *pass.uniforms.begin()->second
				, DownsampleDistributionPass::eConfig );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWrite( *pass.inputs.begin()->second
				, DownsampleDistributionPass::eInput );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWrite( *pass.outputs.begin()->second
				, DownsampleDistributionPass::eOutput );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			auto descriptorSet = pool.createDescriptorSet( DownsampleDistributionPass::Name );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( c3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( DownsampleDistributionPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		static ashes::ComputePipelinePtr createPipeline( c3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, c3d::ShaderModule & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( DownsampleDistributionPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, c3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		static c3d::ShaderPtr createShader( c3d::RenderDevice const & device )
		{
			sdw::ComputeWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_FftOcean( writer, DownsampleDistributionPass::eConfig, 0u );

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
					writer.returnStmt( mix( i, i - N, vec2( greaterThan( i, 0.5_f * N ) ) ) );
				}
				, sdw::InVec2{ writer, "i" }
				, sdw::InVec2{ writer, "n" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 64u, 1u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto outN = writer.declLocale( "outN"
						, in.workGroupSize.xy() * in.numWorkGroups.xy() );
					auto inN = writer.declLocale( "inN"
						, uvec2( outN.x() << writer.cast< sdw::UInt >( c3d_oceanData.displacementDownsample() )
							, outN.y() << writer.cast< sdw::UInt >( c3d_oceanData.displacementDownsample() ) ) );
					auto i = writer.declLocale( "i"
						, in.globalInvocationID.xy() );

					auto aliased = writer.declLocale( "aliased"
						, ivec2( alias( vec2( i ), vec2( outN ) ) ) );

					sdwIF( writer, aliased.x() < 0_i )
					{
						aliased.x() += writer.cast< sdw::Int >( inN.x() );
					}
					sdwFI
					sdwIF( writer, aliased.y() < 0_i )
					{
						aliased.y() += writer.cast< sdw::Int >( inN.y() );
					}
					sdwFI

					downsampled[i.y() * outN.x() + i.x()] = distribution[writer.cast< sdw::UInt >( aliased.y() ) * inN.x() + writer.cast< sdw::UInt >( aliased.x() )];
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	c3d::MbString const DownsampleDistributionPass::Name{ "GenerateDistribution" };

	DownsampleDistributionPass::DownsampleDistributionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, c3d::RenderDevice const & device
		, c3d::Extent2D const & extent
		, uint32_t downsample
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t index ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( c3d::PipelineStageFlags::eComputeShader ); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 1u } }
		, m_device{ device }
		, m_descriptorSetLayout{ downdist::createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ downdist::createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, c3d::makeString( Name ), downdist::createShader( device ) }
		, m_pipeline{ downdist::createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ downdist::createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
		, m_extent{ crg::convert( ashes::getSubresourceDimensions( convert( extent ), downsample ) ) }
	{
	}

	void DownsampleDistributionPass::accept( c3d::RenderTechniqueVisitor & visitor )const
	{
		visitor.visit( m_shader );
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

	uint32_t DownsampleDistributionPass::doGetPassIndex()const
	{
		return 0u;
	}

	bool DownsampleDistributionPass::doIsComputePass()const
	{
		return true;
	}

	//************************************************************************************************

	void createDownsampleDistributionPass( c3d::String const & name
		, c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Extent2D const & extent
		, uint32_t downsample
		, OceanUbo const & ubo
		, c3d::BufferBase const & input
		, c3d::BufferBase & output )
	{
		auto mbName = c3d::toUtf8( name );
		auto & pass = graph.createPass( "GenerateDistribution" + mbName
			, [&device, downsample, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnableGraph )
			{
				auto res = c3d::makeRawUnique< DownsampleDistributionPass >( framePass
					, context
					, runnableGraph
					, device
					, extent
					, downsample
					, crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, res->getTimer() );
				return res;
			} );
		ubo.createPassBinding( pass, DownsampleDistributionPass::eConfig );
		pass.addInputStorage( *input.getLastAttach(), DownsampleDistributionPass::eInput );
		output.setLastAttach( pass.addOutputStorageBuffer( output.bufferViewId, DownsampleDistributionPass::eOutput ) );
	}

	//************************************************************************************************
}
