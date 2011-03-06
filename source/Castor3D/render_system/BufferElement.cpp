#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/render_system/BufferElement.h"

using namespace Castor3D;

//*************************************************************************************************

BufferDeclaration :: BufferDeclaration( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
{
	_initialise( p_pElements, p_uiNbElements);
}

BufferDeclaration :: ~BufferDeclaration()
{
}

void BufferDeclaration :: _initialise( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
{
	static const unsigned int s_uiSize[] = {4, 8, 12, 16, 4};
	m_uiStride = 0;

	for (size_t i = 0 ; i < p_uiNbElements ; i++)
	{
		m_arrayElements.push_back( p_pElements[i]);
		m_arrayElements[i].m_uiOffset = m_uiStride;
		m_uiStride += s_uiSize[m_arrayElements[i].m_eDataType];
	}
}

//*************************************************************************************************

BufferElement :: BufferElement( BufferDeclaration * p_pDeclaration)
	:	m_bOwnBuffer( false)
	,	m_pBuffer( NULL)
	,	m_uiStride( 0)
	,	m_pDeclaration( p_pDeclaration)
{
	if (m_pDeclaration->Size() > 0)
	{
		size_t i = 0;

		for (BufferDeclaration::const_iterator l_it = m_pDeclaration->Begin() ; l_it != m_pDeclaration->End() ; ++l_it)
		{
			switch (l_it->m_eDataType)
			{
			case eType1Float:
				m_arrayElements.push_back( new Point<real, 1>());
				break;

			case eType2Floats:
				m_arrayElements.push_back( new Point<real, 2>());
				break;

			case eType3Floats:
				m_arrayElements.push_back( new Point<real, 3>());
				break;

			case eTypeColour:
				m_arrayElements.push_back( new Point<unsigned char, 4>());
				break;
			}

			m_uiStride += m_arrayElements[i]->GetElementSize() * m_arrayElements[i]->GetElementCount();
			i++;
		}
	}

	UnlinkCoords();
}

BufferElement :: BufferElement( const BufferElement & p_element)
	:	m_bOwnBuffer( false)
	,	m_pBuffer( NULL)
	,	m_uiStride( p_element.m_uiStride)
	,	m_pDeclaration( p_element.m_pDeclaration)
{
	if (m_pDeclaration->Size() > 0)
	{
		size_t i = 0;

		for (BufferDeclaration::const_iterator l_it = m_pDeclaration->Begin() ; l_it != m_pDeclaration->End() ; ++l_it)
		{
			switch (l_it->m_eDataType)
			{
			case eType1Float:
				m_arrayElements.push_back( new Point<real, 1>( p_element.GetElement< Point<real, 1> >( i)));
				break;

			case eType2Floats:
				m_arrayElements.push_back( new Point<real, 2>( p_element.GetElement< Point<real, 2> >( i)));
				break;

			case eType3Floats:
				m_arrayElements.push_back( new Point<real, 3>( p_element.GetElement< Point<real, 3> >( i)));
				break;

			case eTypeColour:
				m_arrayElements.push_back( new Point<unsigned char, 4>( p_element.GetElement< Point<unsigned char, 4> >( i)));
				break;
			}

			i++;
		}
	}

	UnlinkCoords();
}

BufferElement :: ~BufferElement()
{
	for (size_t i = 0 ; i < m_arrayElements.size() ; i++)
	{
		delete m_arrayElements[i];
	}

	m_arrayElements.clear();

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}

BufferElement & BufferElement :: operator =( const BufferElement & p_element)
{
	m_pDeclaration = p_element.m_pDeclaration;

	if (m_pDeclaration->Size() > 0)
	{
		size_t i = 0;

		for (BufferDeclaration::const_iterator l_it = m_pDeclaration->Begin() ; l_it != m_pDeclaration->End() ; ++l_it)
		{
			m_arrayElements[i]->UnlinkCoords();

			switch (l_it->m_eDataType)
			{
			case eType1Float:
				(*m_arrayElements[i]) = p_element.GetElement< Point<real, 1> >( i);
				break;

			case eType2Floats:
				(*m_arrayElements[i]) = p_element.GetElement< Point<real, 2> >( i);
				break;

			case eType3Floats:
				(*m_arrayElements[i]) = p_element.GetElement< Point<real, 3> >( i);
				break;

			case eTypeColour:
				(*m_arrayElements[i]) = p_element.GetElement< Point<unsigned char, 4> >( i);
				break;
			}

			i++;
		}
	}

	m_uiStride = p_element.m_uiStride;

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_bOwnBuffer = false;
	UnlinkCoords();

	return * this;
}

void BufferElement :: LinkCoords( void * p_pBuffer)
{
	unsigned char * l_pBufferSave = new unsigned char[m_uiStride];

	if (m_pBuffer != NULL)
	{
		memcpy( l_pBufferSave, m_pBuffer, m_uiStride);
	}
	else
	{
		size_t l_uiOffset = 0;

		for (size_t i = 0 ; i < m_arrayElements.size() ; i++)
		{
			memcpy( & l_pBufferSave[l_uiOffset], m_arrayElements[i]->const_ptr<unsigned char>(), m_arrayElements[i]->GetElementSize() * m_arrayElements[i]->GetElementCount());
			l_uiOffset += m_arrayElements[i]->GetElementSize() * m_arrayElements[i]->GetElementCount();
		}
	}

	for (size_t i = 0 ; i < m_arrayElements.size() ; i++)
	{
		m_arrayElements[i]->UnlinkCoords();
	}

	if (m_bOwnBuffer && m_pBuffer != NULL)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_pBuffer = (unsigned char *)p_pBuffer;
	m_bOwnBuffer = false;
	_link();
	memcpy( m_pBuffer, l_pBufferSave, m_uiStride);
}

void BufferElement :: UnlinkCoords()
{
	unsigned char * l_pBufferSave = new unsigned char[m_uiStride];

	if (m_pBuffer != NULL)
	{
		memcpy( l_pBufferSave, m_pBuffer, m_uiStride);
	}
	else
	{
		size_t l_uiOffset = 0;

		for (size_t i = 0 ; i < m_arrayElements.size() ; i++)
		{
			memcpy( & l_pBufferSave[l_uiOffset], m_arrayElements[i]->const_ptr<unsigned char>(), m_arrayElements[i]->GetElementSize() * m_arrayElements[i]->GetElementCount());
			l_uiOffset += m_arrayElements[i]->GetElementSize() * m_arrayElements[i]->GetElementCount();
		}
	}

	for (size_t i = 0 ; i < m_arrayElements.size() ; i++)
	{
		m_arrayElements[i]->UnlinkCoords();
	}

	if ( ! m_bOwnBuffer)
	{
		m_pBuffer = new unsigned char[m_uiStride];
	}

	m_bOwnBuffer = true;
	_link();
	memcpy( m_pBuffer, l_pBufferSave, m_uiStride);
}

void BufferElement :: _link()
{
	size_t l_uiOffset = 0;

	for (size_t i = 0 ; i < m_arrayElements.size() ; i++)
	{
		m_arrayElements[i]->LinkCoords( & m_pBuffer[l_uiOffset]);
		l_uiOffset += (m_arrayElements[i]->GetElementSize() * m_arrayElements[i]->GetElementCount());
	}
}

//*************************************************************************************************