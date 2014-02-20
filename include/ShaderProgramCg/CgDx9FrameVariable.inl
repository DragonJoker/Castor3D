template<> inline void ApplyVariable< Castor3D::TextureBaseSPtr >( CGparameter p_cgParameter, Castor3D::TextureBaseSPtr const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		IDirect3DTexture9 * l_pDxTexture = NULL;
		Castor3D::TextureBaseSPtr l_pTexture = *p_pValue;

		if( l_pTexture )
		{
			if( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
			{
				l_pDxTexture = std::static_pointer_cast< Dx9Render::DxStaticTexture >( l_pTexture )->GetDxTexture();
			}
			else
			{
				l_pDxTexture = std::static_pointer_cast< Dx9Render::DxDynamicTexture >( l_pTexture )->GetDxTexture();
			}

			if( l_pDxTexture )
			{
				CheckCgError( cgD3D9SetTextureParameter( p_cgParameter, l_pDxTexture ), cuT( "ApplyVariable - cgD3D9SetTextureParameter" ) );
			}
		}
	}
	else
	{
	}
}

//***********************************************************************************************************************

template< typename Type >
inline void CgD3D9FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
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
inline void CgD3D9FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram)
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
inline void CgD3D9FrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
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
CgD3D9OneFrameVariable< T > :: CgD3D9OneFrameVariable( CgD3D9ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D9FrameVariableBase	(						)
	,	CgOneFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
}

template< typename T >
CgD3D9OneFrameVariable< T > :: CgD3D9OneFrameVariable( CgOneFrameVariable<T> * p_pVariable )
	:	CgD3D9FrameVariableBase	( p_pVariable->GetParameter(), p_pVariable->GetProgram()	)
	,	CgOneFrameVariable< T >	( *p_pVariable												)
{
}

template< typename T >
CgD3D9OneFrameVariable< T > :: ~CgD3D9OneFrameVariable()
{
}

template< typename T >
void CgD3D9OneFrameVariable< T > :: Bind()
{
}

template< typename T >
void CgD3D9OneFrameVariable< T > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D9FrameVariableBase::DoApply< T >( Castor3D::FrameVariable::m_strName, Castor3D::OneFrameVariable< T >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Count >
CgD3D9PointFrameVariable< T, Count > :: CgD3D9PointFrameVariable( CgD3D9ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D9FrameVariableBase				(						)
	,	CgPointFrameVariable< T, Count >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Count >
CgD3D9PointFrameVariable< T, Count > :: CgD3D9PointFrameVariable( CgPointFrameVariable< T, Count > * p_pVariable )
	:	CgD3D9FrameVariableBase				(				)
	,	CgPointFrameVariable< T, Count >	( *p_pVariable	)
{
}

template< typename T, uint32_t Count >
CgD3D9PointFrameVariable< T, Count > :: ~CgD3D9PointFrameVariable()
{
}

template< typename T, uint32_t Count >
void CgD3D9PointFrameVariable< T, Count > :: Bind()
{
}

template< typename T, uint32_t Count >
void CgD3D9PointFrameVariable< T, Count > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D9FrameVariableBase::DoApply< T, Count >( Castor3D::FrameVariable::m_strName, Castor3D::PointFrameVariable< T, Count >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D9MatrixFrameVariable< T, Rows, Columns > :: CgD3D9MatrixFrameVariable( CgD3D9ShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgD3D9FrameVariableBase						(						)
	,	CgMatrixFrameVariable< T, Rows, Columns >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D9MatrixFrameVariable< T, Rows, Columns > :: CgD3D9MatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > * p_pVariable )
	:	CgD3D9FrameVariableBase					(				)
	,	CgMatrixFrameVariable<T, Rows, Columns>	( *p_pVariable	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgD3D9MatrixFrameVariable< T, Rows, Columns > :: ~CgD3D9MatrixFrameVariable()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgD3D9MatrixFrameVariable< T, Rows, Columns > :: Bind()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgD3D9MatrixFrameVariable< T, Rows, Columns > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgD3D9FrameVariableBase::DoApply< T, Rows, Columns >( Castor3D::FrameVariable::m_strName, Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************