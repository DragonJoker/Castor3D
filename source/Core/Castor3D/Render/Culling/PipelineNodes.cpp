#include "Castor3D/Render/Culling/PipelineNodes.hpp"

namespace c3d
{
	void registerPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & posBuffer
		, ashes::BufferBase const * idxBuffer
		, Vector< PipelineBuffer > & nodesIds )
	{
		auto it = std::find_if( nodesIds.begin()
			, nodesIds.end()
			, [&hash, &posBuffer, idxBuffer]( PipelineBuffer const & lookup )
			{
				return lookup.hash == hash
					&& lookup.posBuffer == &posBuffer
					&& lookup.idxBuffer == idxBuffer;
			} );

		if ( it == nodesIds.end() )
		{
			nodesIds.emplace_back( hash, &posBuffer, idxBuffer );
		}
	}

	uint32_t getPipelineNodeIndex( PipelineBaseHash hash
		, ashes::BufferBase const & posBuffer
		, ashes::BufferBase const * idxBuffer
		, Vector< PipelineBuffer > const & cont )
	{
		auto it = std::find_if( cont.begin()
			, cont.end()
			, [&hash, &posBuffer, idxBuffer]( PipelineBuffer const & lookup )
			{
				return lookup.hash == hash
					&& lookup.posBuffer == &posBuffer
					&& lookup.idxBuffer == idxBuffer;
			} );
		CU_Require( it != cont.end() );
		return it != cont.end()
			? uint32_t( std::distance( cont.begin(), it ) )
			: InvalidPipelineIndex;
	}

	PipelineNodes & getPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & posBuffer
		, ashes::BufferBase const * idxBuffer
		, Vector< PipelineBuffer > const & cont
		, PipelineNodes * nodes
		, VkDeviceSize maxNodesCount )
	{
		auto index = getPipelineNodeIndex( hash, posBuffer, idxBuffer, cont );
		CU_Require( index < maxNodesCount );
		return nodes[index];
	}
}
