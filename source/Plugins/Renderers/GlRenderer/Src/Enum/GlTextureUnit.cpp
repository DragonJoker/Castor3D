#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlTextureUnit value )
	{
		switch ( value )
		{
		case gl_renderer::GL_TEXTURE0:
			return "GL_TEXTURE0";

		case gl_renderer::GL_TEXTURE1:
			return "GL_TEXTURE1";

		case gl_renderer::GL_TEXTURE2:
			return "GL_TEXTURE2";

		case gl_renderer::GL_TEXTURE3:
			return "GL_TEXTURE3";

		case gl_renderer::GL_TEXTURE4:
			return "GL_TEXTURE4";

		case gl_renderer::GL_TEXTURE5:
			return "GL_TEXTURE5";

		case gl_renderer::GL_TEXTURE6:
			return "GL_TEXTURE6";

		case gl_renderer::GL_TEXTURE7:
			return "GL_TEXTURE7";

		case gl_renderer::GL_TEXTURE8:
			return "GL_TEXTURE8";

		case gl_renderer::GL_TEXTURE9:
			return "GL_TEXTURE9";

		case gl_renderer::GL_TEXTURE10:
			return "GL_TEXTURE10";

		case gl_renderer::GL_TEXTURE11:
			return "GL_TEXTURE11";

		case gl_renderer::GL_TEXTURE12:
			return "GL_TEXTURE12";

		case gl_renderer::GL_TEXTURE13:
			return "GL_TEXTURE13";

		case gl_renderer::GL_TEXTURE14:
			return "GL_TEXTURE14";

		case gl_renderer::GL_TEXTURE15:
			return "GL_TEXTURE15";

		case gl_renderer::GL_TEXTURE16:
			return "GL_TEXTURE16";

		case gl_renderer::GL_TEXTURE17:
			return "GL_TEXTURE17";

		case gl_renderer::GL_TEXTURE18:
			return "GL_TEXTURE18";

		case gl_renderer::GL_TEXTURE19:
			return "GL_TEXTURE19";

		case gl_renderer::GL_TEXTURE20:
			return "GL_TEXTURE20";

		case gl_renderer::GL_TEXTURE21:
			return "GL_TEXTURE21";

		case gl_renderer::GL_TEXTURE22:
			return "GL_TEXTURE22";

		case gl_renderer::GL_TEXTURE23:
			return "GL_TEXTURE23";

		case gl_renderer::GL_TEXTURE24:
			return "GL_TEXTURE24";

		case gl_renderer::GL_TEXTURE25:
			return "GL_TEXTURE25";

		case gl_renderer::GL_TEXTURE26:
			return "GL_TEXTURE26";

		case gl_renderer::GL_TEXTURE27:
			return "GL_TEXTURE27";

		case gl_renderer::GL_TEXTURE28:
			return "GL_TEXTURE28";

		case gl_renderer::GL_TEXTURE29:
			return "GL_TEXTURE29";

		case gl_renderer::GL_TEXTURE30:
			return "GL_TEXTURE30";

		case gl_renderer::GL_TEXTURE31:
			return "GL_TEXTURE31";

		default:
			assert( false && "Unsupported GlTextureUnit" );
			return "GlTextureUnit_UNKNOWN";
		}
	}
}
