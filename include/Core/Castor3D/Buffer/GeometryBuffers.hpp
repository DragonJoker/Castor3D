/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GeometryBuffers_H___
#define ___C3D_GeometryBuffers_H___

#include "Castor3D/Buffer/BufferModule.hpp"

#include "Castor3D/Buffer/ObjectBufferOffset.hpp"
#include "Castor3D/Render/Raytracing/BottomLevelAS.hpp"

namespace c3d
{
	struct GeometryBuffers
	{
		GeometryBuffers() = default;
		GeometryBuffers( ObjectBufferOffset::GpuBufferChunk indexOffset
			, ashes::BufferCRefArray buffers
			, ashes::UInt64Array offsets
			, ashes::PipelineVertexInputStateCreateInfoCRefArray layouts )noexcept
			: indexOffset{ c3d::move( indexOffset ) }
			, buffers{ c3d::move( buffers ) }
			, offsets{ c3d::move( offsets ) }
			, layouts{ c3d::move( layouts ) }
		{
		}

		ObjectBufferOffset::GpuBufferChunk indexOffset{};
		ashes::BufferCRefArray buffers{};
		ashes::UInt64Array offsets{};
		ashes::PipelineVertexInputStateCreateInfoCRefArray layouts{};
	};
}

#endif
