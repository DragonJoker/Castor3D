#include "Dx11RenderSystem.hpp"
#include "Dx11ShaderProgram.hpp"
#include "Dx11DynamicTexture.hpp"
#include "Dx11StaticTexture.hpp"

namespace Dx11Render
{
	template< typename T > struct OneVariableApplier
	{
		inline void operator()( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "OneVariableApplier - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Count > struct PointVariableApplier
	{
		inline void operator()( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "PointVariableApplier - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplier
	{
		inline void operator()( DxShaderProgram & p_program, T const * CU_PARAM_UNUSED( p_pValue ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableApplier - Unsupported arguments" );
		}
	};

	template <> struct OneVariableApplier< Castor3D::TextureBaseRPtr >
	{
		inline void operator()( DxShaderProgram & p_program, Castor3D::TextureBaseRPtr const * p_pValue )
		{
			HRESULT l_hr = E_NOTIMPL;

			if ( p_program.GetStatus() == Castor3D::ePROGRAM_STATUS_LINKED && *p_pValue )
			{
				Castor3D::TextureBaseRPtr l_pTexture = ( *p_pValue );
				l_pTexture->Bind();
				ID3D11ShaderResourceView * l_pResourceView = NULL;
				DxRenderSystem * l_pDxRS = reinterpret_cast< DxRenderSystem * >( p_program.GetRenderSystem() );

				if ( l_pTexture )
				{
					if ( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
					{
						l_pResourceView = static_cast< DxStaticTexture * >( l_pTexture )->GetShaderResourceView();
					}
					else
					{
						l_pResourceView = static_cast< DxDynamicTexture * >( l_pTexture )->GetShaderResourceView();
					}
				}

				ID3D11DeviceContext * l_pDeviceContext;
				l_pDxRS->GetDevice()->GetImmediateContext( &l_pDeviceContext );
				l_pDeviceContext->PSSetShaderResources( l_pTexture->GetIndex(), 1, &l_pResourceView );
				l_pDeviceContext->Release();
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
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Type * p_pValue )
	{
		OneVariableApplier< Type >()( p_program, p_pValue );
	}
	template< typename Type, uint32_t Count >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Type * p_pValue )
	{
		PointVariableApplier< Type, Count >()( p_program, p_pValue );
	}
	template< typename Type, uint32_t Rows, uint32_t Columns >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Type * p_pValue )
	{
		MatrixVariableApplier< Type, Rows, Columns >()( p_program, p_pValue );
	}
}
