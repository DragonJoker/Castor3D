namespace Castor3D
{
	namespace
	{
		template< typename T >
		void Parse( Castor::String const & p_strValue, T & p_value )
		{
			Castor::string::parse( p_strValue, p_value );
		}
	}

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

	//*************************************************************************************************

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( ShaderProgram * p_program )
		:	TFrameVariable< T >( p_program )
	{
		this->m_values = new T[1];
		memset( this->m_values, 0, size() );
	}

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( ShaderProgram * p_program, uint32_t p_occurences )
		:	TFrameVariable< T >( p_program, p_occurences )
	{
		this->m_values = new T[p_occurences];
		memset( this->m_values, 0, size() );
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
		policy::assign( this->m_values[p_index], p_value );
		TFrameVariable< T >::m_changed = true;
	}

	template< typename T >
	inline T & OneFrameVariable< T >::GetValue()
	{
		return OneFrameVariable< T >::GetValue( 0 );
	}

	template< typename T >
	inline T const & OneFrameVariable< T >::GetValue()const
	{
		return OneFrameVariable< T >::GetValue( 0 );
	}

	template< typename T >
	inline T & OneFrameVariable< T >::GetValue( uint32_t p_index )
	{
		if ( p_index < TFrameVariable< T >::m_occurences )
		{
			return this->m_values[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
		}
	}

	template< typename T >
	inline T const & OneFrameVariable< T >::GetValue( uint32_t p_index )const
	{
		if ( p_index < TFrameVariable< T >::m_occurences )
		{
			return this->m_values[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "OneFrameVariable< T > array subscript out of range" );
		}
	}

	template< typename T >
	uint32_t OneFrameVariable< T >::size()const
	{
		return OneFrameVariableDefinitions< T >::Size * this->m_occurences;
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
		Parse( p_strValue, this->m_values[p_index] );
	}
}
