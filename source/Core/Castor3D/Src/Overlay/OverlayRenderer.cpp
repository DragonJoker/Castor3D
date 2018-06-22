#include "OverlayRenderer.hpp"

#include "Engine.hpp"

#include "BorderPanelOverlay.hpp"
#include "PanelOverlay.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Camera.hpp"
#include "Shader/PassBuffer/PassBuffer.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Buffer/Buffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBufferInheritanceInfo.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>
#include <Shader/ShaderProgram.hpp>
#include <Sync/Fence.hpp>

#include <Graphics/Font.hpp>

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

#if defined( drawText )
#	undef drawText
#endif

namespace castor3d
{
	//*********************************************************************************************

	static int32_t constexpr MaxCharsPerBuffer = 6000;
	static uint32_t constexpr MaxPanelsPerBuffer = 100u;

	namespace
	{
		static uint32_t constexpr MatrixUboBinding = 1u;
		static uint32_t constexpr OverlayUboBinding = 2u;
		static uint32_t constexpr TextMapBinding = 3u;
		static uint32_t constexpr DiffuseMapBinding = 4u;
		static uint32_t constexpr OpacityMapBinding = 5u;

		template< typename VertexT, typename BufferIndexT >
		uint32_t doFillBuffers( typename std::vector< VertexT >::const_iterator begin
			, uint32_t count
			, BufferIndexT & bufferIndex )
		{
			auto const & vertex = *begin;
			auto size = count * sizeof( VertexT );
			std::memcpy( &bufferIndex.pool.data[bufferIndex.index], &vertex, size );
			return count;
		}

		template< typename T >
		void doUpdateUbo( renderer::UniformBuffer< T > & ubo
			, uint32_t index
			, OverlayCategory const & overlay
			, Pass const & pass
			, Size const & size )
		{
			auto & data = ubo.getData( index );
			data.position = Point2f{ overlay.getAbsolutePosition() };
			data.renderSize = Point2i{ size.getWidth(), size.getHeight() };
			data.renderRatio = overlay.getRenderRatio( size );
			data.materialIndex = pass.getId();
		}

		template< typename OverlayT, typename QuadT >
		struct BorderSizeGetter
		{
			castor::Rectangle operator()( OverlayT const & overlay, castor::Size const & size )const
			{
				return castor::Rectangle{};
			}
		};

		template<>
		struct BorderSizeGetter< BorderPanelOverlay, OverlayRenderer::BorderPanelVertexBufferPool::Quad >
		{
			castor::Rectangle operator()( BorderPanelOverlay const & overlay, castor::Size const & size )const
			{
				castor::Rectangle result = overlay.getAbsoluteBorderSize( size );

				switch ( overlay.getBorderPosition() )
				{
				case BorderPosition::eMiddle:
					result.set( result.left() / 2
						, result.top() / 2
						, result.right() / 2
						, result.bottom() / 2 );
					break;
				case BorderPosition::eExternal:
					break;
				default:
					result = castor::Rectangle{};
					break;
				}

				return result;
			}
		};
	}

	//*********************************************************************************************

	OverlayRenderer::Preparer::Preparer( OverlayRenderer & renderer )
		: m_renderer{ renderer }
	{
	}

