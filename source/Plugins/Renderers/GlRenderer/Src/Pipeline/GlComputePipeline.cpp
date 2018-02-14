#include "Pipeline/GlComputePipeline.hpp"

#include "Core/GlDevice.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace gl_renderer
{
	ComputePipeline::ComputePipeline( Device const & device
		, renderer::PipelineLayout const & layout
		, renderer::ShaderProgram const & program )
		: renderer::ComputePipeline{ device
			, layout
			, program }
		, m_device{ device }
		, m_layout{ layout }
		, m_program{ static_cast< ShaderProgram const & >( program ) }
	{
		m_program.link();
	}
}
