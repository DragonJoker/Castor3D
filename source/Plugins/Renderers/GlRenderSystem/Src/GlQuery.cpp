#include "GlQuery.hpp"

#include "OpenGl.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlQuery::GlQuery( OpenGl & p_gl, eGL_QUERY p_query )
		: Object( p_gl,
				  "GlQuery",
				  std::bind( &OpenGl::GenQueries, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
				  std::bind( &OpenGl::DeleteQueries, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
				  std::bind( &OpenGl::IsQuery, std::ref( p_gl ), std::placeholders::_1 )
				  )
		, m_target( p_query )
	{
	}

	GlQuery::~GlQuery()
	{
	}

	bool GlQuery::Begin()
	{
		return GetOpenGl().BeginQuery( m_target, GetGlName() );
	}

	void GlQuery::End()
	{
		GetOpenGl().EndQuery( m_target );
	}
}
