#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	GLuint convert( renderer::QueryResultFlags const & flags )
	{
		GLuint result{ 0 };

		if ( checkFlag( flags, renderer::QueryResultFlag::eWait ) )
		{
			return GL_QUERY_RESULT;
		}

		if ( checkFlag( flags, renderer::QueryResultFlag::eWithAvailability ) )
		{
			return GL_QUERY_RESULT_NO_WAIT;
		}

		return result;
	}
}
