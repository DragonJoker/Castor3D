#include "Castor3D/Render/Technique/Voxelize/VoxelSecondaryBounce.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelSceneData.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Miscellaneous/BitSize.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/GraphContext.hpp>
#include <RenderGraph/RunnableGraph.hpp>

CU_ImplementCUSmartPtr( castor3d, VoxelSecondaryBounce )

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		enum IDs : uint32_t
		{
			eVoxelBuffer,
			eVoxelConfig,
			eFirstBounce,
			eResult,
		};

		ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device
			, uint32_t voxelGridSize )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBinding( eVoxelBuffer
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eVoxelConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eFirstBounce
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eResult
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };

			return device->createDescriptorSetLayout( "VoxelSecondaryBounce"
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass
			, uint32_t voxelGridSize )
		{
			auto voxelsBuffer = pass.buffers.front();
			auto voxelsUbo = pass.buffers.back();
			auto firstBounce = pass.images.front();
			auto secondBounce = pass.images.back();
			ashes::WriteDescriptorSetArray writes;
			auto write = voxelsBuffer.getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;
			write = voxelsUbo.getBufferWrite();
			writes.push_back( ashes::WriteDescriptorSet{ write->dstBinding
				, write->dstArrayElement
				, write->descriptorCount
				, write->descriptorType } );
			writes.back().bufferInfo = write.bufferInfo;
			writes.push_back( ashes::WriteDescriptorSet{ firstBounce.binding
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, { VkDescriptorImageInfo{ graph.createSampler( firstBounce.image.samplerDesc )
					, graph.createImageView( firstBounce.view() )
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } } );
			writes.push_back( ashes::WriteDescriptorSet{ secondBounce.binding
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
				, { VkDescriptorImageInfo{ VK_NULL_HANDLE
					, graph.createImageView( secondBounce.view() )
					, VK_IMAGE_LAYOUT_GENERAL } } } );

			auto descriptorSet = pool.createDescriptorSet( "VoxelSecondaryBounce" );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "VoxelSecondaryBounce"
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & layout
			, ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( "VoxelSecondaryBounce"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, computeShader )
					, layout ) );
		}

		ShaderPtr createShader( uint32_t voxelGridSize )
		{
			using namespace sdw;
			ComputeWriter writer;

			writer.inputLayout( 64u, 1u, 1u );

			// Inputs
			auto voxels( writer.declArrayShaderStorageBuffer< shader::Voxel >( "voxels"
				, eVoxelBuffer
				, 0u ) );
			UBO_VOXELIZER( writer, eVoxelConfig, 0u, true );
			auto firstBounce( writer.declSampledImage< FImg3DRgba32 >( "firstBounce"
				, eFirstBounce
				, 0u ) );
			auto in = writer.getIn();

			// Outputs
			auto output( writer.declImage< RWFImg3DRgba32 >( "output"
				, eResult
				, 0u ) );

			shader::Utils utils{ writer };
			utils.declareDecodeNormal();
			utils.declareUnflatten();

			shader::GlobalIllumination indirect{ writer, utils };
			indirect.declareTraceConeRadiance();

			writer.implementMain( [&]()
				{
					auto coord = writer.declLocale( "coord"
						, ivec3( utils.unflatten( in.globalInvocationID.x()
							, uvec3( UInt{ voxelGridSize } ) ) ) );
					auto color = writer.declLocale( "color"
						, firstBounce.lod( vec3( c3d_voxelData.gridToClip ) * vec3( coord ), 0.0_f ) );

					IF( writer, color.a() > 0.0_f )
					{
						auto normal = writer.declLocale( "normal"
							, utils.decodeNormal( voxels[in.globalInvocationID.x()].normalMask ) );
						// [0.5,gridSize.5] => [0,1]
						// center of voxel, to apply normal on it.
						auto position = writer.declLocale( "position"
							, ( vec3( coord ) + vec3( 0.5_f ) ) * c3d_voxelData.gridToClip );
						// [0,1] => [-1,1]
						position = position * 2 - 1;
						position.y() *= -1;
						// [-1,1] => [-gridSize/2,gridSize/2]
						position *= c3d_voxelData.clipToGrid / 2.0f;
						// to world
						position *= c3d_voxelData.gridToWorld;

						auto surface = writer.declLocale< shader::Surface >( "surface" );
						surface.create( position, normal );
						auto radiance = writer.declLocale( "radiance"
							, indirect.traceConeRadiance( firstBounce
								, surface
								, c3d_voxelData ) );
						output.store( coord, vec4( color.rgb() + radiance.rgb(), color.a() ) );
					}
					ELSE
					{
						output.store( coord, vec4( 0.0_f ) );
					}
					FI;

					voxels[in.globalInvocationID.x()].normalMask = 0_u;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	VoxelSecondaryBounce::VoxelSecondaryBounce( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, VoxelSceneData const & vctConfig )
		: crg::RunnablePass{ pass, context, graph, 2u }
		, m_vctConfig{ vctConfig }
		, m_timer{ std::make_shared< RenderPassTimer >( device, "Voxelize", "Secondary Bounce" ) }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, "VoxelSecondaryBounce", createShader( m_vctConfig.gridSize.value() ) }
		, m_descriptorSetLayout{ createDescriptorLayout( device, m_vctConfig.gridSize.value() ) }
		, m_pipelineLayout{ createPipelineLayout( device, *m_descriptorSetLayout ) }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( m_graph, *m_descriptorSetPool, m_pass, m_vctConfig.gridSize.value() ) }
	{
	}

	void VoxelSecondaryBounce::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_shader );
	}

	void VoxelSecondaryBounce::doInitialise( uint32_t index )
	{
	}

	void VoxelSecondaryBounce::doRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		auto voxelGridSize = m_vctConfig.gridSize.value();
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		auto view = m_pass.images.back().view();
		auto transition = getTransition( index, view );
		auto image = m_graph.createImage( view.data->image );

		// Clear result
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
		m_context.vkCmdDispatch( commandBuffer, voxelGridSize * voxelGridSize * voxelGridSize / 64u, 1u, 1u );
	}

	VkPipelineStageFlags VoxelSecondaryBounce::doGetSemaphoreWaitFlags()const
	{
		return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	bool VoxelSecondaryBounce::doIsComputePass()const
	{
		return true;
	}

	//*********************************************************************************************
}