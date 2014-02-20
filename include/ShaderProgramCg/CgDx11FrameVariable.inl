template<> inline void ApplyVariable< Castor3D::TextureBaseSPtr >( CGparameter p_cgParameter, Castor3D::TextureBaseSPtr const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		ID3D11ShaderResourceView * l_pResourceView = NULL;
		Castor3D::TextureBaseSPtr l_pTexture = *p_pValue;

		if( l_pTexture )
		{
			if( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
			{
				l_pResourceView = std::static_pointer_cast< Dx11Render::DxStaticTexture>( l_pTexture )->GetShaderResourceView();
			}
			else
			{
				l_pResourceView = std::static_pointer_cast< Dx11Render::DxDynamicTexture>( l_pTexture )->GetShaderResourceView();
			}

			ID3D11Resource * l_pResource;
			l_pResourceView->GetResource( &l_pResource );

			if( l_pResource )
			{
				CheckCgError( cgD3D11SetTextureParameter( p_cgParameter, l_pResource ), cuT( "ApplyVariable - cgD3D11SetTextureParameter" ) );
			}
		}
	}
	else
	{
	}
}

//***********************************************************************************************************************

template< typename Type >
inline void CgD3D11FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
{
	if( m_bPresentInProgram )
	{
		if( !m_cgParameter )
		{
			GetVariableLocation( Castor::str_utils::to_str( p_strName ).c_str(), p_cgProgram );
		}

		if( m_cgParameter )
		{
			ApplyVariable< Type >( m_cgParameter, p_pValue, p_uiOcc );
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

template< typename Type, int Count >
inline void CgD3D11FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram)
{
	if( m_bPresentInProgram )
	{
		if( !m_cgParameter )
		{
			GetVariableLocation( Castor::str_utils::to_str( p_strName ).c_str(), p_cgProgram );
		}

		if( m_cgParameter )
		{
			ApplyVariable< Type, Count >( m_cgParameter, p_pValue, p_uiOcc );
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

template< typename Type, int Rows, int Columns >
inline void CgD3D11FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
{
	if( m_bPresentInProgram )
	{
		if( m_cgParameter )
		{
			GetVariableLocation( Castor::str_utils::to_str( p_strName ).c_str(), p_cgProgram );
		}

		if( m_cgParameter )
		{
			ApplyVariable< Type, Rows, Columns >( m_cgParameter, p_pValue, p_uiOcc );
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

//***********************************************************************************************************************

template< typename T >
CgD3D11OneFrameVariable< T > :: CgD3D11OneFrameVariable( CgD3D11ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D11FrameVariableBase	(						)
	,	CgOneFrameVariable< T >		( p_pProgram, p_uiOcc	)
{
}

template< typename T >
CgD3D11OneFrameVariable< T > :: CgD3D11OneFrameVariable( CgOneFrameVariable<T> * p_pVariable )
	:	CgD3D11FrameVariableBase	( p_pVariable->GetParameter(), p_pVariable->GetProgram()	)
	,	CgOneFrameVariable< T >		( *p_pVariable												)
{
}

template< typename T >
CgD3D11OneFrameVariable< T > :: ~CgD3D11OneFrameVariable()
{
}

template< typename T >
void CgD3D11OneFrameVariable< T > :: Bind()
{
}

template< typename T >
void CgD3D11OneFrameVariable< T > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D11FrameVariableBase::DoApply< T >( Castor3D::FrameVariable::m_strName, Castor3D::OneFrameVariable< T >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Count >
CgD3D11PointFrameVariable< T, Count > :: CgD3D11PointFrameVariable( CgD3D11ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D11FrameVariableBase			(						)
	,	CgPointFrameVariable< T, Count >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Count >
CgD3D11PointFrameVariable< T, Count > :: CgD3D11PointFrameVariable( CgPointFrameVariable< T, Count > * p_pVariable )
	:	CgD3D11FrameVariableBase			(				)
	,	CgPointFrameVariable< T, Count >	( *p_pVariable	)
{
}

template< typename T, uint32_t Count >
CgD3D11PointFrameVariable< T, Count > :: ~CgD3D11PointFrameVariable()
{
}

template< typename T, uint32_t Count >
void CgD3D11PointFrameVariable< T, Count > :: Bind()
{
}

template< typename T, uint32_t Count >
void CgD3D11PointFrameVariable< T, Count > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D11FrameVariableBase::DoApply< T, Count >( Castor3D::FrameVariable::m_strName, Castor3D::PointFrameVariable< T, Count >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D11MatrixFrameVariable< T, Rows, Columns > :: CgD3D11MatrixFrameVariable( CgD3D11ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D11FrameVariableBase					(						)
	,	CgMatrixFrameVariable< T, Rows, Columns >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D11MatrixFrameVariable< T, Rows, Columns > :: CgD3D11MatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > * p_pVariable )
	:	CgD3D11FrameVariableBase				(				)
	,	CgMatrixFrameVariable<T, Rows, Columns>	( *p_pVariable	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D11MatrixFrameVariable< T, Rows, Columns > :: ~CgD3D11MatrixFrameVariable()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgD3D11MatrixFrameVariable< T, Rows, Columns > :: Bind()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgD3D11MatrixFrameVariable< T, Rows, Columns > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D11FrameVariableBase::DoApply< T, Rows, Columns >( Castor3D::FrameVariable::m_strName, Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************