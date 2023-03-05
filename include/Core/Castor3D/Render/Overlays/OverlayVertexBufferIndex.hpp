/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayVertexBufferIndex_H___
#define ___C3D_OverlayVertexBufferIndex_H___

#include "Castor3D/Render/Overlays/OverlaysModule.hpp"

namespace castor3d
{
	template< typename VertexT, uint32_t CountT >
	struct OverlayVertexBufferIndexT
	{
		operator bool()const
		{
			return index != InvalidIndex;
		}

		OverlayVertexBufferPoolT< VertexT, CountT > & pool;
		OverlayRenderNode & node;
		uint32_t index;
		OverlayGeometryBuffers geometryBuffers{};
		OverlayTextBufferIndex textBuffer{};
	};
}

#endif
