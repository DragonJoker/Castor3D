#include "Castor3D/Render/RenderPipeline.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	RenderPipeline::RenderPipeline( RenderPass & owner
		, RenderSystem & renderSystem
		, ashes::PipelineDepthStencilStateCreateInfo dsState
		, ashes::PipelineRasterizationStateCreateInfo rsState
		, ashes::PipelineColorBlendStateCreateInfo blState
		, ashes::PipelineMultisampleStateCreateInfo msState
		, ShaderProgramSPtr program
		, PipelineFlags const & flags )
		: OwnedBy< RenderPass >{ owner }
		, m_renderSystem{ renderSystem }
		, m_dsState{ std::move( dsState ) }
		, m_rsState{ std::move( rsState ) }
		, m_blState{ std::move( blState ) }
		, m_msState{ std::move( msState ) }
		, m_program{ std::move( program ) }
		, m_flags( flags )
	{
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::initialise( RenderDevice const & device
		, ashes::RenderPass const & renderPass )
	{
		ashes::VkVertexInputBindingDescriptionArray bindings;
		ashes::VkVertexInputAttributeDescriptionArray attributes;

		for ( auto & layout : m_vertexLayouts )
		{
			bindings.insert( bindings.end()
				, layout.vertexBindingDescriptions.begin()
				, layout.vertexBindingDescriptions.end() );
			attributes.insert( attributes.end()
				, layout.vertexAttributeDescriptions.begin()
				, layout.vertexAttributeDescriptions.end() );
		}

		ashes::DescriptorSetLayoutCRefArray descriptorLayouts;

		for ( auto & descriptorLayout : m_descriptorLayouts )
		{
			if ( !descriptorLayout->getBindings().empty() )
			{
				descriptorLayouts.emplace_back( *descriptorLayout );
			}
		}

		m_program->initialise( device );
		ashes::VkDynamicStateArray dynamicStates;
		ashes::VkViewportArray viewports;
		ashes::VkScissorArray scissors;

		if ( m_viewport )
		{
			viewports.push_back( *m_viewport );
		}
		else
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_VIEWPORT );
		}

		if ( m_scissor )
		{
			scissors.push_back( *m_scissor );
		}
		else
		{
			dynamicStates.push_back( VK_DYNAMIC_STATE_SCISSOR );
		}

		ashes::Optional< ashes::PipelineDynamicStateCreateInfo > dynamicState;

		if ( !dynamicStates.empty() )
		{
			dynamicState = ashes::PipelineDynamicStateCreateInfo{ 0u, std::move( dynamicStates ) };
		}

		m_pipelineLayout = device->createPipelineLayout( "RenderPipeline"
			, descriptorLayouts
			, m_pushConstantRanges );
		ashes::GraphicsPipelineCreateInfo createInfo
		(
			0u,
			m_program->getStates(),
			ashes::PipelineVertexInputStateCreateInfo{ 0u, std::move( bindings ), std::move( attributes ) },
			ashes::PipelineInputAssemblyStateCreateInfo{ 0u, m_flags.topology },
			ashes::nullopt,
			ashes::PipelineViewportStateCreateInfo{ 0u, 1u, std::move( viewports ), 1u, std::move( scissors ) },
			std::move( m_rsState ),
			std::move( m_msState ),
			std::move( m_dsState ),
			std::move( m_blState ),
			std::move( dynamicState ),
			*m_pipelineLayout,
			renderPass
		);
		m_pipeline = device->createPipeline( "RenderPipeline"
			, std::move( createInfo ) );
	}

	void RenderPipeline::cleanup( RenderDevice const & device )
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
	}

	void RenderPipeline::createDescriptorPools( uint32_t maxSets )
	{
		m_descriptorPools.clear();

		for ( auto & layout : m_descriptorLayouts )
		{
			auto & bindings = layout->getBindings();
			auto it = std::find_if( bindings.begin()
				, bindings.end()
				, []( VkDescriptorSetLayoutBinding const & lookup )
				{
					return lookup.descriptorCount == 0u;
				} );

			if ( it == bindings.end()
				&& !bindings.empty() )
			{
				m_descriptorPools.emplace_back( layout->createPool( "RenderPipeline", maxSets ) );
			}
		}
	}

	void RenderPipeline::setVertexLayouts( ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		CU_Require( !m_pipeline );

		for ( auto & layout : layouts )
		{
			m_vertexLayouts.emplace_back( layout.get() );
		}
	}

	void RenderPipeline::setDescriptorSetLayouts( std::vector< ashes::DescriptorSetLayoutPtr > && layouts )
	{
		CU_Require( !m_pipeline );
		m_descriptorLayouts = std::move( layouts );
	}
}
