#include "Castor3D/Overlay/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/TextureConfigurationBuffer/TextureConfigurationBuffer.hpp"

#include <CastorUtils/Graphics/Rectangle.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementCUSmartPtr( castor3d, OverlayRenderer )

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	static int32_t constexpr MaxCharsPerBuffer = 6000;
	static uint32_t constexpr MaxPanelsPerBuffer = 100u;

	namespace
	{
		enum class OverlayBindingId : uint32_t
		{
			eMaterials,
			eTexturesBuffer,
			eMatrix,
			eOverlay,
			eTexturesConfig,
			eTextMap,
			eMaps,
		};

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
		void doUpdateUbo( UniformBufferOffsetT< T > & overlayUbo
			, UniformBufferOffsetT< TexturesUbo::Configuration > & texturesUbo
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
			auto & texturesData = texturesUbo.getData();
			uint32_t texIndex = 0u;

			for ( auto & unit : pass )
			{
				texturesData.indices[texIndex / 4u][texIndex % 4] = unit->getId();
				++texIndex;
			}
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
		CU_ImplementFlags( OverlayTexture )

		uint32_t makeKey( FilteredTextureFlags const & textures
			, bool text )
		{
			OverlayTextures tex{ text ? OverlayTexture::eText : OverlayTexture::eNone };

			if ( checkFlags( textures, TextureFlag::eDiffuse ) != textures.end() )
			{
				tex |= OverlayTexture::eColour;
			}

			if ( checkFlags( textures, TextureFlag::eOpacity ) != textures.end() )
			{
				tex |= OverlayTexture::eOpacity;
			}

			uint32_t result{ tex << 24 };
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
		template< typename VertexBufferIndexT, typename VertexBufferPoolT >
		VertexBufferIndexT & getVertexBuffer( std::vector< std::unique_ptr< VertexBufferPoolT > > & pools
			, std::map< size_t, VertexBufferIndexT > & overlays
			, Overlay const & overlay
			, Pass const & pass
			, OverlayRenderer::OverlayRenderNode & node
			, Engine & engine
			, RenderDevice const & device
			, ashes::PipelineVertexInputStateCreateInfo const & layout
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
					pools.emplace_back( std::make_unique< VertexBufferPoolT >( engine
						, *device.uboPools
						, device
						, layout
						, maxCount ) );
					auto result = pools.back()->allocate( node );
					it = overlays.emplace( hash, std::move( result ) ).first;
				}
			}

			return it->second;
		}
	}

	//*********************************************************************************************

	template< typename QuadT, typename OverlayT, typename BufferIndexT, typename BufferPoolT, typename VertexT >
	void OverlayRenderer::Preparer::doPrepareOverlay( RenderDevice const & device
		, OverlayT const & overlay
		, Pass const & pass
		, std::map< size_t, BufferIndexT > & overlays
		, std::vector< BufferPoolT > & vertexBuffers
		, std::vector < VertexT > const & vertices
		, FontTextureSPtr fontTexture )
	{
		if ( !vertices.empty() )
		{
			auto & bufferIndex = getVertexBuffer< BufferIndexT >( vertexBuffers
				, overlays
				, overlay.getOverlay()
				, pass
				, ( fontTexture
					? m_renderer.doGetTextNode( device, pass, *fontTexture->getTexture(), *fontTexture->getSampler() )
					: m_renderer.doGetPanelNode( device, pass ) )
				, *pass.getOwner()->getEngine()
				, device
				, ( fontTexture
					? m_renderer.m_textDeclaration
					: ( filterTexturesFlags( pass.getTexturesMask(), TextureFlag::eAlbedo | TextureFlag::eOpacity ).empty()
						? m_renderer.m_noTexDeclaration
						: m_renderer.m_texDeclaration ) )
				, MaxPanelsPerBuffer );
			doUpdateUbo( bufferIndex.overlayUbo
				, bufferIndex.texturesUbo
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
						, pass.getTextures()
						, pass
						, bufferIndex.overlayUbo
						, bufferIndex.texturesUbo
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
						, pass.getTextures()
						, pass
						, bufferIndex.overlayUbo
						, bufferIndex.texturesUbo
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
			commandBuffer.setViewport( makeViewport( m_renderer.m_size ) );
			commandBuffer.setScissor( makeScissor( position, size ) );
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
	OverlayRenderer::VertexBufferPool< VertexT, CountT >::VertexBufferPool( Engine & engine
		, UniformBufferPools & uboPools
		, RenderDevice const & device
		, ashes::PipelineVertexInputStateCreateInfo const & decl
		, uint32_t count )
		: engine{ engine }
		, uboPools{ uboPools }
		, maxCount{ count }
		, data{ count, Quad{} }
		, declaration{ decl }
		, buffer{ makeVertexBuffer< Quad >( device
			, count
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "OverlayRenderer" ) }
	{
		for ( auto i = 0u; i < count; ++i )
		{
			free.insert( i );
		}
	}

	template< typename VertexT, uint32_t CountT >
	OverlayRenderer::VertexBufferIndex< VertexT, CountT > OverlayRenderer::VertexBufferPool< VertexT, CountT >::allocate( OverlayRenderNode & node )
	{
		OverlayRenderer::VertexBufferIndex< VertexT, CountT > result{ *this
			, node
			, InvalidIndex };

		if ( !free.empty() )
		{
			result.overlayUbo = uboPools.getBuffer< Configuration >( 0u );
			result.texturesUbo = uboPools.getBuffer< TexturesUbo::Configuration >( 0u );
			result.index = *free.begin();
			result.geometryBuffers.noTexture.vbo.emplace_back( buffer->getBuffer() );
			result.geometryBuffers.noTexture.layouts.emplace_back( declaration );
			result.geometryBuffers.noTexture.vboOffsets.emplace_back( result.index );
			result.geometryBuffers.textured.vbo.emplace_back( buffer->getBuffer() );
			result.geometryBuffers.textured.layouts.emplace_back( declaration );
			result.geometryBuffers.textured.vboOffsets.emplace_back( result.index );
			free.erase( free.begin() );
		}

		return result;
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPool< VertexT, CountT >::deallocate( OverlayRenderer::VertexBufferIndex< VertexT, CountT > const & index )
	{
		CU_Require( &index.pool == this );
		index.geometryBuffers.noTexture.vbo.clear();
		index.geometryBuffers.noTexture.layouts.clear();
		index.geometryBuffers.noTexture.vboOffsets.clear();
		index.geometryBuffers.textured.vbo.clear();
		index.geometryBuffers.textured.layouts.clear();
		index.geometryBuffers.textured.vboOffsets.clear();
		free.insert( index.index );
		uboPools.putBuffer( index.overlayUbo );
		uboPools.putBuffer( index.texturesUbo );
	}

	template< typename VertexT, uint32_t CountT >
	void OverlayRenderer::VertexBufferPool< VertexT, CountT >::upload()
	{
		if ( auto bufferData = buffer->lock( 0u
			, buffer->getCount()
			, 0u ) )
		{
			std::memcpy( bufferData, data.data(), buffer->getSize() );
			buffer->flush( 0u, buffer->getCount() );
			buffer->unlock();
		}
	}

	//*********************************************************************************************

	OverlayRenderer::OverlayRenderer( RenderSystem & renderSystem
		, UniformBufferPools & uboPools
		, ashes::ImageView const & target )
		: OwnedBy< RenderSystem >( renderSystem )
		, m_uboPools{ uboPools }
		, m_target{ target }
		, m_matrixUbo{ *renderSystem.getEngine() }
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
		, m_textDeclaration{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( TextOverlay::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, coords ) }
				, { 1u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, texture ) }
				, { 2u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( TextOverlay::Vertex, text ) } } }
	{
	}

	OverlayRenderer::~OverlayRenderer()
	{
	}

	void OverlayRenderer::initialise( RenderDevice const & device )
	{
		if ( !m_renderPass )
		{
			doCreateRenderPass( device );
		}

		if ( !m_commandBuffer )
		{
			m_commandBuffer = device.graphicsCommandPool->createCommandBuffer( "OverlayRenderer" );
			m_finished = device->createSemaphore( "OverlayRenderer" );
		}


		// Create one panel overlays buffer pool
		m_panelVertexBuffers.emplace_back( std::make_unique< PanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, m_uboPools
			, device
			, m_texDeclaration
			, MaxPanelsPerBuffer ) );

		// Create one border overlays buffer pool
		m_borderVertexBuffers.emplace_back( std::make_unique< BorderPanelVertexBufferPool >( *getRenderSystem()->getEngine()
			, m_uboPools
			, device
			, m_texDeclaration
			, MaxPanelsPerBuffer ) );

		// create one text overlays buffer
		m_textVertexBuffers.emplace_back( std::make_unique< TextVertexBufferPool >( *getRenderSystem()->getEngine()
			, m_uboPools
			, device
			, m_textDeclaration
			, MaxPanelsPerBuffer ) );
	}

	void OverlayRenderer::cleanup( RenderDevice const & device )
	{
		m_matrixUbo.cleanup( device );
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
		m_commandBuffer.reset();
		m_frameBuffer.reset();
		m_renderPass.reset();
		m_toWait = nullptr;
		m_finished.reset();
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

	void OverlayRenderer::beginPrepare( RenderPassTimer const & timer
		, ashes::Semaphore const & toWait )
	{
		m_toWait = &toWait;
		m_commandBuffer->begin( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
		m_commandBuffer->beginDebugBlock(
			{
				"Overlays",
				makeFloatArray( getRenderSystem()->getEngine()->getNextRainbowColour() ),
			} );
		timer.beginPass( *m_commandBuffer );
		m_commandBuffer->beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
	}

	void OverlayRenderer::endPrepare( RenderPassTimer const & timer )
	{
		m_commandBuffer->endRenderPass();
		timer.endPass( *m_commandBuffer );
		m_commandBuffer->endDebugBlock();
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

	void OverlayRenderer::render( RenderDevice const & device
		, RenderPassTimer & timer )
	{
		auto & queue = *device.graphicsQueue;
		RenderPassTimerBlock timerBlock{ timer.start() };
		timerBlock->notifyPassRender();
		queue.submit( *m_commandBuffer
			, *m_toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, nullptr );
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
		, UniformBufferOffsetT< TexturesUbo::Configuration > const & texturesUbo
		, uint32_t index
		, bool update )
	{
		remFlag( textures, TextureFlag::eAllButColourAndOpacity );
		auto result = pipeline.descriptorPool->createDescriptorSet( "OverlayRenderer" + string::toString( index ) );

		// Pass buffer
		getRenderSystem()->getEngine()->getMaterialCache().getPassBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		// Textures buffer
		getRenderSystem()->getEngine()->getMaterialCache().getTextureBuffer().createBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eTexturesBuffer ) ) );
		// Matrix UBO
		m_matrixUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eMatrix ) ) );
		// Overlay UBO
		overlayUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eOverlay ) ) );
		// Textures UBO
		texturesUbo.createSizedBinding( *result
			, pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eTexturesConfig ) ) );
		uint32_t texIndex = 0u;

		for ( auto & unit : pass.getTextureUnits( textures ) )
		{
			auto config = unit->getConfiguration();

			if ( config.colourMask[0]
				|| config.opacityMask[0] )
			{
				result->createBinding( pipeline.descriptorLayout->getBinding( uint32_t( OverlayBindingId::eMaps ) )
					, unit->getTexture()->getDefaultView().getSampledView()
					, unit->getSampler()->getSampler()
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
		, UniformBufferOffsetT< TexturesUbo::Configuration > const & texturesUbo
		, uint32_t index
		, TextureLayout const & texture
		, Sampler const & sampler )
	{
		auto result = doCreateDescriptorSet( pipeline
			, std::move( textures )
			, pass
			, overlayUbo
			, texturesUbo
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
		ashes::VkAttachmentDescriptionArray attaches
		{
			{
				0u,
				m_target.getFormat(),
				VK_SAMPLE_COUNT_1_BIT,
				VK_ATTACHMENT_LOAD_OP_CLEAR,
				VK_ATTACHMENT_STORE_OP_STORE,
				VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				VK_ATTACHMENT_STORE_OP_DONT_CARE,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			}
		};
		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription
			{
				0u,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				{},
				{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
				{},
				ashes::nullopt,
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
		m_renderPass = device->createRenderPass( "OverlayRenderer"
			, std::move( createInfo ) );

		ashes::ImageViewCRefArray fbAttaches;
		fbAttaches.emplace_back( m_target );
		m_frameBuffer = m_renderPass->createFrameBuffer( "OverlayRenderer"
			, { m_target.image->getDimensions().width, m_target.image->getDimensions().height }
			, std::move( fbAttaches ) );
	}

	OverlayRenderer::Pipeline OverlayRenderer::doCreatePipeline( RenderDevice const & device
		, Pass const & pass
		, ashes::PipelineShaderStageCreateInfoArray program
		, bool text )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachments
		{
			VkPipelineColorBlendAttachmentState
			{
				VK_TRUE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				defaultColorWriteMask,
			},
		};
		ashes::PipelineColorBlendStateCreateInfo blState
		{
			0u,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			std::move( attachments ),
		};
		auto & materials = getRenderSystem()->getEngine()->getMaterialCache();
		ashes::VkDescriptorSetLayoutBindingArray bindings;

		bindings.emplace_back( materials.getPassBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eMaterials ) ) );
		bindings.emplace_back( materials.getTextureBuffer().createLayoutBinding( uint32_t( OverlayBindingId::eTexturesBuffer ) ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eOverlay )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eTexturesConfig )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		auto textures = filterTexturesFlags( pass.getTexturesMask(), TextureFlag::eAlbedo | TextureFlag::eOpacity );
		auto vertexLayout = ( textures.empty() 
			? &m_noTexDeclaration
			: &m_texDeclaration );

		if ( text )
		{
			vertexLayout = &m_textDeclaration;
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eTextMap )
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		}

		if ( !textures.empty() )
		{
			bindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( OverlayBindingId::eMaps )
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT
				, uint32_t( textures.size() ) ) );
		}

		auto descriptorLayout = device->createDescriptorSetLayout( std::move( bindings ) );
		auto descriptorPool = descriptorLayout->createPool( 1000u );
		auto pipelineLayout = device->createPipelineLayout( *descriptorLayout );
		auto pipeline = device->createPipeline( ashes::GraphicsPipelineCreateInfo
			{
				0u,
				std::move( program ),
				*vertexLayout,
				ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
				ashes::nullopt,
				ashes::PipelineViewportStateCreateInfo{},
				ashes::PipelineRasterizationStateCreateInfo{},
				ashes::PipelineMultisampleStateCreateInfo{},
				ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
				std::move( blState ),
				ashes::PipelineDynamicStateCreateInfo{ 0u, { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR } },
				*pipelineLayout,
				*m_renderPass,
			} );
		return Pipeline
		{
			std::move( descriptorLayout ),
			std::move( descriptorPool ),
			std::move( pipelineLayout ),
			std::move( pipeline ),
		};
	}

	OverlayRenderer::Pipeline & OverlayRenderer::doGetPipeline( RenderDevice const & device
		, Pass const & pass
		, std::map< uint32_t, Pipeline > & pipelines
		, bool text )
	{
		// Count wanted textures
		auto count = uint32_t( std::count_if( pass.begin()
			, pass.end()
			, []( TextureUnitSPtr unit )
			{
				auto & config = unit->getConfiguration();
				return config.colourMask[0] || config.opacityMask[0];
			}
		) );
		// Remove unwanted flags
		auto textures = filterTexturesFlags( pass.getTexturesMask(), TextureFlag::eAlbedo | TextureFlag::eOpacity );
		auto key = makeKey( textures, text );
		auto it = pipelines.find( key );

		if ( it == pipelines.end() )
		{
			// Since it does not exist yet, create it and initialise it
			it = pipelines.emplace( key
				, doCreatePipeline( device
					, pass
					, doCreateOverlayProgram( device, textures, text )
					, text ) ).first;
		}

		return it->second;
	}

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

			UBO_MATRIX( writer, uint32_t( OverlayBindingId::eMatrix ), 0u );
			UBO_OVERLAY( writer, uint32_t( OverlayBindingId::eOverlay ), 0u );

			// Shader inputs
			uint32_t index = 0u;
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u, hasTexture );
			auto text = writer.declInput< Vec2 >( "text", 2u, textOverlay );

			// Shader outputs
			auto vtx_text = writer.declOutput< Vec2 >( "vtx_text", 0u, textOverlay );
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 1u, hasTexture );
			auto out = writer.getOut();

			writer.implementFunction< void >( "main"
				, [&]()
				{
					vtx_text = text;
					vtx_texture = uv;
					auto size = writer.declLocale( "size"
						, c3d_overlayData.getOverlaySize() );
					out.vtx.position = c3d_matrixData.viewToProj( vec4( size * c3d_overlayData.modelToView( position )
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

			auto materials = std::make_unique< shader::PhongMaterials >( writer );
			materials->declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
				, uint32_t( OverlayBindingId::eMaterials ) );
			shader::TextureConfigurations textureConfigs{ writer };

			if ( hasTexture )
			{
				textureConfigs.declare( renderSystem.getGpuInformations().hasShaderStorageBuffers()
					, uint32_t( OverlayBindingId::eTexturesBuffer ) );
			}

			UBO_OVERLAY( writer, uint32_t( OverlayBindingId::eOverlay ), 0u );
			UBO_TEXTURES( writer, uint32_t( OverlayBindingId::eTexturesConfig ), 0u, hasTexture );

			// Shader inputs
			auto vtx_text = writer.declInput< Vec2 >( "vtx_text"
				, 0u
				, textOverlay );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture"
				, 1u
				, hasTexture );
			auto c3d_mapText = writer.declSampledImage< FImg2DR32 >( "c3d_mapText"
				, uint32_t( OverlayBindingId::eTextMap )
				, 0u
				, textOverlay );
			auto c3d_maps( writer.declSampledImageArray< FImg2DRgba32 >( "c3d_maps"
				, uint32_t( OverlayBindingId::eMaps )
				, 0u
				, std::max( 1u, texturesCount )
				, hasTexture ) );

			shader::Utils utils{ writer };

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto material = materials->getBaseMaterial( c3d_overlayData.getMaterialIndex() );
					auto diffuse = writer.declLocale( "diffuse"
						, material->m_diffuse() );
					auto alpha = writer.declLocale( "alpha"
						, material->m_opacity );

					if ( textOverlay )
					{
						alpha *= c3d_mapText.sample( vtx_text, 0.0_f );
					}

					if ( hasTexture )
					{
						utils.computeColourMapContribution( texturesFlags
							, textureConfigs
							, c3d_textureData.config
							, c3d_maps
							, vec3( vtx_texture, 0.0 )
							, diffuse );
						utils.computeOpacityMapContribution( texturesFlags
							, textureConfigs
							, c3d_textureData.config
							, c3d_maps
							, vec3( vtx_texture, 0.0 )
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
