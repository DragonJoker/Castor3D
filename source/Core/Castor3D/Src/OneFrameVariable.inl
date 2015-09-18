namespace Castor3D
{
	template<> struct OneFrameVariableDefinitions< bool >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_BOOL;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< int >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_INT;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< uint32_t >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_UINT;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< float >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_FLOAT;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< double >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_DOUBLE;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< TextureBase * >
	{
		static const uint32_t Size = uint32_t( sizeof( TextureBase * ) * 1 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_ONE;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_SAMPLER;
		C3D_API static const xchar * FullTypeName;
	};

	//*************************************************************************************************

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
	OneFrameVariable< T > & OneFrameVariable< T >::operator=( OneFrameVariable< T > const & p_rVariable )
	{
		OneFrameVariable< T > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T >
	OneFrameVariable< T > & OneFrameVariable< T >::operator=( OneFrameVariable< T > && p_rVariable )
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
	inline void OneFrameVariable< T >::SetValue( T const & p_value )
	{
		OneFrameVariable< T >::SetValue( p_value, 0 );
	}

	template< typename T >
	inline void OneFrameVariable< T >::SetValue( T const & p_value, uint32_t p_index )
	{
		policy::assign( this->m_pValues[p_index], p_value );
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
	inline T & OneFrameVariable< T >::GetValue( uint32_t p_index )throw( std::out_of_range )
	{
		if ( p_index < TFrameVariable< T >::m_uiOcc )
		{
			return this->m_pValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
		}
	}

	template< typename T >
	inline T const & OneFrameVariable< T >::GetValue( uint32_t p_index )const throw( std::out_of_range )
	{
		if ( p_index < TFrameVariable< T >::m_uiOcc )
		{
			return this->m_pValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
		}
	}

	template< typename T >
	uint32_t OneFrameVariable< T >::size()const
	{
		return OneFrameVariableDefinitions< T >::Size * this->m_uiOcc;
	}

	template< typename T >
	inline eFRAME_VARIABLE_TYPE OneFrameVariable< T >::GetFrameVariableType()
	{
		return eFRAME_VARIABLE_TYPE( OneFrameVariableDefinitions< T >::Full );
	}

	template< typename T >
	inline Castor::String OneFrameVariable< T >::GetFrameVariableTypeName()
	{
		return OneFrameVariableDefinitions< T >::FullTypeName;
	}

	template< typename T >
	inline void OneFrameVariable< T >::DoSetValueStr( Castor::String const & p_strValue, uint32_t p_index )
	{
		Castor::string::parse( p_strValue, this->m_pValues[p_index] );
	}
}
