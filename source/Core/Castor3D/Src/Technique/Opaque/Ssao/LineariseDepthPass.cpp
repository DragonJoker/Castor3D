#include "LineariseDepthPass.hpp"

#include "Engine.hpp"
#include "Render/RenderPassTimer.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Camera.hpp"
#include "Shader/Program.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Technique/Opaque/Ssao/SsaoConfigUbo.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Command/Queue.hpp>
#include <Core/Device.hpp>
#include <Core/Renderer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetLayoutBinding.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Miscellaneous/RendererFeatures.hpp>
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
#include <Shader/GlslToSpv.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslUtils.hpp"

#include "Shader/Shaders/GlslLight.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		ShaderPtr doGetVertexProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto out = writer.getOut();

			writer.implementFunction< void >( cuT( "main" )
				, [&]()
				{
					out.gl_out.gl_Position = vec4( position, 0.0, 1.0 );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr doGetLinearisePixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo clipInfo{ writer, cuT( "ClipInfo" ), 1u, 0u, ast::type::MemoryLayout::eStd140 };
			auto c3d_clipInfo = clipInfo.declMember< Vec3 >( cuT( "c3d_clipInfo" ) );
			clipInfo.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDepth" ), 0u, 0u );
			auto in = writer.getIn();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< Void >( cuT( "main" )
				, [&]()
				{
					auto depth = writer.declLocale( cuT( "depth" )
						, texelFetch( c3d_mapDepth, ivec2( in.gl_FragCoord.xy() ), 0_i ).r() );
					pxl_fragColor = c3d_clipInfo[0] / writer.paren( c3d_clipInfo[1] * depth + c3d_clipInfo[2] );
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}
		
		ShaderPtr doGetMinifyPixelProgram( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			Ubo previousLevel{ writer, cuT( "PreviousLevel" ), 1u, 0u, ast::type::MemoryLayout::eStd140 };
			auto c3d_textureSize = previousLevel.declMember< IVec2 >( cuT( "c3d_textureSize" ) );
			auto c3d_previousLevel = previousLevel.declMember< Int >( cuT( "c3d_previousLevel" ) );
			previousLevel.end();
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDepth" ), 0u, 0u );
			auto in = writer.getIn();

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Float >( cuT( "pxl_fragColor" ), 0u );

			writer.implementFunction< sdw::Void >( cuT( "main" )
				, [&]()
				{
					auto ssPosition = writer.declLocale( cuT( "ssPosition" )
						, ivec2( in.gl_FragCoord.xy() ) );

					// Rotated grid subsampling to avoid XY directional bias or Z precision bias while downsampling.
					if ( renderSystem.getCurrentDevice()->getRenderer().getFeatures().hasStorageBuffers )
					{
						pxl_fragColor = texelFetch( c3d_mapDepth
							, clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
								, ivec2( 0_i, 0_i )
								, c3d_textureSize - ivec2( 1_i, 1_i ) )
							, 0_i ).r();
					}
					else
					{
						pxl_fragColor = texelFetch( c3d_mapDepth
							, clamp( ssPosition * 2 + ivec2( ssPosition.y() & 1, ssPosition.x() & 1 )
								, ivec2( 0_i, 0_i )
								, c3d_textureSize - ivec2( 1_i, 1_i ) )
							, c3d_previousLevel ).r();
					}
				} );
			return std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ShaderStageStateArray doGetLineariseProgram( Engine & engine
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetLinearisePixelProgram( engine );
			ashes::ShaderStageStateArray program
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( engine.getRenderSystem()->compileShader( vertexShader ) );
			program[1].module->loadShader( engine.getRenderSystem()->compileShader( pixelShader ) );
			return program;
		}

		ashes::ShaderStageStateArray doGetMinifyProgram( Engine & engine
			, ShaderModule & vertexShader
			, ShaderModule & pixelShader )
		{
			auto & device = getCurrentDevice( engine );
			vertexShader.shader = doGetVertexProgram( engine );
			pixelShader.shader = doGetMinifyPixelProgram( engine );
			ashes::ShaderStageStateArray program
			{
				{ device.createShaderModule( ashes::ShaderStageFlag::eVertex ) },
				{ device.createShaderModule( ashes::ShaderStageFlag::eFragment ) },
			};
			program[0].module->loadShader( engine.getRenderSystem()->compileShader( vertexShader ) );
			program[1].module->loadShader( engine.getRenderSystem()->compileShader( pixelShader ) );
			return program;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, ashes::WrapMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( ashes::Filter::eNearest );
				sampler->setMagFilter( ashes::Filter::eNearest );
				sampler->setMipFilter( ashes::MipmapMode::eNearest );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
				sampler->setMinLod( 0.0 );
				sampler->setMaxLod( float( LineariseDepthPass::MaxMipLevel ) );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine, ashes::Extent2D const & size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, cuT( "LinearisePass_Result" ), ashes::WrapMode::eClampToEdge );

			ashes::ImageCreateInfo image{};
			image.arrayLayers = 1u;
			image.extent.width = size.width;
			image.extent.height = size.height;
			image.extent.depth = 1u;
			image.format = ashes::Format::eR32_SFLOAT;
			image.imageType = ashes::TextureType::e2D;
			image.mipLevels = LineariseDepthPass::MaxMipLevel + 1u;
			image.samples = ashes::SampleCountFlag::e1;
			image.usage = ashes::ImageUsageFlag::eColourAttachment | ashes::ImageUsageFlag::eSampled | ashes::ImageUsageFlag::eTransferDst | ashes::ImageUsageFlag::eTransferSrc;

			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, ashes::MemoryPropertyFlag::eDeviceLocal );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		ashes::RenderPassPtr doCreateRenderPass( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );

			// Create the render pass.
			ashes::RenderPassCreateInfo renderPass;
			renderPass.flags = 0u;

			renderPass.attachments.resize( 1u );
			renderPass.attachments[0].format = ashes::Format::eR32_SFLOAT;
			renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
			renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
			renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
			renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
			renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
			renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
			renderPass.attachments[0].finalLayout = ashes::ImageLayout::eColourAttachmentOptimal;

			renderPass.subpasses.resize( 1u );
			renderPass.subpasses[0].flags = 0u;
			renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
			renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

			renderPass.dependencies.resize( 2u );
			renderPass.dependencies[0].srcSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[0].dstSubpass = 0u;
			renderPass.dependencies[0].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].dstAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[0].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dstStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[0].dependencyFlags = ashes::DependencyFlag::eByRegion;

			renderPass.dependencies[1].srcSubpass = 0u;
			renderPass.dependencies[1].dstSubpass = ashes::ExternalSubpass;
			renderPass.dependencies[1].srcAccessMask = ashes::AccessFlag::eColourAttachmentWrite;
			renderPass.dependencies[1].dstAccessMask = ashes::AccessFlag::eShaderRead;
			renderPass.dependencies[1].srcStageMask = ashes::PipelineStageFlag::eColourAttachmentOutput;
			renderPass.dependencies[1].dstStageMask = ashes::PipelineStageFlag::eFragmentShader;
			renderPass.dependencies[1].dependencyFlags = ashes::DependencyFlag::eByRegion;

			return device.createRenderPass( renderPass );
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, TextureUnit const & texture )
		{
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( renderPass.getAttachments().begin() ), texture.getTexture()->getDefaultView() );
			auto size = texture.getTexture()->getDimensions();
			return renderPass.createFrameBuffer( ashes::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
		}

		ashes::VertexBufferPtr< NonTexturedQuad > doCreateVertexBuffer( Engine & engine )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto & device = getCurrentDevice( renderSystem );
			auto result = ashes::makeVertexBuffer< NonTexturedQuad >( device
				, 1u
				, 0u
				, ashes::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = reinterpret_cast< NonTexturedQuad * >( result->getBuffer().lock( 0u
				, ~( 0ull )
				, ashes::MemoryMapFlag::eInvalidateRange | ashes::MemoryMapFlag::eWrite ) ) )
			{
				*buffer = NonTexturedQuad
				{
					{
						{ Point2f{ -1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ +1.0, -1.0 } },
						{ Point2f{ -1.0, +1.0 } },
						{ Point2f{ +1.0, +1.0 } },
					}
				};
				result->getBuffer().flush( 0u, ~( 0ull ) );
				result->getBuffer().unlock();
			}

			return result;
		}

		ashes::VertexLayoutPtr doCreateVertexLayout( Engine & engine )
		{
			auto result = ashes::makeLayout< NonTexturedQuad::Vertex >( 0u );
			result->createAttribute( 0u, ashes::Format::eR32G32_SFLOAT, offsetof( NonTexturedQuad::Vertex, position ) );
			return result;
		}
	}

	//*********************************************************************************************

	LineariseDepthPass::LineariseDepthPass( Engine & engine
		, ashes::Extent2D const & size
		, SsaoConfigUbo & ssaoConfigUbo
		, ashes::TextureView const & depthBuffer )
		: m_engine{ engine }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_depthBuffer{ depthBuffer }
		, m_size{ size }
		, m_result{ doCreateTexture( m_engine, m_size ) }
		, m_timer{ std::make_shared< RenderPassTimer >( m_engine, cuT( "SSAO" ), cuT( "Linearise depth" ) ) }
		, m_renderPass{ doCreateRenderPass( m_engine ) }
		, m_vertexBuffer{ doCreateVertexBuffer( m_engine ) }
		, m_vertexLayout{ doCreateVertexLayout( m_engine ) }
		, m_lineariseSampler{ getCurrentDevice( m_engine ).createSampler( ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::Filter::eNearest
			, ashes::Filter::eNearest ) }
		, m_minifySampler{ getCurrentDevice( m_engine ).createSampler( ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::WrapMode::eClampToEdge
			, ashes::Filter::eNearest
			, ashes::Filter::eNearest ) }
		, m_commandBuffer{ getCurrentDevice( m_engine ).getGraphicsCommandPool().createCommandBuffer() }
		, m_finished{ getCurrentDevice( m_engine ).createSemaphore() }
		, m_clipInfo{ ashes::makeUniformBuffer< Point3f >( getCurrentDevice( m_engine ), 1u, 0u, ashes::MemoryPropertyFlag::eHostVisible ) }
		, m_lineariseVertexShader{ ashes::ShaderStageFlag::eVertex, "LineariseDepth" }
		, m_linearisePixelShader{ ashes::ShaderStageFlag::eFragment, "LineariseDepth" }
		, m_lineariseProgram{ doGetLineariseProgram( m_engine, m_lineariseVertexShader, m_linearisePixelShader ) }
		, m_minifyVertexShader{ ashes::ShaderStageFlag::eVertex, "Minify" }
		, m_minifyPixelShader{ ashes::ShaderStageFlag::eFragment, "Minify" }
		, m_minifyProgram{ doGetMinifyProgram( m_engine, m_minifyVertexShader, m_minifyPixelShader ) }
	{
		doInitialiseLinearisePass();
		doInitialiseMinifyPass();
	}

	LineariseDepthPass::~LineariseDepthPass()
	{
		doCleanupMinifyPass();
		doCleanupLinearisePass();
		m_renderPass.reset();
		m_timer.reset();
		m_result.cleanup();
	}

	void LineariseDepthPass::update( Camera const & camera )
	{
		auto z_f = camera.getFar();
		auto z_n = camera.getNear();
		auto clipInfo = std::isinf( z_f )
			? Point3f{ z_n, -1.0f, 1.0f }
			: Point3f{ z_n * z_f, z_n - z_f, z_f };
		// result = clipInfo[0] / ( clipInfo[1] * depth + clipInfo[2] );
		// depth = 0 => result = z_n
		// depth = 1 => result = z_f
		m_clipInfoValue = clipInfo;

		if ( m_clipInfoValue.isDirty() )
		{
			m_clipInfo->getData() = m_clipInfoValue;
			m_clipInfo->upload( 0u );
			doPrepareFrame();
		}
	}

	ashes::Semaphore const & LineariseDepthPass::linearise( ashes::Semaphore const & toWait )const
	{
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		auto * result = &toWait;

		device.getGraphicsQueue().submit( *m_commandBuffer
			, *result
			, ashes::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
		result = m_finished.get();

		return *result;
	}

	void LineariseDepthPass::accept( RenderTechniqueVisitor & visitor )
	{
		visitor.visit( cuT( "SSAO - Linearise" )
			, ashes::ShaderStageFlag::eVertex
			, *m_lineariseVertexShader.shader );
		visitor.visit( cuT( "SSAO - Linearise" )
			, ashes::ShaderStageFlag::eFragment
			, *m_linearisePixelShader.shader );

		visitor.visit( cuT( "SSAO - Minify" )
			, ashes::ShaderStageFlag::eVertex
			, *m_minifyVertexShader.shader );
		visitor.visit( cuT( "SSAO - Minify" )
			, ashes::ShaderStageFlag::eFragment
			, *m_minifyPixelShader.shader );
	}

	void LineariseDepthPass::doInitialiseLinearisePass()
	{
		auto size = m_result.getTexture()->getDimensions();
		ashes::FrameBufferAttachmentArray attaches;
		m_linearisedView = m_result.getTexture()->getTexture().createView( ashes::TextureViewType::e2D
			, m_result.getTexture()->getPixelFormat() );
		attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), *m_linearisedView );
		m_lineariseFrameBuffer = m_renderPass->createFrameBuffer( ashes::Extent2D{ size.width, size.height }
			, std::move( attaches ) );
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
		};
		m_lineariseDescriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_linearisePipelineLayout = device.createPipelineLayout( *m_lineariseDescriptorLayout );

		m_lineariseDescriptorPool = m_lineariseDescriptorLayout->createPool( 1u );
		m_lineariseDescriptor = m_lineariseDescriptorPool->createDescriptorSet();
		m_lineariseDescriptor->createBinding( m_lineariseDescriptorLayout->getBinding( 0u )
			, m_depthBuffer
			, *m_lineariseSampler );
		m_lineariseDescriptor->createBinding( m_lineariseDescriptorLayout->getBinding( 1u )
			, *m_clipInfo
			, 0u );
		m_lineariseDescriptor->update();
		m_linearisePipeline = m_linearisePipelineLayout->createPipeline( ashes::GraphicsPipelineCreateInfo
		{
			m_lineariseProgram,
			*m_renderPass,
			ashes::VertexInputState::create( *m_vertexLayout ),
			ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
			ashes::RasterisationState{},
			ashes::MultisampleState{},
			ashes::ColourBlendState::createDefault(),
			{},
			std::nullopt,
			std::nullopt,
			ashes::Viewport{ size.width, size.height, 0, 0 },
			ashes::Scissor{ 0, 0, size.width, size.height },
		} );
	}

	void LineariseDepthPass::doInitialiseMinifyPass()
	{
		auto & renderSystem = *m_engine.getRenderSystem();
		auto & device = getCurrentDevice( renderSystem );
		auto size = m_result.getTexture()->getDimensions();
		ashes::DescriptorSetLayoutBindingArray bindings
		{
			{ 0u, ashes::DescriptorType::eCombinedImageSampler, ashes::ShaderStageFlag::eFragment },
			{ 1u, ashes::DescriptorType::eUniformBuffer, ashes::ShaderStageFlag::eFragment },
		};
		m_minifyDescriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
		m_minifyPipelineLayout = device.createPipelineLayout( *m_minifyDescriptorLayout );

		m_minifyDescriptorPool = m_minifyDescriptorLayout->createPool( MaxMipLevel );
		uint32_t index = 1u;
		auto * sourceView = m_linearisedView.get();
		ashes::ImageViewCreateInfo viewInfo{};
		viewInfo.format = m_result.getTexture()->getPixelFormat();
		viewInfo.viewType = ashes::TextureViewType::e2D;
		viewInfo.subresourceRange.aspectMask = ashes::ImageAspectFlag::eColour;
		viewInfo.subresourceRange.baseArrayLayer = 0u;
		viewInfo.subresourceRange.layerCount = 1u;
		viewInfo.subresourceRange.baseMipLevel = 0u;
		viewInfo.subresourceRange.levelCount = 1u;

		m_previousLevel = ashes::makeUniformBuffer< MinifyConfiguration >( device
			, MaxMipLevel
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible );

		for ( auto & pipeline : m_minifyPipelines )
		{
			size.width >>= 1;
			size.height >>= 1;
			pipeline.sourceView = sourceView;
			viewInfo.subresourceRange.baseMipLevel = index;
			pipeline.targetView = m_result.getTexture()->getTexture().createView( viewInfo );
			pipeline.descriptor = m_minifyDescriptorPool->createDescriptorSet();
			pipeline.descriptor->createBinding( m_minifyDescriptorLayout->getBinding( 0u )
				, *pipeline.sourceView
				, *m_minifySampler );
			pipeline.descriptor->createBinding( m_minifyDescriptorLayout->getBinding( 1u )
				, *m_previousLevel
				, index - 1u );
			pipeline.descriptor->update();
			ashes::FrameBufferAttachmentArray attaches;
			attaches.emplace_back( *( m_renderPass->getAttachments().begin() ), *pipeline.targetView );
			pipeline.frameBuffer = m_renderPass->createFrameBuffer( ashes::Extent2D{ size.width, size.height }
				, std::move( attaches ) );
			auto & data = m_previousLevel->getData( index - 1u );
			data.previousLevel = index - 1;
			data.textureSize = Point2i{ size.width << 1, size.height << 1 };
			pipeline.pipeline = m_minifyPipelineLayout->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				m_minifyProgram,
				*m_renderPass,
				ashes::VertexInputState::create( *m_vertexLayout ),
				ashes::InputAssemblyState{ ashes::PrimitiveTopology::eTriangleList },
				ashes::RasterisationState{},
				ashes::MultisampleState{},
				ashes::ColourBlendState::createDefault(),
				{},
				std::nullopt,
				std::nullopt,
				ashes::Viewport{ size.width, size.height, 0, 0 },
				ashes::Scissor{ 0, 0, size.width, size.height },
			} );
			sourceView = pipeline.targetView.get();
			++index;
		}

		m_previousLevel->upload( 0u, MaxMipLevel );
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
			pipeline.frameBuffer.reset();
			pipeline.descriptor.reset();
			pipeline.targetView.reset();
		}

		m_previousLevel.reset();
		m_minifyDescriptorPool.reset();
		m_minifyPipelineLayout.reset();
		m_minifyDescriptorLayout.reset();
	}

	void LineariseDepthPass::doPrepareFrame()
	{
		static ashes::ClearColorValue const colour{ 0.0, 0.0, 0.0, 0.0 };

		m_commandBuffer->begin();
		m_timer->beginPass( *m_commandBuffer );

		// Linearisation pass.
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_lineariseFrameBuffer
			, { colour }
			, ashes::SubpassContents::eInline );
		m_commandBuffer->bindPipeline( *m_linearisePipeline );
		m_commandBuffer->bindDescriptorSet( *m_lineariseDescriptor, *m_linearisePipelineLayout );
		m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		m_commandBuffer->draw( 6u );
		m_commandBuffer->endRenderPass();
		m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
			, ashes::PipelineStageFlag::eFragmentShader
			, m_linearisedView->makeShaderInputResource( ashes::ImageLayout::eColourAttachmentOptimal
				, ashes::AccessFlag::eColourAttachmentWrite ) );

		// Minification passes.
		for ( auto & pipeline : m_minifyPipelines )
		{
			m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eFragmentShader
				, ashes::PipelineStageFlag::eColourAttachmentOutput
				, pipeline.targetView->makeColourAttachment( ashes::ImageLayout::eUndefined, 0u ) );
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *pipeline.frameBuffer
				, { colour }
				, ashes::SubpassContents::eInline );
			m_commandBuffer->bindPipeline( *pipeline.pipeline );
			m_commandBuffer->bindDescriptorSet( *pipeline.descriptor, *m_minifyPipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 6u );
			m_commandBuffer->endRenderPass();
			m_commandBuffer->memoryBarrier( ashes::PipelineStageFlag::eColourAttachmentOutput
				, ashes::PipelineStageFlag::eFragmentShader
				, pipeline.targetView->makeShaderInputResource( ashes::ImageLayout::eColourAttachmentOptimal
					, ashes::AccessFlag::eColourAttachmentWrite ) );
		}

		m_timer->endPass( *m_commandBuffer );
		m_commandBuffer->end();
	}
}
