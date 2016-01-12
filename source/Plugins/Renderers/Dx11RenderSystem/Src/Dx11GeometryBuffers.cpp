#include "Dx11GeometryBuffers.hpp"
#include "Dx11RenderSystem.hpp"

#include <VertexBuffer.hpp>
#include <IndexBuffer.hpp>

using namespace Castor3D;

namespace Dx11Render
{
	DxGeometryBuffers::DxGeometryBuffers( DxRenderSystem * p_renderSystem, VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer, eTOPOLOGY p_topology )
		: GeometryBuffers( std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ), p_topology )
		, m_renderSystem( p_renderSystem )
	{
	}

	DxGeometryBuffers::~DxGeometryBuffers()
	{
	}

	bool DxGeometryBuffers::Draw( ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index )
	{
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->IASetPrimitiveTopology( DirectX11::Get( m_topology ) );
		bool l_return = m_pVertexBuffer->Bind();

		if ( l_return )
		{
			if ( m_pIndexBuffer )
			{
				l_return = m_pIndexBuffer->Bind();

				if ( l_return )
				{
					l_deviceContext->DrawIndexed( p_uiSize, p_index, 0 );
					m_pIndexBuffer->Unbind();
				}
			}
			else
			{
				l_deviceContext->Draw( p_uiSize, p_index );
			}

			m_pVertexBuffer->Unbind();
		}

		return l_return;
	}

	bool DxGeometryBuffers::DrawInstanced( ShaderProgramBaseSPtr p_program, uint32_t p_uiSize, uint32_t p_index, uint32_t p_count )
	{
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->IASetPrimitiveTopology( DirectX11::Get( m_topology ) );
		bool l_return = m_pVertexBuffer->Bind();

		if ( l_return )
		{
			if ( m_pMatrixBuffer )
			{
				l_return = m_pMatrixBuffer->Bind( p_count );
			}

			if ( l_return )
			{
				if ( m_pIndexBuffer )
				{
					l_return = m_pIndexBuffer->Bind();

					if ( l_return )
					{
						l_deviceContext->DrawIndexedInstanced( p_uiSize, p_count, p_index, 0, 0 );
						m_pIndexBuffer->Unbind();
					}
				}
				else
				{
					l_deviceContext->DrawInstanced( p_uiSize, p_count, p_index, 0 );
				}
			}

			m_pVertexBuffer->Unbind();
		}

		return l_return;
	}

	bool DxGeometryBuffers::DoCreate()
	{
		return true;
	}

	void DxGeometryBuffers::DoDestroy()
	{
	}

	bool DxGeometryBuffers::DoInitialise()
	{
		return true;
	}

	void DxGeometryBuffers::DoCleanup()
	{
	}
}
