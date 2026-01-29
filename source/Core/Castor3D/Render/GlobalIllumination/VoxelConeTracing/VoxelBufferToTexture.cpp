#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelBufferToTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( c3d, VoxelBufferToTexture )

namespace c3d
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
				, c3d::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			auto voxels = pass.getInputs().begin();
			auto result = pass.getOutputs().begin();
			ashes::WriteDescriptorSetArray writes;

			auto write = graph.getDescriptorWrite( *voxels->second, voxels->first );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWrite( *result->second, result->first );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().imageInfo = write.imageInfo;

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

		static ShaderPtr createShader( RenderDevice const & device
			, bool temporalSmoothing
			, uint32_t voxelGridSize )
		{
			sdw::ComputeWriter writer{ &c3d::getEngine( device ).getShaderAllocator() };

			// Inputs
			auto voxels( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
				, eVoxels
				, 0u ) );

			// Outputs
			auto output( writer.declStorageImg< RWFImg3DRgba32 >( "output"
				, eResult
				, 0u ) );

			shader::Utils utils{ writer };

			writer.implementMainT< sdw::VoidT >( 256u
				, [&]( sdw::ComputeIn const & in )
				{
					auto color = writer.declLocale( "color"
						, utils.decodeColor( voxels[in.globalInvocationID.x()].colorMask() ) );

					sdwIF( writer, color.a() > 0.0_f )
					{
						auto coord = writer.declLocale( "coord"
							, ivec3( utils.unflatten( in.globalInvocationID.x()
								, uvec3( sdw::UInt{ voxelGridSize } ) ) ) );

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
					sdwFI

					// delete emission data, but keep normals (no need to delete, we will only read normal values of filled voxels)
					voxels[in.globalInvocationID.x()].colorMask() = 0_u;
				} );
			return writer.getBuilder().releaseShader();
		}

		static VoxelBufferToTexture::Pipeline createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, uint32_t index
			, uint32_t voxelGridSize )
		{
			auto temporalSmoothing = ( ( index >> 0 ) % 2 ) == 1u;
			VoxelBufferToTexture::Pipeline result{ { VK_SHADER_STAGE_COMPUTE_BIT
				, cuT( "VoxelBufferToTexture" )
				, createShader( device, temporalSmoothing, voxelGridSize ) } };
			result.pipeline = createPipeline( device, pipelineLayout, result.shader );
			return result;
		}

		static Array< VoxelBufferToTexture::Pipeline, 4u > createPipelines( RenderDevice const & device
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
		, VctConfig const & vctConfig
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { crg::defaultV< InitialiseCallback >
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doRecordInto( context, cb, i ); }
				, GetPassIndexCallback( [this](){ return doGetPassIndex(); } )
				, c3d::move( isEnabled )
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, crg::ru::Config{ 2u, false }.implicitAction( pass.getOutputs().begin()->second->view()
				, crg::RecordContext::clearAttachment( pass.getOutputs().begin()->second->view(), transparentBlackClearColor ) ) }
		, m_device{ device }
		, m_vctConfig{ vctConfig }
		, m_descriptorSetLayout{ vxlbuftotex::createDescriptorLayout( m_device ) }
		, m_pipelineLayout{ vxlbuftotex::createPipelineLayout( m_device, *m_descriptorSetLayout ) }
		, m_pipelines{ vxlbuftotex::createPipelines( device, *m_pipelineLayout, m_vctConfig.gridSize.value() ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ vxlbuftotex::createDescriptorSet( getGraph(), *m_descriptorSetPool, getPass() ) }
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
		auto view = getPass().getOutputs().begin()->second->view( index );
		auto layoutState = getLayoutState( view );
		auto image = getGraph().createImage( view.data->image );

		if ( !temporalSmoothing )
		{
			auto color = convert( transparentBlackClearColor );
			auto subresourceRange = convert( view.data->info.subresourceRange );
			context.memoryBarrier( commandBuffer
				, view
				, ImageLayout::eUndefined
				, makeLayoutState( ImageLayout::eTransferDst ) );
			context->vkCmdClearColorImage( commandBuffer
				, image
				, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, &color
				, 1
				, &subresourceRange );
			context.memoryBarrier( commandBuffer
				, view
				, ImageLayout::eTransferDst
				, layoutState );
		}

		context->vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelines[doGetPassIndex()].pipeline );
		context->vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		context->vkCmdDispatch( commandBuffer, voxelGridSize * voxelGridSize * voxelGridSize / 256, 1u, 1u );
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
