/*
This file belongs to GlRenderer.
See LICENSE file in root folder.
*/
#include "GlDrawIndexedCommand.hpp"

namespace gl_renderer
{
	DrawIndexedCommand::DrawIndexedCommand( uint32_t indexCount
		, uint32_t instCount
		, uint32_t firstIndex
		, uint32_t vertexOffset
		, uint32_t firstInstance
		, renderer::PrimitiveTopology mode
		, renderer::IndexType type )
		: m_indexCount{ indexCount }
		, m_instCount{ instCount }
		, m_firstIndex{ firstIndex }
		, m_vertexOffset{ vertexOffset }
		, m_firstInstance{ firstInstance }
		, m_mode{ convert( mode ) }
		, m_type{ convert( type ) }
	{
	}

	void DrawIndexedCommand::apply()const
	{
		if ( m_instCount > 1 )
		{
			glLogCall( gl::DrawElementsInstancedBaseInstance
				, m_mode
				, m_indexCount
				, m_type
				, ( ( GLvoid * )m_firstIndex )
				, m_instCount
				, m_firstInstance );
		}
		else
		{
			glLogCall( gl::DrawElementsBaseVertex
				, m_mode
				, m_indexCount
				, m_type
				, ( ( GLvoid * )m_firstIndex )
				, m_vertexOffset );
		}
	}

	CommandPtr DrawIndexedCommand::clone()const
	{
		return std::make_unique< DrawIndexedCommand >( *this );
	}
}
