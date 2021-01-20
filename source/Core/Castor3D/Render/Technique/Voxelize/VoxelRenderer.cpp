#include "Castor3D/Render/Technique/Voxelize/VoxelRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslVoxel.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>
#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Core/Device.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineDepthStencilStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineMultisampleStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineRasterizationStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <ShaderWriter/Source.hpp>

#include <algorithm>

CU_ImplementCUSmartPtr( castor3d, VoxelRenderer )

namespace castor3d
{
	namespace
	{
		TextureUnit createTexture( RenderDevice const & device
			, ashes::ImageView const & colourView
			, castor::String const & name
			, VkFormat format
			, VkImageUsageFlags usage )
		{
			auto & renderSystem = device.renderSystem;
			auto & engine = *renderSystem.getEngine();
			auto size = colourView.image->getDimensions();
			ashes::ImageCreateInfo image{ 0u
				, VK_IMAGE_TYPE_2D
				, format
				, size
				, 1u
				, 1u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, usage };
			auto layout = std::make_shared< TextureLayout >( renderSystem
				, std::move( image )
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "VoxelRendererDepth" );
			TextureUnit result{ engine };
			result.setSampler( createSampler( engine, name, VK_FILTER_NEAREST, nullptr ) );
			result.setTexture( layout );
			result.initialise( device );
			return result;
		}

