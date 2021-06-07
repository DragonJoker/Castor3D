#include "Castor3D/Render/Technique/Voxelize/VoxelBufferToTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"
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

CU_ImplementCUSmartPtr( castor3d, VoxelBufferToTexture )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		enum IDs : uint32_t
		{
			eVoxels,
			eResult,
		};

		ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
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

		ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
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

		ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "VoxelBufferToTexture"
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( "VoxelBufferToTexture"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, computeShader )
					, pipelineLayout ) );
		}

		ShaderPtr createShader( bool temporalSmoothing
			, uint32_t voxelGridSize )
		{
			using namespace sdw;
			ComputeWriter writer;

			writer.inputLayout( 256u, 1u, 1u );

			// Inputs
			auto voxels( writer.declArrayShaderStorageBuffer< shader::Voxel >( "voxels"
				, eVoxels
				, 0u ) );
			auto in = writer.getIn();

			// Outputs
			auto output( writer.declImage< RWFImg3DRgba32 >( "output"
				, eResult
				, 0u ) );

			shader::Utils utils{ writer };
			utils.declareDecodeColor();
			utils.declareUnflatten();

			writer.implementMain( [&]()
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

		VoxelBufferToTexture::Pipeline createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, uint32_t index
			, uint32_t voxelGridSize )
		{
			auto temporalSmoothing = ( ( index >> 0 ) % 2 ) == 1u;
			VoxelBufferToTexture::Pipeline result{ { VK_SHADER_STAGE_COMPUTE_BIT, "VoxelBufferToTexture", createShader( temporalSmoothing, voxelGridSize ) } };
			result.pipeline = createPipeline( device, pipelineLayout, result.shader );
			return result;
		}

		std::array< VoxelBufferToTexture::Pipeline, 4u > createPipelines( RenderDevice const & device
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
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, VoxelSceneData const & vctConfig )
		: crg::RunnablePass{ pass, context, graph, 2u }
		, m_device{ device }
		, m_vctConfig{ vctConfig }
		, m_timer{ std::make_shared< RenderPassTimer >( m_device, "Voxelize", "Buffer To Texture" ) }
		, m_descriptorSetLayout{ createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_pipelines{ createPipelines( device, *m_pipelineLayout, m_vctConfig.gridSize.value() ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass ) }
	{
	}

	VoxelBufferToTexture::~VoxelBufferToTexture()
	{
	}

	void VoxelBufferToTexture::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_pipelines[doGetPassIndex()].shader );
	}

	void VoxelBufferToTexture::doInitialise()
	{
	}

	void VoxelBufferToTexture::doRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		auto temporalSmoothing = ( ( index >> 0 ) % 2 ) == 1u;
		auto voxelGridSize = m_vctConfig.gridSize.value();
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		auto view = m_pass.images.front().view();
		auto transition = doGetTransition( index, view );
		auto image = m_graph.createImage( view.data->image );

		if ( !temporalSmoothing )
		{
			m_graph.memoryBarrier( commandBuffer
				, view
				, transition.needed
				, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
					, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) } );
			m_context.vkCmdClearColorImage( commandBuffer
				, image
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, &transparentBlackClearColor.color
				, 1
				, &view.data->info.subresourceRange );
			m_graph.memoryBarrier( commandBuffer
				, view
				, { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, crg::getAccessMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL )
					, crg::getStageMask( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) }
				, transition.needed );
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

	VkPipelineStageFlags VoxelBufferToTexture::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
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
