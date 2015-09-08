#include "Dx11FragmentShader.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameVariableBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxFragmentShader::DxFragmentShader( DxShaderProgram * p_pParent )
		: DxShaderObject( p_pParent, eSHADER_TYPE_PIXEL )
		, m_pPixelShader( NULL )
	{
	}

	DxFragmentShader::~DxFragmentShader()
	{
		ReleaseTracked( m_pRenderSystem, m_pPixelShader );
	}

	void DxFragmentShader::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->PSSetShader( m_pPixelShader, NULL, 0 );
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_PIXEL );

		if ( !l_ubos.empty() )
		{
			std::vector< ID3D11Buffer * > l_buffers;
			l_buffers.reserve( l_ubos.size() );

			for ( auto l_variableBuffer : m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_PIXEL ) )
			{
				l_buffers.push_back( std::static_pointer_cast< DxFrameVariableBuffer >( l_variableBuffer )->GetDxBuffer() );
			}

			if ( m_pShaderProgram->HasObject( eSHADER_TYPE_PIXEL ) )
			{
				l_pDeviceContext->PSSetConstantBuffers( 0, l_buffers.size(), l_buffers.data() );
			}
		}
	}

	void DxFragmentShader::DoUnbind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_PIXEL );

		if ( !l_ubos.empty() )
		{
			if ( m_pShaderProgram->HasObject( eSHADER_TYPE_PIXEL ) )
			{
				ID3D11Buffer * l_buffer = NULL;
				l_pDeviceContext->PSSetConstantBuffers( 0, 1, &l_buffer );
			}
		}

		l_pDeviceContext->PSSetShader( NULL, NULL, 0 );
	}

	void DxFragmentShader::AttachTo( ShaderProgramBase & p_program )
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreatePixelShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pPixelShader );
				dxTrack( m_pRenderSystem, m_pPixelShader, PSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}

	void DxFragmentShader::Detach()
	{
		ReleaseTracked( m_pRenderSystem, m_pPixelShader );
	}
}
