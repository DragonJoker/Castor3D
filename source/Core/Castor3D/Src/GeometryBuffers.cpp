#include "GeometryBuffers.hpp"
#include "IndexBuffer.hpp"
#include "MatrixBuffer.hpp"
#include "VertexBuffer.hpp"

using namespace Castor;

namespace Castor3D
{
	GeometryBuffers::GeometryBuffers( VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
		:	m_pVertexBuffer( std::move( p_pVertexBuffer ) )
		,	m_pIndexBuffer( std::move( p_pIndexBuffer ) )
		,	m_pMatrixBuffer( std::move( p_pMatrixBuffer ) )
	{
		m_bIndexBuffer = m_pIndexBuffer != nullptr;
		m_bMatrixBuffer = m_pMatrixBuffer != nullptr;
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

		if ( l_return && m_pIndexBuffer && m_bIndexBuffer )
		{
			l_return = m_pIndexBuffer->Create();
		}

		if ( l_return && m_pMatrixBuffer && m_bMatrixBuffer )
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
		
		if ( m_pIndexBuffer && m_bIndexBuffer )
		{
			m_pIndexBuffer->Destroy();
		}

		if ( m_pMatrixBuffer && m_bMatrixBuffer )
		{
			m_pMatrixBuffer->Destroy();
		}
	}

	bool GeometryBuffers::Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature )
	{
		bool l_return = true;

		if ( l_return && m_pVertexBuffer )
		{
			l_return = m_pVertexBuffer->Initialise( p_vtxType, p_vtxNature, p_shader );
		}

		if ( l_return )
		{
			l_return = DoInitialise();
		}

		return l_return;
	}

	bool GeometryBuffers::Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature, eBUFFER_ACCESS_TYPE p_idxType, eBUFFER_ACCESS_NATURE p_idxNature )
	{
		bool l_return = true;

		if ( l_return && m_pVertexBuffer )
		{
			l_return = m_pVertexBuffer->Initialise( p_vtxType, p_vtxNature, p_shader );
		}

		if ( l_return && m_pIndexBuffer && m_bIndexBuffer )
		{
			l_return = m_pIndexBuffer->Initialise( p_idxType, p_idxNature, p_shader );
		}

		if ( l_return )
		{
			l_return = DoInitialise();
		}

		return l_return;
	}

	bool GeometryBuffers::Initialise( ShaderProgramBaseSPtr p_shader, eBUFFER_ACCESS_TYPE p_vtxType, eBUFFER_ACCESS_NATURE p_vtxNature, eBUFFER_ACCESS_TYPE p_idxType, eBUFFER_ACCESS_NATURE p_idxNature, eBUFFER_ACCESS_TYPE p_mtxType, eBUFFER_ACCESS_NATURE p_mtxNature )
	{
		bool l_return = true;

		if ( l_return && m_pVertexBuffer )
		{
			l_return = m_pVertexBuffer->Initialise( p_vtxType, p_vtxNature, p_shader );
		}

		if ( l_return && m_pIndexBuffer && m_bIndexBuffer )
		{
			l_return = m_pIndexBuffer->Initialise( p_idxType, p_idxNature, p_shader );
		}

		if ( l_return && m_pMatrixBuffer && m_bMatrixBuffer )
		{
			l_return = m_pMatrixBuffer->Initialise( p_mtxType, p_mtxNature, p_shader );
		}

		if ( l_return )
		{
			l_return = DoInitialise();
		}

		return l_return;
	}

	void GeometryBuffers::Cleanup()
	{
		DoCleanup();

		if ( m_pVertexBuffer )
		{
			m_pVertexBuffer->Cleanup();
		}
		
		if ( m_pIndexBuffer && m_bIndexBuffer )
		{
			m_pIndexBuffer->Cleanup();
		}

		if ( m_pMatrixBuffer && m_bMatrixBuffer )
		{
			m_pMatrixBuffer->Cleanup();
		}
	}

	bool GeometryBuffers::Bind()
	{
		bool l_return = m_pVertexBuffer;

		if ( l_return )
		{
			l_return = m_pVertexBuffer->Bind();

			if ( l_return && m_pMatrixBuffer && m_bMatrixBuffer )
			{
				l_return = m_pMatrixBuffer->Bind( 1 );
			}

			if ( l_return && m_pIndexBuffer && m_bIndexBuffer )
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
			if ( m_bMatrixBuffer )
			{
				m_pMatrixBuffer->Unbind();
			}

			m_pVertexBuffer->Unbind();

			if ( m_bIndexBuffer )
			{
				m_pIndexBuffer->Unbind();
			}
		}
	}
}
