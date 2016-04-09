#include "BufferElementGroup.hpp"

#include <cstring>

using namespace Castor;

namespace Castor3D
{
	BufferElementGroup::BufferElementGroup( uint8_t * p_buffer, uint32_t p_index )
		:	m_pBuffer( nullptr )
		,	m_index( p_index )
	{
		LinkCoords( p_buffer );
	}

	BufferElementGroup::~BufferElementGroup()
	{
	}

	void BufferElementGroup::LinkCoords( uint8_t * p_buffer )
	{
		m_pBuffer = p_buffer;
	}

	void BufferElementGroup::LinkCoords( uint8_t * p_buffer, uint32_t p_stride )
	{
		if ( p_buffer )
		{
			if ( m_pBuffer )
			{
				std::memcpy( p_buffer, m_pBuffer, p_stride );
			}
			else
			{
				std::memset( p_buffer, 0, p_stride );
			}
		}

		m_pBuffer = p_buffer;
	}
}
