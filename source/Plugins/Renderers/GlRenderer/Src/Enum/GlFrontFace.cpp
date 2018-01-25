#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GlFrontFace convert( renderer::FrontFace const & value )
	{
		switch ( value )
		{
		case renderer::FrontFace::eCounterClockwise:
			return GL_FRONT_FACE_CW;

		case renderer::FrontFace::eClockwise:
			return GL_FRONT_FACE_CCW;

		default:
			assert( false && "Unsupported front face mode");
			return GL_FRONT_FACE_CCW;
		}
	}
}
