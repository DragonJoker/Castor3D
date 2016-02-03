#include "VertexLayout.hpp"

using namespace Castor;

namespace Castor3D
{
	VertexLayout::VertexLayout()
	{
	}

	VertexLayout::~VertexLayout()
	{
	}

	void VertexLayout::AddAttribute( Castor::String const & p_name, eELEMENT_TYPE p_type, uint32_t p_offset )
	{
		m_bufferLayout.push_back( BufferElementDeclaration{ p_name, p_type, p_offset } );
	}

	BufferDeclaration VertexLayout::CreateDeclaration()const
	{
		return BufferDeclaration( m_bufferLayout.data(), m_bufferLayout.size() );
	}
}
