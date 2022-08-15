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
	using PipelineHashDetails = std::tuple< SubmeshFlags
		, ProgramFlags
		, PassTypeID
		, PassFlags
		, uint32_t // maxTexcoordSet
		, uint32_t // texturesCount
		, TextureFlags >;

	uint64_t getHash( SubmeshFlags submeshFlags
		, ProgramFlags programFlags
		, PassTypeID passType
		, PassFlags passFlags
		, uint32_t maxTexcoordSet
		, uint32_t texturesCount
		, TextureFlags texturesFlags );
	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, Submesh const & data
		, Pass const & pass
		, bool isFrontCulled );
	PipelineBaseHash getPipelineBaseHash( RenderNodesPass const & renderPass
		, BillboardBase const & data
		, Pass const & pass
		, bool isFrontCulled );
	PipelineHashDetails getPipelineHashDetails( PipelineBaseHash const & hash );
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
