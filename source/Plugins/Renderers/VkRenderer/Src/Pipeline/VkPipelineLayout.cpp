#include "Pipeline/VkPipelineLayout.hpp"

#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Core/VkDevice.hpp"
#include "Pipeline/VkComputePipeline.hpp"
#include "Pipeline/VkPipeline.hpp"

namespace vk_renderer
{
	namespace
	{
		DescriptorSetLayoutCRefArray convert( renderer::DescriptorSetLayoutCRefArray const & values )
		{
			DescriptorSetLayoutCRefArray result;
			result.reserve( values.size() );

			for ( auto & value : values )
			{
				result.emplace_back( static_cast< DescriptorSetLayout const & >( value.get() ) );
			}

			return result;
		}
	}

	PipelineLayout::PipelineLayout( Device const & device
		, renderer::DescriptorSetLayoutCRefArray const & setLayouts
		, renderer::PushConstantRangeCRefArray const & pushConstantRanges )
		: renderer::PipelineLayout{ device, setLayouts, pushConstantRanges }
		, m_device{ device }
	{
		auto vklayouts = makeVkArray< VkDescriptorSetLayout >( convert( setLayouts ) );
		auto vkranges = convert< VkPushConstantRange >( pushConstantRanges );

		VkPipelineLayoutCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			uint32_t( vklayouts.size() ),                                 // setLayoutCount
			vklayouts.empty()                                             // pSetLayouts
				? nullptr
				: vklayouts.data(),
			uint32_t( vkranges.size() ),                                  // pushConstantRangeCount
			vkranges.empty()                                              // pPushConstantRanges
				? nullptr
				: vkranges.data()
		};
		DEBUG_DUMP( createInfo );
		auto res = m_device.vkCreatePipelineLayout( m_device
			, &createInfo
			, nullptr
			, &m_layout );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Pipeline layout creation failed: " + getLastError() };
		}
	}

	PipelineLayout::~PipelineLayout()
	{
		m_device.vkDestroyPipelineLayout( m_device
			, m_layout
			, nullptr );
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
