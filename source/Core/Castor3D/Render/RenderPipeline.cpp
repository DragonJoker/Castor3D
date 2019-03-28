#include "Castor3D/Render/RenderPipeline.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"

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

		for ( auto & layout : m_descriptorLayouts )
		{
			auto & bindings = layout->getBindings();
			auto it = std::find_if( bindings.begin()
				, bindings.end()
				, []( ashes::DescriptorSetLayoutBinding const & lookup )
				{
					return lookup.getDescriptorsCount() == 0u;
				} );

			if ( it == bindings.end()
				&& !layout->getBindings().empty() )
			{
				m_descriptorPools.emplace_back( layout->createPool( maxSets ) );
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

	void RenderPipeline::setDescriptorSetLayouts( std::vector< ashes::DescriptorSetLayoutPtr > && layouts )
	{
		CU_Require( !m_pipeline );
		m_descriptorLayouts = std::move( layouts );
	}
}
