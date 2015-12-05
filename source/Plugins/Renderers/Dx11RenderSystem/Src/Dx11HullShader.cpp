#include "Dx11HullShader.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameVariableBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxHullShader::DxHullShader( DxShaderProgram * p_parent )
		: DxShaderObject( p_parent, eSHADER_TYPE_HULL )
		, m_pHullShader( NULL )
	{
	}

	DxHullShader::~DxHullShader()
	{
		ReleaseTracked( m_renderSystem, m_pHullShader );
	}

	void DxHullShader::DoBind()
	{
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_deviceContext->HSSetShader( m_pHullShader, NULL, 0 );
		auto l_ubos = m_shaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_HULL );

		if ( !l_ubos.empty() )
		{
			std::vector< ID3D11Buffer * > l_buffers;
			l_buffers.reserve( l_ubos.size() );

			for ( auto l_variableBuffer : m_shaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_HULL ) )
			{
				l_buffers.push_back( std::static_pointer_cast< DxFrameVariableBuffer >( l_variableBuffer )->GetDxBuffer() );
			}

			if ( m_shaderProgram->HasObject( eSHADER_TYPE_HULL ) )
			{
				l_deviceContext->HSSetConstantBuffers( 0, l_buffers.size(), l_buffers.data() );
			}
		}
	}

	void DxHullShader::DoUnbind()
	{
		ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		auto l_ubos = m_shaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_HULL );

		if ( !l_ubos.empty() )
		{
			if ( m_shaderProgram->HasObject( eSHADER_TYPE_HULL ) )
			{
				ID3D11Buffer * l_buffer = NULL;
				l_deviceContext->HSSetConstantBuffers( 0, 1, &l_buffer );
			}
		}

		l_deviceContext->HSSetShader( NULL, NULL, 0 );
	}

	void DxHullShader::AttachTo( ShaderProgramBase & p_program )
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_renderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateHullShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pHullShader );
				dxTrack( m_renderSystem, m_pHullShader, HSShader );

				if ( l_hr == S_OK )
				{
					m_status = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}

	void DxHullShader::Detach()
	{
		ReleaseTracked( m_renderSystem, m_pHullShader );
	}
}
