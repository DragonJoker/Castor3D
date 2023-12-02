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
		ObjectBufferOffset::GpuBufferChunk indexOffset;
		ashes::BufferCRefArray buffers;
		ashes::UInt64Array offsets;
		ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
	};
}

#endif
