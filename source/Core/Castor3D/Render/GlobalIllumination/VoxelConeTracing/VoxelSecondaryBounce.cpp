#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelSecondaryBounce.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTechniqueVisitor.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VctConfig.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
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

CU_ImplementSmartPtr( c3d, VoxelSecondaryBounce )

namespace c3d
{
	//*********************************************************************************************

	namespace vxlscnd
	{
		enum class Bindings : uint32_t
		{
			eVoxelBuffer,
			eVoxelConfig,
			eFirstBounce,
			eResult,
		};

		static ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBindingT( Bindings::eVoxelBuffer
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBindingT( Bindings::eVoxelConfig
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBindingT( Bindings::eFirstBounce
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBindingT( Bindings::eResult
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };

			return device->createDescriptorSetLayout( "VoxelSecondaryBounce"
				, c3d::move( bindings ) );
		}

		static ashes::DescriptorSetPtr createDescriptorSet( crg::RunnableGraph & graph
			, ashes::DescriptorSetPool const & pool
			, crg::FramePass const & pass )
		{
			auto voxelsBuffer = pass.getInouts().begin();
			auto voxelsUbo = pass.getUniforms().begin();
			auto firstBounce = pass.getSampled().begin();
			auto secondBounce = pass.getOutputs().begin();
			ashes::WriteDescriptorSetArray writes;

			auto write = graph.getDescriptorWrite( *voxelsBuffer->second, voxelsBuffer->first );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWrite( *voxelsUbo->second, voxelsUbo->first );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().bufferInfo = write.bufferInfo;

			write = graph.getDescriptorWrite( *firstBounce->second.attach, firstBounce->first );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().imageInfo = write.imageInfo;

			write = graph.getDescriptorWrite( *secondBounce->second, secondBounce->first );
			writes.emplace_back( write->dstBinding, write->dstArrayElement
				, write->descriptorCount, write->descriptorType );
			writes.back().imageInfo = write.imageInfo;

			auto descriptorSet = pool.createDescriptorSet( "VoxelSecondaryBounce" );
			descriptorSet->setBindings( writes );
			descriptorSet->update();
			return descriptorSet;
		}

		static ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "VoxelSecondaryBounce"
				, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
		}

		static ashes::ComputePipelinePtr createPipeline( RenderDevice const & device
			, ashes::PipelineLayout const & layout
			, ShaderModule & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( "VoxelSecondaryBounce"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, computeShader )
					, layout ) );
		}

