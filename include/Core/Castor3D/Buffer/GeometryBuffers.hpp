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
		ObjectBufferOffset bufferOffset;
		ashes::BufferCRefArray other;
		ashes::UInt64Array otherOffsets;
		ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
	};
}

#endif
