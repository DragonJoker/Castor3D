#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GLsizei convert( renderer::SampleCountFlag const & flags )
	{
		switch ( flags )
		{
		case renderer::SampleCountFlag::e1:
			return 1;

		case renderer::SampleCountFlag::e2:
			return 2;

		case renderer::SampleCountFlag::e4:
			return 4;

		case renderer::SampleCountFlag::e8:
			return 8;

		case renderer::SampleCountFlag::e16:
			return 16;

		case renderer::SampleCountFlag::e32:
			return 32;

		case renderer::SampleCountFlag::e64:
			return 64;

		default:
			assert( false && "Unsupported sample count flag." );
			return 1;
		}
	}
}
