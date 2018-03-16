#include "LineariseDepthPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/ColourBlendState.hpp>
#include <Pipeline/DepthStencilState.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/MultisampleState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/RasterisationState.hpp>
#include <Pipeline/ShaderStageState.hpp>
#include <Pipeline/VertexInputState.hpp>
#include <Pipeline/VertexLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <GlslSource.hpp>
#include <GlslUtils.hpp>

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		glsl::Shader doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetLinearisePixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Pcb clipInfo{ writer, cuT( "ClipInfo" ), cuT( "clip" ) };
			auto c3d_clipInfo = clipInfo.declMember< Vec3 >( cuT( "c3d_clipInfo" ), 0u );
			clipInfo.end();
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), 1u, 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto depth = writer.declLocale( cuT( "depth" )
						, texelFetch( c3d_mapDepth, ivec2( gl_FragCoord.xy() ), 0 ).r() );
					pxl_fragColor = c3d_clipInfo[0] / writer.paren( c3d_clipInfo[1] * depth + c3d_clipInfo[2] );
				} );
			return writer.finalise();
		}
		
		glsl::Shader doGetMinifyPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace glsl;
			auto writer = renderSystem.createGlslWriter();

			// Shader inputs
			Pcb previousLevel{ writer, cuT( "PreviousLevel" ), cuT( "previous" ) };
			auto c3d_previousLevel = previousLevel.declMember< Vec3 >( cuT( "c3d_previousLevel" ), 0u );
			previousLevel.end();
			auto c3d_mapDepth = writer.declSampler< Sampler2D >( cuT( "c3d_mapDepth" ), 1u, 0u );
			auto gl_FragCoord = writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto ssPosition = writer.declLocale( cuT( "ssPosition" )
						, ivec2( gl_FragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					pxl_fragColor = texelFetch( c3d_mapDepth
						, clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
							, ivec2( 0_i )
							, textureSize( c3d_mapDepth, c3d_previousLevel ) - ivec2( 1_i ) )
						, c3d_previousLevel ).r();
				} );
			return writer.finalise();
		}

		renderer::ShaderStageStateArray doGetLineariseProgram( Engine & engine )
		{
			auto & device = *engine.getRenderSystem()->getCurrentDevice();
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetLinearisePixelProgram( engine );
			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vtx.getSource() );
			program[1].module->loadShader( pxl.getSource() );
			return program;
		}

		renderer::ShaderStageStateArray doGetMinifyProgram( Engine & engine )
		{
			auto & device = *engine.getRenderSystem()->getCurrentDevice();
			auto vtx = doGetVertexProgram( engine );
			auto pxl = doGetMinifyPixelProgram( engine );
			renderer::ShaderStageStateArray program
			{
				{ device.createShaderModule( renderer::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( renderer::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( vtx.getSource() );
			program[1].module->loadShader( pxl.getSource() );
			return program;
		}

		RenderPipelineUPtr doCreatePipeline( Engine & engine
			, renderer::ShaderStageStateArray program )
		{
			renderer::DepthStencilState dsstate
			{
				0u,
				false,
				false
			};
			renderer::RasterisationState rsstate
			{
				0u,
				false,
				false,
				renderer::PolygonMode::eFill,
				renderer::CullModeFlag::eNone
			};
			auto bdstate = renderer::ColourBlendState::createDefault();
			return std::make_unique< RenderPipeline >( *engine.getRenderSystem()
				, std::move( dsstate )
				, std::move( rsstate )
				, std::move( bdstate )
				, renderer::MultisampleState{}
				, std::move( program )
				, PipelineFlags{} );
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, renderer::WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( renderer::Filter::eNearest );
				sampler->setMagFilter( renderer::Filter::eNearest );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, renderer::Extent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "LinearisePass_Result" ), renderer::WrapMode::eClampToEdge );

			renderer::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = renderer::Format::eR32_SFLOAT;
			image.imageType = renderer::TextureType::e2D;
			image.mipLevels = LineariseDepthPass::MaxMipLevel + 1u;
			image.samples = renderer::SampleCountFlag::e1;
			image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;

			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, renderer::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		renderer::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();

			// Create the render pass.
			renderer::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].index = 0u;
			renderPass.attachments[0].format = renderer::Format::eR32_SFLOAT;
			renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = renderer::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
			renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		renderer::FrameBufferPtr doCreateFrameBuffer( renderer::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), texture.getTexture()->getDefaultView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( renderer::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
		}

		renderer::VertexBufferPtr< NonTexturedQuad > doCreateVertexBuffer( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			auto result = renderer::makeVertexBuffer< NonTexturedQuad >( device
				, 1u
				, 0u
				, renderer::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = result->lock( 0u
				, 1u
				, renderer::MemoryMapFlag::eInvalidateRange | renderer::MemoryMapFlag::eWrite ) )
			{
				*buffer = NonTexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ +1.0, +1.0 } },
					}
				};
				result->flush( 0u, 1u );
				result->unlock();
			}

			return result;
		}

		renderer::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = *renderSystem.getCurrentDevice();
			auto result = renderer::makeLayout< NonTexturedQuad >( 0u );
			result->createAttribute( 0u, renderer::Format::eR32G32_SFLOAT, offsetof( NonTexturedQuad::Vertex, position ) );
			return result;
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( Engine & engine
		, renderer::Extent2D const & size
		, SsaoConfigUbo & ssaoConfigUbo
		, TextureUnit const & depthBuffer
		, Viewport const & viewport )
		: m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_depthBuffer{ depthBuffer }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Linearise depth" ) ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_sampler{ m_engine.getRenderSystem()->getCurrentDevice()->createSampler( renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::WrapMode::eClampToEdge
			, renderer::Filter::eNearest
			, renderer::Filter::eNearest ) }
		, m_commandBuffer{ m_engine.getRenderSystem()->getCurrentDevice()->getGraphicsCommandPool().createCommandBuffer() }
		, m_finished{ engine.getRenderSystem()->getCurrentDevice()->createSemaphore() }
		, m_clipInfo{ renderer::ShaderStageFlag::eFragment, { { 1u, 0u, renderer::ConstantFormat::eVec3f } } }
		, m_lineariseProgram{ doGetLineariseProgram( m_engine ) }
		, m_minifyProgram{ doGetMinifyProgram( m_engine ) }
	{
		auto z_f = viewport.getFar();
		auto z_n = viewport.getNear();
		auto clipInfo = std::isinf( z_f )
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_n - z_f, z_f };
		// result = clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] );
		// depth = 0 => result = z_n
		// depth = 1 => result = z_f
		*m_clipInfo.getData() = clipInfo;
		doInitialiseLinearisePass();
		doInitialiseMinifyPass();
		doPrepareFrame();
	}

	LineariseDepthPass::~LineariseDepthPass()
	{
		doCleanupMinifyPass();
		doCleanupLinearisePass();
		m_renderPass.reset();
		m_timer.reset();
		m_result.cleanup();
	}

	void LineariseDepthPass::linearise( renderer::Semaphore const & toWait )const
	{
		m_timer->start();
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		device.getGraphicsQueue().submit( *m_commandBuffer
			, toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		m_timer->stop();
	}

	void LineariseDepthPass::doInitialiseLinearisePass()
	{
		auto size = m_result.getTexture()->getDimensions();
		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), m_result.getTexture()->getDefaultView() );
		m_lineariseFrameBuffer = m_renderPass->createFrameBuffer( renderer::Extent2D{ size.width, size.height }
			, std::move( attaches ) );
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_lineariseDescriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		renderer::PushConstantRange pushConstants{ renderer::ShaderStageFlag::eFragment, 0u, renderer::getSize( renderer::Format::eR32G32B32_SFLOAT ) };
		m_linearisePipelineLayout = device.createPipelineLayout( *m_lineariseDescriptorLayout, pushConstants );

		m_lineariseDescriptorPool = m_lineariseDescriptorLayout->createPool( 1u );
		m_lineariseDescriptor = m_lineariseDescriptorPool->createDescriptorSet();
		m_lineariseDescriptor->createBinding( m_lineariseDescriptorLayout->getBinding( 0u )
			, m_result.getTexture()->getDefaultView()
			, *m_sampler );
		m_linearisePipeline = m_linearisePipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
		{
			m_lineariseProgram,
			*m_renderPass,
			renderer::VertexInputState::create( *m_vertexLayout ),
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{},
			std::nullopt,
			std::nullopt,
			renderer::Viewport{ size.width, size.height, 0, 0 },
			renderer::Scissor{ 0, 0, size.width, size.height },
		} );
	}

	void LineariseDepthPass::doInitialiseMinifyPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = *renderSystem.getCurrentDevice();
		auto size = m_result.getTexture()->getDimensions();
		renderer::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_minifyDescriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		renderer::PushConstantRange pushConstants{ renderer::ShaderStageFlag::eFragment, 0u, renderer::getSize( renderer::Format::eR32_SINT ) };
		m_minifyPipelineLayout = device.createPipelineLayout( *m_minifyDescriptorLayout, pushConstants );

		m_minifyDescriptorPool = m_minifyDescriptorLayout->createPool( MaxMipLevel );
		uint32_t index = 1u;

		for ( auto & pipeline : m_minifyPipelines )
		{
			pipeline.view = m_result.getTexture()->getTexture().createView( renderer::TextureViewType::e2D
				, m_result.getTexture()->getPixelFormat()
				, index );
			pipeline.descriptor = m_minifyDescriptorPool->createDescriptorSet();
			pipeline.descriptor->createBinding( m_minifyDescriptorLayout->getBinding( 0u )
				, *pipeline.view
				, *m_sampler );
			renderer::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), m_result.getTexture()->getDefaultView() );
			pipeline.frameBuffer = m_renderPass->createFrameBuffer( renderer::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
			pipeline.previousLevel = std::make_unique< renderer::PushConstantsBuffer< int > >( renderer::ShaderStageFlag::eFragment
				, renderer::PushConstantArray{ { 1u, 0u, renderer::ConstantFormat::eInt } } );
			*pipeline.previousLevel->getData() = index - 1;
			pipeline.pipeline = m_minifyPipelineLayout->createPipeline( renderer::GraphicsPipelineCreateInfo
			{
				m_minifyProgram,
				*m_renderPass,
				renderer::VertexInputState::create( *m_vertexLayout ),
				renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
				renderer::RasterisationState{},
				renderer::MultisampleState{},
				renderer::ColourBlendState::createDefault(),
				{},
				std::nullopt,
				std::nullopt,
				renderer::Viewport{ size.width, size.height, 0, 0 },
				renderer::Scissor{ 0, 0, size.width, size.height },
			} );
			++index;
		}
	}

	void LineariseDepthPass::doCleanupLinearisePass()
	{
		m_linearisePipeline.reset();
		m_linearisePipelineLayout.reset();
		m_lineariseDescriptor.reset();
		m_lineariseDescriptorPool.reset();
		m_lineariseDescriptorLayout.reset();
		m_lineariseFrameBuffer.reset();
	}

	void LineariseDepthPass::doCleanupMinifyPass()
	{
		for ( auto & pipeline : m_minifyPipelines )
		{
			pipeline.pipeline.reset();
			pipeline.previousLevel.reset();
			pipeline.frameBuffer.reset();
			pipeline.descriptor.reset();
			pipeline.view.reset();
		}

		m_minifyDescriptorPool.reset();
		m_minifyPipelineLayout.reset();
		m_minifyDescriptorLayout.reset();
	}

	void LineariseDepthPass::doPrepareFrame()
	{
		static renderer::ClearColorValue const colour{ 0.0, 0.0, 0.0, 0.0 };

		if ( m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eSimultaneousUse ) )
		{
			m_commandBuffer->resetQueryPool( m_timer->getQuery(), 0u, 2u );
			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, m_timer->getQuery()
				, 0u );

			// Linearisation pass.
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *m_lineariseFrameBuffer
				, { colour }
				, renderer::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *m_linearisePipeline );
			m_commandBuffer->bindDescriptorSet( *m_lineariseDescriptor, *m_linearisePipelineLayout );
			m_commandBuffer->pushConstants( *m_linearisePipelineLayout, m_clipInfo );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 4u );
			m_commandBuffer->endRenderPass();

			// Minification passes.
			for ( auto & pipeline : m_minifyPipelines )
			{
				m_commandBuffer->beginRenderPass( *m_renderPass
					, *pipeline.frameBuffer
					, { colour }
					, renderer::SubpassContents::eInline );
				m_commandBuffer->bindPipeline( *pipeline.pipeline );
				m_commandBuffer->bindDescriptorSet( *pipeline.descriptor, *m_minifyPipelineLayout );
				m_commandBuffer->pushConstants( *m_minifyPipelineLayout, *pipeline.previousLevel );
				m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
				m_commandBuffer->draw( 4u );
				m_commandBuffer->endRenderPass();
			}

			m_commandBuffer->writeTimestamp( renderer::PipelineStageFlag::eBottomOfPipe
				, m_timer->getQuery()
				, 1u );
			m_commandBuffer->end();
		}
	}
}
