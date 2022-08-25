/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PipelineNodes_H___
#define ___C3D_PipelineNodes_H___

#include "Castor3D/Model/Mesh/Submesh/SubmeshModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Limits.hpp"

namespace castor3d
{
	void registerPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > & nodesIds );
	uint32_t getPipelineNodeIndex( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > const & cont );
	PipelineNodes & getPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > const & cont
		, PipelineNodes * nodes
		, VkDeviceSize maxNodesCount );
}

#endif
