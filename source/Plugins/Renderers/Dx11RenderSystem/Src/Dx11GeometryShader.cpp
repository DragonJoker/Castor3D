#include "Dx11GeometryShader.hpp"

#include "Dx11RenderSystem.hpp"
#include "Dx11FrameVariableBuffer.hpp"

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Dx11Render
{
	DxGeometryShader::DxGeometryShader( DxShaderProgram * p_parent )
		: DxShaderObject( p_parent, eSHADER_TYPE_GEOMETRY )
		, m_pGeometryShader( NULL )
	{
	}

	DxGeometryShader::~DxGeometryShader()
	{
		ReleaseTracked( m_renderSystem, m_pGeometryShader );
	}

	void DxGeometryShader::DoBind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		l_pDeviceContext->GSSetShader( m_pGeometryShader, NULL, 0 );
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_GEOMETRY );

		if ( !l_ubos.empty() )
		{
			std::vector< ID3D11Buffer * > l_buffers;
			l_buffers.reserve( l_ubos.size() );

			for ( auto l_variableBuffer : m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_GEOMETRY ) )
			{
				l_buffers.push_back( std::static_pointer_cast< DxFrameVariableBuffer >( l_variableBuffer )->GetDxBuffer() );
			}

			if ( m_pShaderProgram->HasObject( eSHADER_TYPE_GEOMETRY ) )
			{
				l_pDeviceContext->GSSetConstantBuffers( 0, l_buffers.size(), l_buffers.data() );
			}
		}
	}

	void DxGeometryShader::DoUnbind()
	{
		ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_renderSystem->GetCurrentContext() )->GetDeviceContext();
		auto l_ubos = m_pShaderProgram->GetFrameVariableBuffers( eSHADER_TYPE_GEOMETRY );

		if ( !l_ubos.empty() )
		{
			if ( m_pShaderProgram->HasObject( eSHADER_TYPE_GEOMETRY ) )
			{
				ID3D11Buffer * l_buffer = NULL;
				l_pDeviceContext->GSSetConstantBuffers( 0, 1, &l_buffer );
			}
		}

		l_pDeviceContext->GSSetShader( NULL, NULL, 0 );
	}

	void DxGeometryShader::AttachTo( ShaderProgramBase & p_program )
	{
		if ( m_pCompiled )
		{
			ID3D11Device * l_pDevice = m_renderSystem->GetDevice();

			if ( l_pDevice )
			{
				HRESULT l_hr = l_pDevice->CreateGeometryShader( reinterpret_cast< DWORD * >( m_pCompiled->GetBufferPointer() ), m_pCompiled->GetBufferSize(), NULL, &m_pGeometryShader );
				dxTrack( m_renderSystem, m_pGeometryShader, GSShader );

				if ( l_hr == S_OK )
				{
					m_eStatus = eSHADER_STATUS_COMPILED;
				}
			}
		}
	}

	void DxGeometryShader::Detach()
	{
		ReleaseTracked( m_renderSystem, m_pGeometryShader );
	}
}
