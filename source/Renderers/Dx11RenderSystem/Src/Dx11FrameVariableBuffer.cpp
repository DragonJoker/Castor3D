#include "Dx11FrameVariableBuffer.hpp"
#include "Dx11OneFrameVariable.hpp"
#include "Dx11PointFrameVariable.hpp"
#include "Dx11MatrixFrameVariable.hpp"
#include "Dx11RenderSystem.hpp"
#include "Dx11ShaderProgram.hpp"

#include <Logger.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Dx11Render
{
	namespace
	{
		template< eFRAME_VARIABLE_TYPE Type > FrameVariableSPtr DxFrameVariableCreator( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc );
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable< int > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable< uint32_t > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable< float > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable< double > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable< TextureBaseRPtr > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< int, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< int, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< int, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< uint32_t, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< uint32_t, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< uint32_t, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< float, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< float, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< float, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< double, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< double, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable< double, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 2, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 2, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 2, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 3, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 3, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 3, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 4, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 4, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< float, 4, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 2, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 2, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 2, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 3, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 3, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 3, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 4, 2 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 4, 3 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable< double, 4, 4 > >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
	}

	DxFrameVariableBuffer::DxFrameVariableBuffer( String const & p_strName, DxRenderSystem * p_pRenderSystem )
		: FrameVariableBuffer( p_strName, p_pRenderSystem )
		, m_pDxRenderSystem( p_pRenderSystem )
		, m_pDxBuffer( NULL )
	{
	}

	DxFrameVariableBuffer::~DxFrameVariableBuffer()
	{
	}

	bool DxFrameVariableBuffer::Bind( uint32_t p_uiIndex )
	{
		if ( m_pDxBuffer )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
			D3D11_MAPPED_SUBRESOURCE l_mapped = { 0 };
			HRESULT l_hr = l_pDeviceContext->Map( m_pDxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &l_mapped );

			if ( l_hr == S_OK )
			{
				memcpy( l_mapped.pData, &m_buffer[0], m_buffer.size() );
				l_pDeviceContext->Unmap( m_pDxBuffer, 0 );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_VERTEX ) )
			{
				l_pDeviceContext->VSSetConstantBuffers( p_uiIndex, 1, &m_pDxBuffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_PIXEL ) )
			{
				l_pDeviceContext->PSSetConstantBuffers( p_uiIndex, 1, &m_pDxBuffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_GEOMETRY ) )
			{
				l_pDeviceContext->GSSetConstantBuffers( p_uiIndex, 1, &m_pDxBuffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_HULL ) )
			{
				l_pDeviceContext->HSSetConstantBuffers( p_uiIndex, 1, &m_pDxBuffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_DOMAIN ) )
			{
				l_pDeviceContext->DSSetConstantBuffers( p_uiIndex, 1, &m_pDxBuffer );
			}
		}

		return true;
	}

	void DxFrameVariableBuffer::Unbind( uint32_t p_uiIndex )
	{
		if ( m_pDxBuffer )
		{
			ID3D11DeviceContext * l_pDeviceContext = static_cast< DxContext * >( m_pRenderSystem->GetCurrentContext() )->GetDeviceContext();
			ID3D11Buffer * buffer[] = { NULL };

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_VERTEX ) )
			{
				l_pDeviceContext->VSSetConstantBuffers( p_uiIndex, 1, buffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_PIXEL ) )
			{
				l_pDeviceContext->PSSetConstantBuffers( p_uiIndex, 1, buffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_GEOMETRY ) )
			{
				l_pDeviceContext->GSSetConstantBuffers( p_uiIndex, 1, buffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_HULL ) )
			{
				l_pDeviceContext->HSSetConstantBuffers( p_uiIndex, 1, buffer );
			}

			if ( m_pShaderProgram->HasProgram( eSHADER_TYPE_DOMAIN ) )
			{
				l_pDeviceContext->DSSetConstantBuffers( p_uiIndex, 1, buffer );
			}
		}
	}

	FrameVariableSPtr DxFrameVariableBuffer::DoCreateVariable( ShaderProgramBase * p_pProgram, eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc )
	{
		FrameVariableSPtr l_pReturn;
		DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( p_pProgram );

		switch ( p_eType )
		{
		case eFRAME_VARIABLE_TYPE_INT:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_INT >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_UINT:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_UINT >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_FLOAT:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_DOUBLE:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_DOUBLE >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_SAMPLER:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2I:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3I:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4I:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2UI:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2UI >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3UI:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3UI >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4UI:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4UI >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC2D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC3D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_VEC4D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4F:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X2D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X2D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X3D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X3D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT2X4D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT2X4D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X2D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X2D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X3D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X3D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT3X4D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT3X4D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X2D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X2D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X3D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X3D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4D:
			l_pReturn = DxFrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4D >( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			break;
		}

		if ( l_pReturn )
		{
			l_pReturn->SetName( p_strName );
		}

		return l_pReturn;
	}

	bool DxFrameVariableBuffer::DoInitialise( ShaderProgramBase * p_pProgram )
	{
		HRESULT l_hr = S_OK;
		m_pShaderProgram = reinterpret_cast< DxShaderProgram * >( p_pProgram );

		if ( m_listVariables.size() )
		{
			ID3D11Device * l_pDevice = m_pDxRenderSystem->GetDevice();
			D3D11_BUFFER_DESC l_d3dBufferDesc = { 0 };
			uint32_t l_uiTotalSize = 0;

			for ( auto && l_variable : m_listVariables )
			{
				l_uiTotalSize += l_variable->size();

				if ( l_uiTotalSize % 16 )
				{
					// We must align on 16 bytes...
					l_uiTotalSize += 16 - ( l_uiTotalSize % 16 );
				}
			}

			m_buffer.resize( l_uiTotalSize );
			l_uiTotalSize = 0;

			for ( auto && l_variable : m_listVariables )
			{
				l_variable->link( &m_buffer[l_uiTotalSize] );
				l_uiTotalSize += l_variable->size();

				if ( l_uiTotalSize % 16 )
				{
					// We must align on 16 bytes...
					l_uiTotalSize += 16 - ( l_uiTotalSize % 16 );
				}
			}

			l_d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			l_d3dBufferDesc.ByteWidth = l_uiTotalSize;
			l_d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			l_d3dBufferDesc.MiscFlags = 0;
			l_d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			l_d3dBufferDesc.StructureByteStride = 0;
			l_hr = l_pDevice->CreateBuffer( &l_d3dBufferDesc, NULL, &m_pDxBuffer );
			dxDebugName( m_pDxBuffer, ConstantBuffer );
		}

		return l_hr == S_OK;
	}

	void DxFrameVariableBuffer::DoCleanup()
	{
		SafeRelease( m_pDxBuffer );
	}

	bool DxFrameVariableBuffer::DoBind()
	{
		return true;
	}

	void DxFrameVariableBuffer::DoUnbind()
	{
	}
}
