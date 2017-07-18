#include "BufferDeclaration.hpp"

using namespace Castor;

namespace Castor3D
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
				m_stride += Castor3D::GetSize( m_arrayElements[i].m_dataType );
			}
		}
	}

	BufferDeclaration::~BufferDeclaration()
	{
	}

	bool operator==( BufferDeclaration const & p_lhs, BufferDeclaration const & p_rhs )
	{
		bool l_result = p_lhs.stride() == p_rhs.stride() && p_lhs.size() == p_rhs.size();
		auto l_itl = p_lhs.begin();
		auto l_itr = p_rhs.begin();

		while ( l_result && l_itl != p_lhs.end() )
		{
			l_result = *l_itl++ == *l_itr++;
			++l_itl;
			++l_itr;
		}

		return l_result;
	}

	bool operator!=( BufferDeclaration const & p_lhs, BufferDeclaration const & p_rhs )
	{
		bool l_result = p_lhs.stride() != p_rhs.stride() || p_lhs.size() != p_rhs.size();
		auto l_itl = p_lhs.begin();
		auto l_itr = p_rhs.begin();

		while ( !l_result && l_itl != p_lhs.end() )
		{
			l_result = *l_itl++ != *l_itr++;
			++l_itl;
			++l_itr;
		}

		return l_result;
	}
}
