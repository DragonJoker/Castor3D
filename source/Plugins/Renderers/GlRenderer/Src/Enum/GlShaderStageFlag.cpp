#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
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
