#include "BufferDeclaration.hpp"

using namespace castor;

namespace castor3d
{
	BufferDeclaration::BufferDeclaration()
		: BufferDeclaration( nullptr, 0 )
	{
	}

	BufferDeclaration::BufferDeclaration( BufferElementDeclaration const * p_elements, uint32_t p_count )
		: m_stride( 0 )
	{
		if ( p_elements )
		{
			for ( uint32_t i = 0; i < p_count; i++ )
			{
				m_arrayElements.push_back( p_elements[i] );
				m_arrayElements[i].m_offset = m_stride;
				m_stride += castor3d::getSize( m_arrayElements[i].m_dataType );
			}
		}
	}

	BufferDeclaration::~BufferDeclaration()
	{
	}

	bool operator==( BufferDeclaration const & p_lhs, BufferDeclaration const & p_rhs )
	{
		bool result = p_lhs.stride() == p_rhs.stride() && p_lhs.size() == p_rhs.size();
		auto itl = p_lhs.begin();
		auto itr = p_rhs.begin();

		while ( result && itl != p_lhs.end() )
		{
			result = *itl++ == *itr++;
			++itl;
			++itr;
		}

		return result;
	}

	bool operator!=( BufferDeclaration const & p_lhs, BufferDeclaration const & p_rhs )
	{
		bool result = p_lhs.stride() != p_rhs.stride() || p_lhs.size() != p_rhs.size();
		auto itl = p_lhs.begin();
		auto itr = p_rhs.begin();

		while ( !result && itl != p_lhs.end() )
		{
			result = *itl++ != *itr++;
			++itl;
			++itr;
		}

		return result;
	}
}
