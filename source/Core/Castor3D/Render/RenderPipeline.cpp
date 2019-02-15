#include "RenderPipeline.hpp"

#include "Engine.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Shader/Program.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <Ashes/Descriptor/DescriptorSetLayout.hpp>
#include <Ashes/Pipeline/InputAssemblyState.hpp>
#include <Ashes/Pipeline/Pipeline.hpp>
#include <Ashes/Pipeline/PipelineLayout.hpp>
#include <Ashes/Pipeline/ShaderStageState.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;

namespace castor3d
{
	RenderPipeline::RenderPipeline( RenderSystem & renderSystem
		, ashes::DepthStencilState && dsState
		, ashes::RasterisationState && rsState
		, ashes::ColourBlendState && blState
		, ashes::MultisampleState && msState
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
		//if ( !renderSystem.isTopDown() )
		//{
		//	m_rsState.cullMode = ( ( m_rsState.cullMode == ashes::CullModeFlag::eFront )
		//		? ashes::CullModeFlag::eBack
		//		: ashes::CullModeFlag::eFront );
		//}
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::initialise( ashes::RenderPass const & renderPass )
	{
		ashes::VertexLayoutCRefArray vertexLayouts;

		for ( auto & layout : m_vertexLayouts )
		{
			vertexLayouts.emplace_back( layout );
		}

		ashes::DescriptorSetLayoutCRefArray descriptorLayouts;

		for ( auto & descriptorLayout : m_descriptorLayouts )
		{
			if ( !descriptorLayout->getBindings().empty() )
			{
				descriptorLayouts.emplace_back( *descriptorLayout );
			}
		}

		m_program->initialise();

		ashes::GraphicsPipelineCreateInfo createInfo
		{
			m_program->getStates(),
			renderPass,
			ashes::VertexInputState::create( vertexLayouts ),
			ashes::InputAssemblyState{ m_flags.topology },
			m_rsState,
			m_msState,
			m_blState,
		};
		createInfo.depthStencilState = m_dsState;
		ashes::DynamicStateEnableArray dynamicStates;

		if ( m_viewport )
		{
			createInfo.viewportState.viewports.push_back( *m_viewport );
		}
		else
		{
			dynamicStates.push_back( ashes::DynamicStateEnable::eViewport );
		}

		if ( m_scissor )
		{
			createInfo.viewportState.scissors.push_back( *m_scissor );
		}
		else
		{
			dynamicStates.push_back( ashes::DynamicStateEnable::eScissor );
		}

		if ( !dynamicStates.empty() )
		{
			createInfo.dynamicState = ashes::DynamicState
			{
				0u,
				dynamicStates,
			};
		}

		m_pipelineLayout = getCurrentDevice( *this ).createPipelineLayout( descriptorLayouts
			, m_pushConstantRanges );
		m_pipeline = m_pipelineLayout->createPipeline( createInfo );
	}

	void RenderPipeline::cleanup()
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
	}

	void RenderPipeline::createDescriptorPools( uint32_t maxSets )
	{
		m_descriptorPools.clear();

		for ( auto & descriptorLayout : m_descriptorLayouts )
		{
			if ( !descriptorLayout->getBindings().empty() )
			{
				m_descriptorPools.emplace_back( descriptorLayout->createPool( maxSets ) );
			}
		}
	}

	void RenderPipeline::setVertexLayouts( ashes::VertexLayoutCRefArray const & layouts )
	{
		CU_Require( !m_pipeline );

		for ( auto & layout : layouts )
		{
			m_vertexLayouts.emplace_back( layout.get() );
		}
	}
}
