#include "Pipeline/VkComputePipeline.hpp"

#include "Core/VkDevice.hpp"
#include "Pipeline/VkPipelineLayout.hpp"
#include "Pipeline/VkSpecialisationInfo.hpp"
#include "Pipeline/VkSpecialisationMapEntry.hpp"
#include "Shader/VkShaderModule.hpp"

namespace vk_renderer
{
	ComputePipeline::ComputePipeline( Device const & device
		, renderer::PipelineLayout const & layout
		, renderer::ComputePipelineCreateInfo && createInfo )
		: renderer::ComputePipeline{ device
			, layout
			, std::move( createInfo ) }
		, m_device{ device }
		, m_layout{ static_cast< PipelineLayout const & >( layout ) }
	{
		auto & module = static_cast< ShaderModule const & >( *m_createInfo.stage.module );

		if ( m_createInfo.stage.specialisationInfo )
		{
			auto & info = *m_createInfo.stage.specialisationInfo;
			m_specialisationEntries = convert< VkSpecializationMapEntry >( info.begin(), info.end() );
			m_specialisationInfos = convert( info, m_specialisationEntries );
			m_shaderStage = convert( m_createInfo.stage, &m_specialisationInfos );
		}
		else
		{
			m_shaderStage = convert( m_createInfo.stage );
		}

		VkComputePipelineCreateInfo pipeline
		{
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			m_shaderStage,                                                // stage
			m_layout,                                                     // layout
			VK_NULL_HANDLE,                                               // basePipelineHandle
			-1                                                            // basePipelineIndex
		};
		DEBUG_DUMP( pipeline );
		DEBUG_WRITE( "pipeline.log" );
		auto res = m_device.vkCreateComputePipelines( m_device
			, VK_NULL_HANDLE
			, 1
			, &pipeline
			, nullptr
			, &m_pipeline );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Pipeline creation failed: " + getLastError() };
		}
	}

	ComputePipeline::~ComputePipeline()
	{
		m_device.vkDestroyPipeline( m_device, m_pipeline, nullptr );
	}
}
