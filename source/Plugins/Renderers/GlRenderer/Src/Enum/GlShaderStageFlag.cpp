#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlShaderStageFlags value )
	{
		std::string result;
		std::string sep;

		if ( checkFlag( value, gl_renderer::GlShaderStageFlag::GL_SHADER_STAGE_VERTEX ) )
		{
			result += sep + " GL_VERTEX_SHADER";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlShaderStageFlag::GL_SHADER_STAGE_TESS_CONTROL ) )
		{
			result += sep + "GL_TESS_CONTROL_SHADER";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlShaderStageFlag::GL_SHADER_STAGE_TESS_EVALUATION ) )
		{
			result += sep + "GL_TESS_EVALUATION_SHADER";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlShaderStageFlag::GL_SHADER_STAGE_GEOMETRY ) )
		{
			result += sep + "GL_GEOMETRY_SHADER";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlShaderStageFlag::GL_SHADER_STAGE_FRAGMENT ) )
		{
			result += sep + "GL_FRAGMENT_SHADER";
			sep = " | ";
		}

		if ( checkFlag( value, gl_renderer::GlShaderStageFlag::GL_SHADER_STAGE_COMPUTE ) )
		{
			result += sep + "GL_COMPUTE_SHADER";
		}

		return result;
	}

	GlShaderStageFlags convert( renderer::ShaderStageFlags const & flags )
	{
		GLenum result{ 0 };

		if ( checkFlag( flags, renderer::ShaderStageFlag::eVertex ) )
		{
			result |= GL_SHADER_STAGE_VERTEX;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eTessellationControl ) )
		{
			result |= GL_SHADER_STAGE_TESS_CONTROL;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eTessellationEvaluation ) )
		{
			result |= GL_SHADER_STAGE_TESS_EVALUATION;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eGeometry ) )
		{
			result |= GL_SHADER_STAGE_GEOMETRY;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eFragment ) )
		{
			result |= GL_SHADER_STAGE_FRAGMENT;
		}

		if ( checkFlag( flags, renderer::ShaderStageFlag::eCompute ) )
		{
			result |= GL_SHADER_STAGE_COMPUTE;
		}

		return result;
	}

	GlShaderStageFlag convert( renderer::ShaderStageFlag const & flag )
	{
		switch ( flag )
		{
		case renderer::ShaderStageFlag::eVertex:
			return GL_SHADER_STAGE_VERTEX;

		case renderer::ShaderStageFlag::eTessellationControl:
			return GL_SHADER_STAGE_TESS_CONTROL;

		case renderer::ShaderStageFlag::eTessellationEvaluation:
			return GL_SHADER_STAGE_TESS_EVALUATION;

		case renderer::ShaderStageFlag::eGeometry:
			return GL_SHADER_STAGE_GEOMETRY;

		case renderer::ShaderStageFlag::eFragment:
			return GL_SHADER_STAGE_FRAGMENT;

		case renderer::ShaderStageFlag::eCompute:
			return GL_SHADER_STAGE_COMPUTE;

		default:
			assert( false && "Unsupported shader stage." );
			return GL_SHADER_STAGE_VERTEX;
		}
	}
}
