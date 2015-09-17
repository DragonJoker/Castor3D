#include "Dx11GeometryBuffers.hpp"
#include "Dx11RenderSystem.hpp"

#include <VertexBuffer.hpp>
#include <IndexBuffer.hpp>

using namespace Castor3D;

namespace Dx11Render
{
	DxGeometryBuffers::DxGeometryBuffers( DxRenderSystem * p_pRenderSystem, VertexBufferUPtr p_pVertexBuffer, IndexBufferUPtr p_pIndexBuffer, MatrixBufferUPtr p_pMatrixBuffer )
		: GeometryBuffers( std::move( p_pVertexBuffer ), std::move( p_pIndexBuffer ), std::move( p_pMatrixBuffer ) )
		, m_renderSystem( p_pRenderSystem )
	{
	}

	DxGeometryBuffers::~DxGeometryBuffers()
	{
	}

	bool DxGeometryBuffers::Draw( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex )
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->IASetPrimitiveTopology( DirectX11::Get( p_ePrimitiveType ) );
		bool l_return = m_pVertexBuffer->Bind();

		if ( l_return )
		{
			if ( m_pIndexBuffer )
			{
				l_return = m_pIndexBuffer->Bind();

				if ( l_return )
				{
					l_pDeviceContext->DrawIndexed( p_uiSize, p_uiIndex, 0 );
					m_pIndexBuffer->Unbind();
				}
			}
			else
			{
				l_pDeviceContext->Draw( p_uiSize, p_uiIndex );
			}

			m_pVertexBuffer->Unbind();
		}

		return l_return;
	}

	bool DxGeometryBuffers::DrawInstanced( eTOPOLOGY p_ePrimitiveType, ShaderProgramBaseSPtr p_pProgram, uint32_t p_uiSize, uint32_t p_uiIndex, uint32_t p_uiCount )
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->IASetPrimitiveTopology( DirectX11::Get( p_ePrimitiveType ) );
		bool l_return = m_pVertexBuffer->Bind();

		if ( l_return )
		{
			if ( m_pMatrixBuffer )
			{
				l_return = m_pMatrixBuffer->Bind( p_uiCount );
			}

			if ( l_return )
			{
				if ( m_pIndexBuffer )
				{
					l_return = m_pIndexBuffer->Bind();

					if ( l_return )
					{
						l_pDeviceContext->DrawIndexedInstanced( p_uiSize, p_uiCount, p_uiIndex, 0, 0 );
						m_pIndexBuffer->Unbind();
					}
				}
				else
				{
					l_pDeviceContext->DrawInstanced( p_uiSize, p_uiCount, p_uiIndex, 0 );
				}
			}

			m_pVertexBuffer->Unbind();
		}

		return l_return;
	}

	bool DxGeometryBuffers::DoInitialise()
	{
		return true;
	}

	void DxGeometryBuffers::DoCleanup()
	{
	}
}
