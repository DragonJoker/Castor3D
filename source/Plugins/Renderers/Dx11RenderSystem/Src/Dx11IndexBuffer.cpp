#include "Dx11IndexBuffer.hpp"
#include "Dx11RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;

namespace Dx11Render
{
	DxIndexBuffer::DxIndexBuffer( DxRenderSystem & p_renderSystem, HardwareBufferPtr p_buffer )
		: DxBufferObject< uint32_t, ID3D11Buffer >( p_renderSystem, p_buffer )
	{
	}

	DxIndexBuffer::~DxIndexBuffer()
	{
	}

	bool DxIndexBuffer::Create()
	{
		return true;
	}

	void DxIndexBuffer::Destroy()
	{
		DxBufferObject< uint32_t, ID3D11Buffer >::DoDestroy();
	}

	void DxIndexBuffer::Cleanup()
	{
		DxBufferObject< uint32_t, ID3D11Buffer >::DoCleanup();
	}

	bool DxIndexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature, ShaderProgramBaseSPtr p_program )
	{
		bool l_return = false;

		if ( m_pBuffer )
		{
			DxRenderSystem * l_renderSystem = static_cast< DxRenderSystem *>( m_pBuffer->GetOwner()->GetRenderSystem() );
			ID3D11Device * l_pDevice = l_renderSystem->GetDevice();
			UINT l_uiSize = UINT( m_pBuffer->GetSize() );

			if ( l_uiSize > 0 )
			{
				HRESULT l_hr;
				D3D11_BUFFER_DESC l_desc = { 0 };
				l_desc.ByteWidth = l_uiSize * UINT( sizeof( uint32_t ) );
				l_desc.Usage = DirectX11::Get( p_type );
				l_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				l_desc.CPUAccessFlags = DirectX11::GetCpuAccessFlags( p_type | p_nature );
				l_desc.MiscFlags = 0;
				l_desc.StructureByteStride = 0;

				if ( p_type == eBUFFER_ACCESS_TYPE_STATIC )
				{
					D3D11_SUBRESOURCE_DATA l_data = { 0 };
					l_data.pSysMem = &m_pBuffer->data()[0];
					l_hr = l_pDevice->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
					dxTrack( l_renderSystem, m_pBufferObject, IndexBuffer );
				}
				else
				{
					l_hr = l_pDevice->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
					dxTrack( l_renderSystem, m_pBufferObject, IndexBuffer );
				}

				l_return = dxCheckError( l_hr, "ID3D11Device::CreateIndexBuffer" );
			}
			else
			{
				l_return = false;
			}

			if ( m_pBufferObject )
			{
				if ( p_type != eBUFFER_ACCESS_TYPE_STATIC )
				{
					UINT l_uiSize = UINT( m_pBuffer->GetSize() );

					if ( l_uiSize )
					{
						uint32_t * l_pBuffer = Lock( 0, l_uiSize, eACCESS_TYPE_WRITE );

						if ( l_pBuffer )
						{
							memcpy( l_pBuffer, &m_pBuffer->data()[0], l_uiSize * sizeof( uint32_t ) );
							Unlock();
						}
					}
				}

				m_pBuffer->Assign();
				l_return = true;
			}
		}

		return l_return;
	}

	bool DxIndexBuffer::Bind()
	{
		bool l_return = true;

		if ( m_pBuffer && m_pBuffer->IsAssigned() )
		{
			ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_pBuffer->GetOwner()->GetRenderSystem()->GetCurrentContext() )->GetDeviceContext();
			l_deviceContext->IASetIndexBuffer( m_pBufferObject, DXGI_FORMAT_R32_UINT, 0 );
		}

		return l_return;
	}

	void DxIndexBuffer::Unbind()
	{
	}

	uint32_t * DxIndexBuffer::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		return DxBufferObject< uint32_t, ID3D11Buffer >::DoLock( p_offset, p_count, p_flags );
	}

	void DxIndexBuffer::Unlock()
	{
		DxBufferObject< uint32_t, ID3D11Buffer >::DoUnlock();
	}
}
