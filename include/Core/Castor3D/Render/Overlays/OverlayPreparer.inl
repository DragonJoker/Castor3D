/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <RenderGraph/FramePassTimer.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace ovrlrend
	{
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

		static void doUpdateUbo( OverlayUboConfiguration & data
			, OverlayCategory const & overlay
			, Pass const & pass
			, castor::Size const & size )
		{
			auto position = overlay.getAbsolutePosition();
			auto ratio = overlay.getRenderRatio( size );
			data.position = castor::Point2f{ position };
			data.ratio = castor::Point2f{ ratio };
			data.refRenderSize = castor::Point2ui{ size.getWidth(), size.getHeight() };
			data.materialId = pass.getId();
		}

		template< typename VertexT, uint32_t CountT >
		static uint32_t doFillBuffers( typename std::vector< VertexT >::const_iterator begin
			, uint32_t count
			, OverlayVertexBufferIndexT< VertexT, CountT > & bufferIndex )
		{
			auto & bufferOffset = bufferIndex.geometryBuffers.textured.bufferOffset;
			std::copy( begin
				, std::next( begin, count )
				, bufferOffset.template getData< VertexT >( SubmeshFlag::ePositions ).begin() );
			bufferOffset.markDirty( SubmeshFlag::ePositions );
			return count;
		}

		template< typename VertexT, uint32_t CountT >
		OverlayVertexBufferIndexT< VertexT, CountT > & getVertexBuffer( std::vector< OverlayVertexBufferPoolPtrT< VertexT, CountT > > & pools
			, std::map< size_t, OverlayVertexBufferIndexT< VertexT, CountT > > & overlays
			, Overlay const & overlay
			, Pass const & pass
			, OverlayRenderNode & node
			, Engine & engine
			, RenderDevice const & device
			, CameraUbo const & cameraUbo
			, ashes::DescriptorSetLayout const & descriptorLayout
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
					pools.emplace_back( std::make_unique< OverlayVertexBufferPoolT< VertexT, CountT > >( engine
						, getName( overlay.getType() )
						, device
						, cameraUbo
						, descriptorLayout
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
	void OverlayPreparer::doPrepareOverlay( RenderDevice const & device
		, OverlayT const & overlay
		, Pass const & pass
		, std::map< size_t, OverlayVertexBufferIndexT< VertexT, CountT > > & overlays
		, std::vector< OverlayVertexBufferPoolPtrT< VertexT, CountT > > & vertexBuffers
		, std::vector < VertexT > const & vertices
		, FontTextureSPtr fontTexture )
	{
		if ( !vertices.empty() )
		{
			OverlayVertexBufferIndexT< VertexT, CountT > & bufferIndex = ovrlrend::getVertexBuffer< VertexT, CountT >( vertexBuffers
				, overlays
				, overlay.getOverlay()
				, pass
				, ( fontTexture
					? m_renderer.doGetTextNode( device, m_renderPass, pass, *fontTexture->getTexture(), *fontTexture->getSampler().lock() )
					: m_renderer.doGetPanelNode( device, m_renderPass, pass ) )
				, *pass.getOwner()->getEngine()
				, device
				, m_renderer.m_cameraUbo
				, *m_renderer.m_baseDescriptorLayout
				, ( fontTexture ? m_renderer.m_noTexTextDeclaration : m_renderer.m_noTexDeclaration )
				, ( fontTexture ? m_renderer.m_texTextDeclaration : m_renderer.m_texDeclaration )
				, MaxOverlayPanelsPerBuffer );
			ovrlrend::doUpdateUbo( bufferIndex.pool.overlaysBuffer[bufferIndex.index]
				, overlay
				, pass
				, m_renderer.m_size );
			ovrlrend::doFillBuffers( vertices.begin()
				, uint32_t( vertices.size() )
				, bufferIndex );
			ashes::DescriptorSetCRefArray descriptorSets;
			descriptorSets.push_back( *bufferIndex.pool.descriptorSet );
			descriptorSets.push_back( *device.renderSystem.getEngine()->getTextureUnitCache().getDescriptorSet() );

			if ( fontTexture )
			{
				descriptorSets.push_back( m_renderer.doCreateTextDescriptorSet( *fontTexture ) );
			}

			auto borderSize = ovrlrend::BorderSizeGetter< OverlayT, QuadT >{}( overlay, m_renderer.m_size );
			auto borderOffset = castor::Size{ uint32_t( borderSize.left() ), uint32_t( borderSize.top() ) };
			auto borderExtent = borderOffset + castor::Size{ uint32_t( borderSize.right() ), uint32_t( borderSize.bottom() ) };
			auto position = overlay.getAbsolutePosition( m_renderer.m_size ) - borderOffset;
			position->x = std::max( 0, position->x );
			position->y = std::max( 0, position->y );
			auto size = overlay.getAbsoluteSize( m_renderer.m_size ) + borderExtent;
			size->x = std::max( 1u, size->x );
			size->y = std::max( 1u, size->y );
			ashes::CommandBuffer & commandBuffer = *m_renderer.m_commands.commandBuffer;
			commandBuffer.bindPipeline( *bufferIndex.node.pipeline.pipeline );
			commandBuffer.setViewport( makeViewport( m_renderer.m_size ) );
			commandBuffer.setScissor( makeScissor( position, size ) );
			commandBuffer.bindDescriptorSets( descriptorSets, *bufferIndex.node.pipeline.pipelineLayout );
			DrawConstants constants{ bufferIndex.index };
			commandBuffer.pushConstants( *bufferIndex.node.pipeline.pipelineLayout
				, VK_SHADER_STAGE_VERTEX_BIT
				, 0u
				, sizeof( constants )
				, &constants );
			commandBuffer.bindVertexBuffer( 0u
				, bufferIndex.geometryBuffers.noTexture.bufferOffset.getBuffer( SubmeshFlag::ePositions )
				, bufferIndex.geometryBuffers.noTexture.bufferOffset.getOffset( SubmeshFlag::ePositions ) );
			commandBuffer.draw( uint32_t( vertices.size() )
				, 1u
				, 0u
				, 0u );
		}
	}
}
