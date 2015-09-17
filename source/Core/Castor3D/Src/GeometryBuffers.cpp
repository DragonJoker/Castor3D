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

	bool GeometryBuffers::Bind()
	{
		bool l_return = m_pVertexBuffer && m_pIndexBuffer;

		if ( l_return )
		{
			l_return = m_pVertexBuffer->Bind();

			if ( l_return )
			{
				if ( m_bMatrixBuffer )
				{
					l_return = m_pMatrixBuffer->Bind( 1 );
				}

				if ( l_return && m_bIndexBuffer )
				{
					l_return = m_pIndexBuffer->Bind();

					if ( !l_return )
					{
						m_pIndexBuffer->Unbind();
					}
				}
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
