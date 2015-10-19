#include "Dx11VertexBuffer.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11ShaderProgram.hpp"

#include <BufferDeclaration.hpp>
#include <Logger.hpp>

using namespace Castor3D;

namespace Dx11Render
{
	DxVertexBuffer::DxVertexBuffer( DxRenderSystem & p_renderSystem, BufferDeclaration const & p_declaration, HardwareBufferPtr p_pBuffer )
		: DxBufferObject< uint8_t, ID3D11Buffer >( p_renderSystem, p_pBuffer )
		, m_pDxDeclaration( NULL )
		, m_declaration( p_declaration )
	{
	}

	DxVertexBuffer::~DxVertexBuffer()
	{
	}

	bool DxVertexBuffer::Create()
	{
		return true;
	}

	void DxVertexBuffer::Destroy()
	{
		DxBufferObject< uint8_t, ID3D11Buffer >::DoDestroy();
	}

	void DxVertexBuffer::Cleanup()
	{
		ReleaseTracked( m_pBuffer->GetOwner()->GetRenderSystem(), m_pDxDeclaration );
		DxBufferObject< uint8_t, ID3D11Buffer >::DoCleanup();
	}

	bool DxVertexBuffer::Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature, ShaderProgramBaseSPtr p_pProgram )
	{
		bool l_return = ( m_pBuffer != NULL );
		m_pProgram = std::static_pointer_cast< DxShaderProgram >( p_pProgram );

		if ( l_return )
		{
			m_pBuffer->Unassign();
			// Création de la déclaration
			l_return = DoCreateLayout();

			// Création du vertex buffer
			if ( l_return )
			{
				l_return = DoCreateBuffer( p_type, p_eNature );
			}

			// Remplissage du buffer si besoin est
			if ( l_return )
			{
				if ( p_type != eBUFFER_ACCESS_TYPE_STATIC )
				{
					UINT l_uiSize = UINT( m_pBuffer->GetSize() );

					if ( l_uiSize )
					{
						uint8_t * l_pBuffer = Lock( 0, l_uiSize, eACCESS_TYPE_WRITE );

						if ( l_pBuffer )
						{
							memcpy( l_pBuffer, m_pBuffer->data(), l_uiSize * sizeof( uint8_t ) );
							Unlock();
						}
					}
				}

				m_pBuffer->Assign();
			}
		}

		return l_return;
	}

	bool DxVertexBuffer::Bind()
	{
		bool l_return = m_pBuffer && m_pBuffer->IsAssigned() && m_pBufferObject;

		if ( l_return )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pBuffer->GetOwner()->GetRenderSystem()->GetCurrentContext() )->GetDeviceContext();
			UINT l_uiStrides[1] = { m_declaration.GetStride() };
			UINT l_uiOffsets[1] = { 0 };
			ID3D11Buffer * l_pBuffers[1] = { m_pBufferObject };
			l_pDeviceContext->IASetVertexBuffers( 0, 1, l_pBuffers, l_uiStrides, l_uiOffsets );
			l_pDeviceContext->IASetInputLayout( m_pDxDeclaration );
		}

		return l_return;
	}

	void DxVertexBuffer::Unbind()
	{
	}

	uint8_t * DxVertexBuffer::Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags )
	{
		return DxBufferObject< uint8_t, ID3D11Buffer >::DoLock( p_uiOffset, p_uiCount, p_uiFlags );
	}

	void DxVertexBuffer::Unlock()
	{
		DxBufferObject< uint8_t, ID3D11Buffer >::DoUnlock();
	}

	bool DxVertexBuffer::DoCreateLayout()
	{
		DxShaderProgramSPtr l_pProgram = m_pProgram.lock();
		bool l_return = false;

		if ( l_pProgram )
		{
			std::vector< D3D11_INPUT_ELEMENT_DESC >	l_arrayDxElements;
			UINT l_uiPosition = 0;
			UINT l_uiNormal = 0;
			UINT l_uiTangent = 0;
			UINT l_uiBinormal = 0;
			UINT l_uiColour = 0;
			UINT l_uiOffset = 0;

			for ( auto && l_element : m_declaration )
			{
				bool l_bAdd = true;
				D3D11_INPUT_ELEMENT_DESC l_d3dCurrentElement = { 0 };
				l_d3dCurrentElement.InputSlot = l_element.m_uiStream;
				l_d3dCurrentElement.AlignedByteOffset = l_uiOffset;//l_it->m_uiOffset;
				l_d3dCurrentElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				l_d3dCurrentElement.InstanceDataStepRate = 0;
				l_uiOffset = D3D11_APPEND_ALIGNED_ELEMENT;

				switch ( l_element.m_eUsage )
				{
				case eELEMENT_USAGE_POSITION:
					l_d3dCurrentElement.SemanticName = "POSITION";
					l_d3dCurrentElement.SemanticIndex = l_uiPosition++;
					break;

				case eELEMENT_USAGE_NORMAL:
					l_d3dCurrentElement.SemanticName = "NORMAL";
					l_d3dCurrentElement.SemanticIndex = l_uiNormal++;
					break;

				case eELEMENT_USAGE_TANGENT:
					l_d3dCurrentElement.SemanticName = "TANGENT";
					l_d3dCurrentElement.SemanticIndex = l_uiTangent++;
					break;

				case eELEMENT_USAGE_BITANGENT:
					l_d3dCurrentElement.SemanticName = "BINORMAL";
					l_d3dCurrentElement.SemanticIndex = l_uiBinormal++;
					break;

				case eELEMENT_USAGE_DIFFUSE:
					l_d3dCurrentElement.SemanticName = "COLOR";
					l_d3dCurrentElement.SemanticIndex = l_uiColour++;
					break;

				case eELEMENT_USAGE_TEXCOORDS0:
					l_d3dCurrentElement.SemanticName = "TEXCOORD";
					l_d3dCurrentElement.SemanticIndex = 0;
					break;

				case eELEMENT_USAGE_TEXCOORDS1:
					l_d3dCurrentElement.SemanticName = "TEXCOORD";
					l_d3dCurrentElement.SemanticIndex = 1;
					break;

				case eELEMENT_USAGE_TEXCOORDS2:
					l_d3dCurrentElement.SemanticName = "TEXCOORD";
					l_d3dCurrentElement.SemanticIndex = 2;
					break;

				case eELEMENT_USAGE_TEXCOORDS3:
					l_d3dCurrentElement.SemanticName = "TEXCOORD";
					l_d3dCurrentElement.SemanticIndex = 3;
					break;

				//case eELEMENT_USAGE_BONE_IDS:
				//	l_d3dCurrentElement.SemanticName = "BLENDINDICES";
				//	l_d3dCurrentElement.SemanticIndex = 0;
				//	break;

				//case eELEMENT_USAGE_BONE_WEIGHTS:
				//	l_d3dCurrentElement.SemanticName = "BLENDWEIGHT";
				//	l_d3dCurrentElement.SemanticIndex = 0;
				//	break;

				default:
					l_bAdd	= false;
				}

				switch ( l_element.m_eDataType )
				{
				case eELEMENT_TYPE_1FLOAT:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32_FLOAT;
					break;

				case eELEMENT_TYPE_2FLOATS:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32_FLOAT;
					break;

				case eELEMENT_TYPE_3FLOATS:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					break;

				case eELEMENT_TYPE_4FLOATS:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					break;

				case eELEMENT_TYPE_COLOUR:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32_UINT;
					break;

				case eELEMENT_TYPE_1INT:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32_UINT;
					break;

				case eELEMENT_TYPE_2INTS:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32_UINT;
					break;

				case eELEMENT_TYPE_3INTS:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32_UINT;
					break;

				case eELEMENT_TYPE_4INTS:
					l_d3dCurrentElement.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					break;
				}

				if ( l_bAdd )
				{
					l_arrayDxElements.push_back( l_d3dCurrentElement );
				}
			}

			ID3DBlob * l_pBlob = l_pProgram->GetCompiled( eSHADER_TYPE_VERTEX );

			if ( l_pBlob )
			{
				DxRenderSystem * l_renderSystem = reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetOwner()->GetRenderSystem() );
				HRESULT l_hr = l_renderSystem->GetDevice()->CreateInputLayout( &l_arrayDxElements[0], UINT( l_arrayDxElements.size() ), reinterpret_cast< DWORD const * >( l_pBlob->GetBufferPointer() ), l_pBlob->GetBufferSize(), &m_pDxDeclaration );
				dxTrack( l_renderSystem, m_pDxDeclaration, VtxInputLayout );
				l_return = dxCheckError( l_hr, "ID3D11Device::CreateInputLayout" );
			}
		}

		return l_return;
	}

	bool DxVertexBuffer::DoCreateBuffer( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_eNature )
	{
		bool l_return = true;
		UINT l_uiSize = UINT( m_pBuffer->GetSize() );
		m_type = p_type;
		m_eNature = p_eNature;

		if ( l_uiSize )
		{
			HRESULT l_hr;
			D3D11_BUFFER_DESC l_desc = { 0 };
			l_desc.ByteWidth = l_uiSize * sizeof( uint8_t );
			l_desc.Usage = DirectX11::Get( p_type );
			l_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			l_desc.CPUAccessFlags = DirectX11::GetCpuAccessFlags( p_type | p_eNature );
			l_desc.MiscFlags = 0;
			l_desc.StructureByteStride = 0;//m_declaration.GetStride();
			DxRenderSystem * l_renderSystem = reinterpret_cast< DxRenderSystem * >( m_pBuffer->GetOwner()->GetRenderSystem() );

			if ( p_type == eBUFFER_ACCESS_TYPE_STATIC )
			{
				D3D11_SUBRESOURCE_DATA l_data = { 0 };
				l_data.pSysMem = m_pBuffer->data();
				l_hr = l_renderSystem->GetDevice()->CreateBuffer( &l_desc, &l_data, &m_pBufferObject );
				dxTrack( l_renderSystem, m_pBufferObject, VertexBuffer );
			}
			else
			{
				l_hr = l_renderSystem->GetDevice()->CreateBuffer( &l_desc, NULL, &m_pBufferObject );
				dxTrack( l_renderSystem, m_pBufferObject, VertexBuffer );
			}

			l_return = dxCheckError( l_hr, "ID3D11Device::CreateBuffer" );
		}

		return l_return;
	}
}
