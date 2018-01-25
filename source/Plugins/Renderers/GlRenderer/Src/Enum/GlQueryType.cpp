#include "GlRendererPrerequisites.hpp"

namespace gl_renderer
{
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
