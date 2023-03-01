/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
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
		static void doFillBuffers( typename std::vector< VertexT >::const_iterator begin
			, uint32_t count
			, OverlayVertexBufferIndexT< VertexT, CountT > & bufferIndex )
		{
			if ( bufferIndex.geometryBuffers.buffer )
			{
				GpuBufferBase & buffer = *bufferIndex.geometryBuffers.buffer;
				std::copy( begin
					, std::next( begin, count )
					, &buffer.template getData< VertexT >( bufferIndex.geometryBuffers.offset ) );
			}
		}
	}

	//*********************************************************************************************

	template< typename QuadT, typename OverlayT, typename VertexT, uint32_t CountT >
	void OverlayPreparer::doPrepareOverlay( RenderDevice const & device
		, OverlayT const & overlay
		, Pass const & pass
		, OverlayVertexBufferPoolT< VertexT, CountT > & vertexBuffer
		, FontTextureSPtr fontTexture
		, bool secondary )
	{
		OverlayVertexBufferIndexT< VertexT, CountT > bufferIndex = ( fontTexture
			? vertexBuffer.fill( overlay, m_renderer.doGetTextNode( device, m_renderPass, pass, *fontTexture->getTexture(), *fontTexture->getSampler().lock() ), secondary )
			: vertexBuffer.fill( overlay, m_renderer.doGetPanelNode( device, m_renderPass, pass ), secondary ) );

		if ( !bufferIndex.geometryBuffers.buffer )
		{
			return;
		}

		ovrlrend::doUpdateUbo( bufferIndex.pool.overlaysBuffer[bufferIndex.index]
			, overlay
			, pass
			, m_renderer.getSize() );
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
			, bufferIndex.geometryBuffers.buffer->getBuffer().getBuffer()
			, bufferIndex.geometryBuffers.offset );
		commandBuffer.draw( bufferIndex.geometryBuffers.count
			, 1u
			, 0u
			, 0u );
	}
}
