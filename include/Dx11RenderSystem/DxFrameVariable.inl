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

		if( p_program.GetStatus() == Castor3D::ePROGRAM_STATUS_LINKED && *p_pValue )
		{
			Castor3D::TextureBaseRPtr l_pTexture = (*p_pValue);
			l_pTexture->Bind();
			ID3D11ShaderResourceView * l_pResourceView = NULL;
			DxRenderSystem * l_pDxRS = reinterpret_cast< DxRenderSystem* >( p_program.GetRenderSystem() );

			if( l_pTexture )
			{
				if( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
				{
					l_pResourceView = static_cast< DxStaticTexture* >( l_pTexture )->GetShaderResourceView();
				}
				else
				{
					l_pResourceView = static_cast< DxDynamicTexture* >( l_pTexture )->GetShaderResourceView();
				}
			}

			ID3D11DeviceContext * l_pDeviceContext;
			l_pDxRS->GetDevice()->GetImmediateContext( &l_pDeviceContext );
			l_pDeviceContext->PSSetShaderResources( l_pTexture->GetIndex(), 1, &l_pResourceView );
			l_pDeviceContext->Release();
			l_hr = S_OK;
		}
		else if( !(*p_pValue) )
		{
			l_hr = S_OK;
		}

		dxCheckError( l_hr, "ApplyVariable texture" );
	}
};

//***********************************************************************************************************************

template< typename Type >
inline void DxFrameVariableBase :: DoApply( DxShaderProgram & p_program, Type * p_pValue )
{
	OneVariableApplier< Type >()( p_program, p_pValue );
}
template< typename Type, uint32_t Count >
inline void DxFrameVariableBase :: DoApply( DxShaderProgram & p_program, Type * p_pValue )
{
	PointVariableApplier< Type, Count >()( p_program, p_pValue );
}
template< typename Type, uint32_t Rows, uint32_t Columns >
inline void DxFrameVariableBase :: DoApply( DxShaderProgram & p_program, Type * p_pValue )
{
	MatrixVariableApplier< Type, Rows, Columns >()( p_program, p_pValue );
}

//***********************************************************************************************************************

template< typename T >
DxOneFrameVariable< T > :: DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	DxFrameVariableBase			( p_pRenderSystem		)
	,	Castor3D::OneFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
}

template< typename T >
DxOneFrameVariable< T > :: DxOneFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::OneFrameVariable< T > * p_pVariable )
	:	DxFrameVariableBase			( p_pRenderSystem	)
	,	Castor3D::OneFrameVariable< T >	( *p_pVariable		)
{
}

template< typename T >
DxOneFrameVariable< T > :: ~DxOneFrameVariable()
{
}

template< typename T >
void DxOneFrameVariable< T > :: Bind()
{
}

template< typename T >
void DxOneFrameVariable< T > :: Unbind()
{
}

template< typename T >
 void DxOneFrameVariable< T > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		DxFrameVariableBase::DoApply< T >( *static_cast< DxShaderProgram* >( Castor3D::FrameVariable::GetProgram() ), Castor3D::OneFrameVariable< T >::m_pValues );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Count >
DxPointFrameVariable< T, Count > :: DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	DxFrameVariableBase						( p_pRenderSystem		)
	,	Castor3D::PointFrameVariable< T, Count >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Count >
DxPointFrameVariable< T, Count > :: DxPointFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::PointFrameVariable<T, Count> * p_pVariable )
	:	DxFrameVariableBase						( p_pRenderSystem	)
	,	Castor3D::PointFrameVariable< T, Count >	( *p_pVariable		)
{
}

template< typename T, uint32_t Count >
DxPointFrameVariable< T, Count > :: ~DxPointFrameVariable()
{
}

template< typename T, uint32_t Count >
void DxPointFrameVariable< T, Count > :: Bind()
{
}

template< typename T, uint32_t Count >
void DxPointFrameVariable< T, Count > :: Unbind()
{
}

template< typename T, uint32_t Count >
void DxPointFrameVariable< T, Count > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		DxFrameVariableBase::DoApply< T, Count >( *static_cast< DxShaderProgram* >( Castor3D::FrameVariable::GetProgram() ), Castor3D::PointFrameVariable< T, Count >::m_pValues );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
DxMatrixFrameVariable< T, Rows, Columns > :: DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	DxFrameVariableBase								( p_pRenderSystem		)
	,	Castor3D::MatrixFrameVariable< T, Rows, Columns >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
DxMatrixFrameVariable< T, Rows, Columns > :: DxMatrixFrameVariable( DxRenderSystem * p_pRenderSystem, Castor3D::MatrixFrameVariable<T, Rows, Columns> * p_pVariable )
	:	DxFrameVariableBase								( p_pRenderSystem	)
	,	Castor3D::MatrixFrameVariable< T, Rows, Columns >	( *p_pVariable		)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
DxMatrixFrameVariable< T, Rows, Columns > :: ~DxMatrixFrameVariable()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void DxMatrixFrameVariable< T, Rows, Columns > :: Bind()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void DxMatrixFrameVariable< T, Rows, Columns > :: Unbind()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void DxMatrixFrameVariable< T, Rows, Columns > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		DxFrameVariableBase::DoApply< T, Rows, Columns >( *static_cast< DxShaderProgram* >( Castor3D::FrameVariable::GetProgram() ), Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************