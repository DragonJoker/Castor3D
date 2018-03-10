/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlPushConstantsCommand.hpp"

#include "Buffer/PushConstantsBuffer.hpp"

namespace gl_renderer
{
	PushConstantsCommand::PushConstantsCommand( renderer::PipelineLayout const & layout
		, renderer::PushConstantsBufferBase const & pcb )
		: m_pcb{ pcb }
		, m_data{ pcb.getData(), pcb.getData() + pcb.getSize() }
	{
	}

	void PushConstantsCommand::apply()const
	{
		glLogCommand( "PushConstantsCommand" );
		auto buffer = m_data.data();

		for ( auto & constant : m_pcb )
		{
			switch ( constant.format )
			{
			case renderer::ConstantFormat::eFloat:
				glLogCall( gl::Uniform1fv, constant.location, constant.arraySize, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec2f:
				glLogCall( gl::Uniform2fv, constant.location, constant.arraySize, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec3f:
				glLogCall( gl::Uniform3fv, constant.location, constant.arraySize, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec4f:
				glLogCall( gl::Uniform4fv, constant.location, constant.arraySize, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eMat2f:
				glLogCall( gl::UniformMatrix2fv, constant.location, constant.arraySize, GL_FALSE, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eMat3f:
				glLogCall( gl::UniformMatrix3fv, constant.location, constant.arraySize, GL_FALSE, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eMat4f:
				glLogCall( gl::UniformMatrix4fv, constant.location, constant.arraySize, GL_FALSE, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eInt:
				glLogCall( gl::Uniform1iv, constant.location, constant.arraySize, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec2i:
				glLogCall( gl::Uniform2iv, constant.location, constant.arraySize, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec3i:
				glLogCall( gl::Uniform3iv, constant.location, constant.arraySize, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec4i:
				glLogCall( gl::Uniform4iv, constant.location, constant.arraySize, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eUInt:
				glLogCall( gl::Uniform1uiv, constant.location, constant.arraySize, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec2ui:
				glLogCall( gl::Uniform2uiv, constant.location, constant.arraySize, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec3ui:
				glLogCall( gl::Uniform3uiv, constant.location, constant.arraySize, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eVec4ui:
				glLogCall( gl::Uniform4uiv, constant.location, constant.arraySize, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::ConstantFormat::eColour:
				glLogCall( gl::Uniform4fv, constant.location, constant.arraySize, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			default:
				assert( false && "Unsupported constant format" );
				break;
			}

			buffer += getSize( constant.format );
		}
	}

	CommandPtr PushConstantsCommand::clone()const
	{
		return std::make_unique< PushConstantsCommand >( *this );
	}
}
