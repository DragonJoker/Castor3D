#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
	std::string getName( GlQueryType value )
	{
		switch ( value )
		{
		case gl_renderer::GL_QUERY_TYPE_SAMPLES_PASSED:
			return "GL_SAMPLES_PASSED";

		case gl_renderer::GL_QUERY_TYPE_TIMESTAMP:
			return "GL_TIMESTAMP";

		case gl_renderer::GL_QUERY_TYPE_PRIMITIVES_GENERATED:
			return "GL_PRIMITIVES_GENERATED";

		default:
			assert( false && "Unsupported GlQueryType" );
			return "GlQueryType_UNKNOWN";
		}
	}

	GlQueryType convert( renderer::QueryType const & value )
	{
		switch ( value )
		{
		case renderer::QueryType::eOcclusion:
			return GL_QUERY_TYPE_SAMPLES_PASSED;

		case renderer::QueryType::eTimestamp:
			return GL_QUERY_TYPE_TIMESTAMP;

		case renderer::QueryType::ePipelineStatistics:
			return GL_QUERY_TYPE_PRIMITIVES_GENERATED;

		default:
			assert( false && "Unsupported query type" );
			return GL_QUERY_TYPE_TIMESTAMP;
		}
	}
}
