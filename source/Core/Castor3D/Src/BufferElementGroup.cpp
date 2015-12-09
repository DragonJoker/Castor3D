#include "BufferElementGroup.hpp"

#include <cstring>

using namespace Castor;

namespace Castor3D
{
	BufferElementGroup::BufferElementGroup( uint8_t * p_buffer, uint32_t p_index )
		:	m_pBuffer( NULL )
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

	void BufferElementGroup::LinkCoords( uint8_t * p_buffer, uint32_t p_uiStride )
	{
		if ( p_buffer )
		{
			if ( m_pBuffer )
			{
				std::memcpy( p_buffer, m_pBuffer, p_uiStride );
			}
			else
			{
				std::memset( p_buffer, 0, p_uiStride );
			}
		}

		m_pBuffer = p_buffer;
	}
}
