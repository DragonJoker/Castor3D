#include "Dx10DynamicTexture.hpp"
#include "Dx10StaticTexture.hpp"

namespace Dx10Render
{
#if C3DDX10_USE_DXEFFECTS
	template<> struct OneVariableApplier< Castor3D::TextureBaseSPtr >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, Castor3D::TextureBaseSPtr const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				ID3D10ShaderResourceView * l_pDxTexture = NULL;
				Castor3D::TextureBaseSPtr l_pTexture = ( *p_pValue );

				if ( l_pTexture )
				{
					if ( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
					{
						l_pDxTexture = std::static_pointer_cast< Dx10StaticTexture >( l_pTexture )->GetDxTexture();
					}
					else
					{
						l_pDxTexture = std::static_pointer_cast< Dx10DynamicTexture >( l_pTexture )->GetDxTexture();
					}
				}

				l_hr = p_pVariable->AsShaderResource()->SetResource( l_pDxTexture );
				l_pDxTexture->Release();
			}
			else
			{
				l_hr = E_NOTIMPL;
			}

			dxCheckError( l_hr, "ApplyVariable texture" );
		}
	};

	template<> struct OneVariableApplier< int >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				l_hr = p_pVariable->AsScalar()->SetInt( *p_pValue );
			}
			else
			{
				l_hr = p_pVariable->AsScalar()->SetIntArray( const_cast< int * >( p_pValue ), 0, p_uiOcc ); //const_cast bouargl !!
			}

			dxCheckError( l_hr, "ApplyVariable int" );
		}
	};

	template<> struct OneVariableApplier< float >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				l_hr = p_pVariable->AsScalar()->SetFloat( *p_pValue );
			}
			else
			{
				l_hr = p_pVariable->AsScalar()->SetFloatArray( const_cast< float * >( p_pValue ), 0, p_uiOcc );
			}

			dxCheckError( l_hr, "ApplyVariable float" );
		}
	};

	template<> struct PointVariableApplier< int, 2 >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = p_pVariable->AsScalar()->SetIntArray( const_cast< int * >( p_pValue ), 0, p_uiOcc * 2 ); //const_cast bouargl !!
			dxCheckError( l_hr, "ApplyVariable 2 int" );
		}
	};

	template<> struct PointVariableApplier< int, 3 >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = p_pVariable->AsScalar()->SetIntArray( const_cast< int * >( p_pValue ), 0, p_uiOcc * 3 ); //const_cast bouargl !!
			dxCheckError( l_hr, "ApplyVariable 3 ints" );
		}
	};

	template<> struct PointVariableApplier< int, 4 >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, int const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = p_pVariable->AsScalar()->SetIntArray( const_cast< int * >( p_pValue ), 0, p_uiOcc * 4 ); //const_cast bouargl !!
			dxCheckError( l_hr, "ApplyVariable 4 ints" );
		}
	};

	template<> struct PointVariableApplier< float, 2 >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = p_pVariable->AsScalar()->SetFloatArray( const_cast< float * >( p_pValue ), 0, p_uiOcc * 2 ); //const_cast bouargl !!
			dxCheckError( l_hr, "ApplyVariable 2 floats" );
		}
	};

	template<> struct PointVariableApplier< float, 3 >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = p_pVariable->AsScalar()->SetFloatArray( const_cast< float * >( p_pValue ), 0, p_uiOcc * 3 ); //const_cast bouargl !!
			dxCheckError( l_hr, "ApplyVariable 3 floats" );
		}
	};

	template<> struct PointVariableApplier< float, 4 >
	{
		inline void operator()( ID3D10EffectVariable * p_pVariable, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = p_pVariable->AsScalar()->SetFloatArray( const_cast< float * >( p_pValue ), 0, p_uiOcc * 4 ); //const_cast bouargl !!
			dxCheckError( l_hr, "ApplyVariable 4 floats" );
		}
	};

	template<> struct MatrixVariableApplier< float, 4, 4 >
	{
		inline operator()( ID3D10EffectVariable * p_pVariable, float const * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr;

			if ( p_uiOcc <= 1 )
			{
				l_hr = p_pVariable->AsMatrix()->SetMatrix( const_cast< float * >( p_pValue ) ); //const_cast bouargl !!
			}
			else
			{
				l_hr = p_pVariable->AsMatrix()->SetMatrixArray( const_cast< float * >( p_pValue ), 0, p_uiOcc ); //const_cast bouargl !!
			}

			dxCheckError( l_hr, "ApplyVariable 4x4 floats" );
		}
	};

	template< typename T > struct OneVariableApplier
	{
		inline void operator()( ID3D10EffectVariable * CU_PARAM_UNUSED( p_pVariable ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments" );
		}
	};

	template< typename T, std::size_t Count > struct PointVariableApplier
	{
		inline void operator()( ID3D10EffectVariable * CU_PARAM_UNUSED( p_pVariable ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments" );
		}
	};

	template< typename T, std::size_t Rows, std::size_t Columns > struct MatrixVariableApplier
	{
		inline void operator()( ID3D10EffectVariable * CU_PARAM_UNUSED( p_pVariable ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "ApplyVariable - Unsupported arguments" );
		}
	};
#else
	template<> struct OneVariableApplier< Castor3D::TextureBaseRPtr >
	{
		inline void operator()( DxShaderProgram & p_program, Castor3D::TextureBaseRPtr * p_pValue, uint32_t p_uiOcc )
		{
			HRESULT l_hr = E_NOTIMPL;

			if ( p_program.GetStatus() == Castor3D::ePROGRAM_STATUS_LINKED && *p_pValue )
			{
				Castor3D::TextureBaseRPtr l_pTexture = *p_pValue;
				l_pTexture->Bind();
				ID3D10ShaderResourceView * l_pResourceView = NULL;
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

				l_pDxRS->GetDevice()->PSSetShaderResources( l_pTexture->GetIndex(), 1, &l_pResourceView );
				l_hr = S_OK;
			}
			else if ( !( *p_pValue ) )
			{
				l_hr = S_OK;
			}

			dxCheckError( l_hr, "OneVariableApplier texture" );
		}
	};

	template< typename T > struct OneVariableApplier
	{
		inline void operator()( DxShaderProgram & CU_PARAM_UNUSED( p_program ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "OneVariableApplier - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Count > struct PointVariableApplier
	{
		inline void operator()( DxShaderProgram & CU_PARAM_UNUSED( p_program ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "PointVariableApplier - Unsupported arguments" );
		}
	};

	template< typename T, uint32_t Rows, uint32_t Columns > struct MatrixVariableApplier
	{
		inline void operator()( DxShaderProgram & CU_PARAM_UNUSED( p_program ), T const * CU_PARAM_UNUSED( p_pValue ), uint32_t CU_PARAM_UNUSED( p_uiOcc ) )
		{
			CASTOR_EXCEPTION( "MatrixVariableApplier - Unsupported arguments" );
		}
	};
#endif
	//***********************************************************************************************************************

	template< typename Type >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Type * p_pValue, uint32_t p_uiOcc )
	{
#if C3DDX10_USE_DXEFFECTS

		if ( m_bPresentInProgram )
		{
			if ( !m_pVariable )
			{
				GetVariableLocation( str_utils::to_str( p_pVariable->GetName() ).c_str(), p_program.GetEffect() );
			}

			if ( m_pVariable )
			{
				OneVariableApplier< Type >()( m_pVariable, p_pValue, p_uiOcc );
			}
			else
			{
				m_bPresentInProgram = false;
			}
		}

#else
		OneVariableApplier< Type >()( p_program, p_pValue, p_uiOcc );
#endif
	}
	template< typename Type, uint32_t Count >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Type * p_pValue, uint32_t p_uiOcc )
	{
#if C3DDX10_USE_DXEFFECTS

		if ( m_bPresentInProgram )
		{
			if ( !m_pVariable )
			{
				GetVariableLocation( str_utils::to_str( p_pVariable->GetName() ).c_str(), p_program.GetEffect() );
			}

			if ( m_pVariable )
			{
				PointVariableApplier< Type, Count >()( m_pVariable, p_pValue, p_pVariable->GetOccCount() );
			}
			else
			{
				m_bPresentInProgram = false;
			}
		}

#else
		PointVariableApplier< Type, Count >()( p_program, p_pValue, p_uiOcc );
#endif
	}
	template< typename Type, uint32_t Rows, uint32_t Columns >
	inline void DxFrameVariableBase::DoApply( DxShaderProgram & p_program, Type * p_pValue, uint32_t p_uiOcc )
	{
#if C3DDX10_USE_DXEFFECTS

		if ( m_bPresentInProgram )
		{
			if ( !m_pVariable )
			{
				GetVariableLocation( str_utils::to_str( p_pVariable->GetName() ).c_str(), p_program.GetEffect() );
			}

			if ( m_pVariable )
			{
				MatrixVariableApplier< Type, Rows, Columns >()( m_pVariable, p_pValue, p_uiOcc );
			}
			else
			{
				m_bPresentInProgram = false;
			}
		}

#else
		MatrixVariableApplier<Type, Rows, Columns >()( p_program, p_pValue, p_uiOcc );
#endif
	}
}
