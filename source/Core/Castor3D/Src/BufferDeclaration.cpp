#include "BufferDeclaration.hpp"

using namespace Castor;

namespace Castor3D
{
	BufferDeclaration::BufferDeclaration()
		: BufferDeclaration( NULL, 0 )
	{
	}

	BufferDeclaration::BufferDeclaration( BufferElementDeclaration const * p_elements, uint32_t p_count )
		: m_uiStride( 0 )
	{
		for ( uint32_t i = 0; i < p_count; i++ )
		{
			m_arrayElements.push_back( p_elements[i] );
			m_arrayElements[i].m_offset = m_uiStride;
			m_uiStride += Castor3D::GetSize( m_arrayElements[i].m_dataType );
		}
	}

	BufferDeclaration::BufferDeclaration( std::vector< BufferElementDeclaration > const & p_elements )
		: BufferDeclaration( p_elements.data(), p_elements.size() )
	{
	}

	BufferDeclaration::~BufferDeclaration()
	{
	}

	bool operator==( BufferDeclaration const & p_lhs, BufferDeclaration const & p_rhs )
	{
		bool l_return = p_lhs.GetStride() == p_rhs.GetStride() && p_lhs.GetSize() == p_rhs.GetSize();
		auto l_itl = p_lhs.begin();
		auto l_itr = p_rhs.begin();

		while ( l_return && l_itl != p_lhs.end() )
		{
			l_return = *l_itl++ == *l_itr++;
			++l_itl;
			++l_itr;
		}

		return l_return;
	}

	bool operator!=( BufferDeclaration const & p_lhs, BufferDeclaration const & p_rhs )
	{
		bool l_return = p_lhs.GetStride() != p_rhs.GetStride() || p_lhs.GetSize() != p_rhs.GetSize();
		auto l_itl = p_lhs.begin();
		auto l_itr = p_rhs.begin();

		while ( !l_return && l_itl != p_lhs.end() )
		{
			l_return = *l_itl++ != *l_itr++;
			++l_itl;
			++l_itr;
		}

		return l_return;
	}
}
