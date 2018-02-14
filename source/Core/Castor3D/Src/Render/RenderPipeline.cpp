#include "RenderPipeline.hpp"

#include "Engine.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslShadow.hpp"

#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>

#include <GlslSource.hpp>

using namespace castor;

namespace castor3d
{
	RenderPipeline::RenderPipeline( RenderSystem & renderSystem
		, renderer::DepthStencilState && dsState
		, renderer::RasterisationState && rsState
		, renderer::ColourBlendState && blState
		, renderer::MultisampleState && msState
		, renderer::ShaderProgram & program
		, PipelineFlags const & flags )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_dsState{ std::move( dsState ) }
		, m_rsState{ std::move( rsState ) }
		, m_blState{ std::move( blState ) }
		, m_msState{ std::move( msState ) }
		, m_program{ program }
		, m_flags( flags )
	{
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::cleanup()
	{
		m_pipeline.reset();
		m_pipelineLayout.reset();
	}

	void RenderPipeline::initialise( renderer::RenderPass const & renderPass
		, renderer::PrimitiveTopology topology )
	{
		m_pipelineLayout = getRenderSystem()->getCurrentDevice()->createPipelineLayout( m_descriptorLayouts
			, m_pushConstantRanges );
		m_pipeline = m_pipelineLayout->createPipeline( m_program
			, m_vertexLayouts
			, renderPass
			, topology
			, m_rsState
			, m_blState );
		m_pipeline->depthStencilState( m_dsState );
		m_pipeline->multisampleState( m_msState );
		m_pipeline->finish();
	}
}
