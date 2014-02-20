template<> inline void ApplyVariable< Castor3D::TextureBaseSPtr >( CGparameter p_cgParameter, Castor3D::TextureBaseSPtr const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		ID3D10ShaderResourceView * l_pResourceView = NULL;
		Castor3D::TextureBaseSPtr l_pTexture = *p_pValue;

		if( l_pTexture )
		{
			if( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
			{
				l_pResourceView = std::static_pointer_cast< Dx10Render::DxStaticTexture>( l_pTexture )->GetShaderResourceView();
			}
			else
			{
				l_pResourceView = std::static_pointer_cast< Dx10Render::DxDynamicTexture>( l_pTexture )->GetShaderResourceView();
			}

			ID3D10Resource * l_pResource;
			l_pResourceView->GetResource( &l_pResource );

			if( l_pResource )
			{
				CheckCgError( cgD3D10SetTextureParameter( p_cgParameter, l_pResource ), cuT( "ApplyVariable - cgD3D10SetTextureParameter" ) );
			}
		}
	}
	else
	{
	}
}

//***********************************************************************************************************************

template< typename Type >
inline void CgD3D10FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
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
inline void CgD3D10FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram)
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
inline void CgD3D10FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
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
CgD3D10OneFrameVariable< T > :: CgD3D10OneFrameVariable( CgD3D10ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D10FrameVariableBase	(						)
	,	CgOneFrameVariable< T >		( p_pProgram, p_uiOcc	)
{
}

template< typename T >
CgD3D10OneFrameVariable< T > :: CgD3D10OneFrameVariable( CgOneFrameVariable<T> * p_pVariable )
	:	CgD3D10FrameVariableBase	( p_pVariable->GetParameter(), p_pVariable->GetProgram()	)
	,	CgOneFrameVariable< T >		( *p_pVariable												)
{
}

template< typename T >
CgD3D10OneFrameVariable< T > :: ~CgD3D10OneFrameVariable()
{
}

template< typename T >
void CgD3D10OneFrameVariable< T > :: Bind()
{
}

template< typename T >
void CgD3D10OneFrameVariable< T > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D10FrameVariableBase::DoApply< T >( Castor3D::FrameVariable::m_strName, Castor3D::OneFrameVariable< T >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Count >
CgD3D10PointFrameVariable< T, Count > :: CgD3D10PointFrameVariable( CgD3D10ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D10FrameVariableBase			(						)
	,	CgPointFrameVariable< T, Count >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Count >
CgD3D10PointFrameVariable< T, Count > :: CgD3D10PointFrameVariable( CgPointFrameVariable< T, Count > * p_pVariable )
	:	CgD3D10FrameVariableBase			(				)
	,	CgPointFrameVariable< T, Count >	( *p_pVariable	)
{
}

template< typename T, uint32_t Count >
CgD3D10PointFrameVariable< T, Count > :: ~CgD3D10PointFrameVariable()
{
}

template< typename T, uint32_t Count >
void CgD3D10PointFrameVariable< T, Count > :: Bind()
{
}

template< typename T, uint32_t Count >
void CgD3D10PointFrameVariable< T, Count > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D10FrameVariableBase::DoApply< T, Count >( Castor3D::FrameVariable::m_strName, Castor3D::PointFrameVariable< T, Count >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D10MatrixFrameVariable< T, Rows, Columns > :: CgD3D10MatrixFrameVariable( CgD3D10ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D10FrameVariableBase					(						)
	,	CgMatrixFrameVariable< T, Rows, Columns >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D10MatrixFrameVariable< T, Rows, Columns > :: CgD3D10MatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > * p_pVariable )
	:	CgD3D10FrameVariableBase				(				)
	,	CgMatrixFrameVariable<T, Rows, Columns>	( *p_pVariable	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D10MatrixFrameVariable< T, Rows, Columns > :: ~CgD3D10MatrixFrameVariable()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgD3D10MatrixFrameVariable< T, Rows, Columns > :: Bind()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgD3D10MatrixFrameVariable< T, Rows, Columns > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D10FrameVariableBase::DoApply< T, Rows, Columns >( Castor3D::FrameVariable::m_strName, Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************