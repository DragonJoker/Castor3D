/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBindComputePipelineCommand.hpp"

#include "Core/GlDevice.hpp"
#include "Pipeline/GlComputePipeline.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "Shader/GlShaderProgram.hpp"

namespace gl_renderer
{
	BindComputePipelineCommand::BindComputePipelineCommand( Device const & device
		, renderer::ComputePipeline const & pipeline
		, renderer::PipelineBindPoint bindingPoint )
		: m_device{ device }
		, m_pipeline{ static_cast< ComputePipeline const & > ( pipeline ) }
		, m_layout{ static_cast< PipelineLayout const & > ( m_pipeline.getLayout() ) }
		, m_program{ static_cast< ShaderProgram const & > ( m_pipeline.getProgram() ) }
		, m_bindingPoint{ bindingPoint }
	{
	}

	void BindComputePipelineCommand::apply()const
	{
		glLogCommand( "BindComputePipelineCommand" );
		auto & save = m_device.getCurrentProgram();

		if ( m_program.getProgram() != save )
		{
			glLogCall( gl::UseProgram, m_program.getProgram() );
			save = m_program.getProgram();
		}
	}

	CommandPtr BindComputePipelineCommand::clone()const
	{
		return std::make_unique< BindComputePipelineCommand >( *this );
	}
}
