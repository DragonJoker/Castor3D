#include "Castor3D/Render/RenderPipeline.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Miscellaneous/DebugName.hpp"
#include "Castor3D/Render/PipelineFlags.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineInputAssemblyStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <ShaderWriter/Source.hpp>

CU_ImplementSmartPtr( castor3d, RenderPipeline )

namespace castor3d
{
	namespace rendpipl
	{
		static const castor::String Suffix = cuT( "/RenderPipeline" );
	}

	RenderPipeline::RenderPipeline( RenderNodesPass & owner
		, RenderSystem & renderSystem
		, ashes::PipelineDepthStencilStateCreateInfo dsState
		, ashes::PipelineRasterizationStateCreateInfo rsState
		, ashes::PipelineColorBlendStateCreateInfo blState
		, ashes::PipelineMultisampleStateCreateInfo msState
		, ShaderProgramRPtr program
		, PipelineFlags const & flags )
		: castor::OwnedBy< RenderNodesPass >{ owner }
		, m_renderSystem{ renderSystem }
		, m_dsState{ castor::move( dsState ) }
		, m_rsState{ castor::move( rsState ) }
		, m_blState{ castor::move( blState ) }
		, m_msState{ castor::move( msState ) }
		, m_program{ castor::move( program ) }
		, m_flags{ flags }
		, m_flagsHash{ getPipelineBaseHash( owner.getEngine()->getPassComponentsRegister()
			, owner.getEngine()->getSubmeshComponentsRegister()
			, m_flags ) }
	{
	}

	void RenderPipeline::initialise( RenderDevice const & device
		, VkRenderPass renderPass )
	{
		if ( m_pipelineLayout || m_pipeline )
		{
			return;
		}

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
		descriptorLayouts.emplace_back( *getOwner()->getScene().getBindlessTexDescriptorLayout() );

		if ( m_addDescriptorLayout )
		{
			descriptorLayouts.emplace_back( *m_addDescriptorLayout );
		}

		if ( m_meshletDescriptorLayout )
		{
			descriptorLayouts.emplace_back( *m_meshletDescriptorLayout );
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
			dynamicState = ashes::PipelineDynamicStateCreateInfo{ 0u, castor::move( dynamicStates ) };
		}

		ashes::Optional< ashes::PipelineTessellationStateCreateInfo > tessellationState = ashes::nullopt;

		if ( m_program->hasSource( ast::ShaderStage::eTessellationControl )
			|| m_program->hasSource( ast::ShaderStage::eTessellationEvaluation ) )
		{
			tessellationState = ashes::PipelineTessellationStateCreateInfo{ 0u, m_flags.patchVertices };
		}

		auto mbName = castor::toUtf8( getOwner()->getName() + rendpipl::Suffix );
		m_pipelineLayout = device->createPipelineLayout( mbName
			, descriptorLayouts
			, m_pushConstantRanges );
		ashes::GraphicsPipelineCreateInfo createInfo
		(
			0u,
			m_program->getStates(),
			ashes::PipelineVertexInputStateCreateInfo{ 0u, castor::move( bindings ), castor::move( attributes ) },
			ashes::PipelineInputAssemblyStateCreateInfo{ 0u, m_flags.topology },
			castor::move( tessellationState ),
			ashes::PipelineViewportStateCreateInfo{ 0u, 1u, castor::move( viewports ), 1u, castor::move( scissors ) },
			m_rsState,
			m_msState,
			m_dsState,
			m_blState,
			castor::move( dynamicState ),
			*m_pipelineLayout,
			renderPass
		);
		m_pipeline = device->createPipeline( mbName
			, castor::move( createInfo ) );
	}

	void RenderPipeline::cleanup()
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
	}

	void RenderPipeline::setVertexLayouts( ashes::PipelineVertexInputStateCreateInfoCRefArray const & layouts )
	{
		CU_Require( !m_pipeline );

		for ( auto & layout : layouts )
		{
			m_vertexLayouts.emplace_back( layout.get() );
		}
	}
}
