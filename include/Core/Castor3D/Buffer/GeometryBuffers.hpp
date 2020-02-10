/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryBuffers_H___
#define ___C3D_GeometryBuffers_H___

#include "BufferModule.hpp"

namespace castor3d
{
	struct GeometryBuffers
	{
		ashes::BufferCRefArray vbo;
		ashes::UInt64Array vboOffsets;
		uint32_t vtxCount;
		ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
		ashes::BufferBase const * ibo{ nullptr };
		uint64_t iboOffset;
		uint32_t idxCount;
	};
}

#endif
