#include "TextureProjection.hpp"

#include "Engine.hpp"

#include "Mesh/Vertex.hpp"
#include "Buffer/Buffer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/ShaderStageState.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/ShaderModule.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
	}

	TextureProjection::TextureProjection( Engine & engine )
		: OwnedBy< Engine >{ engine }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_sizePushConstant{ renderer::ShaderStageFlag::eFragment, { { 0u, 0u, renderer::ConstantFormat::eVec2f } } }
	{
		m_sampler = engine.getSamplerCache().add( cuT( "TextureProjection" ) );
		m_sampler->setMinFilter( renderer::Filter::eLinear );
		m_sampler->setMagFilter( renderer::Filter::eLinear );
		m_sampler->setWrapS( renderer::WrapMode::eClampToEdge );
		m_sampler->setWrapT( renderer::WrapMode::eClampToEdge );
		m_sampler->setWrapR( renderer::WrapMode::eClampToEdge );
	}

	TextureProjection::~TextureProjection()
	{
	}

	void TextureProjection::initialise( renderer::TextureView const & source
		, renderer::Format targetColour
		, renderer::Format targetDepth )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[1].index = 0u;
		renderPass.attachments[1].format = targetDepth;
		renderPass.attachments[1].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[1].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[1].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].initialLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;
		renderPass.attachments[1].finalLayout = renderer::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[0].index = 1u;
		renderPass.attachments[0].format = targetColour;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eColourAttachmentOptimal;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].colorAttachments = { { 1u, renderer::ImageLayout::eColourAttachmentOptimal } };
		renderPass.subpasses[0].depthStencilAttachment = { 0u, renderer::ImageLayout::eDepthStencilAttachmentOptimal };

		m_renderPass = device.createRenderPass( renderPass );

		m_sampler->initialise();
		auto & program = doInitialiseShader();
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
		auto & device = *renderSystem.getCurrentDevice();

		if ( !m_commandBuffer )
		{
			m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer( false );
			m_finished = device.createSemaphore();
		}
		else
		{
			m_commandBuffer->reset();
		}

		*m_sizePushConstant.getData() = Point2f{ m_size.getWidth()
			, m_size.getHeight() };

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eRenderPassContinue ) )
		{
			m_commandBuffer->bindPipeline( *m_pipeline );
			m_commandBuffer->setViewport( { m_size.getWidth(), m_size.getHeight(), 0, 0 } );
			m_commandBuffer->setScissor( { 0, 0, m_size.getWidth(), m_size.getHeight() } );
			m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
			m_commandBuffer->pushConstants( *m_pipelineLayout, m_sizePushConstant );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 36u );
			m_commandBuffer->end();
		}
	}

	void TextureProjection::update( Camera const & camera )
	{
		static Matrix3x3r const Identity{ 1.0f };
		auto const & viewport = camera.getViewport();
		auto node = camera.getParent();
		matrix::setTranslate( m_mtxModel, node->getDerivedPosition() );
		m_matrixUbo.update( camera.getView()
			, viewport.getProjection() );
		m_modelMatrixUbo.update( m_mtxModel, Identity );

		if ( m_size != viewport.getSize() )
		{
			m_size = viewport.getSize();
			doPrepareFrame();
		}
	}

	renderer::ShaderStageStateArray TextureProjection::doInitialiseShader()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		glsl::Shader vtx;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto position = writer.declAttribute< Vec3 >( cuT( "position" ), 0u );
			UBO_MATRIX( writer, 0, 0 );
			UBO_MODEL_MATRIX( writer, 1, 0 );

			// Outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				gl_Position = writer.paren( c3d_projection * c3d_curView * c3d_mtxModel * vec4( position, 1.0 ) ).SWIZZLE_XYWW;
			} );

			vtx = writer.finalise();
		}

		glsl::Shader pxl;
		{
			using namespace glsl;
			GlslWriter writer{ renderSystem.createGlslWriter() };

			// Inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 2u, 0u );
			auto c3d_size = writer.declUniform< Vec2 >( cuT( "c3d_size" ), 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Outputs
			auto pxl_FragColor = writer.declFragData< Vec4 >( cuT( "pxl_FragColor" ), 0u );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_FragColor = texture( c3d_mapDiffuse, gl_FragCoord.xy() / c3d_size );
			} );

			pxl = writer.finalise();
		}

		auto & cache = getEngine()->getShaderProgramCache();
		auto program = cache.getNewProgram( false );
		program.push_back( { renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		program.push_back( { renderSystem.getCurrentDevice()->createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		program[0].module->loadShader( vtx.getSource() );
		program[1].module->loadShader( pxl.getSource() );
		return program;
	}

	bool TextureProjection::doInitialiseVertexBuffer()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		m_vertexBuffer = renderer::makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u
			, 1u
			, renderer::MemoryMapFlag::eWrite | renderer::MemoryMapFlag::eInvalidateRange ) )
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

	bool TextureProjection::doInitialisePipeline( renderer::ShaderStageStateArray & program
		, renderer::TextureView const & texture
		, renderer::RenderPass const & renderPass )
	{
		renderer::DepthStencilState dsState
		{
			0u,
			true,
			false,
			renderer::CompareOp::eLessEqual
		};

		renderer::RasterisationState rsState
		{
			0u,
			false,
			false,
			renderer::PolygonMode::eFill,
			renderer::CullModeFlag::eFront
		};

		auto vertexLayout = renderer::makeLayout< NonTexturedCube >( 0u );
		vertexLayout->createAttribute( 0u
			, renderer::Format::eR32G32B32_SFLOAT
			, offsetof( NonTexturedCube::Quad::Vertex, position ) );

		auto blState = renderer::ColourBlendState::createDefault();
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();

		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 1u, renderer::DescriptorType::eUniformBuffer, renderer::ShaderStageFlag::eVertex },
			{ 2u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		renderer::PushConstantRange pushRange{ renderer::ShaderStageFlag::eFragment, m_sizePushConstant.getOffset(), m_sizePushConstant.getSize() };
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout, pushRange );

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			program,
			renderPass,
			renderer::VertexInputState::create( *vertexLayout ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			rsState,
			renderer::MultisampleState{},
			blState,
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			std::move( dsState )
		} );

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

		return true;
	}
}
