/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryBuffers_H___
#define ___C3D_GeometryBuffers_H___

#include "BufferModule.hpp"
#include "ObjectBufferOffset.hpp"

namespace castor3d
{
	struct GeometryBuffers
	{
		GeometryBuffers() = default;
		GeometryBuffers( ObjectBufferOffset::GpuBufferChunk indexOffset
			, ashes::BufferCRefArray buffers
			, ashes::UInt64Array offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray layouts )noexcept
			: indexOffset{ castor::move( indexOffset ) }
			, buffers{ castor::move( buffers ) }
			, offsets{ castor::move( offsets ) }
			, layouts{ castor::move( layouts ) }
		{
		}
		ObjectBufferOffset::GpuBufferChunk indexOffset{};
		ashes::BufferCRefArray buffers{};
		ashes::UInt64Array offsets{};
		ashes::PipelineVertexInputStateCreateInfoCRefArray layouts{};
	};
}

#endif
