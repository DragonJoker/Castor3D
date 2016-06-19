#include "BufferElementGroup.hpp"

#include <cstring>

using namespace Castor;

namespace Castor3D
{
	BufferElementGroup::BufferElementGroup( uint8_t * p_buffer, uint32_t p_index )
		:	m_buffer( nullptr )
		,	m_index( p_index )
	{
		LinkCoords( p_buffer );
	}

	BufferElementGroup::~BufferElementGroup()
	{
	}

	void BufferElementGroup::LinkCoords( uint8_t * p_buffer )
	{
		m_buffer = p_buffer;
	}

	void BufferElementGroup::LinkCoords( uint8_t * p_buffer, uint32_t p_stride )
	{
		if ( p_buffer )
		{
			if ( m_buffer )
			{
				std::memcpy( p_buffer, m_buffer, p_stride );
			}
			else
			{
				std::memset( p_buffer, 0, p_stride );
			}
		}

		m_buffer = p_buffer;
	}
}
