#include "Pipeline/VkComputePipeline.hpp"

#include "Core/VkDevice.hpp"
#include "Pipeline/VkPipelineLayout.hpp"
#include "Shader/VkShaderProgram.hpp"

namespace vk_renderer
{
	ComputePipeline::ComputePipeline( Device const & device
		, renderer::PipelineLayout const & layout
		, renderer::ShaderProgram const & program )
		: renderer::ComputePipeline{ device
			, layout
			, program }
		, m_device{ device }
		, m_layout{ static_cast< PipelineLayout const & >( layout ) }
		, m_shader{ static_cast< ShaderProgram const & >( program ) }
	{
		auto & module = *m_shader.begin();
		VkPipelineShaderStageCreateInfo shaderStage
		{
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,                                                        // flags
			module.getStage(),                                        // stage
			VkShaderModule( module ),                                 // module
			"main",                                                   // pName
			nullptr,                                                  // pSpecializationInfo
		};

		VkComputePipelineCreateInfo pipeline
		{
			VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			shaderStage,                                                  // stage
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
