namespace Castor3D
{
	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( ShaderProgramBase * p_pProgram )
		:	TFrameVariable< T >( p_pProgram )
	{
		this->m_pValues = new T[Rows * Columns];
		memset( this->m_pValues, 0, sizeof( T ) * Rows * Columns );
		m_mtxValue = new Castor::Matrix< T, Rows, Columns >[1];
		m_mtxValue[0].link( &this->m_pValues[0] );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
		:	TFrameVariable< T >( p_pProgram, p_uiOcc )
	{
		this->m_pValues = new T[Rows * Columns * p_uiOcc];
		memset( this->m_pValues, 0, sizeof( T ) * p_uiOcc * Rows * Columns );
		m_mtxValue = new Castor::Matrix <T, Rows, Columns>[p_uiOcc];

		for ( uint32_t i = 0; i < p_uiOcc; i++ )
		{
			m_mtxValue[i].link( &this->m_pValues[i * Rows * Columns] );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( MatrixFrameVariable< T, Rows, Columns > const & p_rVariable )
		:	TFrameVariable< T >( p_rVariable )
	{
		m_mtxValue = new Castor::Matrix <T, Rows, Columns>[p_rVariable.m_uiOcc];

		for ( uint32_t i = 0; i < p_rVariable.m_uiOcc; i++ )
		{
			m_mtxValue[i].link( &this->m_pValues[i * Rows * Columns] );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( MatrixFrameVariable< T, Rows, Columns > && p_rVariable )
		:	TFrameVariable< T >( std::move( p_rVariable ) )
		,	m_mtxValue( std::move( p_rVariable.m_mtxValue ) )
	{
		p_rVariable.m_mtxValue	= NULL;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns >::operator =( MatrixFrameVariable< T, Rows, Columns > const & p_rVariable )
	{
		MatrixFrameVariable< T, Rows, Columns > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns >::operator =( MatrixFrameVariable< T, Rows, Columns > && p_rVariable )
	{
		TFrameVariable< T >::operator =( std::move( p_rVariable ) );

		if ( this != &p_rVariable )
		{
			delete [] m_mtxValue;
			m_mtxValue	= std::move( p_rVariable.m_mtxValue );
			p_rVariable.m_pValues	= NULL;
			p_rVariable.m_mtxValue	= NULL;
		}

		return *this;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::~MatrixFrameVariable()
	{
		delete [] m_mtxValue;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_mtxValue )
	{
		MatrixFrameVariable< T, Rows, Columns >::SetValue( p_mtxValue, 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_mtxValue, uint32_t p_uiIndex )
	{
		m_mtxValue[p_uiIndex] = p_mtxValue;
		TFrameVariable< T >::m_bChanged = true;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns >::GetValue()throw( std::out_of_range )
	{
		return MatrixFrameVariable< T, Rows, Columns >::GetValue( 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > const & MatrixFrameVariable< T, Rows, Columns >::GetValue()const throw( std::out_of_range )
	{
		return MatrixFrameVariable< T, Rows, Columns >::GetValue( 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns >::GetValue( uint32_t p_uiIndex )throw( std::out_of_range )
	{
		if ( p_uiIndex < TFrameVariable< T >::m_uiOcc )
		{
			return m_mtxValue[p_uiIndex];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixFrameVariable< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > const & MatrixFrameVariable< T, Rows, Columns >::GetValue( uint32_t p_uiIndex )const throw( std::out_of_range )
	{
		if ( p_uiIndex < TFrameVariable< T >::m_uiOcc )
		{
			return m_mtxValue[p_uiIndex];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixFrameVariable< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline eVARIABLE_TYPE MatrixFrameVariable< T, Rows, Columns >::GetVariableType()
	{
		return eVARIABLE_TYPE( MtxFrameVariableDefinitions< T, Rows, Columns >::Count );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline eFRAME_VARIABLE_TYPE MatrixFrameVariable< T, Rows, Columns >::GetFrameVariableType()
	{
		return eFRAME_VARIABLE_TYPE( MtxFrameVariableDefinitions< T, Rows, Columns >::Full );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	uint32_t MatrixFrameVariable< T, Rows, Columns >::size()const
	{
		return MtxFrameVariableDefinitions< T, Rows, Columns >::Size * this->m_uiOcc;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void MatrixFrameVariable< T, Rows, Columns >::link( uint8_t * p_pBuffer )
	{
		memcpy( p_pBuffer, this->m_pValues, size() );
		T * l_pBuffer = reinterpret_cast< T * >( p_pBuffer );

		for ( uint32_t i = 0; i < this->m_uiOcc; i++ )
		{
			m_mtxValue[i].link( &l_pBuffer[i * Rows * Columns] );
		}

		TFrameVariable< T >::link( p_pBuffer );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex )
	{
		Castor::StringArray l_arrayLines = Castor::str_utils::split( p_strValue, cuT( ";" ) );

		if ( l_arrayLines.size() == Rows )
		{
			bool l_bOK = true;

			for ( uint32_t i = 0; i < Rows && l_bOK; i++ )
			{
				l_bOK = false;
				Castor::StringArray l_arraySplitted = Castor::str_utils::split( l_arrayLines[i], cuT( ", \t" ) );

				if ( l_arraySplitted.size() == Columns )
				{
					l_bOK = true;

					for ( uint32_t j = 0; j < Columns; j++ )
					{
						policy::assign( m_mtxValue[p_uiIndex][j][i], policy::parse( l_arraySplitted[j] ) );
					}
				}
			}
		}
	}
}
