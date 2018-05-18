#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
#include <Render/RenderPassTimer.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureLayout.hpp>

#include <Buffer/VertexBuffer.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/FrameBufferAttachment.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <RenderPass/SubpassDependency.hpp>
#include <RenderPass/SubpassDescription.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/ImageMemoryBarrier.hpp>

#include <Graphics/Image.hpp>

#include <numeric>

#include <GlslSource.hpp>

using namespace castor;
using namespace castor3d;

namespace Bloom
{
	namespace
	{
		glsl::Shader getVertexProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			Vec2 position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 0u );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = writer.paren( position + 1.0 ) / 2.0;
				out.gl_Position() = writer.rendererScalePosition( vec4( position, 0.0, 1.0 ) );
			} );
			return writer.finalise();
		}

		glsl::Shader getHiPassProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" ), 0u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, writer.adjustTexCoords( vtx_texture ), 0.0f ).xyz(), 1.0 );
				auto maxComponent = writer.declLocale( cuT( "maxComponent" ), glsl::max( pxl_fragColor.r(), pxl_fragColor.g() ) );
				maxComponent = glsl::max( maxComponent, pxl_fragColor.b() );

				IF( writer, maxComponent > 1.0_f )
				{
					pxl_fragColor.xyz() /= maxComponent;
				}
				ELSE
				{
					pxl_fragColor.xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
				}
				FI;
			} );
			return writer.finalise();
		}

		glsl::Shader getCombineProgram( RenderSystem * renderSystem )
		{
			using namespace glsl;
			GlslWriter writer = renderSystem->createGlslWriter();

			// Shader inputs
			auto c3d_mapPasses = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapPasses, 0u, 0u );
			auto c3d_mapScene = writer.declSampler< Sampler2D >( BloomPostEffect::CombineMapScene, 1u, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" ), 0u );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto texcoords = writer.declLocale( cuT( "texcoords" )
					, writer.adjustTexCoords( vtx_texture ) );

				if ( writer.isTopDown() )
				{
					pxl_fragColor = texture( c3d_mapScene, vtx_texture );
				}
				else
				{
					pxl_fragColor = texture( c3d_mapScene, vec2( vtx_texture.x(), 1.0_f - vtx_texture.y() ) );
				}

				for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
				{
					pxl_fragColor += texture( c3d_mapPasses, texcoords, Float( float( i ) ) );
				}
			} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	BloomPostEffect::Surface::Surface( renderer::Texture const & texture
		, renderer::Extent2D const & size
		, renderer::RenderPass const & renderPass )
	{
		renderer::ImageViewCreateInfo imageView{};
		imageView.components.r = renderer::ComponentSwizzle::eIdentity;
		imageView.components.g = renderer::ComponentSwizzle::eIdentity;
		imageView.components.b = renderer::ComponentSwizzle::eIdentity;
		imageView.components.a = renderer::ComponentSwizzle::eIdentity;
		imageView.format = texture.getFormat();
		imageView.viewType = renderer::TextureViewType::e2D;
		imageView.subresourceRange.aspectMask = renderer::getAspectMask( imageView.format );
		imageView.subresourceRange.baseMipLevel = 0u;
		imageView.subresourceRange.levelCount = 1u;
		imageView.subresourceRange.baseArrayLayer = 0u;
		imageView.subresourceRange.layerCount = 1u;
		view = texture.createView( imageView );

		renderer::FrameBufferAttachmentArray attachments{ { *renderPass.getAttachments().begin(), *view } };
		frameBuffer = renderPass.createFrameBuffer( size, std::move( attachments ) );
	}

	//*********************************************************************************************

	String const BloomPostEffect::Type = cuT( "bloom" );
	String const BloomPostEffect::Name = cuT( "Bloom PostEffect" );
	String const BloomPostEffect::CombineMapPasses = cuT( "c3d_mapPasses" );
	String const BloomPostEffect::CombineMapScene = cuT( "c3d_mapScene" );

	BloomPostEffect::BloomPostEffect( RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & params )
		: PostEffect( BloomPostEffect::Type
			, BloomPostEffect::Name
			, renderTarget
			, renderSystem
			, params )
		, m_size( 5u )
	{
		String count;

		if ( params.get( cuT( "Size" ), count ) )
		{
			m_size = uint32_t( string::toLong( count ) );
		}
	}

	PostEffectSPtr BloomPostEffect::create( RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & params )
	{
		return std::make_shared< BloomPostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void BloomPostEffect::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( cuT( "HiPass" )
			, renderer::ShaderStageFlag::eVertex
			, m_pipelines.hiPass.vertexShader );
		visitor.visit( cuT( "HiPass" )
			, renderer::ShaderStageFlag::eFragment
			, m_pipelines.hiPass.pixelShader );

		visitor.visit( cuT( "BlurX" )
			, renderer::ShaderStageFlag::eVertex
			, m_blurs[0]->getBlurXVertexShader() );
		visitor.visit( cuT( "BlurX" )
			, renderer::ShaderStageFlag::eFragment
			, m_blurs[0]->getBlurXVertexShader() );

		visitor.visit( cuT( "BlurY" )
			, renderer::ShaderStageFlag::eVertex
			, m_blurs[0]->getBlurYVertexShader() );
		visitor.visit( cuT( "BlurY" )
			, renderer::ShaderStageFlag::eFragment
			, m_blurs[0]->getBlurYVertexShader() );

		visitor.visit( cuT( "Combine" )
			, renderer::ShaderStageFlag::eVertex
			, m_pipelines.combine.vertexShader );
		visitor.visit( cuT( "Combine" )
			, renderer::ShaderStageFlag::eFragment
			, m_pipelines.combine.pixelShader );

		visitor.visit( cuT( "Size" )
			, m_size );
	}

	bool BloomPostEffect::doInitialise( castor3d::RenderPassTimer const & timer )
	{
		m_linearSampler = doCreateSampler( true );
		m_nearestSampler = doCreateSampler( false );
		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Create vertex buffer
		m_vertexBuffer = renderer::makeVertexBuffer< castor3d::NonTexturedQuad >( device
			, 1u
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible );

		if ( auto data = m_vertexBuffer->lock( 0u, 1u, renderer::MemoryMapFlag::eWrite ) )
		{
			castor3d::NonTexturedQuad buffer
			{
				{ 
					Point2f{ -1.0, -1.0 },
					Point2f{ -1.0, +1.0 },
					Point2f{ +1.0, -1.0 },
					Point2f{ +1.0, -1.0 },
					Point2f{ -1.0, +1.0 },
					Point2f{ +1.0, +1.0 }
				},
			};
			*data = buffer;
			m_vertexBuffer->flush( 0u, 1u );
			m_vertexBuffer->unlock();
		}

		// Create descriptor and pipeline layouts
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_pipelines.combine.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.combine.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.combine.layout.descriptorLayout );
		m_pipelines.combine.layout.descriptorPool = m_pipelines.combine.layout.descriptorLayout->createPool( 1u );

		setLayoutBindings =
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_pipelines.hiPass.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.hiPass.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.hiPass.layout.descriptorLayout );
		m_pipelines.hiPass.layout.descriptorPool = m_pipelines.hiPass.layout.descriptorLayout->createPool( FILTER_COUNT );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target->getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eDontCare;
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
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Create textures
		renderer::ImageCreateInfo image{};
		image.flags = 0u;
		image.arrayLayers = 1u;
		image.extent.width = size.width >> 1;
		image.extent.height = size.height >> 1;
		image.extent.depth = 1u;
		image.format = m_target->getPixelFormat();
		image.imageType = renderer::TextureType::e2D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = FILTER_COUNT;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eColourAttachment
			| renderer::ImageUsageFlag::eSampled
			| renderer::ImageUsageFlag::eTransferDst
			| renderer::ImageUsageFlag::eTransferSrc;
		m_pipelines.hiPass.image = std::make_shared< TextureLayout >( *getRenderSystem()
			, image
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_pipelines.hiPass.image->initialise();

		image.mipLevels = 1u;
		image.usage = renderer::ImageUsageFlag::eColourAttachment
			| renderer::ImageUsageFlag::eSampled
			| renderer::ImageUsageFlag::eTransferSrc;
		image.extent.width = size.width;
		image.extent.height = size.height;
		m_pipelines.combine.image = std::make_shared< TextureLayout >( *getRenderSystem()
			, image
			, renderer::MemoryPropertyFlag::eDeviceLocal );
		m_pipelines.combine.image->initialise();

		bool result = doInitialiseHiPass();

		if ( result )
		{
			auto * srcView = m_pipelines.hiPass.surface->view.get();

			for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
			{
				size.width >>= 1;
				size.height >>= 1;
				m_blurs.push_back( std::make_shared< GaussianBlur >( *getRenderSystem()->getEngine()
					, *srcView
					, size
					, m_target->getPixelFormat()
					, m_size ) );

				if ( i < FILTER_COUNT - 1 )
				{
					srcView = m_hiPassViews[i].get();
				}
			}
		}

		if ( result )
		{
			result = doInitialiseCombine();
		}

		if ( result )
		{
			result = doBuildCommandBuffer( timer );
		}

		m_result = m_pipelines.combine.image.get();
		return result;
	}

	void BloomPostEffect::doCleanup()
	{
		m_vertexBuffer.reset();
		m_blurs.clear();

		Pipeline dummy1, dummy2;
		std::swap( m_pipelines.combine, dummy1 );
		std::swap( m_pipelines.hiPass, dummy2 );

		m_hiPassViews.clear();
		m_linearSampler->cleanup();
		m_nearestSampler->cleanup();

		m_renderPass.reset();
	}

	bool BloomPostEffect::doWriteInto( TextFile & file )
	{
		return file.writeText( cuT( " -Size=" ) + string::toString( m_size ) ) > 0;
	}

	SamplerSPtr BloomPostEffect::doCreateSampler( bool linear )
	{
		String name = cuT( "BloomSampler_" );
		renderer::Filter mode;

		if ( linear )
		{
			name += cuT( "Linear" );
			mode = renderer::Filter::eLinear;
		}
		else
		{
			name += cuT( "Nearest" );
			mode = renderer::Filter::eNearest;
		}

		SamplerSPtr sampler;

		if ( !m_renderTarget.getEngine()->getSamplerCache().has( name ) )
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().add( name );
			sampler->setMinFilter( mode );
			sampler->setMagFilter( mode );
			sampler->setWrapS( renderer::WrapMode::eClampToEdge );
			sampler->setWrapT( renderer::WrapMode::eClampToEdge );
			sampler->setWrapR( renderer::WrapMode::eClampToEdge );
			sampler->setMinLod( 0.0f );
			sampler->setMaxLod( float( FILTER_COUNT ) );
			sampler->initialise();
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool BloomPostEffect::doInitialiseHiPass()
	{
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		size.width >>= 1;
		size.height >>= 1;
		auto & device = *getRenderSystem()->getCurrentDevice();
		m_pipelines.hiPass.vertexShader = getVertexProgram( getRenderSystem() );
		m_pipelines.hiPass.pixelShader = getHiPassProgram( getRenderSystem() );

		// Create the surface
		m_pipelines.hiPass.surface = std::make_unique< Surface >( m_pipelines.hiPass.image->getTexture()
			, size
			, *m_renderPass );

		// Create downsampled views.
		renderer::ImageViewCreateInfo imageView{};
		imageView.format = m_pipelines.hiPass.image->getPixelFormat();
		imageView.viewType = renderer::TextureViewType::e2D;
		imageView.subresourceRange.aspectMask = renderer::getAspectMask( imageView.format );
		imageView.subresourceRange.baseMipLevel = 0u;
		imageView.subresourceRange.levelCount = 1u;
		imageView.subresourceRange.baseArrayLayer = 0u;
		imageView.subresourceRange.layerCount = 1u;

		for ( uint32_t i = 1u; i < FILTER_COUNT; ++i )
		{
			imageView.subresourceRange.baseMipLevel = i;
			m_hiPassViews.push_back( m_pipelines.hiPass.image->getTexture().createView( imageView ) );
		}

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_pipelines.hiPass.vertexShader.getSource() );
		stages[1].module->loadShader( m_pipelines.hiPass.pixelShader.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, false, false }
		};
		m_pipelines.hiPass.pipeline = m_pipelines.hiPass.layout.pipelineLayout->createPipeline( pipeline );

		// Create descriptor set
		auto & surface = *m_pipelines.hiPass.surface;
		surface.descriptorSet = m_pipelines.hiPass.layout.descriptorPool->createDescriptorSet( 0u );
		surface.descriptorSet->createBinding( m_pipelines.hiPass.layout.descriptorLayout->getBinding( 0u )
			, m_target->getDefaultView()
			, m_linearSampler->getSampler()
			, renderer::ImageLayout::eShaderReadOnlyOptimal );
		surface.descriptorSet->update();

		return true;
	}

	bool BloomPostEffect::doInitialiseCombine()
	{
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };
		auto & device = *getRenderSystem()->getCurrentDevice();
		m_pipelines.combine.vertexShader = getVertexProgram( getRenderSystem() );
		m_pipelines.combine.pixelShader = getCombineProgram( getRenderSystem() );

		// Create view and associated frame buffer.
		m_pipelines.combine.surface = std::make_unique< Surface >( m_pipelines.combine.image->getTexture()
			, size
			, *m_renderPass );

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( NonTexturedQuad::Vertex ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( m_pipelines.combine.vertexShader.getSource() );
		stages[1].module->loadShader( m_pipelines.combine.pixelShader.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleList },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ 0u, false, false }
		};
		m_pipelines.combine.pipeline = m_pipelines.combine.layout.pipelineLayout->createPipeline( pipeline );

		// Create descriptor
		auto & surface = *m_pipelines.combine.surface;
		surface.descriptorSet = m_pipelines.combine.layout.descriptorPool->createDescriptorSet( 0u );
		surface.descriptorSet->createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( 0u )
			, m_pipelines.hiPass.image->getDefaultView()
			, m_linearSampler->getSampler()
			, renderer::ImageLayout::eShaderReadOnlyOptimal );
		surface.descriptorSet->createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( 1u )
			, m_target->getDefaultView()
			, m_linearSampler->getSampler()
			, renderer::ImageLayout::eShaderReadOnlyOptimal );
		surface.descriptorSet->update();

		return true;
	}

	bool BloomPostEffect::doBuildCommandBuffer( castor3d::RenderPassTimer const & timer )
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_target->getWidth(), m_target->getHeight() };

		// Fill command buffer.
		static renderer::ClearValueArray const clearValues[FILTER_COUNT + 1]
		{
			{ renderer::ClearColorValue{ 1.0, 0.0, 0.0, 1.0 } },
			{ renderer::ClearColorValue{ 0.0, 1.0, 0.0, 1.0 } },
			{ renderer::ClearColorValue{ 0.0, 0.0, 1.0, 1.0 } },
			{ renderer::ClearColorValue{ 1.0, 1.0, 0.0, 1.0 } },
			{ renderer::ClearColorValue{ 0.0, 1.0, 1.0, 1.0 } },
		};
		castor3d::CommandsSemaphore hiPassCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & hiPassCmd = *hiPassCommands.commandBuffer;
		bool result = hiPassCmd.begin();

		if ( result )
		{
			hiPassCmd.resetQueryPool( timer.getQuery()
				, 0u
				, 2u );
			hiPassCmd.writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, timer.getQuery()
				, 0u );
			// Put target image in fragment shader input layout.
			hiPassCmd.memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_target->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eColourAttachmentOptimal
					, renderer::AccessFlag::eColourAttachmentWrite ) );

			// Hi-pass.
			hiPassCmd.beginRenderPass( *m_renderPass
				, *m_pipelines.hiPass.surface->frameBuffer
				, clearValues[0]
				, renderer::SubpassContents::eInline );
			hiPassCmd.setViewport( { size.width >> 1, size.height >> 1, 0, 0, } );
			hiPassCmd.setScissor( { 0, 0, size.width >> 1, size.height >> 1 } );
			hiPassCmd.bindPipeline( *m_pipelines.hiPass.pipeline );
			hiPassCmd.bindDescriptorSet( *m_pipelines.hiPass.surface->descriptorSet,
				*m_pipelines.hiPass.layout.pipelineLayout );
			hiPassCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			hiPassCmd.draw( 6u );
			hiPassCmd.endRenderPass();
			result = hiPassCmd.end();
			m_commands.emplace_back( std::move( hiPassCommands ) );
		}

		castor3d::CommandsSemaphore downScaleCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & downScaleCmd = *downScaleCommands.commandBuffer;
		result = downScaleCmd.begin();

		if ( result )
		{
			// Downscale through mipmaps generation.
			m_pipelines.hiPass.image->getTexture().generateMipmaps( downScaleCmd );
			result = downScaleCmd.end();
			m_commands.emplace_back( std::move( downScaleCommands ) );
		}

		castor3d::CommandsSemaphore blurCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & blurCmd = *blurCommands.commandBuffer;
		result = blurCmd.begin();

		if ( result )
		{
			// Blur passes.
			for ( uint32_t i = 0u; i < FILTER_COUNT; ++i )
			{
				auto & gaussianBlur = m_blurs[i];
				// Horizontal.
				blurCmd.beginRenderPass( gaussianBlur->getRenderPass()
					, gaussianBlur->getBlurXFrameBuffer()
					, clearValues[i]
					, renderer::SubpassContents::eSecondaryCommandBuffers );
				blurCmd.executeCommands( { gaussianBlur->getBlurXCommandBuffer() } );
				blurCmd.endRenderPass();

				// Vertical.
				blurCmd.beginRenderPass( gaussianBlur->getRenderPass()
					, gaussianBlur->getBlurYFrameBuffer()
					, clearValues[i]
					, renderer::SubpassContents::eSecondaryCommandBuffers );
				blurCmd.executeCommands( { gaussianBlur->getBlurYCommandBuffer() } );
				blurCmd.endRenderPass();
			}

			// Put Hi-pass general view in fragment shader input layout.
			blurCmd.memoryBarrier( renderer::PipelineStageFlag::eTransfer
				, renderer::PipelineStageFlag::eFragmentShader
				, m_pipelines.hiPass.image->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eUndefined, 0u ) );
			blurCmd.end();
			m_commands.emplace_back( std::move( blurCommands ) );
		}

		castor3d::CommandsSemaphore combineCommands
		{
			device.getGraphicsCommandPool().createCommandBuffer(),
			device.createSemaphore()
		};
		auto & combineCmd = *combineCommands.commandBuffer;
		result = combineCmd.begin();

		if ( result )
		{
			// Combine pass.
			combineCmd.beginRenderPass( *m_renderPass
				, *m_pipelines.combine.surface->frameBuffer
				, clearValues[FILTER_COUNT]
				, renderer::SubpassContents::eInline );
			combineCmd.setViewport( { size.width, size.height, 0, 0, } );
			combineCmd.setScissor( { 0, 0, size.width, size.height } );
			combineCmd.bindPipeline( *m_pipelines.combine.pipeline );
			combineCmd.bindDescriptorSet( *m_pipelines.combine.surface->descriptorSet
				, *m_pipelines.combine.layout.pipelineLayout );
			combineCmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			combineCmd.draw( 6u );
			combineCmd.endRenderPass();

			combineCmd.writeTimestamp( renderer::PipelineStageFlag::eTopOfPipe
				, timer.getQuery()
				, 1u );
			combineCmd.end();
			m_commands.emplace_back( std::move( combineCommands ) );
		}

		return result;
	}
}