		static ShaderPtr createShader( uint32_t voxelGridSize
			, RenderSystem const & renderSystem )
		{
			sdw::ComputeWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			// Inputs
			auto voxels( writer.declArrayStorageBuffer< shader::Voxel >( "voxels"
				, Bindings::eVoxelBuffer
				, 0u ) );
			C3D_Voxelizer( writer, Bindings::eVoxelConfig, 0u, true );
			auto firstBounce( writer.declCombinedImg< FImg3DRgba32 >( "firstBounce"
				, Bindings::eFirstBounce
				, 0u ) );

			// Outputs
			auto output( writer.declStorageImg< RWFImg3DRgba32 >( "output"
				, Bindings::eResult
				, 0u ) );

			shader::Utils utils{ writer };
			shader::GlobalIllumination indirect{ writer, utils };

			writer.implementMainT< sdw::VoidT >( 64u, [&writer, &output, &voxels, &firstBounce, &c3d_voxelData
				, &utils, &indirect, voxelGridSize]( sdw::ComputeIn const & in )
				{
					auto coord = writer.declLocale( "coord"
						, ivec3( utils.unflatten( in.globalInvocationID.x()
							, uvec3( sdw::UInt{ voxelGridSize } ) ) ) );
					auto clip = writer.declLocale( "clip"
						, vec3( c3d_voxelData.gridToClip ) * vec3( coord ) );
					auto color = writer.declLocale( "color"
						, firstBounce.lod( clip, 0.0_f ) );

					sdwIF( writer, color.a() > 0.0_f )
					{
						auto normal = writer.declLocale( "normal"
							, utils.decodeNormal( voxels[in.globalInvocationID.x()].normalMask() ) );
						// [0.5,gridSize.5] => [0,1]
						// center of voxel, to apply normal on it.
						auto position = writer.declLocale( "position"
							, ( vec3( coord ) + vec3( 0.5_f ) ) * c3d_voxelData.gridToClip );
						// [0,1] => [-1,1]
						position = position * 2.0f - 1.0f;
						position.y() *= -1.0f;
						// [-1,1] => [-gridSize/2,gridSize/2]
						position *= c3d_voxelData.clipToGrid / 2.0f;
						// to world
						position *= c3d_voxelData.gridToWorld;

						auto radiance = writer.declLocale( "radiance"
							, indirect.traceConeRadiance( firstBounce
								, normal
								, position
								, c3d_voxelData ) );
						output.store( coord, vec4( color.rgb() + radiance.rgb(), color.a() ) );
					}
					sdwELSE
					{
						output.store( coord, vec4( 0.0_f ) );
					}
					sdwFI

					voxels[in.globalInvocationID.x()].normalMask() = 0_u;
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	VoxelSecondaryBounce::VoxelSecondaryBounce( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, VctConfig const & vctConfig
		, crg::RunnablePass::IsEnabledCallback isEnabled )
		: crg::RunnablePass{ pass
			, context
			, graph
			, { crg::defaultV< crg::RunnablePass::InitialiseCallback >
				, GetPipelineStateCallback( [](){ return crg::getPipelineState( PipelineStageFlags::eComputeShader ); } )
				, [this]( crg::RecordContext & ctx, VkCommandBuffer cb, uint32_t i ){ doRecordInto( ctx, cb, i ); }
				, crg::defaultV< crg::RunnablePass::GetPassIndexCallback >
				, c3d::move( isEnabled )
				, IsComputePassCallback( [this](){ return doIsComputePass(); } ) }
			, crg::ru::Config{ 1u, false }.implicitAction( pass.getOutputs().begin()->second->view()
				, crg::RecordContext::clearAttachment( pass.getOutputs().begin()->second->view(), transparentBlackClearColor ) ) }
		, m_vctConfig{ vctConfig }
		, m_shader{ VK_SHADER_STAGE_COMPUTE_BIT, cuT( "VoxelSecondaryBounce" ), vxlscnd::createShader( m_vctConfig.gridSize.value(), device.renderSystem ) }
		, m_descriptorSetLayout{ vxlscnd::createDescriptorLayout( device ) }
		, m_pipelineLayout{ vxlscnd::createPipelineLayout( device, *m_descriptorSetLayout ) }
		, m_pipeline{ vxlscnd::createPipeline( device, *m_pipelineLayout, m_shader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ vxlscnd::createDescriptorSet( getGraph(), *m_descriptorSetPool, getPass() ) }
	{
	}

	void VoxelSecondaryBounce::accept( RenderTechniqueVisitor & visitor )const
	{
		visitor.visit( m_shader );
	}

	void VoxelSecondaryBounce::doRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )const
	{
		auto voxelGridSize = m_vctConfig.gridSize.value();
		VkDescriptorSet descriptorSet = *m_descriptorSet;
		auto view = getPass().getOutputs().begin()->second->view( index );
		auto layoutState = getLayoutState( view );
		auto image = getGraph().createImage( view.data->image );
		auto color = convert( transparentBlackClearColor );
		auto subresourceRange = convert( view.data->info.subresourceRange );

		// Clear result
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

		context->vkCmdBindPipeline( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipeline );
		context->vkCmdBindDescriptorSets( commandBuffer
			, VK_PIPELINE_BIND_POINT_COMPUTE
			, *m_pipelineLayout
			, 0u
			, 1u
			, &descriptorSet
			, 0u
			, nullptr );
		context->vkCmdDispatch( commandBuffer, voxelGridSize * voxelGridSize * voxelGridSize / 64u, 1u, 1u );
	}

	bool VoxelSecondaryBounce::doIsComputePass()const
	{
		return true;
	}

	//*********************************************************************************************
}
