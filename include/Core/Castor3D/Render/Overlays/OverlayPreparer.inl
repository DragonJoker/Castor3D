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
		static void doUpdateUbo( OverlayUboConfiguration & data
			, OverlayCategory const & overlay
			, Pass const & pass
			, castor::Size const & renderSize )
		{
			auto size = overlay.getAbsoluteSize( renderSize );
			auto ratio = overlay.getRenderRatio( renderSize );
			data.size = castor::Point2f{ size.getWidth(), size.getHeight() };
			data.position = castor::Point2f{ overlay.getAbsolutePosition() };
			data.size *= ratio;
			data.uv = castor::Point4f{ overlay.getUV() };
			data.materialId = pass.getId();
		}

		static void doUpdateUbo( OverlayUboConfiguration & data
			, PanelOverlay const & overlay
			, Pass const & pass
			, castor::Size const & renderSize )
		{
			doUpdateUbo( data
				, static_cast< OverlayCategory const & >( overlay )
				, pass
				, renderSize );
		}

		static void doUpdateUbo( OverlayUboConfiguration & data
			, BorderPanelOverlay const & overlay
			, Pass const & pass
			, castor::Size const & renderSize )
		{
			doUpdateUbo( data
				, static_cast< OverlayCategory const & >( overlay )
				, pass
				, renderSize );
		}

		static void doUpdateUbo( OverlayUboConfiguration & data
			, TextOverlay const & overlay
			, Pass const & pass
			, castor::Size const & renderSize )
		{
			doUpdateUbo( data
				, static_cast< OverlayCategory const & >( overlay )
				, pass
				, renderSize );
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
	void OverlayPreparer::doPrepareOverlayDescriptors( RenderDevice const & device
		, OverlayT const & overlay
		, Pass const & pass
		, OverlayVertexBufferPoolT< VertexT, CountT > & vertexBuffer
		, FontTextureSPtr fontTexture
		, bool secondary )
	{
		OverlayVertexBufferIndexT< VertexT, CountT > bufferIndex = vertexBuffer.fill( m_renderer.getSize()
			, overlay
			, ( fontTexture
				? m_renderer.doGetTextNode( device, m_renderPass, pass, *fontTexture->getTexture(), *fontTexture->getSampler().lock() )
				: m_renderer.doGetPanelNode( device, m_renderPass, pass ) )
			, secondary );

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

		m_overlays.push_back( OverlayData{ &overlay.getOverlay()
			, descriptorSets
			, bufferIndex.geometryBuffers
			, bufferIndex.index
			, &bufferIndex.node.pipeline } );
	}
}