		TextureUnit createDepthBuffer( RenderDevice const & device
			, ashes::ImageView const & colourView )
		{
			return createTexture( device
				, colourView
				, "VoxelRendererDepth"
				, VK_FORMAT_D16_UNORM
				, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		ashes::RenderPassPtr createRenderPass( RenderDevice const & device
			, castor::String const & name
			, ashes::ImageView const & color
			, ashes::ImageView const & depth )
		{
			ashes::VkAttachmentDescriptionArray attaches{ { 0u
					, color.getFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
				, { 0u
					, depth.getFormat()
					, VK_SAMPLE_COUNT_1_BIT
					, VK_ATTACHMENT_LOAD_OP_CLEAR
					, VK_ATTACHMENT_STORE_OP_STORE
					, VK_ATTACHMENT_LOAD_OP_DONT_CARE
					, VK_ATTACHMENT_STORE_OP_DONT_CARE
					, VK_IMAGE_LAYOUT_UNDEFINED
					, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } };
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription{ 0u
					, VK_PIPELINE_BIND_POINT_GRAPHICS
					, {}
					, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
					, {}
					, VkAttachmentReference{ 1u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
					, {} } );
			ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
					, 0u
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_DEPENDENCY_BY_REGION_BIT }
				, { 0u
					, VK_SUBPASS_EXTERNAL
					, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
					, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
					, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
					, VK_ACCESS_SHADER_READ_BIT
					, VK_DEPENDENCY_BY_REGION_BIT } };
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			return device->createRenderPass( name
				, std::move( createInfo ) );
		}

		ashes::FrameBufferPtr createFramebuffer( ashes::RenderPass const & renderPass
			, castor::String const & name
			, ashes::ImageView const & color
			, ashes::ImageView const & depth )
		{
			ashes::ImageViewCRefArray fbAttaches;
			fbAttaches.emplace_back( color );
			fbAttaches.emplace_back( depth );
			return renderPass.createFrameBuffer( name
				, { color.image->getDimensions().width, color.image->getDimensions().height }
			, std::move( fbAttaches ) );
		}

		ashes::VertexBufferPtr< castor::Point3f > createVertexBuffer( RenderDevice const & device )
		{
			std::vector< castor::Point3f > vertex
			{
				castor::Point3f{ +1.000000, -1.000000, -1.000000 },
				castor::Point3f{ +1.000000, -1.000000, +1.000000 },
				castor::Point3f{ -1.000000, -1.000000, +1.000000 },
				castor::Point3f{ -1.000000, -1.000000, -1.000000 },
				castor::Point3f{ +1.000000, +1.000000, -1.000000 },
				castor::Point3f{ +1.000000, +1.000000, +1.000000 },
				castor::Point3f{ -1.000000, +1.000000, +1.000000 },
				castor::Point3f{ -1.000000, +1.000000, -1.000000 },
			};
			std::vector< castor::Point3f > vertexData
			{
				vertex[1], vertex[2], vertex[3],
				vertex[7], vertex[6], vertex[5],
				vertex[0], vertex[4], vertex[5],
				vertex[1], vertex[5], vertex[6],
				vertex[6], vertex[7], vertex[3],
				vertex[0], vertex[3], vertex[7],
				vertex[0], vertex[1], vertex[3],
				vertex[4], vertex[7], vertex[5],
				vertex[1], vertex[0], vertex[5],
				vertex[2], vertex[1], vertex[6],
				vertex[2], vertex[6], vertex[3],
				vertex[4], vertex[0], vertex[7],
			};
			auto result = makeVertexBuffer< castor::Point3f >( device
				, uint32_t( vertexData.size() )
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "VoxelCube" );

			ashes::StagingBuffer stagingBuffer{ *device.device
				, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
				, result->getBuffer().getSize() };
			stagingBuffer.uploadVertexData( *device.graphicsQueue
				, *device.graphicsCommandPool
				, vertexData
				, *result );
			return result;
		}

		namespace vx2scr
		{
			enum IDs : uint32_t
			{
				eVoxelUbo,
				eVoxelizedScene,
			};

			ShaderPtr getVertexProgram( uint32_t voxelGridSize )
			{
				using namespace sdw;
				VertexWriter writer;

				// Shader inputs
				UBO_VOXELIZER( writer, eVoxelUbo, 0u );
				auto voxels( writer.declImage< RWFImg3DRgba32 >( "voxels"
					, eVoxelizedScene
					, 0u ) );
				auto in = writer.getIn();

				// Shader outputs
				auto outVoxelColor = writer.declOutput< Vec4 >( "outVoxelColor", 0u );
				auto out = writer.getOut();

				shader::Utils utils{ writer };
				utils.declareVoxelizeFunctions();

				writer.implementFunction< void >( "main"
					, [&]()
					{
						auto voxelPosition = writer.declLocale( "voxelPosition"
							, utils.unflatten( writer.cast< UInt >( in.vertexIndex )
								, uvec3( UInt{ voxelGridSize } ) ) );
						out.vtx.position = vec4( vec3( voxelPosition ), 1.0f );
						outVoxelColor = voxels.load( ivec3( voxelPosition ) );
					} );
				return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			ShaderPtr getGeometryProgram( uint32_t voxelGridSize )
			{
				using namespace sdw;
				GeometryWriter writer;

				writer.inputLayout( stmt::InputLayout::ePointList );
				writer.outputLayout( stmt::OutputLayout::eTriangleStrip, 14u );

				// Shader inputs
				UBO_VOXELIZER( writer, eVoxelUbo, 0u );
				auto inVoxelColor = writer.declInputArray< Vec4 >( "inVoxelColor", 0u, 1u );
				auto in = writer.getIn();

				// Shader outputs
				auto outVoxelColor = writer.declOutput< Vec4 >( "outVoxelColor", 0u );
				auto out = writer.getOut();

				shader::Utils utils{ writer };
				utils.declareVoxelizeFunctions();

				auto createCube = writer.implementFunction< Vec3 >( "createCube"
					, [&]( UInt const & vertexID )
					{
						auto b = writer.declLocale( "b"
							, 1_u << vertexID );
						writer.returnStmt( vec3( writer.ternary( ( 0x287a_u & b ) != 0_u, 1.0_f, 0.0_f )
							, writer.ternary( ( 0x02af_u & b ) != 0_u, 1.0_f, 0.0_f )
							, writer.ternary( ( 0x31e3_u & b ) != 0_u, 1.0_f, 0.0_f ) ) );
					}
				, InUInt{ writer, "vertexID" } );

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						IF( writer, inVoxelColor[0].a() > 0.0_f )
						{
							FOR( writer, UInt, i, 0u, i < 14u, ++i )
							{
								auto pos = writer.declLocale( "pos"
									, in.vtx[0].position.xyz() );
								outVoxelColor = inVoxelColor[0];

								pos = pos / c3d_voxelResolution * 2.0f - 1.0f;
								pos.y() = -pos.y();
								pos *= c3d_voxelResolution;
								pos += ( createCube( i ) - vec3( 0.0_f, 1.0f, 0.0f ) ) * 2.0f;
								pos *= c3d_voxelResolution * c3d_voxelSize / c3d_voxelResolution;

								out.vtx.position = c3d_voxelTransform * vec4( pos, 1.0f );

								EmitVertex( writer );
							}
							ROF;

							EndPrimitive( writer );
						}
						FI;
					} );
				return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			ShaderPtr getPixelProgram()
			{
				using namespace sdw;
				FragmentWriter writer;

				// Shader inputs
				auto inVoxelColor = writer.declInput< Vec4 >( "inVoxelColor", 0u );

				// Shader outputs
				auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0u );

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						pxl_fragColor = inVoxelColor;
					} );
				return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
			}

			ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
			{
				ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBinding( eVoxelUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT )
					, makeDescriptorSetLayoutBinding( eVoxelizedScene
						, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
						, VK_SHADER_STAGE_VERTEX_BIT ) };
				return device->createDescriptorSetLayout( std::move( bindings ) );
			}

			ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
				, ashes::DescriptorSetLayout const & dslayout )
			{
				return device->createPipelineLayout( "VoxelRenderer"
					, ashes::DescriptorSetLayoutCRefArray{ std::ref( dslayout ) } );
			}

			ashes::GraphicsPipelinePtr createPipeline( RenderDevice const & device
				, ashes::PipelineLayout const & layout
				, ashes::RenderPass const & renderPass
				, ShaderModule const & vertexShader
				, ShaderModule const & geometryShader
				, ShaderModule const & pixelShader
				, ashes::ImageView const & target )
			{
				ashes::PipelineShaderStageCreateInfoArray program;
				program.push_back( makeShaderState( device, vertexShader ) );
				program.push_back( makeShaderState( device, geometryShader ) );
				program.push_back( makeShaderState( device, pixelShader ) );
				// Initialise the pipeline.
				VkViewport viewport{ 0.0f, 0.0f, float( target.image->getDimensions().width ), float( target.image->getDimensions().height ) };
				VkRect2D scissor{ 0, 0, target.image->getDimensions().width, target.image->getDimensions().height };
				return device->createPipeline( "VoxelRenderer"
					, ashes::GraphicsPipelineCreateInfo( 0u
						, program
						, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
						, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_POINT_LIST }
						, ashes::nullopt
						, ashes::PipelineViewportStateCreateInfo{ 0u, 1u, ashes::VkViewportArray{ viewport }, 1u, ashes::VkScissorArray{ scissor } }
						, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE }
						, ashes::PipelineMultisampleStateCreateInfo{}
						, ashes::PipelineDepthStencilStateCreateInfo{}
						, RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
						, ashes::nullopt
						, layout
						, static_cast< VkRenderPass const & >( renderPass ) ) );
			}

			ashes::DescriptorSetPtr createDescriptorSet( ashes::DescriptorSetPool const & pool
				, UniformBufferOffsetT< VoxelizerUboConfiguration > const & voxelizerUbo
				, TextureUnit const & voxels )
			{
				auto descriptorSet = pool.createDescriptorSet( "VoxelRenderer" );
				voxelizerUbo.createSizedBinding( *descriptorSet, pool.getLayout().getBinding( eVoxelUbo ) );
				descriptorSet->createBinding( pool.getLayout().getBinding( eVoxelizedScene )
					, voxels.getTexture()->getDefaultView().getSampledView() );
				descriptorSet->update();
				return descriptorSet;
			}

			CommandsSemaphore createCommandBuffer( RenderDevice const & device
				, ashes::RenderPass const & renderPass
				, ashes::FrameBuffer const & frameBuffer
				, ashes::PipelineLayout const & pipelineLayout
				, ashes::GraphicsPipeline const & pipeline
				, ashes::DescriptorSet const & descriptorSet
				, uint32_t voxelGridSize )
			{
				CommandsSemaphore result{ device, "VoxelRenderer" };
				auto & cb = *result.commandBuffer;
				cb.begin();
				cb.beginDebugBlock( { "Render voxels"
					, makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ) } );
				cb.beginRenderPass( renderPass
					, frameBuffer
					, { opaqueBlackClearColor, defaultClearDepthStencil }
				, VK_SUBPASS_CONTENTS_INLINE );
				cb.bindPipeline( pipeline );
				cb.bindDescriptorSet( descriptorSet, pipelineLayout );
				cb.draw( voxelGridSize * voxelGridSize * voxelGridSize );
				cb.endRenderPass();
				cb.endDebugBlock();
				cb.end();

				return result;
			}
		}
	}

	VoxelRenderer::VoxelToScreen::VoxelToScreen( RenderDevice const & device
		, UniformBufferOffsetT< VoxelizerUboConfiguration > const & voxelizerUbo
		, TextureUnit const & voxels
		, ashes::ImageView color
		, ashes::ImageView depth
		, uint32_t voxelGridSize )
		: castor::Named{ "VoxelRenderer" }
		, descriptorSetLayout{ vx2scr::createDescriptorLayout( device ) }
		, pipelineLayout{ vx2scr::createPipelineLayout( device, *descriptorSetLayout ) }
		, renderPass{ createRenderPass( device, getName(), color, depth ) }
		, vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), vx2scr::getVertexProgram( voxelGridSize ) }
		, geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), vx2scr::getGeometryProgram( voxelGridSize ) }
		, pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), vx2scr::getPixelProgram() }
		, pipeline{ vx2scr::createPipeline( device, *pipelineLayout, *renderPass, vertexShader, geometryShader, pixelShader, color ) }
		, frameBuffer{ createFramebuffer( *renderPass, getName(), color, depth ) }
		, descriptorSetPool{ descriptorSetLayout->createPool( 1u ) }
		, descriptorSet{ vx2scr::createDescriptorSet( *descriptorSetPool, voxelizerUbo, voxels ) }
		, commands{ vx2scr::createCommandBuffer( device, *renderPass, *frameBuffer, *pipelineLayout, *pipeline, *descriptorSet, voxelGridSize ) }
	{
	}

	VoxelRenderer::VoxelRenderer( RenderDevice const & device
		, UniformBufferOffsetT< VoxelizerUboConfiguration > const & voxelizerUbo
		, TextureUnit const & voxels
		, ashes::ImageView target
		, uint32_t voxelGridSize )
		: m_renderSystem{ device.renderSystem }
		, m_device{ device }
		, m_depthBuffer{ createDepthBuffer( device, target ) }
		, m_voxelToScreen{ device, voxelizerUbo, voxels, target, m_depthBuffer.getTexture()->getDefaultView().getTargetView(), voxelGridSize }
	{
	}

	ashes::Semaphore const & VoxelRenderer::render( RenderDevice const & device
		, ashes::Semaphore const & toWait )
	{
		auto result = &toWait;
		result = &m_voxelToScreen.commands.submit( *device.graphicsQueue, *result );
		return *result;
	}
}
