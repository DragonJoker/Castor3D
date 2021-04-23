#include "Castor3D/Render/ToTexture/Texture3DTo2D.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

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

CU_ImplementCUSmartPtr( castor3d, Texture3DTo2D )

#define UBO_GRID( Writer, Binding )\
	auto ubo = Writer.declUniformBuffer<>( "ubo", Binding, 0u );\
	auto gridCenterCellSize = ubo.declMember< sdw::Vec4 >( "gridCenter" );\
	auto gridSize = ubo.declMember< sdw::UInt >( "gridSize" );\
	ubo.end();\
	auto gridCenter = gridCenterCellSize.xyz();\
	auto cellSize = gridCenterCellSize.w()

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		enum IDs : uint32_t
		{
			eGridUbo,
			eMatrixUbo,
			eSource,
		};

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
				, name );
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
				, "Texture3DToTexture2DDepth"
				, VK_FORMAT_D32_SFLOAT
				, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT );
		}

		ashes::ImagePtr createTarget( RenderDevice const & device
			, VkExtent2D const & size )
		{
			ashes::ImageCreateInfo createInfo{ 0u
				, VK_IMAGE_TYPE_2D
				, VK_FORMAT_R8G8B8A8_UNORM
				, { size.width, size.height, 1u }
				, 1u
				, 1u
				, VK_SAMPLE_COUNT_1_BIT
				, VK_IMAGE_TILING_OPTIMAL
				, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT };
			return makeImage( device
				, createInfo
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, "Texture3DToTexture2DColor" );
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

		ashes::DescriptorSetLayoutPtr createDescriptorLayout( RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings{ makeDescriptorSetLayoutBinding( eGridUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT )
				, makeDescriptorSetLayoutBinding( eMatrixUbo
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_GEOMETRY_BIT )
				, makeDescriptorSetLayoutBinding( eSource
					, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
					, VK_SHADER_STAGE_VERTEX_BIT ) };
			return device->createDescriptorSetLayout( "Texture3DTo2D"
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr createDescriptorSet( ashes::DescriptorSetPool const & pool
			, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
			, MatrixUbo const & matrixUbo
			, ashes::ImageView const & texture3D )
		{
			auto descriptorSet = pool.createDescriptorSet( "Texture3DTo2D" );
			uniformBuffer.createSizedBinding( *descriptorSet
				, pool.getLayout().getBinding( eGridUbo ) );
			matrixUbo.createSizedBinding( *descriptorSet
				, pool.getLayout().getBinding( eMatrixUbo ) );
			descriptorSet->createBinding( pool.getLayout().getBinding( eSource )
				, texture3D );
			descriptorSet->update();
			return descriptorSet;
		}

		ashes::PipelineLayoutPtr createPipelineLayout( RenderDevice const & device
			, ashes::DescriptorSetLayout const & dslayout )
		{
			return device->createPipelineLayout( "Texture3DTo2D"
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
			VkViewport viewport{ 0.0f, 0.0f, float( target.image->getDimensions().width ), float( target.image->getDimensions().height ), 0.0f, 1.0f };
			VkRect2D scissor{ 0, 0, target.image->getDimensions().width, target.image->getDimensions().height };
			return device->createPipeline( "Texture3DTo2D"
				, ashes::GraphicsPipelineCreateInfo( 0u
					, program
					, ashes::PipelineVertexInputStateCreateInfo{ 0u, {}, {} }
					, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_POINT_LIST }
					, ashes::nullopt
					, ashes::PipelineViewportStateCreateInfo{ 0u, 1u, ashes::VkViewportArray{ viewport }, 1u, ashes::VkScissorArray{ scissor } }
					, ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT }
					, ashes::PipelineMultisampleStateCreateInfo{}
					, ashes::PipelineDepthStencilStateCreateInfo{}
					, SceneRenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u )
					, ashes::nullopt
					, layout
					, static_cast< VkRenderPass const & >( renderPass ) ) );
		}

		CommandsSemaphore createCommandBuffer( RenderDevice const & device
			, ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::GraphicsPipeline const & pipeline
			, ashes::DescriptorSet const & descriptorSet
			, IntermediateView const & view )
		{
			auto textureSize = view.view.image->getDimensions().width;
			CommandsSemaphore result{ device, "Texture3DTo2D" };
			auto & cmd = *result.commandBuffer;
			cmd.begin();
			cmd.beginDebugBlock( { "Texture3D To Texture2D"
				, makeFloatArray( device.renderSystem.getEngine()->getNextRainbowColour() ) } );

			if ( view.layout != VK_IMAGE_LAYOUT_GENERAL )
			{
				cmd.memoryBarrier( ashes::getStageMask( view.layout )
					, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, view.view.makeLayoutTransition( view.layout
						, VK_IMAGE_LAYOUT_GENERAL
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			cmd.beginRenderPass( renderPass
				, frameBuffer
				, { opaqueBlackClearColor, defaultClearDepthStencil }
				, VK_SUBPASS_CONTENTS_INLINE );
			cmd.bindPipeline( pipeline );
			cmd.bindDescriptorSet( descriptorSet, pipelineLayout );
			cmd.draw( textureSize * textureSize * textureSize );
			cmd.endRenderPass();

			if ( view.layout != VK_IMAGE_LAYOUT_GENERAL )
			{
				cmd.memoryBarrier( VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
					, ashes::getStageMask( view.layout )
					, view.view.makeLayoutTransition( VK_IMAGE_LAYOUT_GENERAL
						, view.layout
						, VK_QUEUE_FAMILY_IGNORED
						, VK_QUEUE_FAMILY_IGNORED ) );
			}

			cmd.endDebugBlock();
			cmd.end();

			return result;
		}

		ShaderPtr getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_GRID( writer, eGridUbo );
			auto inSource( writer.declImage< RWFImg3DRgba32 >( "inSource"
				, eSource
				, 0u ) );
			auto in = writer.getIn();

			// Shader outputs
			auto outVoxelColor = writer.declOutput< Vec4 >( "outVoxelColor", 0u );
			auto out = writer.getOut();

			shader::Utils utils{ writer };
			utils.declareUnflatten();

			writer.implementFunction< void >( "main"
				, [&]()
				{
					auto coord = writer.declLocale( "coord"
						, utils.unflatten( writer.cast< UInt >( in.vertexIndex )
							, uvec3( gridSize ) ) );
					out.vtx.position = vec4( vec3( coord ), 1.0f );

					outVoxelColor = inSource.load( ivec3( coord ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getGeometryProgram()
		{
			using namespace sdw;
			GeometryWriter writer;

			writer.inputLayout( stmt::InputLayout::ePointList );
			writer.outputLayout( stmt::OutputLayout::eTriangleStrip, 14u );

			// Shader inputs
			UBO_GRID( writer, eGridUbo );
			UBO_MATRIX( writer, eMatrixUbo, 0u );
			auto inVoxelColor = writer.declInputArray< Vec4 >( "inVoxelColor", 0u, 1u );
			auto in = writer.getIn();

			// Shader outputs
			auto outVoxelColor = writer.declOutput< Vec4 >( "outVoxelColor", 0u );
			auto out = writer.getOut();

			// Creates a unit cube triangle strip from just vertex ID (14 vertices)
			auto createCube = writer.implementFunction< Vec3 >( "createCube"
				, [&]( UInt const vertexID )
				{
					auto b = writer.declLocale( "b"
						, 1_u << vertexID );
					writer.returnStmt( vec3( writer.ternary( ( 0x287a_u & b ) != 0, 1.0_f, 0.0_f )
						, writer.ternary( ( 0x02af_u & b ) != 0, 1.0_f, 0.0_f )
						, writer.ternary( ( 0x31e3_u & b ) != 0, 1.0_f, 0.0_f ) ) );
				}
				, InUInt{ writer, "vertexID" } );

			writer.implementFunction< void >( "main"
				, [&]()
				{
					IF( writer, inVoxelColor[0].a() > 0 )
					{
						FOR( writer, UInt, i, 0_u, i < 14_u, ++i )
						{
							// [0, 1] => [0, 2] (y => [-1, 1])
							auto cubeVtxPos = writer.declLocale( "cubeVtxPos"
								, ( createCube( i ) - vec3( 0.0_f, 1.0f, 0.0f ) ) * 2.0f );

							// [0, gridSize]
							auto pos = writer.declLocale( "pos"
								, in.vtx[0].position.xyz() );
							// [0, gridSize] => [0, 1] => [-1, 1]
							pos = pos / writer.cast< Float >( gridSize ) * 2 - 1;
							pos.y() = -pos.y();
							// [-1, 1] => [-gridSize, gridSize]
							pos *= writer.cast< Float >( gridSize );
							// Offset by cube position
							pos += cubeVtxPos;
							pos *= ( writer.cast< Float >( gridSize ) * ( 1.0_f / cellSize ) ) / writer.cast< Float >( gridSize );

							outVoxelColor = inVoxelColor[0];
							out.vtx.position = c3d_matrixData.worldToCurProj( vec4( pos, 1.0f ) );

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
					IF( writer, inVoxelColor.a() > 0.0_f )
					{
						pxl_fragColor = inVoxelColor;
					}
					ELSE
					{
						writer.discard();
					}
					FI;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	Texture3DTo2D::Texture3DToScreen::Texture3DToScreen( RenderDevice const & device
		, UniformBufferOffsetT< Texture3DTo2DData > const & uniformBuffer
		, MatrixUbo const & matrixUbo
		, IntermediateView const & texture3D
		, ashes::RenderPass const & renderPass
		, ashes::DescriptorSetPool const & descriptorSetPool
		, ashes::FrameBuffer const & frameBuffer
		, ashes::PipelineLayout const & pipelineLayout
		, ashes::GraphicsPipeline const & pipeline )
		: descriptorSet{ createDescriptorSet( descriptorSetPool, uniformBuffer, matrixUbo, texture3D.view ) }
		, commands{ createCommandBuffer( device, renderPass, frameBuffer, pipelineLayout, pipeline, *descriptorSet, texture3D ) }
	{
	}

	Texture3DTo2D::Texture3DToScreen::Texture3DToScreen()
	{
	}

	//*********************************************************************************************

	Texture3DTo2D::Texture3DTo2D( RenderDevice const & device
		, VkExtent2D const & size
		, MatrixUbo const & matrixUbo )
		: m_renderSystem{ device.renderSystem }
		, m_device{ device }
		, m_matrixUbo{ matrixUbo }
		, m_target{ createTarget( device, size ) }
		, m_targetView{ m_target->createView( "Texture3DTo2D"
			, VK_IMAGE_VIEW_TYPE_2D
			, VK_FORMAT_R8G8B8A8_UNORM ) }
		, m_depthBuffer{ createDepthBuffer( device, m_targetView ) }
		, m_uniformBuffer{ device.uboPools->getBuffer< Texture3DTo2DData >( 0u ) }
		, m_descriptorSetLayout{ createDescriptorLayout( device ) }
		, m_pipelineLayout{ createPipelineLayout( device, *m_descriptorSetLayout ) }
		, m_renderPass{ createRenderPass( device, "Texture3DTo2D", m_targetView, m_depthBuffer.getTexture()->getDefaultView().getTargetView() ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "Texture3DTo2D", getVertexProgram() }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, "Texture3DTo2D", getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "Texture3DTo2D", getPixelProgram() }
		, m_pipeline{ createPipeline( device, *m_pipelineLayout, *m_renderPass, m_vertexShader, m_geometryShader, m_pixelShader, m_targetView ) }
		, m_frameBuffer{ createFramebuffer( *m_renderPass, "Texture3DTo2D", m_targetView, m_depthBuffer.getTexture()->getDefaultView().getTargetView() ) }
	{
	}

	Texture3DTo2D::~Texture3DTo2D()
	{
		m_device.uboPools->putBuffer( m_uniformBuffer );
	}

	void Texture3DTo2D::createPasses( IntermediateViewArray intermediates )
	{
		m_textures = std::move( intermediates );
		initialise();
	}

	void Texture3DTo2D::initialise()
	{
		m_descriptorSetPool = m_descriptorSetLayout->createPool( m_textures.size() );

		for ( auto & intermediate : m_textures )
		{
			if ( intermediate.view.image->getType() == VK_IMAGE_TYPE_3D )
			{
				m_texture3DToScreen.emplace_back( m_device
					, m_uniformBuffer
					, m_matrixUbo
					, intermediate
					, *m_renderPass
					, *m_descriptorSetPool
					, *m_frameBuffer
					, *m_pipelineLayout
					, *m_pipeline );
			}
			else
			{
				m_texture3DToScreen.emplace_back();
			}
		}
	}

	void Texture3DTo2D::update( CpuUpdater & updater )
	{
		m_index = updater.combineIndex;

		if ( updater.cellSize != 0.0f )
		{
			auto & data = m_uniformBuffer.getData();
			data.gridCenterCellSize = castor::Point4f{ updater.gridCenter->x
				, updater.gridCenter->y
				, updater.gridCenter->z
				, updater.cellSize };
			data.gridSize = m_textures[m_index].view.image->getDimensions().width;
		}
	}

	ashes::Semaphore const & Texture3DTo2D::render( ashes::Semaphore const & toWait )
	{
		auto result = &toWait;

		if ( m_texture3DToScreen[m_index].commands.commandBuffer )
		{
			result = &m_texture3DToScreen[m_index].commands.submit( *m_device.graphicsQueue, *result );
		}

		return *result;
	}

	//*********************************************************************************************
}
