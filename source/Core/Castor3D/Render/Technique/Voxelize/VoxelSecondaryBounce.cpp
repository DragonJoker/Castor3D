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
			eResult,
			eFirstBounceLevel0,
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
				, makeDescriptorSetLayoutBinding( eResult
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT )
				, makeDescriptorSetLayoutBinding( eFirstBounceLevel0
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_COMPUTE_BIT
					, castor::getBitSize( voxelGridSize ) ) };

			return device->createDescriptorSetLayout( "VoxelSecondaryBounce"
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( ashes::DescriptorSetPool const & pool
			, ashes::Buffer< Voxel > const & voxels
			, VoxelizerUbo const & voxelUbo
			, TextureUnit const & firstBounce
			, TextureUnit const & result
			, uint32_t voxelGridSize )
		{
			auto descriptorSet = pool.createDescriptorSet( "VoxelSecondaryBounce" );
			descriptorSet->createBinding( pool.getLayout().getBinding( eVoxelBuffer )
				, voxels
				, 0u
				, voxels.getCount() );
			voxelUbo.createSizedBinding( *descriptorSet
				, pool.getLayout().getBinding( eVoxelConfig ) );
			descriptorSet->createBinding( pool.getLayout().getBinding( eResult )
				, result.getTexture()->getDefaultView().getSampledView() );

			for ( uint32_t i = 0; i < castor::getBitSize( voxelGridSize ); ++i )
			{
				descriptorSet->createBinding( pool.getLayout().getBinding( eFirstBounceLevel0 )
					, firstBounce.getTexture()->getMipView( i ).getSampledView()
					, i );
			}

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

		CommandsSemaphore createCommandBuffer( RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::ComputePipeline const & pipeline
			, ashes::DescriptorSet const & descriptorSet
			, ashes::BufferBase const & voxels
			, TextureUnit const & firstBounce
			, TextureUnit const & secondaryBounce
			, RenderPassTimer & timer
			, uint32_t voxelGridSize )
		{
			CommandsSemaphore result{ device, "VoxelSecondaryBounce" };
			auto & cmd = *result.commandBuffer;
			cmd.begin();
			timer.beginPass( cmd );
			cmd.beginDebugBlock( { "Voxel Secondary Bounce"
				, makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ) } );
			// Clear result
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_TRANSFER_BIT
				, secondaryBounce.getTexture()->getDefaultView().getSampledView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
			cmd.clear( secondaryBounce.getTexture()->getDefaultView().getSampledView()
				, transparentBlackClearColor.color );
			cmd.memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
				, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, secondaryBounce.getTexture()->getDefaultView().getSampledView().makeGeneralLayout( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
					, VK_ACCESS_SHADER_WRITE_BIT ) );

			cmd.bindPipeline( pipeline );
			cmd.bindDescriptorSet( descriptorSet
				, pipelineLayout
				, VK_PIPELINE_BIND_POINT_COMPUTE );
			cmd.dispatch( voxelGridSize * voxelGridSize * voxelGridSize / 64u, 1u, 1u );

			cmd.memoryBarrier( VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, firstBounce.getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_GENERAL ) );
			secondaryBounce.getTexture()->getTexture().generateMipmaps( cmd
				, VK_IMAGE_LAYOUT_GENERAL
				, VK_IMAGE_LAYOUT_GENERAL
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
			cmd.endDebugBlock();
			timer.endPass( cmd );
			cmd.end();

			return result;
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
			auto firstBounce( writer.declImageArray< RFImg3DRgba32 >( "firstBounce"
				, eFirstBounceLevel0
				, 0u
				, castor::getBitSize( voxelGridSize ) ) );
			auto in = writer.getIn();

			// Outputs
			auto output( writer.declImage< RWFImg3DRgba32 >( "output"
				, eResult
				, 0u ) );

			shader::Utils utils{ writer };
			utils.declareDecodeNormal();
			utils.declareUnflatten();

			shader::GlobalIllumination indirect{ writer, utils };
			indirect.declareTraceConeRadianceImg( "firstBounce" );

			writer.implementMain( [&]()
				{
					auto coord = writer.declLocale( "coord"
						, ivec3( utils.unflatten( in.globalInvocationID.x()
							, uvec3( UInt{ voxelGridSize } ) ) ) );
					auto color = writer.declLocale( "color"
						, firstBounce[0].load( coord ) );

					IF( writer, color.a() > 0.0_f )
					{
						auto normal = writer.declLocale( "normal"
							, utils.decodeNormal( voxels[in.globalInvocationID.x()].normalMask ) );
						auto position = writer.declLocale( "position"
							, ( vec3( coord ) + vec3( 0.5_f ) ) * c3d_voxelData.gridToClip );
						position = position * 2 - 1;
						position.y() *= -1;
						position *= c3d_voxelData.clipToGrid;
						position *= c3d_voxelData.gridToWorld;
						auto surface = writer.declLocale< shader::Surface >( "surface" );
						surface.create( position, normal );
						auto radiance = writer.declLocale( "radiance"
							, indirect.traceConeRadiance( surface, c3d_voxelData ) );
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

	VoxelSecondaryBounce::SecondaryBounce::SecondaryBounce( RenderDevice const & device
		, ashes::PipelineLayout const & pipelineLayout
		, ashes::DescriptorSet const & descriptorSet
		, ashes::Buffer< Voxel > const & voxels
		, VoxelizerUbo const & voxelUbo
		, TextureUnit const & firstBounce
		, TextureUnit const & result
		, RenderPassTimer & timer
		, uint32_t voxelGridSize )
		: computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, "VoxelSecondaryBounce", createShader( voxelGridSize ) }
		, pipeline{ createPipeline( device, pipelineLayout, computeShader ) }
		, commands{ createCommandBuffer( device, pipelineLayout, *pipeline, descriptorSet, voxels.getBuffer(), firstBounce, result, timer, voxelGridSize ) }
	{
	}

	//*********************************************************************************************

	VoxelSecondaryBounce::VoxelSecondaryBounce( RenderDevice const & device
		, VoxelSceneData const & vctConfig
		, ashes::Buffer< Voxel > const & voxels
		, VoxelizerUbo const & voxelUbo
		, TextureUnit const & firstBounce
		, TextureUnit const & result )
		: m_vctConfig{ vctConfig }
		, m_timer{ std::make_shared< RenderPassTimer >( device, "Voxelize", "Secondary Bounce" ) }
		, m_descriptorSetLayout{ createDescriptorLayout( device, vctConfig.gridSize.value() ) }
		, m_pipelineLayout{ createPipelineLayout( device, *m_descriptorSetLayout ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( 1u ) }
		, m_descriptorSet{ createDescriptorSet( *m_descriptorSetPool, voxels, voxelUbo, firstBounce, result, vctConfig.gridSize.value() ) }
		, m_secondaryBounce{ device, *m_pipelineLayout, *m_descriptorSet, voxels, voxelUbo, firstBounce, result, *m_timer, vctConfig.gridSize.value() }
	{
	}

	void VoxelSecondaryBounce::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( m_secondaryBounce.computeShader );
	}

	ashes::Semaphore const & VoxelSecondaryBounce::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		auto result = &toWait;
		auto timerBlock = m_timer->start();
		timerBlock->notifyPassRender();
		result = &m_secondaryBounce.commands.submit( *device.computeQueue, *result );
		return *result;
	}

	//*********************************************************************************************
}