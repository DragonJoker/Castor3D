template<> inline void ApplyVariable< Castor3D::TextureBaseSPtr >( CGparameter p_cgParameter, Castor3D::TextureBaseSPtr const * p_pValue, uint32_t p_uiOcc )
{
	if( p_uiOcc <= 1 )
	{
		uint32_t l_uiTexture = 0;
		Castor3D::TextureBaseSPtr l_pTexture = (*p_pValue);

		if( l_pTexture )
		{
			if( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
			{
				l_uiTexture = std::static_pointer_cast< GlRender::GlStaticTexture >( l_pTexture )->GetGlName();
			}
			else
			{
				l_uiTexture = std::static_pointer_cast< GlRender::GlDynamicTexture >( l_pTexture )->GetGlName();
			}
		}

		if( l_uiTexture )
		{
			CheckCgError( cgGLSetTextureParameter( p_cgParameter, l_uiTexture ), cuT( "ApplyVariable - cgGLSetTextureParameter" ) );
		}
	}
	else
	{
	}
}

//***********************************************************************************************************************

template< typename Type >
inline void CgGlFrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
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
inline void CgGlFrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
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
inline void CgGlFrameVariableBase :: DoApply( Castor::String const & p_strName, Type const * p_pValue, uint32_t p_uiOcc, CGprogram p_cgProgram )
{
	if( m_bPresentInProgram )
	{
		if( !m_cgParameter )
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
CgGlOneFrameVariable< T > :: CgGlOneFrameVariable( CgGlShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgGlFrameVariableBase	(						)
	,	CgOneFrameVariable< T >	( p_pProgram, p_uiOcc	)
{
}

template< typename T >
CgGlOneFrameVariable< T > :: CgGlOneFrameVariable( CgOneFrameVariable<T> * p_pVariable )
	:	CgGlFrameVariableBase	( p_pVariable->GetParameter(), p_pVariable->GetProgram()	)
	,	CgOneFrameVariable< T >	( *p_pVariable												)
{
}

template< typename T >
CgGlOneFrameVariable< T > :: ~CgGlOneFrameVariable()
{
}

template< typename T >
void CgGlOneFrameVariable< T > :: Bind()
{
}

template< typename T >
void CgGlOneFrameVariable< T > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgGlFrameVariableBase::DoApply< T >( Castor3D::FrameVariable::m_strName, Castor3D::OneFrameVariable< T >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Count >
CgGlPointFrameVariable< T, Count > :: CgGlPointFrameVariable( CgGlShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgGlFrameVariableBase				(						)
	,	CgPointFrameVariable< T, Count >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Count >
CgGlPointFrameVariable< T, Count > :: CgGlPointFrameVariable( CgPointFrameVariable< T, Count > * p_pVariable )
	:	CgGlFrameVariableBase				(				)
	,	CgPointFrameVariable< T, Count >	( *p_pVariable	)
{
}

template< typename T, uint32_t Count >
CgGlPointFrameVariable< T, Count > :: ~CgGlPointFrameVariable()
{
}

template< typename T, uint32_t Count >
void CgGlPointFrameVariable< T, Count > :: Bind()
{
}

template< typename T, uint32_t Count >
void CgGlPointFrameVariable< T, Count > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgGlFrameVariableBase::DoApply< T, Count >( Castor3D::FrameVariable::m_strName, Castor3D::PointFrameVariable< T, Count >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************

template< typename T, uint32_t Rows, uint32_t Columns >
CgGlMatrixFrameVariable< T, Rows, Columns > :: CgGlMatrixFrameVariable( CgGlShaderProgram * p_pProgram, uint32_t p_uiOcc )
	:	CgGlFrameVariableBase						(						)
	,	CgMatrixFrameVariable< T, Rows, Columns >	( p_pProgram, p_uiOcc	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgGlMatrixFrameVariable< T, Rows, Columns > :: CgGlMatrixFrameVariable( CgMatrixFrameVariable< T, Rows, Columns > * p_pVariable )
	:	CgGlFrameVariableBase					(				)
	,	CgMatrixFrameVariable<T, Rows, Columns>	( *p_pVariable	)
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
CgGlMatrixFrameVariable< T, Rows, Columns > :: ~CgGlMatrixFrameVariable()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgGlMatrixFrameVariable< T, Rows, Columns > :: Bind()
{
}

template< typename T, uint32_t Rows, uint32_t Columns >
void CgGlMatrixFrameVariable< T, Rows, Columns > :: Apply()
{
	if( Castor3D::FrameVariable::m_bChanged )
	{
		CgGlFrameVariableBase::DoApply< T, Rows, Columns >( Castor3D::FrameVariable::m_strName, Castor3D::MatrixFrameVariable< T, Rows, Columns >::m_pValues, Castor3D::FrameVariable::m_uiOcc, CgFrameVariable::m_cgProgram );
		Castor3D::FrameVariable::m_bChanged = false;
	}
}

//***********************************************************************************************************************
