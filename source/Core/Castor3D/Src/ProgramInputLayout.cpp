#include "ProgramInputLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	ProgramInputLayout::ProgramInputLayout( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
	{
	}

	ProgramInputLayout::~ProgramInputLayout()
	{
	}

	void ProgramInputLayout::DoAddAttribute( Castor::String const & p_name, eELEMENT_TYPE p_type, uint32_t p_offset )
	{
		m_layout.push_back( BufferElementDeclaration{ p_name, p_type, p_offset } );
		m_stride = std::max( m_stride, m_stride + p_offset + GetSize( p_type ) );
	}

	bool operator==( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs )
	{
		bool l_return = p_lhs.GetStride() == p_rhs.GetStride() && p_lhs.m_layout.size() == p_rhs.m_layout.size();
		auto l_itl = p_lhs.m_layout.begin();
		auto l_itr = p_rhs.m_layout.begin();

		while ( l_return && l_itl != p_lhs.m_layout.end() )
		{
			l_return = *l_itl == *l_itr;
			++l_itl;
			++l_itr;
		}

		return l_return;
	}

	bool operator!=( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs )
	{
		return !( p_lhs == p_rhs );
	}
}
