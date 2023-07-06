#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelBufferToTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSceneData.hpp"
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

CU_ImplementSmartPtr( castor3d, VoxelBufferToTexture )

namespace castor3d
{
	//*********************************************************************************************

	namespace vxlbuftotex
	{
		enum IDs : uint32_t
		{
			eVoxels,
			eResult,
		};

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBinding( eVoxels
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eResult
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( "VoxelBufferToTexture"
				, std::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			auto voxels = pass.buffers.front();
			auto result = pass.images.front();
			ashes::WriteDescriptorSetArray writes;
			auto write = voxels.getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;
			writes.push_back( ashes::WriteDescriptorSet{ result.binding
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( result.view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );
			auto descriptorSet = pool.createDescriptorSet( "VoxelBufferToTexture" );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "VoxelBufferToTexture"
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		static ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ShaderModule & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( "VoxelBufferToTexture"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		static ShaderPtr createShader( bool temporalSmoothing
			, uint32_t voxelGridSize )
		{
			using namespace sdw;
			ComputeWriter writer;

			// Inputs
			auto voxels( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
				, eVoxels
				, 0u ) );

			// Outputs
			auto output( writer.declStorageImg< RWFImg3DRgba32 >( "output"
				, eResult
				, 0u ) );

			shader::Utils utils{ writer };

			writer.implementMainT< VoidT >( 256u
				, [&]( ComputeIn in )
				{
					auto color = writer.declLocale( "color"
						, utils.decodeColor( voxels[in.globalInvocationID.x()].colorMask ) );

					IF( writer, color.a() > 0.0_f )
					{
						auto coord = writer.declLocale( "coord"
							, ivec3( utils.unflatten( in.globalInvocationID.x()
								, uvec3( UInt{ voxelGridSize } ) ) ) );

						if ( temporalSmoothing )
						{
							output.store( coord
								, mix( output.load( coord ), vec4( color.rgb(), 1.0_f ), vec4( 0.2_f ) ) );
						}
						else
						{
							output.store( coord, vec4( color.rgb(), 1.0_f ) );
						}
					}
					FI;

					// delete emission data, but keep normals (no need to delete, we will only read normal values of filled voxels)
					voxels[in.globalInvocationID.x()].colorMask = 0_u;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static VoxelBufferToTexture::Pipeline createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, uint32_t index
			, uint32_t voxelGridSize )
		{
			auto temporalSmoothing = ( ( index >> 0 ) % 2 ) == 1u;
			VoxelBufferToTexture::Pipeline result{ { VK_SHADER_STAGE_COMPUTE_BIT
				, "VoxelBufferToTexture"
				, createShader( temporalSmoothing, voxelGridSize ) } };
			result.pipeline = createPipeline( device, pipelineLayout, result.shader );
			return result;
		}

		static std::array< VoxelBufferToTexture::Pipeline, 4u > createPipelines( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, uint32_t voxelGridSize )
		{
			return { createPipeline( device, pipelineLayout, 0u, voxelGridSize )
				, createPipeline( device, pipelineLayout, 1u, voxelGridSize )
				, createPipeline( device, pipelineLayout, 2u, voxelGridSize )
				, createPipeline( device, pipelineLayout, 3u, voxelGridSize ) };
		}
	}
	
	//*********************************************************************************************

	VoxelBufferToTexture::VoxelBufferToTexture( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, VoxelSceneData const & vctConfig
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { []( uint32_t index ){}
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT ); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, isEnabled
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, crg::ru::Config{ 2u, false }.implicitAction( pass.images.front().view()
				, crg::RecordContext::clearAttachment( pass.images.front().view(), transparentBlackClearColor ) ) }
		, m_device{ device }
		, m_vctConfig{ vctConfig }
		, m_descriptorSetLayout{ vxlbuftotex::createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ vxlbuftotex::createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_pipelines{ vxlbuftotex::createPipelines( device, *m_pipelineLayout, m_vctConfig.gridSize.value() ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ vxlbuftotex::createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
	{
	}

	void VoxelBufferToTexture::accept( RenderTechniqueVisitor & visitor )
	{
		if ( visitor.config.allowProgramsVisit )
		{
			visitor.visit( m_pipelines[doGetPassIndex()].shader );
		}
	}

	void VoxelBufferToTexture::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		auto temporalSmoothing = ( ( index >> 0 ) % 2 ) == 1u;
		auto voxelGridSize = m_vctConfig.gridSize.value();
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		auto view = m_pass.images.front().view( index );
		auto layoutState = getLayoutState( view );
		auto image = m_graph.createImage( view.data->image );

		if ( !temporalSmoothing )
		{
			context.memoryBarrier( commandBuffer
				, view.data->image
				, view.data->info.viewType
				, view.data->info.subresourceRange
				, VK_IMAGE_LAYOUT_UNDEFINED
				, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
					, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );
			m_context.vkCmdClearColorImage( commandBuffer
				, image
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, &transparentBlackClearColor.color
				, 1
				, &view.data->info.subresourceRange );
			context.memoryBarrier( commandBuffer
				, view.data->image
				, view.data->info.viewType
				, view.data->info.subresourceRange
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, layoutState );
		}

		m_context.vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelines[doGetPassIndex()].pipeline );
		m_context.vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		m_context.vkCmdDispatch( commandBuffer, voxelGridSize * voxelGridSize * voxelGridSize / 256, 1u, 1u );
	}

	uint32_t VoxelBufferToTexture::doGetPassIndex()const
	{
		return ( m_vctConfig.enableTemporalSmoothing ? 1u : 0u );
	}

	bool VoxelBufferToTexture::doIsComputePass()const
	{
		return true;
	}

	//*********************************************************************************************
}
