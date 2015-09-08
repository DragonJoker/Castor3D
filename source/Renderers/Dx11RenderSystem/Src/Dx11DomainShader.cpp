#include "Dx11DomainShader.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameVariableBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxDomainShader::DxDomainShader( DxShaderProgram * p_pParent )
		: DxShaderObject( p_pParent, eSHADER_TYPE_DOMAIN )
		, m_pDomainShader( NULL )
	{
	}

	DxDomainShader::~DxDomainShader()
	{
		ReleaseTracked( m_pRenderSystem, m_pDomainShader );
	}

	void DxDomainShader::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->DSSetShader( m_pDomainShader, NULL, 0 );
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_DOMAIN );

		if ( !l_ubos.empty() )
		{
			std::vector< ID3D11Buffer * > l_buffers;
			l_buffers.reserve( l_ubos.size() );

			for ( auto l_variableBuffer : l_ubos )
			{
				l_buffers.push_back( std::static_pointer_cast< DxFrameVariableBuffer >( l_variableBuffer )->GetDxBuffer() );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_DOMAIN ) )
			{
				l_pDeviceContext->DSSetConstantBuffers( 0, l_buffers.size(), l_buffers.data() );
			}
		}
	}

	void DxDomainShader::DoUnbind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_DOMAIN );

		if ( !l_ubos.empty() )
		{
			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_DOMAIN ) )
			{
				ID3D11Buffer * l_buffer = NULL;
				l_pDeviceContext->DSSetConstantBuffers( 0, 1, &l_buffer );
			}
		}

		l_pDeviceContext->DSSetShader( NULL, NULL, 0 );
	}

	void DxDomainShader::AttachTo( ShaderProgramBase & p_program )
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_pRenderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateDomainShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pDomainShader );
				dxDebugName( m_pRenderSystem, m_pDomainShader, DSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}

	void DxDomainShader::Detach()
	{
		ReleaseTracked( m_pRenderSystem, m_pDomainShader );
	}
}
