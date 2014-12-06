namespace Castor3D
{
	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( ShaderProgramBase * p_pProgram )
		:	TFrameVariable< T >( p_pProgram )
	{
		this->m_pValues = new T[Count];
		memset( this->m_pValues, 0, size() );
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
		:	TFrameVariable< T >( p_pProgram, p_uiOcc )
	{
		this->m_pValues = new T[p_uiOcc * Count];
		memset( this->m_pValues, 0, size() );
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( PointFrameVariable< T, Count> const & p_rVariable )
		:	TFrameVariable< T >( p_rVariable )
	{
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( PointFrameVariable< T, Count > && p_rVariable )
		:	TFrameVariable< T >( std::move( p_rVariable ) )
	{
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count > & PointFrameVariable< T, Count >::operator =( PointFrameVariable< T, Count> const & p_rVariable )
	{
		PointFrameVariable< T, Count > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count > & PointFrameVariable< T, Count >::operator =( PointFrameVariable< T, Count > && p_rVariable )
	{
		TFrameVariable< T >::operator =( std::move( p_rVariable ) );

		if ( this != &p_rVariable )
		{
		}

		return *this;
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::~PointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::SetValue( Castor::Point< T, Count > const & p_ptValue )
	{
		PointFrameVariable< T, Count >::SetValue( p_ptValue, 0 );
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::SetValue( Castor::Point< T, Count > const & p_ptValue, uint32_t p_uiIndex )
	{
		Castor::Coords< T, Count > l_ptValue( GetValue( p_uiIndex ) );
		memcpy( l_ptValue.ptr(), p_ptValue.const_ptr(), l_ptValue.size() );
		TFrameVariable< T >::m_bChanged = true;
	}

	template< typename T, uint32_t Count >
	inline Castor::Coords< T, Count > PointFrameVariable< T, Count >::GetValue()throw( std::out_of_range )
	{
		return PointFrameVariable< T, Count >::GetValue( 0 );
	}

	template< typename T, uint32_t Count >
	inline Castor::Point< T, Count > PointFrameVariable< T, Count >::GetValue()const throw( std::out_of_range )
	{
		return PointFrameVariable< T, Count >::GetValue( 0 );
	}

	template< typename T, uint32_t Count >
	inline Castor::Coords< T, Count > PointFrameVariable< T, Count >::GetValue( uint32_t p_uiIndex )throw( std::out_of_range )
	{
		if ( p_uiIndex < this->m_uiOcc )
		{
			return Castor::Coords< T, Count >( &this->m_pValues[p_uiIndex * Count] );
		}
		else
		{
			throw ( std::out_of_range )( "PointFrameVariable< T, Count > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Count >
	inline Castor::Point< T, Count > PointFrameVariable< T, Count >::GetValue( uint32_t p_uiIndex )const throw( std::out_of_range )
	{
		if ( p_uiIndex < this->m_uiOcc )
		{
			return Castor::Point< T, Count >( &this->m_pValues[p_uiIndex * Count] );
		}
		else
		{
			throw ( std::out_of_range )( "PointFrameVariable< T, Count > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Count >
	inline eVARIABLE_TYPE PointFrameVariable< T, Count >::GetVariableType()
	{
		return eVARIABLE_TYPE( PntFrameVariableDefinitions< T, Count >::Count );
	}

	template< typename T, uint32_t Count >
	inline eFRAME_VARIABLE_TYPE PointFrameVariable< T, Count >::GetFrameVariableType()
	{
		return eFRAME_VARIABLE_TYPE( PntFrameVariableDefinitions< T, Count >::Full );
	}

	template< typename T, uint32_t Count >
	uint32_t PointFrameVariable< T, Count >::size()const
	{
		return PntFrameVariableDefinitions< T, Count >::Size * this->m_uiOcc;
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::DoSetValueStr( Castor::String const & p_strValue, uint32_t p_uiIndex )
	{
		Castor::StringArray l_arraySplitted = Castor::str_utils::split( p_strValue, cuT( ", \t" ) );
		Castor::Coords< T, Count > l_ptValue( GetValue( p_uiIndex ) );

		if ( l_arraySplitted.size() == Count )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				policy::assign( l_ptValue[i], policy::parse( l_arraySplitted[i] ) );
			}
		}
	}
}
