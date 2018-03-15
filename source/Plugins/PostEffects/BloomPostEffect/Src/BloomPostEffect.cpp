#include "BloomPostEffect.hpp"

#include <Engine.hpp>
#include <Cache/SamplerCache.hpp>
#include <Mesh/Vertex.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Render/RenderSystem.hpp>
#include <Render/RenderTarget.hpp>
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
			auto gl_Position = writer.declBuiltin< Vec4 >( cuT( "gl_Position" ) );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				vtx_texture = position;
				gl_Position = vec4( position.xy(), 0.0, 1.0 );
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
				pxl_fragColor = vec4( texture( c3d_mapDiffuse, vec2( vtx_texture.x(), vtx_texture.y() ) ).xyz(), 1.0 );
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
				pxl_fragColor = texture( c3d_mapScene, vtx_texture );

				for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
				{
					pxl_fragColor += texture( c3d_mapPasses, vtx_texture, 0 );
				}
			} );
			return writer.finalise();
		}
	}

	//*********************************************************************************************

	BloomPostEffect::Surface::Surface( renderer::Texture const & texture
		, uint32_t mipLevel
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
		imageView.subresourceRange.baseMipLevel = mipLevel;
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
			, renderTarget
			, renderSystem
			, params )
		, m_size( 5u )
		, m_buffer
		{
			castor::Point2f{ -1.0f, -1.0f },
			castor::Point2f{ +1.0f, -1.0f },
			castor::Point2f{ +1.0f, +1.0f },
			castor::Point2f{ -1.0f, +1.0f },
		}
	{
		String count;

		if ( params.get( cuT( "Size" ), count ) )
		{
			m_size = uint32_t( string::toLong( count ) );
		}

		m_linearSampler = doCreateSampler( true );
		m_nearestSampler = doCreateSampler( false );
	}

	PostEffectSPtr BloomPostEffect::create( RenderTarget & renderTarget
		, RenderSystem & renderSystem
		, Parameters const & params )
	{
		return std::make_shared< BloomPostEffect >( renderTarget
			, renderSystem
			, params );
	}

	bool BloomPostEffect::initialise()
	{
		auto & device = *getRenderSystem()->getCurrentDevice();
		renderer::Extent2D size{ m_renderTarget.getSize()[0], m_renderTarget.getSize()[1] };

		// Create descriptor and pipeline layouts
		renderer::DescriptorSetLayoutBindingArray setLayoutBindings
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_pipelines.combine.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.combine.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.combine.layout.descriptorLayout );
		m_pipelines.combine.layout.descriptorPool = m_pipelines.combine.layout.descriptorLayout->createPool( 1u );

		setLayoutBindings = 
		{
			{ 0u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment },
			{ 1u, renderer::DescriptorType::eCombinedImageSampler, renderer::ShaderStageFlag::eFragment }
		};
		m_pipelines.hiPass.layout.descriptorLayout = device.createDescriptorSetLayout( std::move( setLayoutBindings ) );
		m_pipelines.hiPass.layout.pipelineLayout = device.createPipelineLayout( *m_pipelines.hiPass.layout.descriptorLayout );
		m_pipelines.hiPass.layout.descriptorPool = m_pipelines.hiPass.layout.descriptorLayout->createPool( FILTER_COUNT );

		// Create the render pass.
		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].index = 0u;
		renderPass.attachments[0].format = m_renderTarget.getPixelFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = renderer::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, renderer::ImageLayout::eColourAttachmentOptimal } );

		renderPass.dependencies.resize( 2u );
		renderPass.dependencies[0].srcSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[0].dstSubpass = 0u;
		renderPass.dependencies[0].srcStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[0].dstStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[0].srcAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[0].dstAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[0].dependencyFlags = renderer::DependencyFlag::eByRegion;

		renderPass.dependencies[1].srcSubpass = 0u;
		renderPass.dependencies[1].dstSubpass = renderer::ExternalSubpass;
		renderPass.dependencies[1].srcStageMask = renderer::PipelineStageFlag::eColourAttachmentOutput;
		renderPass.dependencies[1].dstStageMask = renderer::PipelineStageFlag::eFragmentShader;
		renderPass.dependencies[1].srcAccessMask = renderer::AccessFlag::eColourAttachmentWrite;
		renderPass.dependencies[1].dstAccessMask = renderer::AccessFlag::eShaderRead;
		renderPass.dependencies[1].dependencyFlags = renderer::DependencyFlag::eByRegion;

		m_renderPass = device.createRenderPass( renderPass );

		// Create textures
		renderer::ImageCreateInfo image{};
		image.flags = renderer::ImageCreateFlag::eCubeCompatible;
		image.arrayLayers = 1u;
		image.extent.width = size.width;
		image.extent.height = size.height;
		image.extent.depth = 1u;
		image.format = m_renderTarget.getPixelFormat();
		image.imageType = renderer::TextureType::e2D;
		image.initialLayout = renderer::ImageLayout::eUndefined;
		image.mipLevels = FILTER_COUNT;
		image.samples = renderer::SampleCountFlag::e1;
		image.sharingMode = renderer::SharingMode::eExclusive;
		image.tiling = renderer::ImageTiling::eOptimal;
		image.usage = renderer::ImageUsageFlag::eColourAttachment | renderer::ImageUsageFlag::eSampled;

		m_pipelines.hiPass.image = device.createTexture( image, renderer::MemoryPropertyFlag::eDeviceLocal );
		image.mipLevels = 1u;
		m_pipelines.combine.image = device.createTexture( image, renderer::MemoryPropertyFlag::eDeviceLocal );

		bool result = doInitialiseHiPass();

		if ( result )
		{
			for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
			{
				m_blurs.push_back( std::make_shared< GaussianBlur >( *getRenderSystem()->getEngine()
					, *m_pipelines.hiPass.surfaces[i].view
					, size
					, m_renderTarget.getPixelFormat()
					, m_size ) );
			}
		}

		if ( result )
		{
			result = doInitialiseCombine();
		}

		if ( result )
		{
			result = doBuildCommandBuffer();
		}

		return result;
	}

	bool BloomPostEffect::doBuildCommandBuffer()
	{
		renderer::Extent2D size{ m_renderTarget.getSize()[0], m_renderTarget.getSize()[1] };
		std::vector< renderer::Extent2D > dimensions( FILTER_COUNT );
		dimensions[0] = size;

		for ( uint32_t i = 1; i < FILTER_COUNT; ++i )
		{
			dimensions[i] = renderer::Extent2D{ dimensions[i - 1].width >> 1, dimensions[i - 1].height >> 1 };
		}

		// Fill command buffer.
		renderer::ClearValueArray clearValues
		{
			renderer::ClearColorValue{}
		};

		bool result = m_commandBuffer->begin();

		if ( result )
		{
			// Put image in the right state for rendering.
			m_commandBuffer->memoryBarrier( renderer::PipelineStageFlag::eColourAttachmentOutput
				, renderer::PipelineStageFlag::eFragmentShader
				, m_renderTarget.getTexture().getTexture()->getDefaultView().makeShaderInputResource( renderer::ImageLayout::eColourAttachmentOptimal
					, renderer::AccessFlag::eColourAttachmentWrite ) );

			// Hi-passes
			for ( uint32_t i = 0u; i < FILTER_COUNT; ++i )
			{
				auto & surface = m_pipelines.hiPass.surfaces[i];

				m_commandBuffer->beginRenderPass( *m_renderPass
					, *surface.frameBuffer
					, clearValues
					, renderer::SubpassContents::eInline );
				m_commandBuffer->setViewport( { dimensions[i].width, dimensions[i].height, 0, 0, } );
				m_commandBuffer->setScissor( { 0, 0, dimensions[i].width, dimensions[i].height } );
				m_commandBuffer->bindPipeline( *m_pipelines.hiPass.pipeline );
				m_commandBuffer->bindDescriptorSet( *surface.descriptorSet, *m_pipelines.hiPass.layout.pipelineLayout );
				m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
				m_commandBuffer->draw( 4u );
				m_commandBuffer->endRenderPass();
			}

			// Blur passes
			for ( auto & gaussianBlur : m_blurs )
			{
				// Horizontal
				m_commandBuffer->beginRenderPass( gaussianBlur->getBlurXPass()
					, gaussianBlur->getBlurXFrameBuffer()
					, clearValues
				, renderer::SubpassContents::eSecondaryCommandBuffers );
				m_commandBuffer->executeCommands( { gaussianBlur->getBlurXCommandBuffer() } );
				m_commandBuffer->endRenderPass();

				// Vertical
				m_commandBuffer->beginRenderPass( gaussianBlur->getBlurYPass()
					, gaussianBlur->getBlurYFrameBuffer()
					, clearValues
				, renderer::SubpassContents::eSecondaryCommandBuffers );
				m_commandBuffer->executeCommands( { gaussianBlur->getBlurYCommandBuffer() } );
				m_commandBuffer->endRenderPass();
			}

			// Combine pass
			auto & surface = m_pipelines.combine.surfaces[0];
			m_commandBuffer->beginRenderPass( *m_renderPass
				, *surface.frameBuffer
				, clearValues
				, renderer::SubpassContents::eInline );
			m_commandBuffer->setViewport( { dimensions[0].width, dimensions[0].height, 0, 0, } );
			m_commandBuffer->setScissor( { 0, 0, dimensions[0].width, dimensions[0].height } );
			m_commandBuffer->bindPipeline( *m_pipelines.combine.pipeline );
			m_commandBuffer->bindDescriptorSet( *surface.descriptorSet, *m_pipelines.combine.layout.pipelineLayout );
			m_commandBuffer->bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
			m_commandBuffer->draw( 4u );
			m_commandBuffer->endRenderPass();

			m_commandBuffer->end();
		}

		return result;
	}

	void BloomPostEffect::cleanup()
	{
		m_vertexBuffer.reset();
		m_blurs.clear();

		m_pipelines.combine.pipeline.reset();
		m_pipelines.combine.surfaces.clear();
		m_pipelines.combine.image.reset();
		m_pipelines.combine.layout.pipelineLayout.reset();
		m_pipelines.combine.layout.descriptorPool.reset();
		m_pipelines.combine.layout.descriptorLayout.reset();

		m_pipelines.hiPass.pipeline.reset();
		m_pipelines.hiPass.surfaces.clear();
		m_pipelines.hiPass.image.reset();
		m_pipelines.hiPass.layout.pipelineLayout.reset();
		m_pipelines.hiPass.layout.descriptorPool.reset();
		m_pipelines.hiPass.layout.descriptorLayout.reset();

		m_renderPass.reset();
	}

	bool BloomPostEffect::doWriteInto( TextFile & p_file )
	{
		return p_file.writeText( cuT( " -Size=" ) + string::toString( m_size ) ) > 0;
	}

	SamplerSPtr BloomPostEffect::doCreateSampler( bool p_linear )
	{
		String name = cuT( "BloomSampler_" );
		renderer::Filter mode;

		if ( p_linear )
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
		}
		else
		{
			sampler = m_renderTarget.getEngine()->getSamplerCache().find( name );
		}

		return sampler;
	}

	bool BloomPostEffect::doInitialiseHiPass()
	{
		renderer::Extent2D size{ m_renderTarget.getSize()[0], m_renderTarget.getSize()[1] };
		auto & device = *getRenderSystem()->getCurrentDevice();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const hipass = getHiPassProgram( getRenderSystem() );

		// Create views and associated frame buffers
		std::vector< renderer::Extent2D > dimensions( FILTER_COUNT );
		dimensions[0] = size;

		for ( uint32_t i = 1; i < FILTER_COUNT; ++i )
		{
			dimensions[i] = renderer::Extent2D{ dimensions[i - 1].width >> 1, dimensions[i - 1].height >> 1 };
		}

		for ( uint32_t i = 0; i < FILTER_COUNT; ++i )
		{
			m_pipelines.hiPass.surfaces.emplace_back( *m_pipelines.hiPass.image
				, i
				, dimensions[i]
				, *m_renderPass );
		}

		// Create global view on hi-pass image.
		renderer::ImageViewCreateInfo imageView{};
		imageView.components.r = renderer::ComponentSwizzle::eIdentity;
		imageView.components.g = renderer::ComponentSwizzle::eIdentity;
		imageView.components.b = renderer::ComponentSwizzle::eIdentity;
		imageView.components.a = renderer::ComponentSwizzle::eIdentity;
		imageView.format = m_pipelines.hiPass.image->getFormat();
		imageView.viewType = renderer::TextureViewType::e2D;
		imageView.subresourceRange.aspectMask = renderer::getAspectMask( imageView.format );
		imageView.subresourceRange.baseMipLevel = 0u;
		imageView.subresourceRange.levelCount = FILTER_COUNT;
		imageView.subresourceRange.baseArrayLayer = 0u;
		imageView.subresourceRange.layerCount = 1u;
		m_hiPassMipView = m_pipelines.hiPass.image->createView( imageView );

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( NonTexturedQuad ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( hipass.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ false, false }
		};
		m_pipelines.hiPass.pipeline = m_pipelines.hiPass.layout.pipelineLayout->createPipeline( pipeline );
		auto * srcView = &m_renderTarget.getTexture().getTexture()->getDefaultView();

		// Create descriptors
		for ( auto & surface : m_pipelines.hiPass.surfaces )
		{
			surface.descriptorSet = m_pipelines.hiPass.layout.descriptorPool->createDescriptorSet( 0u );
			surface.descriptorSet->createBinding( m_pipelines.hiPass.layout.descriptorLayout->getBinding( 0u )
				, *srcView
				, m_linearSampler->getSampler()
				, renderer::ImageLayout::eShaderReadOnlyOptimal );
			surface.descriptorSet->update();
			srcView = surface.view.get();
		}

		return true;
	}

	bool BloomPostEffect::doInitialiseCombine()
	{
		renderer::Extent2D size{ m_renderTarget.getSize()[0], m_renderTarget.getSize()[1] };
		auto & device = *getRenderSystem()->getCurrentDevice();
		auto const vertex = getVertexProgram( getRenderSystem() );
		auto const combine = getCombineProgram( getRenderSystem() );

		// Create view and associated frame buffer.
		m_pipelines.combine.surfaces.emplace_back( *m_pipelines.combine.image
			, 0u
			, size
			, *m_renderPass );

		// Create pipeline
		renderer::VertexInputState inputState;
		inputState.vertexBindingDescriptions.push_back( { 0u, sizeof( NonTexturedQuad ), renderer::VertexInputRate::eVertex } );
		inputState.vertexAttributeDescriptions.push_back( { 0u, 0u, renderer::Format::eR32G32_SFLOAT, 0u } );

		renderer::ShaderStageStateArray stages;
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		stages.push_back( { device.createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		stages[0].module->loadShader( vertex.getSource() );
		stages[1].module->loadShader( combine.getSource() );

		renderer::GraphicsPipelineCreateInfo pipeline
		{
			stages,
			*m_renderPass,
			inputState,
			renderer::InputAssemblyState{ renderer::PrimitiveTopology::eTriangleStrip },
			renderer::RasterisationState{},
			renderer::MultisampleState{},
			renderer::ColourBlendState::createDefault(),
			{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
			renderer::DepthStencilState{ false, false }
		};
		m_pipelines.combine.pipeline = m_pipelines.combine.layout.pipelineLayout->createPipeline( pipeline );

		// Create descriptor
		auto & surface = m_pipelines.combine.surfaces[0];
		surface.descriptorSet = m_pipelines.combine.layout.descriptorPool->createDescriptorSet( 0u );
		surface.descriptorSet->createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( 0u )
			, *m_hiPassMipView
			, m_linearSampler->getSampler()
			, renderer::ImageLayout::eShaderReadOnlyOptimal );
		surface.descriptorSet->createBinding( m_pipelines.combine.layout.descriptorLayout->getBinding( 1u )
			, m_renderTarget.getTexture().getTexture()->getDefaultView()
			, m_linearSampler->getSampler()
			, renderer::ImageLayout::eShaderReadOnlyOptimal );
		surface.descriptorSet->update();

		return true;
	}
}
