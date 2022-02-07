#include "FFTOceanRendering/GenerateMipmapsPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
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
			ashes::VkDescriptorSetLayoutBindingArray bindings{ castor3d::makeDescriptorSetLayoutBinding( GenerateMipmapsPass::eInput
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, castor3d::makeDescriptorSetLayoutBinding( GenerateMipmapsPass::eOutput
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( GenerateMipmapsPass::Name
				, std::move( bindings ) );
		}

		std::vector< ashes::DescriptorSetPtr > createDescriptorSets( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			std::vector< ashes::DescriptorSetPtr > result;
			auto & srcAttach = pass.images.front();
			auto & dstAttach = pass.images.back();
			auto inViewId = srcAttach.view();
			auto imageId = inViewId.data->image;
			auto data = *dstAttach.view().data;
			auto range = data.info.subresourceRange;
			data.info.subresourceRange.levelCount = 1u;
			auto sampler = graph.createSampler( crg::SamplerDesc{} );
			inViewId = graph.createView( data );
			auto inView = graph.createImageView( inViewId );

			for ( uint32_t level = range.baseMipLevel; level < range.baseMipLevel + range.levelCount - 1u; ++level )
			{
				ashes::WriteDescriptorSetArray writes;
				writes.push_back( ashes::WriteDescriptorSet{ GenerateMipmapsPass::eInput
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER } );
				writes.back().imageInfo.push_back( { sampler
					, inView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } );

				data.info.subresourceRange.baseMipLevel++;
				data.name = imageId.data->name + "_L" + std::to_string( data.info.subresourceRange.baseMipLevel );
				auto outViewId = graph.createView( data );
				auto outView = graph.createImageView( outViewId );
				writes.push_back( ashes::WriteDescriptorSet{ GenerateMipmapsPass::eOutput
					, 0u
					, 1u
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE } );
				writes.back().imageInfo.push_back( { nullptr
					, outView
					, VK_IMAGE_LAYOUT_GENERAL } );

				auto descriptorSet = pool.createDescriptorSet( GenerateMipmapsPass::Name );
				descriptorSet->setBindings( writes );
				descriptorSet->update();

				result.emplace_back( std::move( descriptorSet ) );
				inView = outView;
			}

			return result;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( castor3d::RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( GenerateMipmapsPass::Name
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) }
				, ashes::VkPushConstantRangeArray{ { VK_SHADER_STAGE_COMPUTE_BIT, 0u, uint32_t( sizeof( castor::Point2f ) ) } } );
		}

		ashes::ComputePipelinePtr createPipeline( castor3d::RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, castor3d::ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( GenerateMipmapsPass::Name
				, ashes::ComputePipelineCreateInfo( 0u
					, castor3d::makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		castor3d::ShaderPtr createShader()
		{
			sdw::ComputeWriter writer;
			auto const G = writer.declConstant( "G", 9.81_f );

			auto pcb = writer.declPushConstantsBuffer( "MipmapsData" );
			auto invSize = pcb.declMember< sdw::Vec2 >( "invSize" );
			pcb.end();

			auto inImg = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "inImg", GenerateMipmapsPass::eInput, 0u );
			auto outImg = writer.declStorageImg< sdw::WImage2DRgba16 >( "outImg", GenerateMipmapsPass::eOutput, 0u );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 4u, 4u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto uv = writer.declLocale( "uv"
						, invSize * fma( vec2( in.globalInvocationID.xy() ), vec2( 2.0_f ), vec2( 0.5_f ) ) );
					auto filtered = writer.declLocale( "filtered"
						, vec4( 0.0_f ) );
					auto D = writer.declConstant( "D"
						, 0.5_f );

					filtered += 0.25_f * inImg.lod( uv + vec2( -D, -D ) * invSize, 0.0_f );
					filtered += 0.25_f * inImg.lod( uv + vec2( +D, -D ) * invSize, 0.0_f );
					filtered += 0.25_f * inImg.lod( uv + vec2( -D, +D ) * invSize, 0.0_f );
					filtered += 0.25_f * inImg.lod( uv + vec2( +D, +D ) * invSize, 0.0_f );

					outImg.store( ivec2( in.globalInvocationID.xy() ), filtered );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	castor::String const GenerateMipmapsPass::Name{ "GenerateMipmaps" };

	GenerateMipmapsPass::GenerateMipmapsPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, castor3d::RenderDevice const & device
		, crg::ru::Config ruConfig
		, crg::RunnablePass::GetPassIndexCallback passIndex
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { [this](){ doInitialise(); }
				, GetSemaphoreWaitFlagsCallback( [this](){ return doGetSemaphoreWaitFlags(); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i );}
				, passIndex
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, { 1u } }
		, m_device{ device }
		, m_descriptorSetLayout{ createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, Name, createShader() }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( crg::getMipLevels( m_pass.images.front().image.view() ) + crg::getMipLevels( m_pass.images.back().image.view() ) ) }
		, m_descriptorSets{ createDescriptorSets( m_graph, *m_descriptorSetPool, m_pass ) }
	{
		auto extent = getExtent( m_pass.images.front().view() );

		for ( size_t i = 0u; i < m_descriptorSets.size(); ++i )
		{
			m_invSizes.push_back( { 1.0f / float( extent.width )
				, 1.0f / float( extent.height ) } );
			extent.width >>= 1u;
			extent.height >>= 1u;
		}
	}

	void GenerateMipmapsPass::doInitialise()
	{
		auto & attach = m_pass.images.front();

		for ( auto passIndex = 0u; passIndex < m_commandBuffers.size(); ++passIndex )
		{
			auto viewId = attach.view( passIndex );
			auto layoutState = ( m_outputLayout.layout != VK_IMAGE_LAYOUT_UNDEFINED
				? m_outputLayout
				: m_graph.getOutputLayout( m_pass, viewId, false ) );
			doUpdateFinalLayout( passIndex
				, viewId
				, layoutState.layout
				, layoutState.access
				, layoutState.pipelineStage );
		}
	}

	void GenerateMipmapsPass::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		auto viewAttach{ m_pass.images.front() };
		auto viewId{ viewAttach.view( index ) };
		auto dstViewId{ m_pass.images.back().view( index ) };
		auto imageId{ viewId.data->image };
		auto extent = getExtent( viewId );
		auto range = viewId.data->info.subresourceRange;
		range.levelCount = getMipLevels( dstViewId );
		auto invSizeIt = m_invSizes.begin();
		auto transition = getTransition( index, viewId );
		auto dstTransition = getTransition( index, dstViewId );
		crg::LayoutState shaderRead{ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, VK_ACCESS_SHADER_READ_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };
		crg::LayoutState shaderWrite{ VK_IMAGE_LAYOUT_GENERAL
			, VK_ACCESS_SHADER_WRITE_BIT
			, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT };
		auto mipLevels = imageId.data->info.mipLevels;
		auto srcImageLayout = transition.needed;
		auto dstMipImageLayout = ( range.levelCount == mipLevels )
			? srcImageLayout
			: transition.to;
		auto format = getFormat( imageId );
		auto const aspectMask = crg::getAspectMask( format );
		VkImageSubresourceRange mipSubRange{ aspectMask
			, 0u
			, 1u
			, 0u
			, 1u };
		// Transition first mip level to shader source for read in next iteration
		m_graph.memoryBarrier( context
			, commandBuffer
			, imageId
			, viewId.data->info.viewType
			, mipSubRange
			, viewAttach.image.initialLayout
			, shaderRead );

		for ( auto & ds : m_descriptorSets )
		{
			extent.width >>= 1u;
			extent.height >>= 1u;
			++mipSubRange.baseMipLevel;
			// Transition current mip level to shader write
			m_graph.memoryBarrier( context
				, commandBuffer
				, imageId
				, viewId.data->info.viewType
				, mipSubRange
				, VK_IMAGE_LAYOUT_UNDEFINED
				, shaderWrite );

			// Generate mip level
			VkDescriptorSet descriptorSet = *ds;
			m_context.vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *m_pipeline );
			m_context.vkCmdPushConstants( commandBuffer
				, *m_pipelineLayout
				, VK_SHADER_STAGE_COMPUTE_BIT
				, 0u
				, uint32_t( sizeof( castor::Point2f ) )
				, &( *invSizeIt ) );
			m_context.vkCmdBindDescriptorSets( commandBuffer
				, VK_PIPELINE_BIND_POINT_COMPUTE
				, *m_pipelineLayout
				, 0u
				, 1u
				, &descriptorSet
				, 0u
				, nullptr );
			m_context.vkCmdDispatch( commandBuffer
				, extent.width / 4u
				, extent.height / 4u
				, 1u );
			++invSizeIt;

			// Transition previous mip level to wanted output layout
			m_graph.memoryBarrier( context
				, commandBuffer
				, imageId
				, viewId.data->info.viewType
				, { mipSubRange.aspectMask
					, mipSubRange.baseMipLevel - 1u
					, 1u
					, mipSubRange.baseArrayLayer
					, 1u }
				, VK_IMAGE_LAYOUT_UNDEFINED
				, dstMipImageLayout );

			if ( mipSubRange.baseMipLevel == ( mipLevels - 1u ) )
			{
				// Transition final mip level to wanted output layout
				m_graph.memoryBarrier( context
					, commandBuffer
					, imageId
					, viewId.data->info.viewType
					, mipSubRange
					, VK_IMAGE_LAYOUT_UNDEFINED
					, dstMipImageLayout );
			}
			else
			{
				// Transition current mip level to shader source for read in next iteration
				m_graph.memoryBarrier( context
					, commandBuffer
					, imageId
					, viewId.data->info.viewType
					, mipSubRange
					, VK_IMAGE_LAYOUT_UNDEFINED
					, shaderRead );
			}
		}
	}

	VkPipelineStageFlags GenerateMipmapsPass::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	bool GenerateMipmapsPass::doIsComputePass()const
	{
		return true;
	}

//************************************************************************************************
}
