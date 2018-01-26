#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlSamplerParameter value )
	{
		switch ( value )
		{
		case gl_renderer::GL_SAMPLER_PARAMETER_BORDER_COLOR:
			return "GL_TEXTURE_BORDER_COLOR";

		case gl_renderer::GL_SAMPLER_PARAMETER_MAG_FILTER:
			return "GL_TEXTURE_MAG_FILTER";

		case gl_renderer::GL_SAMPLER_PARAMETER_MIN_FILTER:
			return "GL_TEXTURE_MIN_FILTER";

		case gl_renderer::GL_SAMPLER_PARAMETER_WRAP_S:
			return "GL_TEXTURE_WRAP_S";

		case gl_renderer::GL_SAMPLER_PARAMETER_WRAP_T:
			return "GL_TEXTURE_WRAP_T";

		case gl_renderer::GL_SAMPLER_PARAMETER_WRAP_R:
			return "GL_TEXTURE_WRAP_R";

		case gl_renderer::GL_SAMPLER_PARAMETER_MIN_LOD:
			return "GL_TEXTURE_MIN_LOD";

		case gl_renderer::GL_SAMPLER_PARAMETER_MAX_LOD:
			return "GL_TEXTURE_MAX_LOD";

		case gl_renderer::GL_SAMPLER_PARAMETER_COMPARE_MODE:
			return "GL_TEXTURE_COMPARE_MODE";

		case gl_renderer::GL_SAMPLER_PARAMETER_COMPARE_FUNC:
			return "GL_TEXTURE_COMPARE_FUNC";

		case gl_renderer::GL_SAMPLER_PARAMETER_COMPARE_REF_TO_TEXTURE:
			return "GL_COMPARE_REF_TO_TEXTURE";

		default:
			assert( false && "Unsupported GlSamplerParameter" );
			return "GlSamplerParameter_UNKNOWN";
		}
	}
}
