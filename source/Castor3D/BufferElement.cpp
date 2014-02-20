#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/BufferElement.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

BufferDeclaration :: BufferDeclaration( BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements )
{
	DoInitialise( p_pElements, p_uiNbElements );
}

BufferDeclaration :: BufferDeclaration( BufferDeclaration const & p_declaration )
	:	m_arrayElements	( p_declaration.m_arrayElements	)
	,	m_uiStride		( p_declaration.m_uiStride		)
{
}

BufferDeclaration :: BufferDeclaration( BufferDeclaration && p_declaration )
	:	m_arrayElements	( std::move( p_declaration.m_arrayElements	) )
	,	m_uiStride		( std::move( p_declaration.m_uiStride		) )
{
	p_declaration.m_arrayElements.clear();
	p_declaration.m_uiStride = 0;
}

BufferDeclaration & BufferDeclaration :: operator =( BufferDeclaration p_declaration )
{
	swap( *this, p_declaration );
	return *this;
}

BufferDeclaration :: ~BufferDeclaration()
{
}

void BufferDeclaration :: DoInitialise( BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements )
{
	static const uint32_t s_uiSize[] = { 1 * sizeof( real ), 2 * sizeof( real ), 3 * sizeof( real ), 4 * sizeof( real ), sizeof( uint32_t ), 1 * sizeof( int32_t ), 2 * sizeof( int32_t ), 3 * sizeof( int32_t ), 4 * sizeof( int32_t ) };
	m_uiStride = 0;

	for( uint32_t i = 0; i < p_uiNbElements; i++ )
	{
		m_arrayElements.push_back( p_pElements[i] );
		m_arrayElements[i].m_uiOffset = m_uiStride;
		m_uiStride += s_uiSize[m_arrayElements[i].m_eDataType];
	}
}

void Castor3D::swap( BufferDeclaration & p_obj1, BufferDeclaration & p_obj2 )
{
	using std::swap;
	swap( p_obj1.m_arrayElements,	p_obj2.m_arrayElements	);
	swap( p_obj1.m_uiStride,		p_obj2.m_uiStride		);
}

//*************************************************************************************************

BufferElementGroup :: BufferElementGroup( uint8_t * p_pBuffer, uint32_t p_uiIndex )
	:	m_pBuffer	( NULL		)
	,	m_uiIndex	( p_uiIndex	)
{
	LinkCoords( p_pBuffer );
}

BufferElementGroup :: ~BufferElementGroup()
{
}

void BufferElementGroup :: LinkCoords( uint8_t * p_pBuffer )
{
	m_pBuffer = p_pBuffer;
}

void BufferElementGroup :: LinkCoords( uint8_t * p_pBuffer, uint32_t p_uiStride )
{
	if( p_pBuffer )
	{
		if( m_pBuffer )
		{
			memcpy( p_pBuffer, m_pBuffer, p_uiStride );
		}
		else
		{
			memset( p_pBuffer, 0, p_uiStride );
		}
	}

	m_pBuffer = p_pBuffer;
}

//*************************************************************************************************
