#include "Pipeline/GlPipeline.hpp"

#include "Command/Commands/GlBindPipelineCommand.hpp"
#include "Core/GlDevice.hpp"
#include "Core/GlRenderer.hpp"
#include "Core/GlRenderingResources.hpp"
#include "Miscellaneous/GlValidator.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "Pipeline/GlVertexLayout.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Shader/GlShaderProgram.hpp"

#if defined( interface )
#	undef interface
#endif

namespace gl_renderer
{
	Pipeline::Pipeline( Device const & device
		, PipelineLayout const & layout
		, renderer::ShaderProgram const & program
		, renderer::VertexLayoutCRefArray const & vertexLayouts
		, renderer::RenderPass const & renderPass
		, renderer::InputAssemblyState const & inputAssemblyState
		, renderer::RasterisationState const & rasterisationState
		, renderer::ColourBlendState const & colourBlendState )
		: renderer::Pipeline{ device
			, layout
			, program
			, vertexLayouts
			, renderPass
			, inputAssemblyState
			, rasterisationState
			, colourBlendState }
		, m_device{ device }
		, m_layout{ layout }
		, m_program{ static_cast< ShaderProgram const & >( program ) }
		, m_vertexLayouts{ vertexLayouts }
		, m_renderPass{ renderPass }
		, m_iaState{ inputAssemblyState }
		, m_cbState{ colourBlendState }
		, m_rsState{ rasterisationState }
		, m_dsState{ 0u, false, true, renderer::CompareOp::eLess,  }
	{
	}

	renderer::Pipeline & Pipeline::finish()
	{
		apply( m_device, m_cbState );
		apply( m_device, m_rsState );
		apply( m_device, m_dsState );
		apply( m_device, m_msState );
		apply( m_device, m_tsState );
		m_program.link();

		if ( m_device.getRenderer().isValidationEnabled() )
		{
			validatePipeline( m_layout
				, m_program
				, m_vertexLayouts
				, m_renderPass );
		}

		return *this;
	}

	renderer::Pipeline & Pipeline::multisampleState( renderer::MultisampleState const & state )
	{
		m_msState = state;
		return *this;
	}

	renderer::Pipeline & Pipeline::depthStencilState( renderer::DepthStencilState const & state )
	{
		m_dsState = state;
		return *this;
	}

	renderer::Pipeline & Pipeline::tessellationState( renderer::TessellationState const & state )
	{
		m_tsState = state;
		return *this;
	}

	renderer::Pipeline & Pipeline::viewport( renderer::Viewport const & viewport )
	{
		m_viewport = std::make_unique< renderer::Viewport >( viewport );
		return *this;
	}

	renderer::Pipeline & Pipeline::scissor( renderer::Scissor const & scissor )
	{
		m_scissor = std::make_unique< renderer::Scissor >( scissor );
		return *this;
	}
}
