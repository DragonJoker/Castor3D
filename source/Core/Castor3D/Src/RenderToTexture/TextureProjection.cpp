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
#include <Shader/GlslToSpv.hpp>
#include <Shader/ShaderModule.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	TextureProjection::TextureProjection( Engine & engine )
		: OwnedBy< Engine >{ engine }
		, m_matrixUbo{ engine }
		, m_modelMatrixUbo{ engine }
		, m_sizePushConstant{ ashes::ShaderStageFlag::eFragment, { { 0u, ashes::ConstantFormat::eVec2f } } }
	{
		m_sampler = engine.getSamplerCache().add( cuT( "TextureProjection" ) );
		m_sampler->setMinFilter( ashes::Filter::eLinear );
		m_sampler->setMagFilter( ashes::Filter::eLinear );
		m_sampler->setWrapS( ashes::WrapMode::eClampToEdge );
		m_sampler->setWrapT( ashes::WrapMode::eClampToEdge );
		m_sampler->setWrapR( ashes::WrapMode::eClampToEdge );
	}

	TextureProjection::~TextureProjection()
	{
	}

	void TextureProjection::initialise( ashes::TextureView const & source
		, ashes::Format targetColour
		, ashes::Format targetDepth )
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0;

		renderPass.attachments.resize( 2u );
		renderPass.attachments[0].format = targetDepth;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eLoad;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eDepthStencilAttachmentOptimal;

		renderPass.attachments[1].format = targetColour;
		renderPass.attachments[1].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[1].loadOp = ashes::AttachmentLoadOp::eLoad;
		renderPass.attachments[1].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[1].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[1].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[1].initialLayout = ashes::ImageLayout::eColourAttachmentOptimal;
		renderPass.attachments[1].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].colorAttachments = { { 1u, ashes::ImageLayout::eColourAttachmentOptimal } };
		renderPass.subpasses[0].depthStencilAttachment = { 0u, ashes::ImageLayout::eDepthStencilAttachmentOptimal };

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

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
		auto & device = getCurrentDevice( renderSystem );

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

		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eRenderPassContinue );
		m_commandBuffer->bindPipeline( *m_pipeline );
		m_commandBuffer->setViewport( { m_size.getWidth(), m_size.getHeight(), 0, 0 } );
		m_commandBuffer->setScissor( { 0, 0, m_size.getWidth(), m_size.getHeight() } );
		m_commandBuffer->bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		m_commandBuffer->pushConstants( *m_pipelineLayout, m_sizePushConstant );
		m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		m_commandBuffer->draw( 36u );
		m_commandBuffer->end();
	}

	void TextureProjection::update( Camera const & camera )
	{
		static Matrix3x3r const Identity{ 1.0f };
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

	ashes::ShaderStageStateArray TextureProjection::doInitialiseShader()
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
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				out.gl_Position() = writer.paren( c3d_projection * c3d_curView * c3d_curMtxModel * vec4( position, 1.0 ) ).xyww();
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
				pxl_FragColor = texture( c3d_mapDiffuse
					, writer.ashesBottomUpToTopDown( gl_FragCoord.xy() / c3d_size ) );
			} );

			pxl = writer.finalise();
		}

		auto & device = getCurrentDevice( renderSystem );
		ashes::ShaderStageStateArray program
		{
			{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
			{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) }
		};
		program[0].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eVertex
			, vtx.getSource() ) );
		program[1].module->loadShader( compileGlslToSpv( device
			, ashes::ShaderStageFlag::eFragment
			, pxl.getSource() ) );
		return program;
	}

	bool TextureProjection::doInitialiseVertexBuffer()
	{
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		m_vertexBuffer = ashes::makeVertexBuffer< NonTexturedCube >( device
			, 1u
			, ashes::BufferTarget::eTransferDst
			, ashes::MemoryPropertyFlag::eHostVisible );

		if ( auto buffer = m_vertexBuffer->lock( 0u
			, 1u
			, ashes::MemoryMapFlag::eWrite | ashes::MemoryMapFlag::eInvalidateRange ) )
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

	bool TextureProjection::doInitialisePipeline( ashes::ShaderStageStateArray & program
		, ashes::TextureView const & texture
		, ashes::RenderPass const & renderPass )
	{
		ashes::DepthStencilState dsState
		{
			0u,
			true,
			false,
			ashes::CompareOp::eLessEqual
		};

		ashes::RasterisationState rsState
		{
			0u,
			false,
			false,
			ashes::PolygonMode::eFill,
			ashes::CullModeFlag::eFront
		};

		auto vertexLayout = ashes::makeLayout< NonTexturedCube >( 0u );
		vertexLayout->createAttribute( 0u
			, ashes::Format::eR32G32B32_SFLOAT
			, offsetof( NonTexturedCube::Quad::Vertex, position ) );

		auto blState = ashes::ColourBlendState::createDefault();
		auto & renderSystem = *getEngine()->getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );

		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eVertex },
			{ 2u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
		};
		m_descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		ashes::PushConstantRange pushRange{ ashes::ShaderStageFlag::eFragment, m_sizePushConstant.getOffset(), m_sizePushConstant.getSize() };
		m_pipelineLayout = device.createPipelineLayout( *m_descriptorLayout, pushRange );

		m_pipeline = m_pipelineLayout->createPipeline(
		{
			program,
			renderPass,
			ashes::VertexInputState::create( *vertexLayout ),
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			rsState,
			ashes::MultisampleState{},
			blState,
			{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
			std::move( dsState )
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
