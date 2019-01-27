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
#include <Shader/GlslToSpv.hpp>
#include <Sync/Fence.hpp>

#include <Graphics/Font.hpp>

#include <ShaderWriter/Source.hpp>
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
		void doUpdateUbo( ashes::UniformBuffer< T > & ubo
			, uint32_t index
			, OverlayCategory const & overlay
			, Pass const & pass
			, Size const & size )
		{
			auto & data = ubo.getData( index );
			auto position = overlay.getAbsolutePosition();
			auto ratio = overlay.getRenderRatio( size );
			data.positionRatio = Point4f
			{
				position[0],
				position[1],
				ratio[0],
				ratio[1],
			};
			data.renderSizeIndex = Point4i
			{
				size.getWidth(),
				size.getHeight(),
				pass.getId(),
				0,
			};
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
				if ( !pass->isImplicit() )
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
	}

	void OverlayRenderer::Preparer::visit( BorderPanelOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
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

		if ( auto material = overlay.getBorderMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
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
	}

	void OverlayRenderer::Preparer::visit( TextOverlay const & overlay )
	{
		if ( auto material = overlay.getMaterial() )
		{
			for ( auto & pass : *material )
			{
				if ( !pass->isImplicit() )
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
	OverlayRenderer::VertexBufferPool< VertexT, CountT >::VertexBufferPool( ashes::Device const & device
		, ashes::VertexLayout const & declaration
		, uint32_t count )
		: maxCount{ count }
		, data{ count, Quad{} }
		, declaration{ declaration }
		, buffer{ ashes::makeVertexBuffer< Quad >( device
			, count
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible ) }
		, ubo{ ashes::makeUniformBuffer< Configuration >( device
			, count
			, 0u
			, ashes::MemoryPropertyFlag::eHostVisible ) }
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
		CU_Require( &index.pool == this );
		free.insert( index.index );
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPool< VertexT, CountT >::upload()
	{
		if ( auto bufferData = buffer->lock( 0u
			, buffer->getCount()
			, ashes::MemoryMapFlag::eWrite ) )
		{
			std::memcpy( bufferData, data.data(), buffer->getSize() );
			buffer->flush( 0u, buffer->getCount() );
			buffer->unlock();
		}

		ubo->upload( 0u, uint32_t( data.size() ) );
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderSystem & renderSystem
		, ashes::TextureView const & target )
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
			m_fence = device.createFence( ashes::FenceCreateFlag::eSignaled );
		}

		if ( !m_declaration )
		{
			m_declaration = ashes::makeLayout< OverlayCategory::Vertex >( 0u );
			m_declaration->createAttribute( 0u
				, ashes::Format::eR32G32_SFLOAT
				, offsetof( OverlayCategory::Vertex, coords ) );
			m_declaration->createAttribute( 1u
				, ashes::Format::eR32G32_SFLOAT
				, offsetof( OverlayCategory::Vertex, texture ) );
		}

		if ( !m_textDeclaration )
		{
			m_textDeclaration = ashes::makeLayout< TextOverlay::Vertex >( 0u );
			m_textDeclaration->createAttribute( 0u
				, ashes::Format::eR32G32_SFLOAT
				, offsetof( TextOverlay::Vertex, coords ) );
			m_textDeclaration->createAttribute( 1u
				, ashes::Format::eR32G32_SFLOAT
				, offsetof( TextOverlay::Vertex, texture ) );
			m_textDeclaration->createAttribute( 2u
				, ashes::Format::eR32G32_SFLOAT
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
		, ashes::Semaphore const & toWait )
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

		static ashes::ClearColorValue clear{ 0.0f, 0.0f, 0.0f, 0.0f };
		m_toWait = &toWait;
		m_commandBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
		timer.beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { clear }
			, ashes::SubpassContents::eInline );
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
			, ashes::PipelineStageFlag::eColourAttachmentOutput
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

	ashes::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureChannels textureFlags
		, Pass const & pass
		, ashes::UniformBuffer< Configuration > const & ubo
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

	ashes::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureChannels textureFlags
		, Pass const & pass
		, ashes::UniformBuffer< Configuration > const & ubo
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

		ashes::RenderPassCreateInfo renderPass;
		renderPass.flags = 0u;

		renderPass.attachments.resize( 1u );
		renderPass.attachments[0].format = m_target.getFormat();
		renderPass.attachments[0].loadOp = ashes::AttachmentLoadOp::eClear;
		renderPass.attachments[0].storeOp = ashes::AttachmentStoreOp::eStore;
		renderPass.attachments[0].stencilLoadOp = ashes::AttachmentLoadOp::eDontCare;
		renderPass.attachments[0].stencilStoreOp = ashes::AttachmentStoreOp::eDontCare;
		renderPass.attachments[0].samples = ashes::SampleCountFlag::e1;
		renderPass.attachments[0].initialLayout = ashes::ImageLayout::eUndefined;
		renderPass.attachments[0].finalLayout = ashes::ImageLayout::eShaderReadOnlyOptimal;

		renderPass.subpasses.resize( 1u );
		renderPass.subpasses[0].flags = 0u;
		renderPass.subpasses[0].pipelineBindPoint = ashes::PipelineBindPoint::eGraphics;
		renderPass.subpasses[0].colorAttachments.push_back( { 0u, ashes::ImageLayout::eColourAttachmentOptimal } );

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

		ashes::FrameBufferAttachmentArray attaches;
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

			ashes::ColourBlendState blState{};
			blState.attachs.push_back( ashes::ColourBlendStateAttachment{
				true,
				ashes::BlendFactor::eSrcAlpha,
				ashes::BlendFactor::eInvSrcAlpha,
				ashes::BlendOp::eAdd,
				ashes::BlendFactor::eSrcAlpha,
				ashes::BlendFactor::eInvSrcAlpha,
				ashes::BlendOp::eAdd
			} );
			ashes::DescriptorSetLayoutBindingArray bindings;

			// Pass buffer
			bindings.emplace_back( getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createLayoutBinding() );
			// Matrix UBO
			bindings.emplace_back( MatrixUboBinding
				, ashes::DescriptorType::eUniformBuffer
				, ashes::ShaderStageFlag::eVertex );
			// Matrix UBO
			bindings.emplace_back( OverlayUboBinding
				, ashes::DescriptorType::eUniformBuffer
				, ashes::ShaderStageFlag::eVertex | ashes::ShaderStageFlag::eFragment );
			auto vertexLayout = m_declaration.get();

			if ( checkFlag( textureFlags, TextureChannel::eText ) )
			{
				vertexLayout = m_textDeclaration.get();
				bindings.emplace_back( TextMapBinding
					, ashes::DescriptorType::eCombinedImageSampler
					, ashes::ShaderStageFlag::eFragment );
			}

			if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
			{
				bindings.emplace_back( DiffuseMapBinding
					, ashes::DescriptorType::eCombinedImageSampler
					, ashes::ShaderStageFlag::eFragment );
			}

			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				bindings.emplace_back( OpacityMapBinding
					, ashes::DescriptorType::eCombinedImageSampler
					, ashes::ShaderStageFlag::eFragment );
			}

			auto descriptorLayout = device.createDescriptorSetLayout( std::move( bindings ) );
			auto descriptorPool = descriptorLayout->createPool( 1000u );
			auto pipelineLayout = device.createPipelineLayout( *descriptorLayout );
			auto pipeline = pipelineLayout->createPipeline( {
				program,
				*m_renderPass,
				ashes::VertexInputState::create( *vertexLayout ),
				{ ashes::PrimitiveTopology::eTriangleList },
				ashes::RasterisationState{ 0u, false, false, ashes::PolygonMode::eFill, ashes::CullModeFlag::eNone },
				ashes::MultisampleState{},
				blState,
				{ ashes::DynamicState::eViewport, ashes::DynamicState::eScissor },
				ashes::DepthStencilState{ 0u, false, false },
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

	ashes::ShaderStageStateArray OverlayRenderer::doCreateOverlayProgram( TextureChannels const & textureFlags )
	{
		using namespace sdw;
		using namespace shader;

		// Vertex shader
		ShaderModule vtx{ ashes::ShaderStageFlag::eVertex, "Overlay" };
		{
			VertexWriter writer;

			UBO_MATRIX( writer, MatrixUboBinding, 0u );
			UBO_OVERLAY( writer, OverlayUboBinding, 0u );

			// Shader inputs
			uint32_t index = 0u;
			auto position = writer.declInput< Vec2 >( cuT( "position" ), 0u );
			auto uv = writer.declInput< Vec2 >( cuT( "uv" ), 1u, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto text = writer.declInput< Vec2 >( cuT( "text" ), 2u, checkFlag( textureFlags, TextureChannel::eText ) );

			// Shader outputs
			auto vtx_text = writer.declOutput< Vec2 >( cuT( "vtx_text" ), 0u, checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declOutput< Vec2 >( cuT( "vtx_texture" ), 1u, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto out = writer.getOut();

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					if ( checkFlag( textureFlags, TextureChannel::eText ) )
					{
						vtx_text = text;
					}

					if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
					{
						vtx_texture = uv;
					}

					auto size = writer.declLocale( cuT( "size" )
						, vec2( c3d_positionRatio.z() * writer.cast< Float >( c3d_renderSizeIndex.x() )
							, c3d_positionRatio.w() * writer.cast< Float >( c3d_renderSizeIndex.y() ) ) );
					out.gl_out.gl_Position = c3d_projection * vec4( size * writer.paren( c3d_positionRatio.xy() + position )
						, 0.0_f
						, 1.0_f );
				} );

			vtx.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		// Pixel shader
		ShaderModule pxl{ ashes::ShaderStageFlag::eFragment, "Overlay" };
		{
			FragmentWriter writer;

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

			materials->declare( getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() );
			UBO_OVERLAY( writer, OverlayUboBinding, 0u );

			// Shader inputs
			auto vtx_text = writer.declInput< Vec2 >( cuT( "vtx_text" )
				, 0u
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto vtx_texture = writer.declInput< Vec2 >( cuT( "vtx_texture" )
				, 1u
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapText = writer.declSampledImage< FImg2DR32 >( cuT( "c3d_mapText" )
				, TextMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eText ) );
			auto c3d_mapDiffuse = writer.declSampledImage< FImg2DRgba32 >( cuT( "c3d_mapDiffuse" )
				, DiffuseMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eDiffuse ) );
			auto c3d_mapOpacity = writer.declSampledImage< FImg2DR32 >( cuT( "c3d_mapOpacity" )
				, OpacityMapBinding
				, 0u
				, checkFlag( textureFlags, TextureChannel::eOpacity ) );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( cuT( "pxl_fragColor" ), 0 );

			writer.implementFunction< void >( cuT( "main" ), [&]()
				{
					auto material = materials->getBaseMaterial( c3d_renderSizeIndex.z() );
					auto diffuse = writer.declLocale( cuT( "diffuse" )
						, material->m_diffuse() );
					auto alpha = writer.declLocale( cuT( "alpha" )
						, material->m_opacity );

					if ( checkFlag( textureFlags, TextureChannel::eText ) )
					{
						alpha *= texture( c3d_mapText, vtx_text, 0.0_f );
					}

					if ( checkFlag( textureFlags, TextureChannel::eDiffuse ) )
					{
						diffuse = texture( c3d_mapDiffuse, vtx_texture, 0.0_f ).xyz();
					}

					if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
					{
						alpha *= texture( c3d_mapOpacity, vtx_texture, 0.0_f );
					}

					pxl_fragColor = vec4( diffuse.xyz(), alpha );
				} );

			pxl.shader = std::make_unique< sdw::Shader >( std::move( writer.getShader() ) );
		}

		ashes::ShaderStageStateArray result;
		result.push_back( { getCurrentDevice( *this ).createShaderModule( ashes::ShaderStageFlag::eVertex ) } );
		result.push_back( { getCurrentDevice( *this ).createShaderModule( ashes::ShaderStageFlag::eFragment ) } );
		result[0].module->loadShader( getRenderSystem()->compileShader( vtx ) );
		result[1].module->loadShader( getRenderSystem()->compileShader( pxl ) );
		return result;
	}

	//*********************************************************************************************
}
