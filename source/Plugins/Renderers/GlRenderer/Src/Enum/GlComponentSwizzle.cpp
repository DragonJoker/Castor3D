#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlSwizzle value )
	{
		switch ( value )
		{
		case GL_SWIZZLE_R:
			return "GL_TEXTURE_SWIZZLE_R";

		case GL_SWIZZLE_G:
			return "GL_TEXTURE_SWIZZLE_G";

		case GL_SWIZZLE_B:
			return "GL_TEXTURE_SWIZZLE_B";

		case GL_SWIZZLE_A:
			return "GL_TEXTURE_SWIZZLE_A";

		default:
			assert( false && "Unsupported GlSwizzle." );
			return "GlSwizzle_UNKNOWN";
		}
	}

	std::string getName( GlComponentSwizzle value )
	{
		switch ( value )
		{
		case GL_COMPONENT_SWIZZLE_ZERO:
			return "GL_ZERO";

		case GL_COMPONENT_SWIZZLE_ONE:
			return "GL_ONE";

		case GL_COMPONENT_SWIZZLE_RED:
			return "GL_RED";

		case GL_COMPONENT_SWIZZLE_GREEN:
			return "GL_GREEN";

		case GL_COMPONENT_SWIZZLE_BLUE:
			return "GL_BLUE";

		case GL_COMPONENT_SWIZZLE_ALPHA:
			return "GL_ALPHA";

		default:
			assert( false && "Unsupported GlComponentSwizzle." );
			return "GlComponentSwizzle_UNKNOWN";
		}
	}

	GlComponentSwizzle convert( renderer::ComponentSwizzle const & value )
	{
		switch ( value )
		{
		case renderer::ComponentSwizzle::eIdentity:
			return GL_COMPONENT_SWIZZLE_IDENTITTY;

		case renderer::ComponentSwizzle::eZero:
			return GL_COMPONENT_SWIZZLE_ZERO;

		case renderer::ComponentSwizzle::eOne:
			return GL_COMPONENT_SWIZZLE_ONE;

		case renderer::ComponentSwizzle::eR:
			return GL_COMPONENT_SWIZZLE_RED;

		case renderer::ComponentSwizzle::eG:
			return GL_COMPONENT_SWIZZLE_GREEN;

		case renderer::ComponentSwizzle::eB:
			return GL_COMPONENT_SWIZZLE_BLUE;

		case renderer::ComponentSwizzle::eA:
			return GL_COMPONENT_SWIZZLE_ALPHA;

		default:
			assert( false && "Unsupported ComponentSwizzle." );
			return GL_COMPONENT_SWIZZLE_IDENTITTY;
		}
	}
}
