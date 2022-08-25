#include "Castor3D/Render/Culling/PipelineNodes.hpp"

namespace castor3d
{
	void registerPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > & nodesIds )
	{
		auto it = std::find_if( nodesIds.begin()
			, nodesIds.end()
			, [&hash, &buffer]( PipelineBuffer const & lookup )
			{
				return lookup.first == hash
					&& lookup.second == &buffer;
			} );

		if ( it == nodesIds.end() )
		{
			nodesIds.emplace_back( hash, &buffer );
		}
	}

	uint32_t getPipelineNodeIndex( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > const & cont )
	{
		auto it = std::find_if( cont.begin()
			, cont.end()
			, [&hash, &buffer]( PipelineBuffer const & lookup )
			{
				return lookup.first == hash
					&& lookup.second == &buffer;
			} );
		CU_Require( it != cont.end() );
		return uint32_t( std::distance( cont.begin(), it ) );
	}

	PipelineNodes & getPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, std::vector< PipelineBuffer > const & cont
		, PipelineNodes * nodes
		, VkDeviceSize maxNodesCount )
	{
		auto index = getPipelineNodeIndex( hash, buffer, cont );
		CU_Require( index < maxNodesCount );
		return nodes[index];
	}
}
