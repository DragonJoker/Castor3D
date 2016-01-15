#include "Dx11MatrixBuffer.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11ShaderProgram.hpp"

#include <Logger.hpp>

#include <Engine.hpp>

using namespace Castor3D;

namespace Dx11Render
{
	DxMatrixBuffer::DxMatrixBuffer( DxRenderSystem & p_renderSystem, HardwareBufferPtr p_buffer )
		: DxBufferObject< real, ID3D11Buffer >( p_renderSystem, p_buffer )
		, m_pDxDeclaration( NULL )
	{
	}

	DxMatrixBuffer::~DxMatrixBuffer()
	{
	}

	bool DxMatrixBuffer::Create()
	{
		return true;
	}

	void DxMatrixBuffer::Destroy()
	{
		DxBufferObject< real, ID3D11Buffer >::DoDestroy();
	}

	void DxMatrixBuffer::Cleanup()
	{
		ReleaseTracked( m_pBuffer->GetOwner()->GetRenderSystem(), m_pDxDeclaration );
		DxBufferObject< real, ID3D11Buffer >::DoCleanup();
	}

	bool DxMatrixBuffer::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature, ShaderProgramBaseSPtr p_program )
	{
#if 1
		return true;
#else
		bool l_return = false;
		m_program = std::static_pointer_cast< DxShaderProgram >( p_program );

		if ( m_pBuffer )
		{
			DxShaderProgramSPtr l_program = m_program.lock();
			bool l_return = false;

			if ( l_program )
			{
				std::vector< D3D11_INPUT_ELEMENT_DESC >	l_arrayDxElements;
				UINT l_uiMtx = 0;

				for ( int i = 0; i < 4; ++i )
				{
					D3D11_INPUT_ELEMENT_DESC l_d3dCurrentElement = { 0 };
					l_d3dCurrentElement.Format;
					l_d3dCurrentElement.InputSlot = 0;
					l_d3dCurrentElement.AlignedByteOffset = 0;
					l_d3dCurrentElement.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
					l_d3dCurrentElement.InstanceDataStepRate = 1;
					l_d3dCurrentElement.SemanticName = "MATRIX";
					l_d3dCurrentElement.SemanticIndex = l_uiMtx++;
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					l_arrayDxElements.push_back( l_d3dCurrentElement );
				}

				ID3DBlob * l_pBlob = l_program->GetCompiled( eSHADER_TYPE_VERTEX );

				if ( l_pBlob )
				{
					HRESULT l_hr = reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetOwner()->GetRenderSystem() )->GetDevice()->CreateInputLayout( &l_arrayDxElements[0], UINT( l_arrayDxElements.size() ), reinterpret_cast< DWORD const * >( l_pBlob->GetBufferPointer() ), l_pBlob->GetBufferSize(), &m_pDxDeclaration );
					dxTrack( m_renderSystem, m_pBufferObject, MtxInputLayout );
					l_return = dxCheckError( l_hr, "ID3D11Device::CreateInputLayout" );
				}
			}

			UINT l_uiSize = UINT( m_pBuffer->GetSize() );

			if ( l_uiSize > 0 )
			{
				HRESULT l_hr;
				D3D11_BUFFER_DESC l_desc = { 0 };
				l_desc.ByteWidth = l_uiSize * UINT( sizeof( uint32_t ) );
				l_desc.Usage = DirectX11::Get( p_type );
				l_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				l_desc.CPUAccessFlags = DirectX11::GetCpuAccessFlags( p_type | p_nature );
				l_desc.MiscFlags = 0;
				l_desc.StructureByteStride = 0;//sizeof( uint32_t );

				if ( p_type == eBUFFER_ACCESS_TYPE_STATIC )
				{
					D3D11_SUBRESOURCE_DATA l_data = { 0 };
					l_data.pSysMem = &m_pBuffer->data()[0];
					l_hr = reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetOwner()->GetRenderSystem() )->GetDevice()->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
					dxTrack( m_renderSystem, m_pBufferObject, MatrixBuffer );
				}
				else
				{
					l_hr = reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetOwner()->GetRenderSystem() )->GetDevice()->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
					dxTrack( m_renderSystem, m_pBufferObject, MatrixBuffer );
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
						real * l_pBuffer = Lock( 0, l_uiSize, eACCESS_TYPE_WRITE );

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
#endif
	}

	bool DxMatrixBuffer::Bind( uint32_t p_count )
	{
		return true;
		bool l_return = true;

		if ( m_pBuffer && m_pBuffer->IsAssigned() )
		{
		}

		return l_return;
	}

	void DxMatrixBuffer::Unbind()
	{
	}

	real * DxMatrixBuffer::Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
	{
		return NULL;
		return DxBufferObject< real, ID3D11Buffer >::DoLock( p_offset, p_count, p_flags );
	}

	void DxMatrixBuffer::Unlock()
	{
		DxBufferObject< real, ID3D11Buffer >::DoUnlock();
	}
}
