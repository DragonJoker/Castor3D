#include "RenderPipeline.hpp"

#include "Engine.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Shader/Program.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <Descriptor/DescriptorSetLayout.hpp>
#include <Pipeline/InputAssemblyState.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Pipeline/ShaderStageState.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderPipeline::RenderPipeline( RenderSystem & renderSystem
		, renderer::DepthStencilState && dsState
		, renderer::RasterisationState && rsState
		, renderer::ColourBlendState && blState
		, renderer::MultisampleState && msState
		, ShaderProgramSPtr program
		, PipelineFlags const & flags )
		: OwnedBy< RenderSystem >{ renderSystem }
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

	void RenderPipeline::initialise( renderer::RenderPass const & renderPass
		, renderer::PrimitiveTopology topology )
	{
		renderer::VertexLayoutCRefArray vertexLayouts;

		for ( auto & layout : m_vertexLayouts )
		{
			vertexLayouts.emplace_back( layout );
		}

		renderer::DescriptorSetLayoutCRefArray descriptorLayouts;

		for ( auto & layout : m_descriptorLayouts )
		{
			descriptorLayouts.emplace_back( *layout );
		}

		renderer::GraphicsPipelineCreateInfo createInfo
		{
			m_program->getStates(),
			renderPass,
			renderer::VertexInputState::create( vertexLayouts ),
			renderer::InputAssemblyState{ topology },
			m_rsState,
			m_msState,
			m_blState,
		};
		createInfo.depthStencilState = m_dsState;

		if ( m_viewport )
		{
			createInfo.viewport = *m_viewport;
		}
		else
		{
			createInfo.dynamicStates.push_back( renderer::DynamicState::eViewport );
		}

		if ( m_scissor )
		{
			createInfo.scissor = *m_scissor;
		}
		else
		{
			createInfo.dynamicStates.push_back( renderer::DynamicState::eScissor );
		}

		m_pipelineLayout = getRenderSystem()->getCurrentDevice()->createPipelineLayout( descriptorLayouts
			, m_pushConstantRanges );
		m_pipeline = m_pipelineLayout->createPipeline( createInfo );
	}

	void RenderPipeline::cleanup()
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
	}

	void RenderPipeline::createDescriptorPools( renderer::UInt32Array maxSetsPerLayout )
	{
		REQUIRE( maxSetsPerLayout.size() == m_descriptorLayouts.size() );
		uint32_t index = 0u;

		for ( auto & descriptorLayout : m_descriptorLayouts )
		{
			m_descriptorPools.emplace_back( descriptorLayout->createPool( maxSetsPerLayout[index] ) );
			++index;
		}
	}
}
