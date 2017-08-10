#include "VertexBuffer.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	VertexBuffer::VertexBuffer( Engine & engine, BufferDeclaration const & p_declaration )
		: CpuBuffer< uint8_t >( engine )
		, m_bufferDeclaration( p_declaration )
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::initialise( BufferAccessType p_type, BufferAccessNature p_nature )
	{
		if ( !m_gpuBuffer )
		{
			m_gpuBuffer = getEngine()->getRenderSystem()->createBuffer( BufferType::eArray );
		}

		bool result = m_gpuBuffer != nullptr;

		if ( result )
		{
			result = doInitialise( p_type, p_nature );
		}

		return result;
	}

	void VertexBuffer::cleanup()
	{
		doCleanup();
	}
}
