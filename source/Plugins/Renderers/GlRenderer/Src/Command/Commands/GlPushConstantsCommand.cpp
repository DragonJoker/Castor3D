/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlPushConstantsCommand.hpp"

#include "Buffer/PushConstantsBuffer.hpp"

namespace gl_renderer
{
	PushConstantsCommand::PushConstantsCommand( renderer::PipelineLayout const & layout
		, renderer::PushConstantsBuffer const & pcb )
		: m_pcb{ pcb }
		, m_data{ pcb.getData(), pcb.getData() + pcb.getSize() }
	{
	}

	void PushConstantsCommand::apply()const
	{
		auto buffer = m_data.data();

		for ( auto & constant : m_pcb )
		{
			switch ( constant.format )
			{
			case renderer::AttributeFormat::eFloat:
				glLogCall( gl::Uniform1fv, constant.location, 1u, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec2f:
				glLogCall( gl::Uniform2fv, constant.location, 1u, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec3f:
				glLogCall( gl::Uniform3fv, constant.location, 1u, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec4f:
				glLogCall( gl::Uniform4fv, constant.location, 1u, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eMat2f:
				glLogCall( gl::UniformMatrix2fv, constant.location, 1u, GL_FALSE, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eMat3f:
				glLogCall( gl::UniformMatrix3fv, constant.location, 1u, GL_FALSE, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eMat4f:
				glLogCall( gl::UniformMatrix4fv, constant.location, 1u, GL_FALSE, reinterpret_cast< GLfloat const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eInt:
				glLogCall( gl::Uniform1iv, constant.location, 1u, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec2i:
				glLogCall( gl::Uniform2iv, constant.location, 1u, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec3i:
				glLogCall( gl::Uniform3iv, constant.location, 1u, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec4i:
				glLogCall( gl::Uniform4iv, constant.location, 1u, reinterpret_cast< GLint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eUInt:
				glLogCall( gl::Uniform1uiv, constant.location, 1u, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec2ui:
				glLogCall( gl::Uniform2uiv, constant.location, 1u, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec3ui:
				glLogCall( gl::Uniform3uiv, constant.location, 1u, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eVec4ui:
				glLogCall( gl::Uniform4uiv, constant.location, 1u, reinterpret_cast< GLuint const * >( buffer ) );
				break;

			case renderer::AttributeFormat::eColour:
				glLogCall( gl::Uniform4fv, constant.location, 1u, reinterpret_cast< GLfloat const * >( buffer ) );
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
