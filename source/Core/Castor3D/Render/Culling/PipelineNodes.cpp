#include "Castor3D/Render/Culling/PipelineNodes.hpp"

namespace castor3d
{
	void registerPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, castor::Vector< PipelineBuffer > & nodesIds )
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
		, castor::Vector< PipelineBuffer > const & cont )
	{
		auto it = std::find_if( cont.begin()
			, cont.end()
			, [&hash, &buffer]( PipelineBuffer const & lookup )
			{
				return lookup.first == hash
					&& lookup.second == &buffer;
			} );
		CU_Require( it != cont.end() );
		return it != cont.end()
			? uint32_t( std::distance( cont.begin(), it ) )
			: InvalidPipelineIndex;
	}

	PipelineNodes & getPipelineNodes( PipelineBaseHash hash
		, ashes::BufferBase const & buffer
		, castor::Vector< PipelineBuffer > const & cont
		, PipelineNodes * nodes
		, VkDeviceSize maxNodesCount )
	{
		auto index = getPipelineNodeIndex( hash, buffer, cont );
		CU_Require( index < maxNodesCount );
		return nodes[index];
	}
}
