/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlDispatchCommand.hpp"

namespace gl_renderer
{
	DispatchCommand::DispatchCommand( uint32_t groupCountX
		, uint32_t groupCountY
		, uint32_t groupCountZ )
		: m_groupCountX{ groupCountX }
		, m_groupCountY{ groupCountY }
		, m_groupCountZ{ groupCountZ }
	{
	}

	void DispatchCommand::apply()const
	{
		glLogCommand( "DispatchCommand" );
		glLogCall( gl::DispatchCompute
			, m_groupCountX
			, m_groupCountY
			, m_groupCountZ );
	}

	CommandPtr DispatchCommand::clone()const
	{
		return std::make_unique< DispatchCommand >( *this );
	}
}
