#include "GeometryBuffers.hpp"
#include "IndexBuffer.hpp"
#include "MatrixBuffer.hpp"
#include "VertexBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	GeometryBuffers::GeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer, eTOPOLOGY p_topology )
		: m_pVertexBuffer( std::move( p_pVertexBuffer ) )
		, m_pIndexBuffer( std::move( p_pIndexBuffer ) )
		, m_pMatrixBuffer( std::move( p_pMatrixBuffer ) )
		, m_topology( p_topology )
	{
	}

	GeometryBuffers::~GeometryBuffers()
	{
	}

	bool GeometryBuffers::Create()
	{
		bool l_return = true;

		if ( l_return && m_pVertexBuffer )
		{
			l_return = m_pVertexBuffer->Create();
		}

		if ( l_return && m_pIndexBuffer )
		{
			l_return = m_pIndexBuffer->Create();
		}

		if ( l_return && m_pMatrixBuffer )
		{
			l_return = m_pMatrixBuffer->Create();
		}

		return l_return;
	}

	void GeometryBuffers::Destroy()
	{
		if ( m_pVertexBuffer )
		{
			m_pVertexBuffer->Destroy();
		}

		if ( m_pIndexBuffer )
		{
			m_pIndexBuffer->Destroy();
		}

		if ( m_pMatrixBuffer )
		{
			m_pMatrixBuffer->Destroy();
		}
	}

	bool GeometryBuffers::Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature )
	{
		m_initialised = false;

		if ( m_pVertexBuffer )
		{
			m_initialised = m_pVertexBuffer->Initialise( p_vtxType, p_vtxNature );
		}

		m_program = p_shader;

		if ( m_pVertexBuffer )
		{
			m_initialised = m_pVertexBuffer->AttachTo( p_shader );
		}

		return AttachTo( p_shader );
	}

	bool GeometryBuffers::Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature, eBUFFER_ACCESS_TYPE p_idxType, eBUFFER_ACCESS_NATURE p_idxNature )
	{
		m_initialised = false;

		if ( m_pVertexBuffer )
		{
			m_initialised = m_pVertexBuffer->Initialise( p_vtxType, p_vtxNature );
		}

		if ( m_initialised && m_pIndexBuffer )
		{
			m_initialised = m_pIndexBuffer->Initialise( p_idxType, p_idxNature );
		}

		m_program = p_shader;

		if ( m_pVertexBuffer )
		{
			m_initialised = m_pVertexBuffer->AttachTo( p_shader );
		}

		if ( m_initialised && m_pIndexBuffer )
		{
			m_initialised = m_pIndexBuffer->AttachTo( p_shader );
		}

		return AttachTo( p_shader );
	}

	bool GeometryBuffers::Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature, eBUFFER_ACCESS_TYPE p_idxType, eBUFFER_ACCESS_NATURE p_idxNature, eBUFFER_ACCESS_TYPE p_mtxType, eBUFFER_ACCESS_NATURE p_mtxNature )
	{
		m_initialised = false;

		if ( m_pVertexBuffer )
		{
			m_initialised = m_pVertexBuffer->Initialise( p_vtxType, p_vtxNature );
		}

		if ( m_initialised && m_pIndexBuffer )
		{
			m_initialised = m_pIndexBuffer->Initialise( p_idxType, p_idxNature );
		}

		if ( m_initialised && m_pMatrixBuffer )
		{
			m_pMatrixBuffer->Initialise( p_mtxType, p_mtxNature );
		}

		return AttachTo( p_shader );
	}

	bool GeometryBuffers::AttachTo( ShaderProgramBaseSPtr p_program )
	{
		m_program = p_program;

		if ( m_pVertexBuffer )
		{
			m_initialised = m_pVertexBuffer->AttachTo( p_program );
		}

		if ( m_initialised && m_pIndexBuffer )
		{
			m_initialised = m_pIndexBuffer->AttachTo( p_program );
		}

		if ( m_initialised && m_pMatrixBuffer )
		{
			m_pMatrixBuffer->AttachTo( p_program );
		}

		if ( p_program )
		{
			m_initialised = DoInitialise();
		}

		return m_initialised;
	}

	void GeometryBuffers::Cleanup()
	{
		m_program.reset();
		DoCleanup();

		if ( m_pVertexBuffer )
		{
			m_pVertexBuffer->Cleanup();
		}

		if ( m_pIndexBuffer )
		{
			m_pIndexBuffer->Cleanup();
		}

		if ( m_pMatrixBuffer )
		{
			m_pMatrixBuffer->Cleanup();
		}
	}

	bool GeometryBuffers::Bind()
	{
		bool l_return = m_pVertexBuffer != nullptr;

		if ( l_return )
		{
			l_return = m_pVertexBuffer->Bind();

			if ( l_return && m_pMatrixBuffer )
			{
				l_return = m_pMatrixBuffer->Bind( 1 );
			}

			if ( l_return && m_pIndexBuffer )
			{
				l_return = m_pIndexBuffer->Bind();
			}
		}

		return l_return;
	}

	void GeometryBuffers::Unbind()
	{
		if ( m_pVertexBuffer && m_pIndexBuffer )
		{
			if ( m_pMatrixBuffer )
			{
				m_pMatrixBuffer->Unbind();
			}

			m_pVertexBuffer->Unbind();

			if ( m_pIndexBuffer )
			{
				m_pIndexBuffer->Unbind();
			}
		}
	}
}
