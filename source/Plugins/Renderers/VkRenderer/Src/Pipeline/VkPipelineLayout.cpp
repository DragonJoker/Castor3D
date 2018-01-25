#include "Pipeline/VkPipelineLayout.hpp"

#include "Descriptor/VkDescriptorSetLayout.hpp"
#include "Core/VkDevice.hpp"

namespace vk_renderer
{
	PipelineLayout::PipelineLayout( Device const & device
		, renderer::DescriptorSetLayout const * layout )
		: renderer::PipelineLayout{ device, layout }
		, m_device{ device }
	{
		VkDescriptorSetLayout dslayout{ VK_NULL_HANDLE };

		if ( layout )
		{
			dslayout = *static_cast< DescriptorSetLayout const * >( layout );
		}

		VkPipelineLayoutCreateInfo createInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			layout ? 1u : 0u,                                             // setLayoutCount
			layout ? &dslayout : nullptr,                                 // pSetLayouts
			0u,                                                           // pushConstantRangeCount
			nullptr                                                       // pPushConstantRanges
		};
		DEBUG_DUMP( createInfo );
		auto res = vk::CreatePipelineLayout( m_device
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
		vk::DestroyPipelineLayout( m_device
			, m_layout
			, nullptr );
	}
}
