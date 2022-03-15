#include "Castor3D/Overlay/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/ObjectBufferPool.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"
#include "Castor3D/Shader/ShaderBuffers/TextureConfigurationBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, OverlayRenderer )

#pragma GCC diagnostic ignored "-Wclass-memaccess"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		static uint32_t constexpr MaxPanelsPerBuffer = 100u;

		enum class OverlayBindingId : uint32_t
		{
			eMaterials,
			eTexConfigs,
			eTexAnims,
			eMatrix,
			eOverlay,
			eTextMap,
			eMaps,
		};

		template< typename VertexT, uint32_t CountT >
		uint32_t doFillBuffers( typename std::vector< VertexT >::const_iterator begin
			, uint32_t count
			, OverlayRenderer::VertexBufferIndexT< VertexT, CountT > & bufferIndex )
		{
			auto & bufferOffset = bufferIndex.geometryBuffers.textured.bufferOffset;
			std::copy( begin
				, std::next( begin, count )
				, bufferOffset.getVertexData< VertexT >().begin() );
			bufferOffset.vtxBuffer->markDirty( bufferOffset.vtxChunk.offset
				, bufferOffset.vtxChunk.size
				, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			return count;
		}

		template< typename T >
		void doUpdateUbo( UniformBufferOffsetT< T > & overlayUbo
			, OverlayCategory const & overlay
			, Pass const & pass
			, Size const & size )
		{
			auto & data = overlayUbo.getData();
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

		enum class OverlayTexture : uint32_t
		{
			eNone = 0x00,
			eText = 0x01,
			eColour = 0x02,
			eOpacity = 0x04,
		};

		uint32_t makeKey( FilteredTextureFlags const & textures
			, bool text )
		{
			auto tex{ uint32_t( text ? OverlayTexture::eText : OverlayTexture::eNone ) };

			if ( checkFlags( textures, TextureFlag::eColour ) != textures.end() )
			{
				tex |= uint32_t( OverlayTexture::eColour );
			}

			if ( checkFlags( textures, TextureFlag::eOpacity ) != textures.end() )
			{
				tex |= uint32_t( OverlayTexture::eOpacity );
			}

			uint32_t result{ tex << 8 };
			result |= uint32_t( textures.size() );
			return result;
		}
	}

	//*********************************************************************************************

	OverlayRenderer::Preparer::Preparer( OverlayRenderer & renderer
		, RenderDevice const & device )
		: m_renderer{ renderer }
		, m_device{ device }
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
					doPrepareOverlay< PanelVertexBufferPool::Quad >( m_device
						, overlay
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
					doPrepareOverlay< PanelVertexBufferPool::Quad >( m_device
						, overlay
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
					doPrepareOverlay< BorderPanelVertexBufferPool::Quad >( m_device
						, overlay
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
					doPrepareOverlay< TextVertexBufferPool::Quad >( m_device
						, overlay
						, *pass
						, m_renderer.m_textOverlays
						, m_renderer.m_textVertexBuffers
						, overlay.getTextVertex()
						, overlay.getFontTexture() );
				}
			}
		}
	}

	//*********************************************************************************************

	namespace
	{
		template< typename VertexT, uint32_t CountT >
		OverlayRenderer::VertexBufferIndexT< VertexT, CountT > & getVertexBuffer( std::vector< std::unique_ptr< OverlayRenderer::VertexBufferPoolT< VertexT, CountT > > > & pools
			, std::map< size_t, OverlayRenderer::VertexBufferIndexT< VertexT, CountT > > & overlays
			, Overlay const & overlay
			, Pass const & pass
			, OverlayRenderer::OverlayRenderNode & node
			, Engine & engine
			, RenderDevice const & device
			, ashes::PipelineVertexInputStateCreateInfo const & noTexLayout
			, ashes::PipelineVertexInputStateCreateInfo const & texLayout
			, uint32_t maxCount )
		{
			auto hash = std::hash< Overlay const * >{}( &overlay );
			hash = castor::hashCombinePtr( hash, pass );
			auto it = overlays.find( hash );

			if ( it == overlays.end() )
			{
				for ( auto & pool : pools )
				{
					if ( it == overlays.end() )
					{
						auto result = pool->allocate( node );

						if ( bool( result ) )
						{
							it = overlays.emplace( hash, std::move( result ) ).first;
						}
					}
				}

				if ( it == overlays.end() )
				{
					pools.emplace_back( std::make_unique< OverlayRenderer::VertexBufferPoolT< VertexT, CountT > >( engine
						, getName( overlay.getType() )
						, *device.uboPools
						, device
						, noTexLayout
						, texLayout
						, maxCount ) );
					auto result = pools.back()->allocate( node );
					it = overlays.emplace( hash, std::move( result ) ).first;
				}
			}

			return it->second;
		}
	}

	//*********************************************************************************************

	template< typename QuadT, typename OverlayT, typename VertexT, uint32_t CountT >
	void OverlayRenderer::Preparer::doPrepareOverlay( RenderDevice const & device
		, OverlayT const & overlay
		, Pass const & pass
		, std::map< size_t, OverlayRenderer::VertexBufferIndexT< VertexT, CountT > > & overlays
		, std::vector< OverlayRenderer::VertexBufferPoolPtrT< VertexT, CountT > > & vertexBuffers
		, std::vector < VertexT > const & vertices
		, FontTextureSPtr fontTexture )
	{
		if ( !vertices.empty() )
		{
			auto & bufferIndex = getVertexBuffer< VertexT, CountT >( vertexBuffers
				, overlays
				, overlay.getOverlay()
				, pass
				, ( fontTexture
					? m_renderer.doGetTextNode( device, pass, *fontTexture->getTexture(), *fontTexture->getSampler().lock() )
					: m_renderer.doGetPanelNode( device, pass ) )
				, *pass.getOwner()->getEngine()
				, device
				, ( fontTexture ? m_renderer.m_noTexTextDeclaration : m_renderer.m_noTexDeclaration )
				, ( fontTexture ? m_renderer.m_texTextDeclaration : m_renderer.m_texDeclaration )
				, MaxPanelsPerBuffer );
			doUpdateUbo( bufferIndex.overlayUbo
				, overlay
				, pass
				, m_renderer.m_size );
			doFillBuffers( vertices.begin()
				, uint32_t( vertices.size() )
				, bufferIndex );

			if ( !bufferIndex.descriptorSet )
			{
				if ( fontTexture )
				{
					bufferIndex.descriptorSet = m_renderer.doCreateDescriptorSet( bufferIndex.node.pipeline
						, pass.getTextures()
						, pass
						, bufferIndex.overlayUbo
						, bufferIndex.index
						, *fontTexture->getTexture()
						, *fontTexture->getSampler().lock() );
					bufferIndex.connection = fontTexture->onChanged.connect( [&bufferIndex]( FontTexture const & )
						{
							bufferIndex.descriptorSet.reset();
						} );
				}
				else
				{
					bufferIndex.descriptorSet = m_renderer.doCreateDescriptorSet( bufferIndex.node.pipeline
						, pass.getTextures()
						, pass
						, bufferIndex.overlayUbo
						, bufferIndex.index );
				}
			}

			auto borderSize = BorderSizeGetter< OverlayT, QuadT >{}( overlay, m_renderer.m_size );
			auto borderOffset = castor::Size{ uint32_t( borderSize.left() ), uint32_t( borderSize.top() ) };
			auto borderExtent = borderOffset + castor::Size{ uint32_t( borderSize.right() ), uint32_t( borderSize.bottom() ) };
			auto position = overlay.getAbsolutePosition( m_renderer.m_size ) - borderOffset;
			position->x = std::max( 0, position->x );
			position->y = std::max( 0, position->y );
			auto size = overlay.getAbsoluteSize( m_renderer.m_size ) + borderExtent;
			size->x = std::max( 1u, size->x );
			size->y = std::max( 1u, size->y );
			auto & commandBuffer = *m_renderer.m_commands.commandBuffer;
			commandBuffer.bindPipeline( *bufferIndex.node.pipeline.pipeline );
			commandBuffer.setViewport( makeViewport( m_renderer.m_size ) );
			commandBuffer.setScissor( makeScissor( position, size ) );
			commandBuffer.bindDescriptorSet( *bufferIndex.descriptorSet
				, *bufferIndex.node.pipeline.pipelineLayout );
			commandBuffer.bindVertexBuffer( 0u
				, bufferIndex.geometryBuffers.noTexture.bufferOffset.getVertexBuffer()
				, bufferIndex.geometryBuffers.noTexture.bufferOffset.getVertexOffset() );
			commandBuffer.draw( uint32_t( vertices.size() )
				, 1u
				, 0u
				, 0u );
		}
	}

	//*********************************************************************************************

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::VertexBufferPoolT( Engine & engine
		, std::string const & debugName
		, UniformBufferPools & uboPools
		, RenderDevice const & device
		, ashes::PipelineVertexInputStateCreateInfo const & noTexDecl
		, ashes::PipelineVertexInputStateCreateInfo const & texDecl
		, uint32_t count )
		: engine{ engine }
		, device{ device }
		, uboPools{ uboPools }
		, noTexDeclaration{ noTexDecl }
		, texDeclaration{ texDecl }
		, buffer{ castor::makeUnique< VertexBufferPool >( device, debugName ) }
	{
	}

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferIndexT< VertexT, CountT > OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::allocate( OverlayRenderNode & node )
	{
		auto it = std::find_if( allocated.begin()
			, allocated.end()
			, []( ObjectBufferOffset const & lookup )
			{
				return lookup.vtxBuffer == nullptr;
			} );

		if ( it == allocated.end() )
		{
			allocated.emplace_back( buffer->getBuffer< Quad >( 1u ) );
			it = std::next( allocated.begin(), ptrdiff_t( allocated.size() - 1u ) );
		}

		OverlayRenderer::VertexBufferIndexT< VertexT, CountT > result{ *this
			, node
			, uint32_t( std::distance( allocated.begin(), it ) ) };
		ObjectBufferOffset & offsets = *it;
		result.overlayUbo = uboPools.getBuffer< Configuration >( 0u );

		result.geometryBuffers.noTexture.bufferOffset = offsets;
		result.geometryBuffers.noTexture.layouts.emplace_back( noTexDeclaration );

		result.geometryBuffers.textured.bufferOffset = offsets;
		result.geometryBuffers.textured.layouts.emplace_back( texDeclaration );

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::deallocate( OverlayRenderer::VertexBufferIndexT< VertexT, CountT > const & index )
	{
		CU_Require( &index.pool == this );
		auto it = std::find_if( allocated.begin()
			, allocated.end()
			, [&index]( auto const & lookup )
			{
				return lookup.first == index.index;
			} );
		CU_Require( it != allocated.end() );
		buffer->putBuffer( *it );
		*it = {};
		index.geometryBuffers.noTexture.bufferOffset = {};
		index.geometryBuffers.noTexture.layouts.clear();
		index.geometryBuffers.textured.bufferOffset = {};
		index.geometryBuffers.textured.layouts.clear();
		uboPools.putBuffer( index.overlayUbo );
		uboPools.putBuffer( index.texturesUbo );
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPoolT< VertexT, CountT >::upload( ashes::CommandBuffer const & cb )
	{
		buffer->upload( cb );
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderDevice const & device
		, Texture const & target
		, VkCommandBufferLevel level )
		: OwnedBy< RenderSystem >( device.renderSystem )
		, m_uboPools{ *device.uboPools }
		, m_target{ target }
		, m_commands{ device, *device.graphicsData(), "OverlayRenderer", level }
		, m_fence{ device->createFence( "OverlayRenderer", VK_FENCE_CREATE_SIGNALED_BIT ) }
		, m_noTexDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( OverlayCategory::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, coords ) } } }
		, m_texDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( OverlayCategory::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( OverlayCategory::Vertex, texture ) } } }
		, m_noTexTextDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
		, m_texTextDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, texture ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
		, m_size{ makeSize( m_target.getExtent() ) }
		, m_matrixUbo{ device }
	{
		doCreateRenderPass( device );

		// Create one panel overlays buffer pool
		m_panelVertexBuffers.emplace_back( std::make_unique< PanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "PanelOverlays"
			, m_uboPools
			, device
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxPanelsPerBuffer ) );

		// Create one border overlays buffer pool
		m_borderVertexBuffers.emplace_back( std::make_unique< BorderPanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, "BorderPanelOverlays"
			, m_uboPools
			, device
			, m_noTexDeclaration
			, m_texDeclaration
			, MaxPanelsPerBuffer ) );

		// create one text overlays buffer
		m_textVertexBuffers.emplace_back( std::make_unique< TextVertexBufferPool >( *getRenderSystem()->getEngine()
			, "TextOverlays"
			, m_uboPools
			, device
			, m_noTexTextDeclaration
			, m_texTextDeclaration
			, MaxPanelsPerBuffer ) );

		m_matrixUbo.cpuUpdate( getRenderSystem()->getOrtho( 0.0f
			, float( m_size.getWidth() )
			, 0.0f
			, float( m_size.getHeight() )
			, -1.0f
			, 1.0f ) );
	}

	OverlayRenderer::~OverlayRenderer()
	{
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
		m_commands = {};
		m_frameBuffer.reset();
		m_renderPass.reset();
	}

	void OverlayRenderer::update( GpuUpdater & updater )
	{
		auto size = updater.camera->getSize();

		if ( m_size != size )
		{
			m_sizeChanged = true;
			m_size = size;
			m_matrixUbo.cpuUpdate( getRenderSystem()->getOrtho( 0.0f
				, float( m_size.getWidth() )
				, 0.0f
				, float( m_size.getHeight() )
				, -1.0f
				, 1.0f ) );
		}
	}

	void OverlayRenderer::beginPrepare( FramePassTimer const & timer )
	{
		m_fence->wait( ashes::MaxTimeout );
		m_fence->reset();
		auto & cb = m_commands.commandBuffer;
		cb->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		cb->beginDebugBlock(
			{
				"Overlays",
				makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ),
			} );
		timer.beginPass( *cb );
		cb->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
	}

	void OverlayRenderer::endPrepare( FramePassTimer const & timer )
	{
		auto & cb = m_commands.commandBuffer;
		cb->endRenderPass();
		timer.endPass( *cb );
		cb->endDebugBlock();
		endPrepare();
	}

	void OverlayRenderer::beginPrepare( VkRenderPass renderPass
		, VkFramebuffer framebuffer )
	{
		m_commands.commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
			, makeVkStruct< VkCommandBufferInheritanceInfo >( renderPass
				, 0u
				, framebuffer
				, VK_FALSE
				, 0u
				, 0u ) );
	}

	void OverlayRenderer::endPrepare()
	{
		m_commands.commandBuffer->end();
		m_sizeChanged = false;
	}

	void OverlayRenderer::upload( ashes::CommandBuffer const & cb )
	{
		for ( auto & pool : m_panelVertexBuffers )
		{
			pool->upload( cb );
		}

		for ( auto & pool : m_borderVertexBuffers )
		{
			pool->upload( cb );
		}

		for ( auto & pool : m_textVertexBuffers )
		{
			pool->upload( cb );
		}
	}

	crg::SemaphoreWaitArray OverlayRenderer::render( FramePassTimer & timer
		, ashes::Queue const & queue
		, crg::SemaphoreWaitArray const & toWait )
	{
		auto timerBlock( timer.start() );
		timerBlock->notifyPassRender();
		std::vector< VkSemaphore > semaphores;
		std::vector< VkPipelineStageFlags > dstStageMasks;
		crg::convert( toWait, semaphores, dstStageMasks );
		queue.submit( *m_commands.commandBuffer
			, semaphores
			, dstStageMasks
			, *m_commands.semaphore
			, *m_fence );
		return { 1u
			, { *m_commands.semaphore
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT } };
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetPanelNode( RenderDevice const & device
		, Pass const & pass )
	{
		auto it = m_mapPanelNodes.find( &pass );

		if ( it == m_mapPanelNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, pass, m_panelPipelines, false );
			it = m_mapPanelNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	OverlayRenderer::OverlayRenderNode & OverlayRenderer::doGetTextNode( RenderDevice const & device
		, Pass const & pass
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto it = m_mapTextNodes.find( &pass );

		if ( it == m_mapTextNodes.end() )
		{
			auto & pipeline = doGetPipeline( device, pass, m_textPipelines, true );
			it = m_mapTextNodes.insert( { &pass, OverlayRenderNode{ pipeline, pass } } ).first;
		}

		return it->second;
	}

	ashes::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureFlags textures
		, Pass const & pass
		, UniformBufferOffsetT< Configuration > const & overlayUbo
		, uint32_t index
		, bool update )
	{
		remFlag( textures, TextureFlag::eAllButColourAndOpacity );
		auto result = pipeline.descriptorPool->createDescriptorSet( "OverlayRenderer" + string::toString( index ) );

		// Pass buffer
		getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		// Textures buffers
		getRenderSystem()->getEngine()->getMaterialCache().getTexConfigBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
		getRenderSystem()->getEngine()->getMaterialCache().getTexAnimBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
		// Matrix UBO
		m_matrixUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eMatrix ) ) );
		// Overlay UBO
		overlayUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eOverlay ) ) );
		uint32_t texIndex = 0u;

		for ( auto & unit : pass.getTextureUnits( textures ) )
		{
			auto config = unit->getConfiguration();

			if ( unit->isInitialised()
				&& ( config.colourMask[0]
				|| config.opacityMask[0] ) )
			{
				result->createBinding( pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eMaps ) )
					, unit->getTexture()->getDefaultView().getSampledView()
					, unit->getSampler().lock()->getSampler()
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					, texIndex++ );
			}
		}

		if ( update )
		{
			result->update();
		}

		return result;
	}

	ashes::DescriptorSetPtr OverlayRenderer::doCreateDescriptorSet( OverlayRenderer::Pipeline & pipeline
		, TextureFlags textures
		, Pass const & pass
		, UniformBufferOffsetT< Configuration > const & overlayUbo
		, uint32_t index
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto result = doCreateDescriptorSet( pipeline
			, std::move( textures )
			, pass
			, overlayUbo
			, index
			, false );
		result->createBinding( pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eTextMap ) )
			, texture.getDefaultView().getSampledView()
			, sampler.getSampler() );
		result->update();
		return result;
	}

	void OverlayRenderer::doCreateRenderPass( RenderDevice const & device )
	{
		ashes::VkAttachmentDescriptionArray attaches{ { 0u
			, m_target.getFormat()
			, VK_SAMPLE_COUNT_1_BIT
			, VK_ATTACHMENT_LOAD_OP_CLEAR
			, VK_ATTACHMENT_STORE_OP_STORE
			, VK_ATTACHMENT_LOAD_OP_DONT_CARE
			, VK_ATTACHMENT_STORE_OP_DONT_CARE
			, VK_IMAGE_LAYOUT_UNDEFINED
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } };
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription{ 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, { { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }
			, {}
			, ashes::nullopt
			, {} } );
		ashes::VkSubpassDependencyArray dependencies{ { VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo createInfo{ 0u
			, std::move( attaches )
			, std::move( subpasses )
			, std::move( dependencies ) };
		m_renderPass = device->createRenderPass( "OverlayRenderer"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray fbAttaches;
		auto image = std::make_unique< ashes::Image >( *device
			, m_target.image
			, ashes::ImageCreateInfo{ m_target.imageId.data->info } );
		auto view = ashes::ImageView{ ashes::ImageViewCreateInfo{ m_target.image, m_target.targetViewId.data->info }
			, m_target.wholeView
			, image.get() };
		fbAttaches.emplace_back( view );
		m_frameBuffer = m_renderPass->createFrameBuffer( "OverlayRenderer"
			, makeExtent2D( m_target.getExtent() )
			, std::move( fbAttaches ) );
	}

	OverlayRenderer::Pipeline OverlayRenderer::doCreatePipeline( RenderDevice const & device
		, Pass const & pass
		, ashes::PipelineShaderStageCreateInfoArray program
		, FilteredTextureFlags const & texturesFlags
		, bool text )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments{ { VK_TRUE
			, VK_BLEND_FACTOR_SRC_ALPHA
			, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			, VK_BLEND_OP_ADD
			, VK_BLEND_FACTOR_SRC_ALPHA
			, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA
			, VK_BLEND_OP_ADD
			, defaultColorWriteMask } };
		ashes::PipelineColorBlendStateCreateInfo blState{ 0u
			, VK_FALSE
			, VK_LOGIC_OP_COPY
			, std::move( attachments ) };
		auto & materials = getRenderSystem()->getEngine()->getMaterialCache();
		ashes::VkDescriptorSetLayoutBindingArray bindings;

		bindings.emplace_back( materials.getPassBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		bindings.emplace_back( materials.getTexConfigBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexConfigs ) ) );
		bindings.emplace_back( materials.getTexAnimBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexAnims ) ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlay )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto vertexLayout = ( texturesFlags.empty()
			? &m_noTexDeclaration
			: &m_texDeclaration );

		if ( text )
		{
			vertexLayout = ( texturesFlags.empty()
				? &m_noTexTextDeclaration
				: &m_texTextDeclaration );
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eTextMap )
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( !texturesFlags.empty() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eMaps )
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( texturesFlags.size() ) ) );
		}

		auto descriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		auto descriptorPool = descriptorLayout->createPool( 1000u );
		auto pipelineLayout = device->createPipelineLayout( *descriptorLayout );
		auto pipeline = device->createPipeline( { 0u
			, std::move( program )
			, *vertexLayout
			, ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST }
			, ashes::nullopt
			, ashes::PipelineViewportStateCreateInfo{}
			, ashes::PipelineRasterizationStateCreateInfo{}
			, ashes::PipelineMultisampleStateCreateInfo{}
			, ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE }
			, std::move( blState )
			, ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } }
			, *pipelineLayout
			, *m_renderPass } );
		return Pipeline{ std::move( descriptorLayout )
			, std::move( descriptorPool )
			, std::move( pipelineLayout )
			, std::move( pipeline ) };
	}

	OverlayRenderer::Pipeline & OverlayRenderer::doGetPipeline( RenderDevice const & device
		, Pass const & pass
		, std::map< uint32_t, Pipeline > & pipelines
		, bool text )
	{
		// Remove unwanted flags
		auto textures = filterTexturesFlags( pass.getTexturesMask(), TextureFlag::eColour | TextureFlag::eOpacity );
		auto key = makeKey( textures, text );
		auto it = pipelines.find( key );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			it = pipelines.emplace( key
				, doCreatePipeline( device
					, pass
					, doCreateOverlayProgram( device, textures, text )
					, textures
					, text ) ).first;
		}

		return it->second;
	}

	template< ast::var::Flag FlagT >
	struct OverlaySurfaceT
		: public sdw::StructInstance
	{
		OverlaySurfaceT( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstance{ writer, std::move( expr ), enabled }
			, position{ this->getMember< sdw::Vec2 >( "position", true ) }
			, texture{ this->getMember< sdw::Vec2 >( "texcoord", true ) }
			, text{ this->getMember< sdw::Vec2 >( "text", true ) }
		{
		}

		SDW_DeclStructInstance( , OverlaySurfaceT );

		static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
			, bool hasPosition
			, bool isTextOverlay
			, bool hasTextures )
		{
			auto result = cache.getIOStruct( ast::type::MemoryLayout::eC
				, "C3D_" + ( FlagT == sdw::var::Flag::eShaderOutput
					? std::string{ "Out" }
					: std::string{ "In" } ) + "OverlaySurface"
				, FlagT );

			if ( result->empty() )
			{
				result->declMember( "position", ast::type::Kind::eVec2F, ast::type::NotArray, 0u, hasPosition );
				result->declMember( "texcoord", ast::type::Kind::eVec2F, ast::type::NotArray, 1u, hasTextures );
				result->declMember( "text", ast::type::Kind::eVec2F, ast::type::NotArray, 2u, isTextOverlay );
			}

			return result;
		}

		sdw::Vec2 position;
		sdw::Vec2 texture;
		sdw::Vec2 text;
	};

	ashes::PipelineShaderStageCreateInfoArray OverlayRenderer::doCreateOverlayProgram( RenderDevice const & device
		, FilteredTextureFlags const & texturesFlags
		, bool textOverlay )
	{
		using namespace sdw;
		using namespace shader;
		auto texturesCount = uint32_t( texturesFlags.size() );
		bool hasTexture = !texturesFlags.empty();

		// Vertex shader
		ShaderModule vtx{ VK_SHADER_STAGE_VERTEX_BIT, "Overlay" };
		{
			VertexWriter writer;

			C3D_Matrix( writer
				, OverlayBindingId::eMatrix
				, 0u );
			C3D_Overlay( writer
				, OverlayBindingId::eOverlay
				, 0u );

			writer.implementMainT< OverlaySurfaceT, OverlaySurfaceT >( VertexInT< OverlaySurfaceT >{ writer, true, textOverlay, hasTexture }
				, VertexOutT< OverlaySurfaceT >{ writer, false, textOverlay, hasTexture }
				, [&]( VertexInT< OverlaySurfaceT > in
					, VertexOutT< OverlaySurfaceT > out )
				{
					out.texture = in.texture;
					out.text = in.text;
					auto size = writer.declLocale( "size"
						, c3d_overlayData.getOverlaySize() );
					out.vtx.position = c3d_matrixData.viewToProj( vec4( size * c3d_overlayData.modelToView( in.position )
							, 0.0_f
							, 1.0_f ) );
				} );

			vtx.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		// Pixel shader
		ShaderModule pxl{ VK_SHADER_STAGE_FRAGMENT_BIT, "Overlay" };
		{
			auto & renderSystem = *getRenderSystem();
			FragmentWriter writer;

			auto materials = std::make_unique< shader::Materials >( writer
				, uint32_t( OverlayBindingId::eMaterials )
				, 0u );
			shader::TextureConfigurations textureConfigs{ writer
				, uint32_t( OverlayBindingId::eTexConfigs )
				, 0u
				, hasTexture };
			shader::TextureAnimations textureAnims{ writer
				, uint32_t( OverlayBindingId::eTexAnims )
				, 0u
				, hasTexture };

			C3D_Overlay( writer
				, OverlayBindingId::eOverlay
				, 0u );

			auto c3d_mapText = writer.declCombinedImg< FImg2DR32 >( "c3d_mapText"
				, uint32_t( OverlayBindingId::eTextMap )
				, 0u
				, textOverlay );
			auto c3d_maps( writer.declCombinedImgArray< FImg2DRgba32 >( "c3d_maps"
				, uint32_t( OverlayBindingId::eMaps )
				, 0u
				, std::max( 1u, texturesCount )
				, hasTexture ) );

			shader::Utils utils{ writer, *renderSystem.getEngine() };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementMainT< OverlaySurfaceT, VoidT >( FragmentInT< OverlaySurfaceT >{ writer, false, textOverlay, hasTexture }
				, FragmentOut{ writer }
				, [&]( FragmentInT< OverlaySurfaceT > in
				, FragmentOut out )
				{
					auto material = materials->getMaterial( c3d_overlayData.getMaterialIndex() );
					auto diffuse = writer.declLocale( "diffuse"
						, material.colour() );
					auto alpha = writer.declLocale( "alpha"
						, material.opacity );

					if ( textOverlay )
					{
						alpha *= c3d_mapText.sample( in.text, 0.0_f );
					}

					if ( hasTexture )
					{
						utils.compute2DMapsContributions( texturesFlags
							, textureConfigs
							, textureAnims
							, c3d_maps
							, vec3( in.texture, 0.0 )
							, diffuse
							, alpha );
					}

					pxl_fragColor = vec4( diffuse.xyz(), alpha );
				} );

			pxl.shader = std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		return ashes::PipelineShaderStageCreateInfoArray
		{
			makeShaderState( device, vtx ),
			makeShaderState( device, pxl ),
		};
	}

	//*********************************************************************************************
}
