/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlResetQueryPoolCommand.hpp"

namespace gl_renderer
{
	ResetQueryPoolCommand::ResetQueryPoolCommand( renderer::QueryPool const & pool
		, uint32_t firstQuery
		, uint32_t queryCount )
	{
	}

	void ResetQueryPoolCommand::apply()const
	{
	}

	CommandPtr ResetQueryPoolCommand::clone()const
	{
		return std::make_unique< ResetQueryPoolCommand >( *this );
	}
}
