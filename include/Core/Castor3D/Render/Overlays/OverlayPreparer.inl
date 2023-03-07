/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Overlays/OverlayRenderer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
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
	template< typename QuadT, typename OverlayT, typename VertexT, uint32_t CountT >
	void OverlayPreparer::doPrepareOverlayDescriptors( RenderDevice const & device
		, OverlayT const & overlay
		, OverlayData data
		, OverlayVertexBufferPoolT< VertexT, CountT > & vertexBuffer
		, FontTexture const * fontTexture
		, bool secondary )
	{
		OverlayVertexBufferIndexT< VertexT, CountT > bufferIndex = vertexBuffer.fill( m_renderer.getSize()
			, overlay
			, *data.node
			, secondary
			, fontTexture );

		if ( !bufferIndex.geometryBuffers.buffer )
		{
			return;
		}

		this->doUpdateUbo( bufferIndex.pool.overlaysBuffer[bufferIndex.overlayIndex]
			, overlay
			, data.node->pass
			, m_renderer.getSize()
			, uint32_t( bufferIndex.geometryBuffers.offset )
			, bufferIndex.textBuffer );
		data.geometryBuffers = bufferIndex.geometryBuffers;
		data.overlayIndex = bufferIndex.overlayIndex;
		data.pipelineIndex = bufferIndex.pipelineIndex;
		m_overlays.push_back( std::move( data ) );
	}
}
