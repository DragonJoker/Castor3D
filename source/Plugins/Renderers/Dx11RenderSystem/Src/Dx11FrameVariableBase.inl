#include "Dx11RenderSystem.hpp"
#include "Dx11ShaderProgram.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11StaticTexture.hpp"

namespace Dx11Render
{
	template< typename T > struct OneVariableBinder
	{
		static void Bind( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "OneVariableBinder - Unsupported arguments" );
		}
		static void Unbind( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "OneVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Count > struct PointVariableBinder
	{
		static void Bind( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "PointVariableBinder - Unsupported arguments" );
		}
		static void Unbind( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "PointVariableBinder - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Rows, uint32_t Columns > struct MatrixVariableBinder
	{
		static void Bind( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableBinder - Unsupported arguments" );
		}
		static void Unbind( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableBinder - Unsupported arguments" );
		}
	};

	template <> struct OneVariableBinder< Castor3D::TextureBaseRPtr >
	{
		static void Bind( DxShaderProgram & p_program, Castor3D::TextureBaseRPtr const * p_pValue )
		{
			HRESULT l_hr = E_NOTIMPL;

			if ( p_program.GetStatus() == Castor3D::ePROGRAM_STATUS_LINKED && *p_pValue )
			{
				Castor3D::TextureBaseRPtr l_pTexture = ( *p_pValue );
				ID3D11ShaderResourceView * l_pResourceView = NULL;
				DxContext * l_pDxContext = static_cast< DxContext * >( p_program.GetOwner()->GetCurrentContext() );
				ID3D11DeviceContext * l_deviceContext = l_pDxContext->GetDeviceContext();

				if ( l_pTexture )
				{
					if ( l_pTexture->GetBaseType() == Castor3D::eTEXTURE_BASE_TYPE_STATIC )
					{
						l_pResourceView = static_cast< DxStaticTexture * >( l_pTexture )->GetShaderResourceView();
					}
					else
					{
						l_pResourceView = static_cast< DxDynamicTexture * >( l_pTexture )->GetShaderResourceView();
					}
				}

				l_deviceContext->PSSetShaderResources( l_pTexture->GetIndex(), 1, &l_pResourceView );
				l_hr = S_OK;
			}
			else if ( !( *p_pValue ) )
			{
				l_hr = S_OK;
			}

			dxCheckError( l_hr, "ApplyVariable texture" );
		}

		static void Unbind( DxShaderProgram & p_program, Castor3D::TextureBaseRPtr const * p_pValue )
		{
			HRESULT l_hr = E_NOTIMPL;

			if ( p_program.GetStatus() == Castor3D::ePROGRAM_STATUS_LINKED && *p_pValue )
			{
				Castor3D::TextureBaseRPtr l_pTexture = ( *p_pValue );
				ID3D11ShaderResourceView * l_pResourceView = NULL;
				ID3D11DeviceContext * l_deviceContext = static_cast< DxContext * >( p_program.GetOwner()->GetCurrentContext() )->GetDeviceContext();
				l_deviceContext->PSSetShaderResources( l_pTexture->GetIndex(), 1, &l_pResourceView );
				l_hr = S_OK;
			}
			else if ( !( *p_pValue ) )
			{
				l_hr = S_OK;
			}

			dxCheckError( l_hr, "ApplyVariable texture" );
		}
	};

	template< typename Type >
	inline void DxFrameVariableBase::DoBind( DxShaderProgram & p_program, Type * p_pValue )
	{
		OneVariableBinder< Type >::Bind( p_program, p_pValue );
	}
	template< typename Type, uint32_t Count >
	inline void DxFrameVariableBase::DoBind( DxShaderProgram & p_program, Type * p_pValue )
	{
		PointVariableBinder< Type, Count >::Bind( p_program, p_pValue );
	}
	template< typename Type, uint32_t Rows, uint32_t Columns >
	inline void DxFrameVariableBase::DoBind( DxShaderProgram & p_program, Type * p_pValue )
	{
		MatrixVariableBinder< Type, Rows, Columns >::Bind( p_program, p_pValue );
	}

	template< typename Type >
	inline void DxFrameVariableBase::DoUnbind( DxShaderProgram & p_program, Type * p_pValue )
	{
		OneVariableBinder< Type >::Unbind( p_program, p_pValue );
	}
	template< typename Type, uint32_t Count >
	inline void DxFrameVariableBase::DoUnbind( DxShaderProgram & p_program, Type * p_pValue )
	{
		PointVariableBinder< Type, Count >::Unbind( p_program, p_pValue );
	}
	template< typename Type, uint32_t Rows, uint32_t Columns >
	inline void DxFrameVariableBase::DoUnbind( DxShaderProgram & p_program, Type * p_pValue )
	{
		MatrixVariableBinder< Type, Rows, Columns >::Unbind( p_program, p_pValue );
	}
}
