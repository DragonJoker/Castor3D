#include "Castor3D/Render/RenderPipeline.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/RenderPass.hpp"
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

CU_ImplementCUSmartPtr( castor3d, RenderPipeline )

using namespace castor;

namespace castor3d
{
	RenderPipeline::RenderPipeline( SceneRenderPass & owner
		, RenderSystem & renderSystem
		, ashes::PipelineDepthStencilStateCreateInfo dsState
		, ashes::PipelineRasterizationStateCreateInfo rsState
		, ashes::PipelineColorBlendStateCreateInfo blState
		, ashes::PipelineMultisampleStateCreateInfo msState
		, ShaderProgramSPtr program
		, PipelineFlags const & flags )
		: OwnedBy< SceneRenderPass >{ owner }
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
		, VkRenderPass renderPass
		, ashes::DescriptorSetLayoutCRefArray descriptorLayouts )
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

		if ( m_descriptorLayout )
		{
			descriptorLayouts.emplace_back( *m_descriptorLayout );
		}

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

		ashes::Optional< ashes::PipelineTessellationStateCreateInfo > tessellationState = ashes::nullopt;

		if ( m_program->hasSource( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT )
			|| m_program->hasSource( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) )
		{
			tessellationState = ashes::PipelineTessellationStateCreateInfo{ 0u, 3u };
		}

		m_pipelineLayout = device->createPipelineLayout( getOwner()->getName() + "RenderPipeline"
			, descriptorLayouts
			, m_pushConstantRanges );
		ashes::GraphicsPipelineCreateInfo createInfo
		(
			0u,
			m_program->getStates(),
			ashes::PipelineVertexInputStateCreateInfo{ 0u, std::move( bindings ), std::move( attributes ) },
			ashes::PipelineInputAssemblyStateCreateInfo{ 0u, m_flags.topology },
			std::move( tessellationState ),
			ashes::PipelineViewportStateCreateInfo{ 0u, 1u, std::move( viewports ), 1u, std::move( scissors ) },
			std::move( m_rsState ),
			std::move( m_msState ),
			std::move( m_dsState ),
			std::move( m_blState ),
			std::move( dynamicState ),
			*m_pipelineLayout,
			renderPass
		);
		m_pipeline = device->createPipeline( getOwner()->getName() + "RenderPipeline"
			, std::move( createInfo ) );
	}

	void RenderPipeline::cleanup( RenderDevice const & device )
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
	}

	void RenderPipeline::createDescriptorPools( uint32_t maxSets )
	{
		m_billboardAddDescriptors.clear();
		m_submeshAddDescriptors.clear();
		m_descriptorPool.reset();

		if ( hasDescriptorSetLayout() )
		{
			m_descriptorPool = getDescriptorSetLayout().createPool( getOwner()->getName() + "RenderPipeline", maxSets );
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

	void RenderPipeline::setDescriptorSetLayout( ashes::DescriptorSetLayoutPtr layouts )
	{
		CU_Require( !m_pipeline );
		m_descriptorLayout = std::move( layouts );
	}

	void RenderPipeline::setAdditionalDescriptorSet( SubmeshRenderNode const & node
		, ashes::DescriptorSetPtr descriptorSet )
	{
		m_submeshAddDescriptors.emplace( &node, std::move( descriptorSet ) );
	}

	void RenderPipeline::setAdditionalDescriptorSet( BillboardRenderNode const & node
		, ashes::DescriptorSetPtr descriptorSet )
	{
		m_billboardAddDescriptors.emplace( &node, std::move( descriptorSet ) );
	}

	ashes::DescriptorSet const & RenderPipeline::getAdditionalDescriptorSet( SubmeshRenderNode const & node )const
	{
		auto it = m_submeshAddDescriptors.find( &node );
		CU_Require( it != m_submeshAddDescriptors.end() );
		return *it->second;
	}

	ashes::DescriptorSet const & RenderPipeline::getAdditionalDescriptorSet( BillboardRenderNode const & node )const
	{
		auto it = m_billboardAddDescriptors.find( &node );
		CU_Require( it != m_billboardAddDescriptors.end() );
		return *it->second;
	}
}
