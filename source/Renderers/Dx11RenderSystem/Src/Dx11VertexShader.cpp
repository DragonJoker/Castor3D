#include "Dx11VertexShader.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameVariableBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxVertexShader::DxVertexShader( DxShaderProgram * p_pParent )
		: DxShaderObject( p_pParent, eSHADER_TYPE_VERTEX )
		, m_pVertexShader( NULL )
	{
	}

	DxVertexShader::~DxVertexShader()
	{
	}

	void DxVertexShader::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->VSSetShader( m_pVertexShader, NULL, 0 );
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_VERTEX );

		if ( !l_ubos.empty() )
		{
			std::vector< ID3D11Buffer * > l_buffers;
			l_buffers.reserve( l_ubos.size() );

			for ( auto l_variableBuffer : m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_VERTEX ) )
			{
				l_buffers.push_back( std::static_pointer_cast< DxFrameVariableBuffer >( l_variableBuffer )->GetDxBuffer() );
			}

			if ( m_pShaderProgram->HasObject( eSHADER_TYPE_VERTEX ) )
			{
				l_pDeviceContext->VSSetConstantBuffers( 0, l_buffers.size(), l_buffers.data() );
			}
		}
	}

	void DxVertexShader::DoUnbind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_VERTEX );

		if ( !l_ubos.empty() )
		{
			if ( m_pShaderProgram->HasObject( eSHADER_TYPE_VERTEX ) )
			{
				ID3D11Buffer * l_buffer = NULL;
				l_pDeviceContext->VSSetConstantBuffers( 0, 1, &l_buffer );
			}
		}

		l_pDeviceContext->VSSetShader( NULL, NULL, 0 );
	}

	void DxVertexShader::AttachTo( ShaderProgramBase & p_program )
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_renderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateVertexShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pVertexShader );
				dxTrack( m_renderSystem, m_pVertexShader, VSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}

	void DxVertexShader::Detach()
	{
		ReleaseTracked( m_renderSystem, m_pVertexShader );
	}
}
