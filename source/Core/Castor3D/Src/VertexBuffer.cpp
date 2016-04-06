#include "VertexBuffer.hpp"

#include "Engine.hpp"
#include "RenderSystem.hpp"

using namespace Castor;

namespace Castor3D
{
	VertexBuffer::VertexBuffer( Engine & p_engine, BufferDeclaration const & p_declaration )
		: CpuBuffer< uint8_t >( p_engine )
		, m_bufferDeclaration( p_declaration )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::Create()
	{
		if ( !m_pBuffer )
		{
			m_pBuffer = GetEngine()->GetRenderSystem()->CreateVertexBuffer( this );
		}

		bool l_return = m_pBuffer != nullptr;

		if ( l_return )
		{
			l_return = GetGpuBuffer()->Create();
		}

		return l_return;
	}
}
