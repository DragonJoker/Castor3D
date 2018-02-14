#include "Pipeline/GlPipelineLayout.hpp"

#include "Core/GlDevice.hpp"
#include "Descriptor/GlDescriptorSetLayout.hpp"
#include "Pipeline/GlComputePipeline.hpp"
#include "Pipeline/GlPipeline.hpp"

namespace gl_renderer
{
	PipelineLayout::PipelineLayout( Device const & device
		, renderer::DescriptorSetLayoutCRefArray const & setLayouts
		, renderer::PushConstantRangeCRefArray const & pushConstantRanges )
		: renderer::PipelineLayout{ device, setLayouts, pushConstantRanges }
		, m_device{ device }
	{
	}

	renderer::PipelinePtr PipelineLayout::createPipeline( renderer::ShaderProgram const & program
		, renderer::VertexLayoutCRefArray const & vertexLayouts
		, renderer::RenderPass const & renderPass
		, renderer::InputAssemblyState const & inputAssemblyState
		, renderer::RasterisationState const & rasterisationState
		, renderer::ColourBlendState const & colourBlendState )const
	{
		return std::make_unique< Pipeline >( m_device
			, *this
			, program
			, vertexLayouts
			, renderPass
			, inputAssemblyState
			, rasterisationState
			, colourBlendState );
	}

	renderer::ComputePipelinePtr PipelineLayout::createPipeline( renderer::ShaderProgram const & program )const
	{
		return std::make_unique< ComputePipeline >( m_device
			, *this
			, program );
	}
}