	void OverlayRenderer::Preparer::visit( PanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				doPrepareOverlay< PanelVertexBufferPool::Quad >( overlay
					, *pass
					, m_renderer.m_panelOverlays
					, m_renderer.m_panelVertexBuffers
					, overlay.getPanelVertex()
					, nullptr );
			}
		}
	}

	void OverlayRenderer::Preparer::visit( BorderPanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				doPrepareOverlay< PanelVertexBufferPool::Quad >( overlay
					, *pass
					, m_renderer.m_panelOverlays
					, m_renderer.m_panelVertexBuffers
					, overlay.getPanelVertex()
					, nullptr );
			}
		}

		if ( auto material = overlay.getBorderMaterial() )
		{
			for ( auto & pass : *material )
			{
				doPrepareOverlay< BorderPanelVertexBufferPool::Quad >( overlay
					, *pass
					, m_renderer.m_borderPanelOverlays
					, m_renderer.m_borderVertexBuffers
					, overlay.getBorderVertex()
					, nullptr );
			}
		}
	}

	void OverlayRenderer::Preparer::visit( TextOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				doPrepareOverlay< TextVertexBufferPool::Quad >( overlay
					, *pass
					, m_renderer.m_textOverlays
					, m_renderer.m_textVertexBuffers
					, overlay.getTextVertex()
					, overlay.getFontTexture() );
			}
		}
	}

	template< typename QuadT, typename OverlayT, typename BufferIndexT, typename BufferPoolT, typename VertexT >
	void OverlayRenderer::Preparer::doPrepareOverlay( OverlayT const & overlay
		, Pass const & pass
		, std::map< size_t, BufferIndexT > & overlays
		, std::vector< BufferPoolT > & vertexBuffers
		, std::vector < VertexT > const & vertices
		, FontTextureSPtr fontTexture )
	{
		if ( !vertices.empty() )
		{
			auto & bufferIndex = m_renderer.doGetVertexBuffer< BufferIndexT >( vertexBuffers
				, overlays
				, overlay.getOverlay()
				, pass
				, ( fontTexture
					? m_renderer.doGetTextNode( pass, *fontTexture->getTexture(), *fontTexture->getSampler() )
					: m_renderer.doGetPanelNode( pass ) )
				, getCurrentDevice( m_renderer )
				, ( fontTexture
					? *m_renderer.m_textDeclaration
					: *m_renderer.m_declaration )
				, MaxPanelsPerBuffer );
			doUpdateUbo( *bufferIndex.pool.ubo
				, bufferIndex.index
				, overlay
				, pass
				, m_renderer.m_size );
			doFillBuffers< VertexT >( vertices.begin()
				, uint32_t( vertices.size() )
				, bufferIndex );

			if ( !bufferIndex.descriptorSet )
			{
				if ( fontTexture )
				{
					bufferIndex.descriptorSet = m_renderer.doCreateDescriptorSet( bufferIndex.node.pipeline
						, pass.getTextureFlags() | TextureChannel::eText
						, pass
						, *bufferIndex.pool.ubo
						, bufferIndex.index
						, *fontTexture->getTexture()
						, *fontTexture->getSampler() );
					bufferIndex.connection = fontTexture->onChanged.connect( [&bufferIndex]( FontTexture const & )
						{
							bufferIndex.descriptorSet.reset();
						} );
				}
				else
				{
					bufferIndex.descriptorSet = m_renderer.doCreateDescriptorSet( bufferIndex.node.pipeline
						, pass.getTextureFlags()
						, pass
						, *bufferIndex.pool.ubo
						, bufferIndex.index );
				}
			}

			auto borderSize = BorderSizeGetter< OverlayT, QuadT >{}( overlay, m_renderer.m_size );
			auto borderOffset = castor::Size{ uint32_t( borderSize.left() ), uint32_t( borderSize.top() ) };
			auto borderExtent = borderOffset + castor::Size{ uint32_t( borderSize.right() ), uint32_t( borderSize.bottom() ) };
			auto position = overlay.getAbsolutePosition( m_renderer.m_size ) - borderOffset;
			auto size = overlay.getAbsoluteSize( m_renderer.m_size ) + borderExtent;
			auto & commandBuffer = *m_renderer.m_commandBuffer;
			commandBuffer.bindPipeline( *bufferIndex.node.pipeline.pipeline );
			commandBuffer.setViewport( { m_renderer.m_size.getWidth(), m_renderer.m_size.getHeight(), 0, 0 } );
			commandBuffer.setScissor( { position[0], position[1], size[0], size[1] } );
			commandBuffer.bindDescriptorSet( *bufferIndex.descriptorSet
				, *bufferIndex.node.pipeline.pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u
				, bufferIndex.pool.buffer->getBuffer()
				, uint32_t( bufferIndex.index * sizeof( QuadT ) ) );
			commandBuffer.draw( uint32_t( vertices.size() )
				, 1u
				, 0u
				, 0u );
		}
	}

	//*********************************************************************************************

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferPool< VertexT, CountT >::VertexBufferPool( renderer::Device const & device
		, renderer::VertexLayout const & declaration
		, uint32_t count )
		: maxCount{ count }
		, data{ count, Quad{} }
		, declaration{ declaration }
		, buffer{ renderer::makeVertexBuffer< Quad >( device
			, count
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible ) }
		, ubo{ renderer::makeUniformBuffer< Configuration >( device
			, count
			, 0u
			, renderer::MemoryPropertyFlag::eHostVisible ) }
	{
		for ( auto i = 0u; i < count; ++i )
		{
			free.insert( i );
		}
	}

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferIndex< VertexT, CountT > OverlayRenderer::VertexBufferPool< VertexT, CountT >::allocate( OverlayRenderNode & node )
	{
		OverlayRenderer::VertexBufferIndex< VertexT, CountT > result{ *this, node, ~( 0u ) };

		if ( !free.empty() )
		{
			result.index = *free.begin();
			result.geometryBuffers.noTexture.vbo = { buffer->getBuffer() };
			result.geometryBuffers.noTexture.layouts = { declaration };
			result.geometryBuffers.noTexture.vboOffsets = { result.index };
			result.geometryBuffers.textured.vbo = { buffer->getBuffer() };
			result.geometryBuffers.textured.layouts = { declaration };
			result.geometryBuffers.textured.vboOffsets = { result.index };
			free.erase( free.begin() );
		}

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPool< VertexT, CountT >::deallocate( OverlayRenderer::VertexBufferIndex< VertexT, CountT > const & index )
	{
		REQUIRE( &index.pool == this );
		free.insert( index.index );
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPool< VertexT, CountT >::upload()
	{
		if ( auto bufferData = buffer->lock( 0u
			, buffer->getCount()
			, renderer::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( bufferData, data.data(), buffer->getSize() );
			buffer->flush( 0u, buffer->getCount() );
			buffer->unlock();
		}

		ubo->upload( 0u, uint32_t( data.size() ) );
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderSystem & renderSystem
		, renderer::TextureView const & target )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_target{ target }
		, m_matrixUbo{ *renderSystem.getEngine() }
	{
	}

	OverlayRenderer::~OverlayRenderer()
	{
	}

	void OverlayRenderer::initialise()
	{
		auto & device = getCurrentDevice( *this );

		if ( !m_renderPass )
		{
			doCreateRenderPass();
		}

		if ( !m_commandBuffer )
		{
			m_commandBuffer = device.getGraphicsCommandPool().createCommandBuffer();
			m_finished = device.createSemaphore();
			m_fence = device.createFence( renderer::FenceCreateFlag::eSignaled );
		}

		if ( !m_declaration )
		{
			m_declaration = renderer::makeLayout< OverlayCategory::Vertex >( 0u );
			m_declaration->createAttribute( 0u
				, renderer::Format::eR32G32_SFLOAT
				, offsetof( OverlayCategory::Vertex, coords ) );
			m_declaration->createAttribute( 1u
				, renderer::Format::eR32G32_SFLOAT
				, offsetof( OverlayCategory::Vertex, texture ) );
		}

		if ( !m_textDeclaration )
		{
			m_textDeclaration = renderer::makeLayout< TextOverlay::Vertex >( 0u );
			m_textDeclaration->createAttribute( 0u
				, renderer::Format::eR32G32_SFLOAT
				, offsetof( TextOverlay::Vertex, coords ) );
			m_textDeclaration->createAttribute( 1u
				, renderer::Format::eR32G32_SFLOAT
				, offsetof( TextOverlay::Vertex, texture ) );
			m_textDeclaration->createAttribute( 2u
				, renderer::Format::eR32G32_SFLOAT
				, offsetof( TextOverlay::Vertex, text ) );
		}

		doGetPipeline( 0u, m_panelPipelines );
		doGetPipeline( uint32_t( TextureChannel::eDiffuse ), m_panelPipelines );

		// Create one panel overlays buffer pool
		m_panelVertexBuffers.emplace_back( std::make_unique< PanelVertexBufferPool >( device
			, *m_declaration
			, MaxPanelsPerBuffer ) );

		// Create one border overlays buffer pool
		m_borderVertexBuffers.emplace_back( std::make_unique< BorderPanelVertexBufferPool >( device
			, *m_declaration
			, MaxPanelsPerBuffer ) );

		// create one text overlays buffer
		m_textVertexBuffers.emplace_back( std::make_unique< TextVertexBufferPool >( device
			, *m_textDeclaration
			, MaxPanelsPerBuffer ) );
	}

	void OverlayRenderer::cleanup()
	{
		m_matrixUbo.cleanup();
		m_panelOverlays.clear();
		m_borderPanelOverlays.clear();
		m_textOverlays.clear();
		m_mapPanelNodes.clear();
		m_mapTextNodes.clear();
		m_panelPipelines.clear();
		m_textPipelines.clear();
		m_panelVertexBuffers.clear();
		m_borderVertexBuffers.clear();
		m_textVertexBuffers.clear();
		m_fence.reset();
		m_commandBuffer.reset();
		m_declaration.reset();
		m_textDeclaration.reset();
		m_frameBuffer.reset();
		m_renderPass.reset();
		m_toWait = nullptr;
		m_finished.reset();
	}

	void OverlayRenderer::beginPrepare( Camera const & camera
		, RenderPassTimer const & timer
		, renderer::Semaphore const & toWait )
	{
		auto size = camera.getSize();

		if ( m_size != size )
		{
			m_sizeChanged = true;
			m_size = size;
			m_matrixUbo.update( getRenderSystem()->getOrtho( 0.0f
				, float( m_size.getWidth() )
				, 0.0f
				, float( m_size.getHeight() )
				, -1.0f
				, 1.0f ) );
		}

		static renderer::ClearColorValue clear{ 0.0f, 0.0f, 0.0f, 0.0f };
		m_toWait = &toWait;
		m_commandBuffer->begin( renderer::CommandBufferUsageFlag::eOneTimeSubmit );
		timer.beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { clear }
			, renderer::SubpassContents::eInline );
	}

	void OverlayRenderer::endPrepare( RenderPassTimer const & timer )
	{
		m_commandBuffer->endRenderPass();
		timer.endPass( *m_commandBuffer );
		m_commandBuffer->end();

		for ( auto & pool : m_panelVertexBuffers )
		{
			pool->upload();
		}

		for ( auto & pool : m_borderVertexBuffers )
		{
			pool->upload();
		}

		for ( auto & pool : m_textVertexBuffers )
		{
			pool->upload();
		}
		
		m_sizeChanged = false;
	}

	void OverlayRenderer::render( RenderPassTimer & timer )
	{
		auto & queue = getCurrentDevice( *this ).getGraphicsQueue();
		timer.notifyPassRender();
		queue.submit( *m_commandBuffer
			, *m_toWait
			, renderer::PipelineStageFlag::eColourAttachmentOutput
			, *m_finished
			, nullptr );
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetPanelNode( Pass const & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto flags = pass.getTextureFlags();
			auto & pipeline = doGetPipeline( flags, m_panelPipelines );
			it = m_mapPanelNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetTextNode( Pass const & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto flags = pass.getTextureFlags();
			addFlag( flags, TextureChannel::eText );
			auto & pipeline = doGetPipeline( flags, m_textPipelines );
			it = m_mapTextNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	renderer::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureChannels textureFlags
		, Pass const & pass
		, renderer::UniformBuffer< Configuration > const & ubo
		, uint32_t index
		, bool update )
	{
		remFlag( textureFlags, TextureChannel::eNormal );
		remFlag( textureFlags, TextureChannel::eSpecular );
		remFlag( textureFlags, TextureChannel::eGloss );
		remFlag( textureFlags, TextureChannel::eHeight );
		remFlag( textureFlags, TextureChannel::eEmissive );
		auto result = pipeline.descriptorPool->createDescriptorSet();

		// Pass buffer
		getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( PassBufferIndex ) );
		// Matrix UBO
		result->createBinding( pipeline.descriptorLayout->getBinding( MatrixUboBinding )
			, m_matrixUbo.getUbo()
			, 0u
			, 1u );
		// Overlay UBO
		result->createBinding( pipeline.descriptorLayout->getBinding( OverlayUboBinding )
			, ubo
			, index
			, 1u );

		if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
		{
			result->createBinding( pipeline.descriptorLayout->getBinding( DiffuseMapBinding )
				, pass.getTextureUnit( TextureChannel::eDiffuse )->getTexture()->getDefaultView()
				, pass.getTextureUnit( TextureChannel::eDiffuse )->getSampler()->getSampler() );
		}

		if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			result->createBinding( pipeline.descriptorLayout->getBinding( OpacityMapBinding )
				, pass.getTextureUnit( TextureChannel::eOpacity )->getTexture()->getDefaultView()
				, pass.getTextureUnit( TextureChannel::eOpacity )->getSampler()->getSampler() );
		}

		if ( update )
		{
			result->update();
		}

		return result;
	}

	renderer::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureChannels textureFlags
		, Pass const & pass
		, renderer::UniformBuffer< Configuration > const & ubo
		, uint32_t index
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto result = doCreateDescriptorSet( pipeline
			, textureFlags
			, pass
			, ubo
			, index
			, false );

		if ( checkFlag( textureFlags, TextureChannel::eText ) )
		{
			result->createBinding( pipeline.descriptorLayout->getBinding( TextMapBinding )
				, texture.getDefaultView()
				, sampler.getSampler() );
		}

		result->update();
		return result;
	}

	void OverlayRenderer::doCreateRenderPass()
	{
		auto & device = getCurrentDevice( *this );

		renderer::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target.getFormat();
		renderPass.attachments[0].loadOp = renderer::AttachmentLoadOp::eLoad;
		renderPass.attachments[0].storeOp = renderer::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = renderer::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = renderer::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = renderer::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = renderer::ImageLayout::eShaderReadOnlyOptimal;
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

		renderer::FrameBufferAttachmentArray attaches;
		attaches.emplace_back( *m_renderPass->getAttachments().begin(), m_target );
		m_frameBuffer = m_renderPass->createFrameBuffer( { m_target.getTexture().getDimensions().width, m_target.getTexture().getDimensions().height }
			, std::move( attaches ) );
	}

	OverlayRenderer::Pipeline & OverlayRenderer::doGetPipeline( TextureChannels textureFlags
		, std::map< uint32_t, Pipeline > & pipelines )
	{
		// Remove unwanted flags
		remFlag( textureFlags, TextureChannel::eNormal );
		remFlag( textureFlags, TextureChannel::eSpecular );
		remFlag( textureFlags, TextureChannel::eGloss );
		remFlag( textureFlags, TextureChannel::eHeight );
		remFlag( textureFlags, TextureChannel::eEmissive );
		auto it = pipelines.find( textureFlags );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			auto & device = getCurrentDevice( *this );
			auto program = doCreateOverlayProgram( textureFlags );

			renderer::ColourBlendState blState{};
			blState.attachs.push_back( renderer::ColourBlendStateAttachment{
				true,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd,
				renderer::BlendFactor::eSrcAlpha,
				renderer::BlendFactor::eInvSrcAlpha,
				renderer::BlendOp::eAdd
			} );
			renderer::DescriptorSetLayoutBindingArray bindings;

			// Pass buffer
			bindings.emplace_back( getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );
			// Matrix UBO
			bindings.emplace_back( MatrixUboBinding
				, renderer::DescriptorType::eUniformBuffer
				, renderer::ShaderStageFlag::eVertex );
			// Matrix UBO
			bindings.emplace_back( OverlayUboBinding
				, renderer::DescriptorType::eUniformBuffer
				, renderer::ShaderStageFlag::eVertex | renderer::ShaderStageFlag::eFragment );
			auto vertexLayout = m_declaration.get();

			if ( checkFlag( textureFlags, TextureChannel::eText ) )
			{
				vertexLayout = m_textDeclaration.get();
				bindings.emplace_back( TextMapBinding
					, renderer::DescriptorType::eCombinedImageSampler
					, renderer::ShaderStageFlag::eFragment );
			}

			if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
			{
				bindings.emplace_back( DiffuseMapBinding
					, renderer::DescriptorType::eCombinedImageSampler
					, renderer::ShaderStageFlag::eFragment );
			}

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				bindings.emplace_back( OpacityMapBinding
					, renderer::DescriptorType::eCombinedImageSampler
					, renderer::ShaderStageFlag::eFragment );
			}

			auto descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
			auto descriptorPool = descriptorLayout->createPool( 1000u );
			auto pipelineLayout = device.createPipelineLayout( *descriptorLayout );
			auto pipeline = pipelineLayout->createPipeline( {
				program,
				*m_renderPass,
				renderer::VertexInputState::create( *vertexLayout ),
				{ renderer::PrimitiveTopology::eTriangleList },
				renderer::RasterisationState{ 0u, false, false, renderer::PolygonMode::eFill, renderer::CullModeFlag::eNone },
				renderer::MultisampleState{},
				blState,
				{ renderer::DynamicState::eViewport, renderer::DynamicState::eScissor },
				renderer::DepthStencilState{ 0u, false, false },
			} );
			it = pipelines.emplace( textureFlags
				, Pipeline{ std::move( descriptorLayout )
					, std::move( descriptorPool )
					, std::move( pipelineLayout )
					, std::move( pipeline )
				} ).first;
		}

		return it->second;
	}

	renderer::ShaderStageStateArray OverlayRenderer::doCreateOverlayProgram( TextureChannels const & textureFlags )
	{
		using namespace glsl;
		using namespace shader;

		// Vertex shader
		glsl::Shader vtx;
		{
			auto writer = getRenderSystem()->createGlslWriter();

			UBO_MATRIX( writer, MatrixUboBinding, 0u );
			UBO_OVERLAY( writer, OverlayUboBinding, 0u );

			// Shader inputs
			uint32_t index = 0u;
			auto position = writer.declAttribute< Vec2 >( cuT( "position" ), 0u );
			auto texture = writer.declAttribute< Vec2 >( cuT( "texcoord" ), 1u, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto text = writer.declAttribute< Vec2 >( cuT( "text" ), 2u, checkFlag( textureFlags, TextureChannel::eText ) );

			// Shader outputs
			auto vtx_text = writer.declOutput< Vec2 >( cuT( "vtx_text" ), 0u, checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 1u, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto out = gl_PerVertex{ writer };

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				if ( checkFlag( textureFlags, TextureChannel::eText ) )
				{
					vtx_text = text;
				}

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					vtx_texture = texture;
				}

				auto size = writer.declLocale( cuT( "size" )
					, c3d_renderRatio * c3d_renderSize );
				out.gl_Position() = c3d_projection * vec4( size * writer.paren( c3d_position + position )
					, 0.0
					, 1.0 );
			} );

			vtx = writer.finalise();
		}

		// Pixel shader
		glsl::Shader pxl;
		{
			auto writer = getRenderSystem()->createGlslWriter();

			std::unique_ptr< Materials > materials;

			switch ( getRenderSystem()->getEngine()->getMaterialsType() )
			{
			case MaterialType::eLegacy:
				materials = std::make_unique< shader::LegacyMaterials >( writer );
				break;

			case MaterialType::ePbrMetallicRoughness:
				materials = std::make_unique< PbrMRMaterials >( writer );
				break;

			case MaterialType::ePbrSpecularGlossiness:
				materials = std::make_unique< PbrSGMaterials >( writer );
				break;
			}

			materials->declare();
			UBO_OVERLAY( writer, OverlayUboBinding, 0u );

			// Shader inputs
			auto vtx_text = writer.declInput< Vec2 >( cuT( "vtx_text" )
				, 0u
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" )
				, 1u
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapText = writer.declSampler< Sampler2D >( cuT( "c3d_mapText" )
				, TextMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto c3d_mapDiffuse = writer.declSampler< Sampler2D >( cuT( "c3d_mapDiffuse" )
				, DiffuseMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapOpacity = writer.declSampler< Sampler2D >( cuT( "c3d_mapOpacity" )
				, OpacityMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eOpacity ) );

			// Shader outputs
			auto pxl_fragColor = writer.declFragData< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
			{
				auto material = materials->getBaseMaterial( c3d_materialIndex );
				auto diffuse = writer.declLocale( cuT( "diffuse" )
					, material->m_diffuse() );
				auto alpha = writer.declLocale( cuT( "alpha" )
					, material->m_opacity() );

				if ( checkFlag( textureFlags, TextureChannel::eText ) )
				{
					alpha *= texture( c3d_mapText, vtx_text, 0.0_f ).r();
				}

				if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
				{
					diffuse = texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).xyz();
				}

				if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
				{
					alpha *= texture( c3d_mapOpacity, vtx_texture, 0.0_f ).r();
				}

				pxl_fragColor = vec4( diffuse.xyz(), alpha );
			} );

			pxl = writer.finalise();
		}

		renderer::ShaderStageStateArray result;
		result.push_back( { getCurrentDevice( *this ).createShaderModule( renderer::ShaderStageFlag::eVertex ) } );
		result.push_back( { getCurrentDevice( *this ).createShaderModule( renderer::ShaderStageFlag::eFragment ) } );
		result[0].module->loadShader( vtx.getSource() );
		result[1].module->loadShader( pxl.getSource() );
		return result;
	}

	//*********************************************************************************************
}
