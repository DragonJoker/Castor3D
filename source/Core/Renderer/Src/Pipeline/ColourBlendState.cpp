/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "Pipeline/ColourBlendState.hpp"

namespace renderer
{
	namespace
	{
		ColourBlendState doCreateDefault()
		{
			ColourBlendState result;
			result.addAttachment( ColourBlendStateAttachment{} );
			return result;
		}
	}

	ColourBlendState::ColourBlendState( bool logicOpEnable
		, LogicOp logicOp
		, Vec4 const & blendConstants )
		: m_logicOpEnable{ logicOpEnable }
		, m_logicOp{ logicOp }
		, m_blendConstants{ blendConstants }
		, m_hash
		{ uint8_t(
			( uint8_t( m_logicOpEnable ) << 7 )
			| ( uint8_t( m_logicOp ) << 2 )
		) }
	{
	}

	ColourBlendState const & ColourBlendState::createDefault()
	{
		static ColourBlendState const defaultValue = doCreateDefault();
		return defaultValue;
	}

	void ColourBlendState::addAttachment( ColourBlendStateAttachment const & attachment )
	{
		m_attachs.push_back( attachment );
	}
}
