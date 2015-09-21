#include "BufferElementGroup.hpp"

#include <cstring>

using namespace Castor;

namespace Castor3D
{
	BufferElementGroup::BufferElementGroup( uint8_t * p_pBuffer, uint32_t p_index )
		:	m_pBuffer( NULL )
		,	m_uiIndex( p_index )
	{
		LinkCoords( p_pBuffer );
	}

	BufferElementGroup::~BufferElementGroup()
	{
	}

	void BufferElementGroup::LinkCoords( uint8_t * p_pBuffer )
	{
		m_pBuffer = p_pBuffer;
	}

	void BufferElementGroup::LinkCoords( uint8_t * p_pBuffer, uint32_t p_uiStride )
	{
		if ( p_pBuffer )
		{
			if ( m_pBuffer )
			{
				std::memcpy( p_pBuffer, m_pBuffer, p_uiStride );
			}
			else
			{
				std::memset( p_pBuffer, 0, p_uiStride );
			}
		}

		m_pBuffer = p_pBuffer;
	}
}
