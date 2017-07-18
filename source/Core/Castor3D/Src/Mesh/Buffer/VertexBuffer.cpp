#include "VertexBuffer.hpp"

#include "Engine.hpp"

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

	bool VertexBuffer::Initialise( BufferAccessType p_type, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = GetEngine()->GetRenderSystem()->CreateBuffer( BufferType::eArray );
		}

		bool l_result = m_gpuBuffer != nullptr;

		if ( l_result )
		{
			l_result = DoInitialise( p_type, p_nature );
		}

		return l_result;
	}

	void VertexBuffer::Cleanup()
	{
		DoCleanup();
	}
}
