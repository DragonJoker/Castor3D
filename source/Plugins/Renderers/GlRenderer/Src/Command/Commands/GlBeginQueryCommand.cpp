/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlBeginQueryCommand.hpp"

#include "Miscellaneous/GlQueryPool.hpp"

namespace gl_renderer
{
	BeginQueryCommand::BeginQueryCommand( renderer::QueryPool const & pool
		, uint32_t query
		, renderer::QueryControlFlags flags )
		: m_target{ convert( pool.getType() ) }
		, m_query{ *( static_cast< QueryPool const & >( pool ).begin() + query ) }
	{
	}

	void BeginQueryCommand::apply()const
	{
		glLogCommand( "BeginQueryCommand" );
		glLogCall( gl::BeginQuery, m_target, m_query );
	}

	CommandPtr BeginQueryCommand::clone()const
	{
		return std::make_unique< BeginQueryCommand >( *this );
	}
}
