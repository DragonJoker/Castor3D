#include "PrecompiledHeader.h"

#include "render_system/Buffer.h"

using namespace Castor3D;

//***********************************************************************************************

Buffer3D :: Buffer3D()
	:	m_bAssigned( false),
		m_bToDelete( false)
{
	BufferManager::GetSingleton().AddBuffer( this);
}

void Buffer3D :: Negate( unsigned int p_index)
{
	assert( p_index < m_totalSize);

	m_buffer[p_index] = -m_buffer[p_index];
}

//***********************************************************************************************

void BufferManager :: Cleanup()
{
	for (size_t i = 0 ; i < m_arrayBuffers.size() ; i++)
	{
		m_arrayBuffersToDelete.push_back( m_arrayBuffers[i]);
	}

	m_arrayBuffers.clear();
	m_mapBuffersIndexes.clear();
	m_mapBuffers.clear();
}

bool BufferManager :: AddBuffer( Buffer3D * p_pBuffer)
{
	bool l_bReturn = false;

	assert( p_pBuffer != NULL);

	if (m_mapBuffersIndexes.find( p_pBuffer) == m_mapBuffersIndexes.end())
	{
		m_arrayBuffers.push_back( p_pBuffer);
		l_bReturn = true;
	}

	return l_bReturn;
}

bool BufferManager :: AssignBuffer( size_t p_uiID, Buffer3D * p_pBuffer)
{
	bool l_bReturn = false;

	assert (p_pBuffer != NULL && p_uiID != 0);

	if (m_mapBuffers.find( p_uiID) == m_mapBuffers.end())
	{
		for (size_t i = 0 ; i < m_arrayBuffers.size() && ! l_bReturn ; i++)
		{
			if (m_arrayBuffers[i] == p_pBuffer)
			{
				m_mapBuffers.insert( std::map <size_t, Buffer3D *>::value_type( p_uiID, p_pBuffer));
				m_mapBuffersIndexes.insert( std::map <Buffer3D *, size_t>::value_type( p_pBuffer, p_uiID));
				l_bReturn = true;
			}
		}
	}

	return l_bReturn;
}

bool BufferManager :: DeleteBuffer( Buffer3D * p_pBuffer)
{
	bool l_bReturn = false;

	assert( p_pBuffer != NULL);

	std::map <Buffer3D *, size_t>::iterator l_it = m_mapBuffersIndexes.find( p_pBuffer);

	if (l_it != m_mapBuffersIndexes.end())
	{	
		vector::eraseValue( m_arrayBuffers, p_pBuffer);

		if (p_pBuffer->IsAssigned() && m_mapBuffers.find( l_it->second) != m_mapBuffers.end())
		{
			m_mapBuffers.erase( m_mapBuffers.find( l_it->second));
		}

		m_arrayBuffersToDelete.push_back( p_pBuffer);
	}

	return l_bReturn;
}

void BufferManager :: Update()
{
	for (size_t i = 0 ; i < m_arrayBuffersToDelete.size() ; i++)
	{
		delete m_arrayBuffersToDelete[i];
	}

	m_arrayBuffersToDelete.clear();
}

//***********************************************************************************************