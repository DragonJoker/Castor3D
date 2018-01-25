#include "Pipeline/VkPipeline.hpp"

#include "Core/VkDevice.hpp"
#include "Core/VkRenderingResources.hpp"
#include "Pipeline/VkPipelineLayout.hpp"
#include "Pipeline/VkVertexLayout.hpp"
#include "RenderPass/VkRenderPass.hpp"
#include "Shader/VkAttribute.hpp"
#include "Shader/VkShaderProgram.hpp"

namespace vk_renderer
{
	namespace
	{
		VertexLayoutCRefArray convert( renderer::VertexLayoutCRefArray const & layouts )
		{
			VertexLayoutCRefArray result;
			result.reserve( layouts.size() );

			for ( auto & layout : layouts )
			{
				result.emplace_back( static_cast< VertexLayout const & >( layout.get() ) );
			}

			return result;
		}
	}

	Pipeline::Pipeline( Device const & device
		, renderer::PipelineLayout const & layout
		, renderer::ShaderProgram const & program
		, renderer::VertexLayoutCRefArray const & vertexLayouts
		, renderer::RenderPass const & renderPass
		, renderer::PrimitiveTopology topology
		, renderer::RasterisationState const & rasterisationState
		, renderer::ColourBlendState const & colourBlendState )
		: renderer::Pipeline{ device
			, layout
			, program
			, vertexLayouts
			, renderPass
			, topology
			, rasterisationState
			, colourBlendState }
		, m_device{ device }
		, m_layout{ static_cast< PipelineLayout const & >( layout ) }
		, m_shader{ static_cast< ShaderProgram const & >( program ) }
		, m_vertexLayouts{ convert( vertexLayouts ) }
		, m_renderPass{ static_cast< RenderPass const & >( renderPass ) }
		, m_topology{ convert( topology ) }
		, m_rasterisationState{ convert( rasterisationState ) }
		, m_colourBlendStateAttachments{ convert< VkPipelineColorBlendAttachmentState >( colourBlendState.begin(), colourBlendState.end() ) }
		, m_colourBlendState{ convert( colourBlendState, m_colourBlendStateAttachments ) }
	{
	}

	renderer::Pipeline & Pipeline::finish()
	{
		// Les informations li�es aux shaders utilis�s.
		std::vector< VkPipelineShaderStageCreateInfo > shaderStage;

		for ( auto & module : m_shader )
		{
			shaderStage.emplace_back( VkPipelineShaderStageCreateInfo
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,                                                        // flags
				module.getStage(),                                        // stage
				VkShaderModule( module ),                                 // module
				"main",                                                   // pName
				nullptr,                                                  // pSpecializationInfo
			} );
		}

		// Les informations des donn�es contenues dans les tampons de sommets.
		std::vector< VkVertexInputBindingDescription > vertexBindingDescriptions;
		std::vector< VkVertexInputAttributeDescription > vertexAttributeDescriptions;
		vertexBindingDescriptions.reserve( m_vertexLayouts.size() );

		for ( auto const & vb : m_vertexLayouts )
		{
			vertexBindingDescriptions.push_back( vb.get().getDescription() );

			for ( auto const & attribute : vb.get() )
			{
				vertexAttributeDescriptions.emplace_back( attribute );
			}
		}

		VkPipelineVertexInputStateCreateInfo vertexInputState
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			static_cast< uint32_t >( vertexBindingDescriptions.size() ),  // vertexBindingDescriptionCount
			vertexBindingDescriptions.data(),                             // pVertexBindingDescriptions
			static_cast< uint32_t >( vertexAttributeDescriptions.size() ),// vertexAttributeDescriptionCount
			vertexAttributeDescriptions.data()                            // pVertexAttributeDescriptions
		};

		// Le format de l'affichage des sommets.
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			m_topology,                                                   // topology
			VK_FALSE                                                      // primitiveRestartEnable
		};

		// Le viewport.
		VkPipelineViewportStateCreateInfo viewportState
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,                                                                      // flags
			1u,                                                                     // viewportCount
			m_viewport                                                              // pViewports
				? m_viewport.get()
				: nullptr,
			1u,                                                                     // scissorCount
			m_scissor                                                               // pScissors
				? m_scissor.get()
				: nullptr,
		};

		// Les �tats dynamiques, le cas �ch�ant
		std::vector< VkDynamicState > dynamicStates;

		if ( !m_viewport )
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
		}

		if ( !m_scissor )
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );
		}

		VkPipelineDynamicStateCreateInfo dynamicState
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,         // sType
			nullptr,                                                      // pNext
			0,                                                            // flags
			static_cast< uint32_t >( dynamicStates.size() ),              // dynamicStateCount
			dynamicStates.data()                                          // pDynamicStates
		};

		// Enfin, on cr�e le pipeline !!
		VkGraphicsPipelineCreateInfo pipeline
		{
			VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			nullptr,
			0,                                                            // flags
			static_cast< uint32_t >( shaderStage.size() ),                // stageCount
			shaderStage.data(),                                           // pStages
			&vertexInputState,                                            // pVertexInputState;
			&inputAssemblyState,                                          // pInputAssemblyState
			m_tessellationState                                           // pTessellationState
				? m_tessellationState.get()
				: nullptr,
			&viewportState,                                               // pViewportState
			&m_rasterisationState,                                        // pRasterizationState
			m_multisampleState                                            // pMultisampleState
				? m_multisampleState.get()
				: nullptr,
			m_depthStencilState                                           // pDepthStencilState
				? m_depthStencilState.get()
				: nullptr,
			&m_colourBlendState,                                          // pColorBlendState
			dynamicStates.empty() ? nullptr : &dynamicState,              // pDynamicState
			m_layout,                                                     // layout
			m_renderPass,                                                 // renderPass
			0,                                                            // subpass
			VK_NULL_HANDLE,                                               // basePipelineHandle
			-1                                                            // basePipelineIndex
		};
		DEBUG_DUMP( pipeline );
		DEBUG_WRITE( "pipeline.log" );
		auto res = vk::CreateGraphicsPipelines( m_device
			, VK_NULL_HANDLE
			, 1
			, &pipeline
			, nullptr
			, &m_pipeline );

		if ( !checkError( res ) )
		{
			throw std::runtime_error{ "Pipeline creation failed: " + getLastError() };
		}

		return *this;
	}

	Pipeline::~Pipeline()
	{
		vk::DestroyPipeline( m_device, m_pipeline, nullptr );
	}

	renderer::Pipeline & Pipeline::multisampleState( renderer::MultisampleState const & state )
	{
		m_multisampleState = std::make_unique< VkPipelineMultisampleStateCreateInfo >( convert( state ) );
		return *this;
	}

	renderer::Pipeline & Pipeline::depthStencilState( renderer::DepthStencilState const & state )
	{
		m_depthStencilState = std::make_unique< VkPipelineDepthStencilStateCreateInfo >( convert( state ) );
		return *this;
	}

	renderer::Pipeline & Pipeline::tessellationState( renderer::TessellationState const & state )
	{
		m_tessellationState = std::make_unique< VkPipelineTessellationStateCreateInfo >( convert( state ) );
		return *this;
	}

	renderer::Pipeline & Pipeline::viewport( renderer::Viewport const & viewport )
	{
		m_viewport = std::make_unique< VkViewport >( convert( viewport ) );
		return *this;
	}

	renderer::Pipeline & Pipeline::scissor( renderer::Scissor const & scissor )
	{
		m_scissor = std::make_unique< VkRect2D >( convert( scissor ) );
		return *this;
	}
}
