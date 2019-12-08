#include "Castor3D/RenderToTexture/TextureProjection.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Mesh/Vertex.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Texture/Sampler.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/Shader/ShaderModule.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static uint32_t constexpr MtxUboIdx = 0u;
		static uint32_t constexpr MdlMtxUboIdx = 1u;
		static uint32_t constexpr DifImgIdx = 2u;
	}

	TextureProjection::TextureProjection( Engine & engine )
		: OwnedBy< Engine >{ engine }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_sizePushConstant{ VK_SHADER_STAGE_FRAGMENT_BIT, { { 0u, VK_FORMAT_R32G32_SFLOAT } } }
	{
		m_sampler = engine.getSamplerCache().add( cuT( "TextureProjection" ) );
		m_sampler->setMinFilter( VK_FILTER_LINEAR );
		m_sampler->setMagFilter( VK_FILTER_LINEAR );
		m_sampler->setWrapS( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
		m_sampler->setWrapT( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
		m_sampler->setWrapR( VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );
	}

	TextureProjection::~TextureProjection()
	{
	}

	void TextureProjection::initialise( ashes::ImageView const & source
		, VkFormat targetColour
		, VkFormat targetDepth )
	{
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				targetDepth,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			},
			{
				0u,
				targetColour,
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_LOAD,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			},
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
				{},
			} );
		ashes::VkSubpassDependencyArray dependencies
		{
			{
				VK_SUBPASS_EXTERNAL,
				0u,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			},
			{
				0u,
				VK_SUBPASS_EXTERNAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				VK_DEPENDENCY_BY_REGION_BIT,
			}
		};
		ashes::RenderPassCreateInfo createInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		m_renderPass = device->createRenderPass( std::move( createInfo ) );
		setDebugObjectName( device, *m_renderPass, "TextureProjectionRenderPass" );

		m_sampler->initialise();
		auto program = doInitialiseShader();
		doInitialiseVertexBuffer();
		doInitialisePipeline( program, source, *m_renderPass );
	}

	void TextureProjection::cleanup()
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
		m_matrixUbo.cleanup();
		m_modelMatrixUbo.cleanup();
		m_sampler.reset();
		m_vertexBuffer.reset();
		m_descriptorSet.reset();
		m_descriptorPool.reset();
		m_descriptorLayout.reset();
	}

	void TextureProjection::doPrepareFrame()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );

		if ( !m_commandBuffer )
		{
			m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_SECONDARY );
			m_finished = device->createSemaphore();
		}
		else
		{
			m_commandBuffer->reset();
		}

		*m_sizePushConstant.getData() = Point2f{ m_size.getWidth()
			, m_size.getHeight() };

		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->setViewport( makeViewport( m_size ) );
		m_commandBuffer->setScissor( makeScissor( m_size ) );
		m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		m_commandBuffer->pushConstants( *m_pipelineLayout, m_sizePushConstant );
		m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		m_commandBuffer->draw( 36u );
		m_commandBuffer->end();
	}

	void TextureProjection::update( Camera const & camera )
	{
		static castor::Matrix3x3f const Identity{ 1.0f };
		auto node = camera.getParent();
		matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		m_matrixUbo.update( camera.getView()
			, camera.getProjection() );
		m_modelMatrixUbo.update( m_mtxModel, Identity );

		if ( m_size != camera.getSize() )
		{
			m_size = camera.getSize();
			doPrepareFrame();
		}
	}

	ashes::PipelineShaderStageCreateInfoArray TextureProjection::doInitialiseShader()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "TextureProj" };
		{
			using namespace sdw;
			VertexWriter writer;

			// Inputs
			auto position = writer.declInput< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, MtxUboIdx, 0 );
			UBO_MODEL_MATRIX( writer, MdlMtxUboIdx, 0 );

			// Outputs
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				out.gl_out.gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_curMtxModel * vec4( position, 1.0_f ) ).xyww();
			} );

			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "TextureProj" };
		{
			using namespace sdw;
			FragmentWriter writer;

			// Inputs
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" ), DifImgIdx, 0u );
			Pcb pcb{ writer, "SizePCB", ast::type::MemoryLayout::eStd140 };
			auto c3d_size = pcb.declMember< Vec2 >( cuT( "c3d_size" ) );
			pcb.end();
			auto in = writer.getIn();

			shader::Utils utils{ writer, renderSystem.isTopDown() };
			utils.declareInvertVec2Y();

			// Outputs
			auto pxl_FragColor = writer.declOutput< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" ), [&]()
			{
				pxl_FragColor = texture( c3d_mapDiffuse
					, utils.bottomUpToTopDown( in.gl_FragCoord.xy() / c3d_size ) );
			} );

			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		auto & device = getCurrentRenderDevice( *this );
		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};
	}

	bool TextureProjection::doInitialiseVertexBuffer()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );
		m_vertexBuffer = makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "TextureProjection" );

		if ( auto buffer = m_vertexBuffer->lock( 0u
			, 1u
			, 0u ) )
		{
			*buffer = NonTexturedCube
			{
				{
					{ Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					{ Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, -1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 } },
					{ Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, +1.0, -1.0 } },
					{ Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, +1.0 } },
					{ Point3f{ -1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ +1.0, +1.0, -1.0 }, Point3f{ +1.0, +1.0, +1.0 }, Point3f{ -1.0, +1.0, -1.0 }, Point3f{ -1.0, +1.0, +1.0 } },
					{ Point3f{ -1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ -1.0, -1.0, +1.0 }, Point3f{ +1.0, -1.0, -1.0 }, Point3f{ +1.0, -1.0, +1.0 }, Point3f{ -1.0, -1.0, +1.0 } },
				}
			};
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		return true;
	}

	bool TextureProjection::doInitialisePipeline( ashes::PipelineShaderStageCreateInfoArray & program
		, ashes::ImageView const & texture
		, ashes::RenderPass const & renderPass )
	{
		ashes::PipelineDepthStencilStateCreateInfo dsState
		{
			0u,
			true,
			false,
			VK_COMPARE_OP_LESS_OR_EQUAL
		};

		ashes::PipelineRasterizationStateCreateInfo rsState
		{
			0u,
			false,
			false,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_FRONT_BIT
		};
		
		auto vertexLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
			, ashes::VkVertexInputBindingDescriptionArray
			{
				{ 0u, sizeof( NonTexturedCube::Quad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
			}
			, ashes::VkVertexInputAttributeDescriptionArray
			{
				{ 0u, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( NonTexturedCube::Quad::Vertex, position ) },
			} );

		auto blState = ashes::PipelineColorBlendStateCreateInfo{};
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentRenderDevice( renderSystem );

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( MtxUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( MdlMtxUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ),
			makeDescriptorSetLayoutBinding( DifImgIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		m_descriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		VkPushConstantRange pushRange{ VK_SHADER_STAGE_FRAGMENT_BIT, m_sizePushConstant.getOffset(), m_sizePushConstant.getSize() };
		m_pipelineLayout = device->createPipelineLayout( *m_descriptorLayout, pushRange );

		m_pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
		{
			0u,
			program,
			*vertexLayout,
			ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
			std::nullopt,
			ashes::PipelineViewportStateCreateInfo{},
			std::move( rsState ),
			ashes::PipelineMultisampleStateCreateInfo{},
			std::move( dsState ),
			std::move( blState ),
			ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
			*m_pipelineLayout,
			renderPass,
		} );

		m_descriptorPool = m_descriptorLayout->createPool( 1u );
		m_descriptorSet = m_descriptorPool->createDescriptorSet( 0u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 0u )
			, m_matrixUbo.getUbo()
			, 0u
			, 1u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 1u )
			, m_modelMatrixUbo.getUbo()
			, 0u
			, 1u );
		m_descriptorSet->createBinding( m_descriptorLayout->getBinding( 2u )
			, texture
			, m_sampler->getSampler() );
		m_descriptorSet->update();
		return true;
	}
}
