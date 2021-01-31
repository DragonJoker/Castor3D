#include "Castor3D/Render/Technique/Voxelize/VoxelBufferToTexture.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Technique/RenderTechniqueVisitor.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		enum IDs : uint32_t
		{
			eVoxelUbo,
			eVoxels,
			eResult,
		};

		ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBinding( eVoxelUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eVoxels
					, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eResult
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT ) };
			return device->createDescriptorSetLayout( "VoxelBufferToTexture"
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( ashes::DescriptorSetPool const & pool
			, VoxelizerUbo const & voxelizerUbo
			, ashes::Buffer< Voxel > const & voxels
			, TextureUnit const & result )
		{
			auto descriptorSet = pool.createDescriptorSet( "VoxelBufferToTexture" );
			voxelizerUbo.createSizedBinding( *descriptorSet
				, pool.getLayout().getBinding( eVoxelUbo ) );
			descriptorSet->createBinding( pool.getLayout().getBinding( eVoxels )
				, voxels
				, 0u
				, voxels.getCount() );
			descriptorSet->createBinding( pool.getLayout().getBinding( eResult )
				, result.getTexture()->getDefaultView().getSampledView() );
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
			, ashes::PipelineLayout const & layout
			, ShaderModule const & computeShader )
		{
			// Initialise the pipeline.
			return device->createPipeline( "VoxelBufferToTexture"
				, ashes::ComputePipelineCreateInfo( 0u
					, makeShaderState( device, computeShader )
					, layout ) );
		}

		CommandsSemaphore createCommandBuffer( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::ComputePipeline const & pipeline
			, ashes::DescriptorSet const & descriptorSet
			, ashes::BufferBase const & voxels
			, TextureUnit const & vxResult
			, uint32_t voxelGridSize )
		{
			CommandsSemaphore result{ device, "VoxelBufferToTexture" };
			auto & cmd = *result.commandBuffer;
			cmd.begin();
			cmd.beginDebugBlock( { "Copy voxels to texture"
				, makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ) } );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, vxResult.getTexture()->getDefaultView().getSampledView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			cmd.clear( vxResult.getTexture()->getDefaultView().getSampledView()
				, transparentBlackClearColor.color );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, vxResult.getTexture()->getDefaultView().getSampledView().makeGeneralLayout( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, VK_ACCESS_SHADER_WRITE_BIT ) );
			cmd.bindPipeline( pipeline );
			cmd.bindDescriptorSet( descriptorSet
				, pipelineLayout
				, VK_PIPELINE_BIND_POINT_COMPUTE );
			cmd.dispatch( voxelGridSize * voxelGridSize * voxelGridSize / 256, 1u, 1u );
			vxResult.getTexture()->getTexture().generateMipmaps( cmd
				, VK_IMAGE_LAYOUT_GENERAL
				, VK_IMAGE_LAYOUT_GENERAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			cmd.endDebugBlock();
			cmd.end();

			return result;
		}

		ShaderPtr createShader()
		{
			using namespace sdw;
			ComputeWriter writer;

			writer.inputLayout( 256u, 1u, 1u );

			// Inputs
			UBO_VOXELIZER( writer, eVoxelUbo, 0u, true );
			auto voxels( writer.declArrayShaderStorageBuffer< shader::Voxel >( "voxels"
				, eVoxels
				, 0u ) );
			auto in = writer.getIn();

			// Outputs
			auto output( writer.declImage< RWFImg3DRgba32 >( "output"
				, eResult
				, 0u ) );

			shader::Utils utils{ writer };
			utils.declareVoxelizeFunctions();

			writer.implementMain( [&]()
				{
					auto color = writer.declLocale( "color"
						, utils.decodeColor( voxels[in.globalInvocationID.x()].colorMask ) );

					IF( writer, color.a() > 0.0_f )
					{
						auto coord = writer.declLocale( "coord"
							, ivec3( utils.unflatten( in.globalInvocationID.x()
								, uvec3( writer.cast< UInt >( c3d_voxelData.resolution ) ) ) ) );
						output.store( coord, color );
					}
					FI;

					// delete emission data, but keep normals (no need to delete, we will only read normal values of filled voxels)
					voxels[in.globalInvocationID.x()].colorMask = 0_u;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	VoxelBufferToTexture::VoxelBufferToTexture( RenderDevice const & device
		, VoxelizerUbo const & voxelizerUbo
		, ashes::Buffer< Voxel > const & voxels
		, TextureUnit const & result
		, uint32_t voxelGridSize )
		: m_descriptorSetLayout{ createDescriptorLayout( device ) }
		, m_pipelineLayout{ createPipelineLayout( device, *m_descriptorSetLayout ) }
		, m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, "VoxelBufferToTexture", createShader() }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, m_computeShader ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( *m_descriptorSetPool, voxelizerUbo, voxels, result ) }
		, m_commands{ createCommandBuffer( device, *m_pipelineLayout, *m_pipeline, *m_descriptorSet, voxels.getBuffer(), result, voxelGridSize ) }
	{
	}

	void VoxelBufferToTexture::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_computeShader );
	}

	ashes::Semaphore const & VoxelBufferToTexture::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		auto result = &toWait;
		result = &m_commands.submit( *device.computeQueue, *result );
		device.computeQueue->waitIdle();
		return *result;
	}

	//*********************************************************************************************
}
