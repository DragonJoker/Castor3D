/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline.hpp"

namespace renderer
{
	Pipeline::Pipeline( Device const & device
		, PipelineLayout const & layout
		, ShaderProgram const & program
		, VertexLayoutCRefArray const & vertexLayouts
		, RenderPass const & renderPass
		, PrimitiveTopology topology
		, RasterisationState const & rasterisationState
		, ColourBlendState const & colourBlendState )
		: m_topology{ topology }
	{
	}
}
