#include "Dx9StaticTexture.hpp"
#include "Dx9DynamicTexture.hpp"

namespace Dx9Render
{
	template<> struct OneVariableApplyer< Castor3D::TextureBaseSPtr >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, Castor3D::TextureBaseSPtr const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				IDirect3DTexture9 * l_pDxTexture = NULL;
				Castor3D::TextureBaseSPtr l_pTexture = ( *p_pValue );

				if ( l_pTexture )
				{
					if ( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
					{
						l_pDxTexture = std::static_pointer_cast< DxStaticTexture >( l_pTexture )->GetDxTexture();
					}
					else
					{
						l_pDxTexture = std::static_pointer_cast< DxDynamicTexture >( l_pTexture )->GetDxTexture();
					}
				}

				l_hr = p_pEffect->SetTexture( LPCSTR( p_handle ), l_pDxTexture );
			}
			else
			{
				l_hr = E_NOTIMPL;
			}

			dxCheckError( l_hr, "ApplyVariable texture" );
		}
	};

	template<> struct OneVariableApplyer< int >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				l_hr = p_pEffect->SetInt( p_handle, *p_pValue );
			}
			else
			{
				l_hr = p_pEffect->SetIntArray( p_handle, p_pValue, p_uiOcc );
			}

			dxCheckError( l_hr, "ApplyVariable int" );
		}
	};

	template<> struct OneVariableApplyer< float >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				l_hr = p_pEffect->SetFloat( p_handle, *p_pValue );
			}
			else
			{
				l_hr = p_pEffect->SetFloatArray( p_handle, p_pValue, p_uiOcc );
			}

			dxCheckError( l_hr, "ApplyVariable float" );
		}
	};

	template<> struct PointVariableApplyer< int, 2 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;
			l_hr = p_pEffect->SetIntArray( p_handle, p_pValue, p_uiOcc * 2 );
			dxCheckError( l_hr, "ApplyVariable 2 int" );
		}
	};

	template<> struct PointVariableApplyer< int, 3 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;
			l_hr = p_pEffect->SetIntArray( p_handle, p_pValue, p_uiOcc * 3 );
			dxCheckError( l_hr, "ApplyVariable 3 ints" );
		}
	};

	template<> struct PointVariableApplyer< int, 4 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;
			l_hr = p_pEffect->SetIntArray( p_handle, p_pValue, p_uiOcc * 4 );
			dxCheckError( l_hr, "ApplyVariable 4 ints" );
		}
	};

	template<> struct PointVariableApplyer< float, 2 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;
			l_hr = p_pEffect->SetFloatArray( p_handle, p_pValue, p_uiOcc * 2 );
			dxCheckError( l_hr, "ApplyVariable 2 floats" );
		}
	};

	template<> struct PointVariableApplyer< float, 3 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;
			l_hr = p_pEffect->SetFloatArray( p_handle, p_pValue, p_uiOcc * 3 );
			dxCheckError( l_hr, "ApplyVariable 3 floats" );
		}
	};

	template<> struct PointVariableApplyer< float, 4 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				D3DXVECTOR4 l_vector( p_pValue );
				l_hr = p_pEffect->SetVector( p_handle, &l_vector );
			}
			else
			{
				std::vector <D3DXVECTOR4> l_arrayVectors( p_uiOcc );

				for ( uint32_t i = 0; i < p_uiOcc; i++ )
				{
					l_arrayVectors[i] = D3DXVECTOR4( p_pValue + ( i * 4 ) );
				}

				l_hr = p_pEffect->SetVectorArray( p_handle, &l_arrayVectors[0], p_uiOcc );
			}

			dxCheckError( l_hr, "ApplyVariable 4 floats" );
		}
	};

	template<> struct MatrixVariableApplyer< float, 4, 4 >
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				D3DXMATRIX l_matrix( p_pValue );
				l_hr = p_pEffect->SetMatrix( p_handle, &l_matrix );
			}
			else
			{
				std::vector <D3DXMATRIX> l_arrayMatrices( p_uiOcc );

				for ( uint32_t i = 0; i < p_uiOcc; i++ )
				{
					l_arrayMatrices[i] = D3DXMATRIX( p_pValue + ( i * 16 ) );
				}

				l_hr = p_pEffect->SetMatrixArray( p_handle, &l_arrayMatrices[0], p_uiOcc );
			}

			dxCheckError( l_hr, "ApplyVariable 4x4 floats" );
		}
	};

	template< typename T > struct OneVariableApplyer
	{
		inline void operator()( ID3DXEffect * CU_PARAM_UNUSED( p_pEffect ), D3DXHANDLE CU_PARAM_UNUSED( p_handle ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "OneVariableApplyer - Unsupported arguments" );
		}
	};

	template <typename T, uint32_t Count> struct PointVariableApplyer
	{
		inline void operator()( ID3DXEffect * CU_PARAM_UNUSED( p_pEffect ), D3DXHANDLE CU_PARAM_UNUSED( p_handle ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "PointVariableApplyer - Unsupported arguments" );
		}
	};

	template <typename T, uint32_t Rows, uint32_t Columns> struct MatrixVariableApplyer
	{
		inline void operator()( ID3DXEffect * CU_PARAM_UNUSED( p_pEffect ), D3DXHANDLE CU_PARAM_UNUSED( p_handle ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableApplyer - Unsupported arguments" );
		}
	};

	//***********************************************************************************************************************

	template< typename Type >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_d3dxHandle )
		{
			OneVariableApplyer< Type >()( p_program.GetShaderEffect(), m_d3dxHandle, p_pValue, p_uiOcc );
		}
	}
	template< typename Type, int Count >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_d3dxHandle )
		{
			PointVariableApplyer< Type, Count >()( p_program.GetShaderEffect(), m_d3dxHandle, p_pValue, p_uiOcc );
		}
	}
	template< typename Type, int Rows, int Columns >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc )
	{
		if ( m_d3dxHandle )
		{
			MatrixVariableApplyer< Type, Rows, Columns >()( p_program.GetShaderEffect(), m_d3dxHandle, p_pValue, p_uiOcc );
		}
	}
}
