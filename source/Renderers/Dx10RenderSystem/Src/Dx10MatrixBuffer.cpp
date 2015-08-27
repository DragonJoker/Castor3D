#include "Dx10MatrixBuffer.hpp"
#include "Dx10RenderSystem.hpp"

#include <Logger.hpp>

using namespace Castor3D;

namespace Dx10Render
{
	DxMatrixBuffer::DxMatrixBuffer( HardwareBufferPtr p_pBuffer )
		:	DxBufferObject< real, ID3D10Buffer >	( p_pBuffer	)
		,	m_pDxDeclaration( NULL	)
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
		DxBufferObject< real, ID3D10Buffer >::DoDestroy();
	}

	void DxMatrixBuffer::Cleanup()
	{
		SafeRelease( m_pDxDeclaration );
		DxBufferObject< real, ID3D10Buffer >::DoCleanup();
	}

	bool DxMatrixBuffer::Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
#if 1
		return true;
#else
		bool l_bReturn = false;
		m_pProgram = std::static_pointer_cast< DxShaderProgram >( p_pProgram );

		if ( m_pBuffer )
		{
			m_pDevice = static_cast< DxRenderSystem *>( m_pBuffer->GetRenderSystem() )->GetDevice();
			DxShaderProgramSPtr l_pProgram = m_pProgram.lock();
			bool l_bReturn = false;

			if ( l_pProgram )
			{
				std::vector< D3D10_INPUT_ELEMENT_DESC >	l_arrayDxElements;
				UINT l_uiMtx = 0;

				for ( int i = 0; i < 4; ++i )
				{
					D3D10_INPUT_ELEMENT_DESC l_d3dCurrentElement = { 0 };
					l_d3dCurrentElement.Format;
					l_d3dCurrentElement.InputSlot				= 0;
					l_d3dCurrentElement.AlignedByteOffset		= 0;
					l_d3dCurrentElement.InputSlotClass			= D3D10_INPUT_PER_INSTANCE_DATA;
					l_d3dCurrentElement.InstanceDataStepRate	= 1;
					l_d3dCurrentElement.SemanticName			= "MATRIX";
					l_d3dCurrentElement.SemanticIndex			= l_uiMtx++;
					l_d3dCurrentElement.Format					= DXGI_FORMAT_R32G32B32A32_FLOAT;
					l_arrayDxElements.push_back( l_d3dCurrentElement );
				}

				ID3DBlob * l_pBlob = l_pProgram->GetCompiled( eSHADER_TYPE_VERTEX );

				if ( l_pBlob )
				{
					HRESULT l_hr = m_pDevice->CreateInputLayout( &l_arrayDxElements[0], UINT( l_arrayDxElements.size() ), reinterpret_cast< DWORD const * >( l_pBlob->GetBufferPointer() ), l_pBlob->GetBufferSize(), &m_pDxDeclaration );
					dxDebugName( m_pDxDeclaration, MtxInputLayout );
					l_bReturn = dxCheckError( l_hr, "ID3D10Device::CreateInputLayout" );
				}
			}

			UINT l_uiSize = UINT( m_pBuffer->GetSize() );

			if ( l_uiSize > 0 )
			{
				HRESULT l_hr;
				D3D10_BUFFER_DESC l_desc	= { 0 };
				l_desc.ByteWidth			= l_uiSize * UINT( sizeof( uint32_t ) );
				l_desc.Usage				= DirectX10::Get( p_eType );
				l_desc.BindFlags			= D3D10_BIND_SHADER_RESOURCE;
				l_desc.CPUAccessFlags		= DirectX10::GetCpuAccessFlags( p_eType | p_eNature );
				l_desc.MiscFlags			= 0;

				if ( p_eType == eBUFFER_ACCESS_TYPE_STATIC )
				{
					D3D10_SUBRESOURCE_DATA l_data = { 0 };
					l_data.pSysMem = &m_pBuffer->data()[0];
					l_hr = m_pDevice->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
					dxDebugName( m_pBufferObject, MatrixBuffer );
				}
				else
				{
					l_hr = m_pDevice->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
					dxDebugName( m_pBufferObject, MatrixBuffer );
				}

				l_bReturn = dxCheckError( l_hr, "ID3D10Device::CreateIndexBuffer" );
			}
			else
			{
				l_bReturn = false;
			}

			if ( m_pBufferObject )
			{
				if ( p_eType != eBUFFER_ACCESS_TYPE_STATIC )
				{
					UINT l_uiSize = UINT( m_pBuffer->GetSize() );

					if ( l_uiSize )
					{
						real * l_pBuffer = Lock( 0, l_uiSize, eLOCK_FLAG_WRITE_ONLY );

						if ( l_pBuffer )
						{
							memcpy( l_pBuffer, &m_pBuffer->data()[0], l_uiSize * sizeof( uint32_t ) );
							Unlock();
						}
					}
				}

				m_pBuffer->Assign();
				l_bReturn = true;
			}
		}

		return l_bReturn;
#endif
	}

	bool DxMatrixBuffer::Bind( uint32_t p_uiCount )
	{
#if 1
		return true;
#else
		bool l_bReturn = true;

		if ( m_pBuffer && m_pBuffer->IsAssigned() )
		{
		}

		return l_bReturn;
#endif
	}

	void DxMatrixBuffer::Unbind()
	{
	}

	real * DxMatrixBuffer::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
#if 1
		return NULL;
#else
		return DxBufferObject< real, ID3D10Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
#endif
	}

	void DxMatrixBuffer::Unlock()
	{
		DxBufferObject< real, ID3D10Buffer >::DoUnlock();
	}
}
