/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlSetDepthBiasCommand.hpp"

namespace gl_renderer
{
	SetDepthBiasCommand::SetDepthBiasCommand( float constantFactor
		, float clamp
		, float slopeFactor )
		: m_constantFactor{ constantFactor }
		, m_clamp{ clamp }
		, m_slopeFactor{ slopeFactor }
	{
	}

	void SetDepthBiasCommand::apply()const
	{
		glLogCommand( "SetDepthBiasCommand" );
		glLogCall( gl::PolygonOffsetClampEXT, m_slopeFactor, m_constantFactor, m_clamp );
	}

	CommandPtr SetDepthBiasCommand::clone()const
	{
		return std::make_unique< SetDepthBiasCommand >( *this );
	}
}
