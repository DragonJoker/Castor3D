namespace Castor3D
{
	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( ShaderProgramBase * p_pProgram )
		:	TFrameVariable< T >( p_pProgram )
	{
		this->m_pValues = new T[1];
		memset( this->m_pValues, 0, size() );
	}

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
		:	TFrameVariable< T >( p_pProgram, p_uiOcc )
	{
		this->m_pValues = new T[p_uiOcc];
		memset( this->m_pValues, 0, size() );
	}

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( OneFrameVariable< T > const & p_rVariable )
		:	TFrameVariable< T >( p_rVariable )
	{
	}

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( OneFrameVariable< T > && p_rVariable )
		:	TFrameVariable< T >( std::move( p_rVariable ) )
	{
	}

	template< typename T >
	OneFrameVariable< T > & OneFrameVariable< T >::operator =( OneFrameVariable< T > const & p_rVariable )
	{
		OneFrameVariable< T > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T >
	OneFrameVariable< T > & OneFrameVariable< T >::operator =( OneFrameVariable< T > && p_rVariable )
	{
		TFrameVariable< T >::operator =( std::move( p_rVariable ) );

		if ( this = &p_rVariable )
		{
		}

		return *this;
	}

	template< typename T >
	OneFrameVariable< T >::~OneFrameVariable()
	{
	}

	template< typename T >
	inline void OneFrameVariable< T >::SetValue( T const & p_tValue )
	{
		OneFrameVariable< T >::SetValue( p_tValue, 0 );
	}

	template< typename T >
	inline void OneFrameVariable< T >::SetValue( T const & p_tValue, uint32_t p_uiIndex )
	{
		policy::assign( this->m_pValues[p_uiIndex], p_tValue );
		TFrameVariable< T >::m_bChanged = true;
	}

	template< typename T >
	inline T & OneFrameVariable< T >::GetValue()throw( std::out_of_range )
	{
		return OneFrameVariable< T >::GetValue( 0 );
	}

	template< typename T >
	inline T const & OneFrameVariable< T >::GetValue()const throw( std::out_of_range )
	{
		return OneFrameVariable< T >::GetValue( 0 );
	}

	template< typename T >
	inline T & OneFrameVariable< T >::GetValue( uint32_t p_uiIndex )throw( std::out_of_range )
	{
		if ( p_uiIndex < TFrameVariable< T >::m_uiOcc )
		{
			return this->m_pValues[p_uiIndex];
		}
		else
		{
			throw ( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
		}
	}

	template< typename T >
	inline T const & OneFrameVariable< T >::GetValue( uint32_t p_uiIndex )const throw( std::out_of_range )
	{
		if ( p_uiIndex < TFrameVariable< T >::m_uiOcc )
		{
			return this->m_pValues[p_uiIndex];
		}
		else
		{
			throw ( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
		}
	}

	template< typename T >
	inline eFRAME_VARIABLE_TYPE OneFrameVariable< T >::GetFrameVariableType()
	{
		return eFRAME_VARIABLE_TYPE( OneFrameVariableDefinitions< T >::Full );
	}

	template< typename T >
	uint32_t OneFrameVariable< T >::size()const
	{
		return OneFrameVariableDefinitions< T >::Size * this->m_uiOcc;
	}

	template< typename T >
	inline void OneFrameVariable< T >::DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex )
	{
		Castor::str_utils::parse( p_strValue, this->m_pValues[p_uiIndex] );
	}
}
