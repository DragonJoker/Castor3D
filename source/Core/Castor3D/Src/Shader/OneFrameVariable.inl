namespace Castor3D
{
	template<> struct OneFrameVariableDefinitions< bool >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 1 * 1 );
		static const VariableType Count = VariableType::eOne;
		static const FrameVariableType Full = FrameVariableType::eBool;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< int >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 1 * 1 );
		static const VariableType Count = VariableType::eOne;
		static const FrameVariableType Full = FrameVariableType::eInt;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< uint32_t >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 1 * 1 );
		static const VariableType Count = VariableType::eOne;
		static const FrameVariableType Full = FrameVariableType::eUInt;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< float >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 1 * 1 );
		static const VariableType Count = VariableType::eOne;
		static const FrameVariableType Full = FrameVariableType::eFloat;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct OneFrameVariableDefinitions< double >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 1 * 1 );
		static const VariableType Count = VariableType::eOne;
		static const FrameVariableType Full = FrameVariableType::eDouble;
		C3D_API static const xchar * FullTypeName;
	};

	//*************************************************************************************************

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( ShaderProgram & p_program )
		: OneFrameVariable< T >( p_program, 1u )
	{
	}

	template< typename T >
	OneFrameVariable< T >::OneFrameVariable( ShaderProgram & p_program, uint32_t p_occurences )
		: TFrameVariable< T >( p_program, p_occurences )
	{
		this->m_values = new T[p_occurences];
		memset( this->m_values, 0, size() );
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
		REQUIRE( p_index < this->m_occurences );
		policy::assign( this->m_values[p_index], p_value );
		TFrameVariable< T >::m_changed = true;
	}

	template< typename T >
	inline void OneFrameVariable< T >::SetValues( T const * p_values, size_t p_size )
	{
		REQUIRE( p_size <= this->m_occurences );
		std::memcpy( this->m_values, p_values, p_size * sizeof( T ) );
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
	inline VariableType OneFrameVariable< T >::GetVariableType()
	{
		return VariableType::eOne;
	}

	template< typename T >
	inline FrameVariableType OneFrameVariable< T >::GetFrameVariableType()
	{
		return FrameVariableType( OneFrameVariableDefinitions< T >::Full );
	}

	template< typename T >
	inline Castor::String OneFrameVariable< T >::GetFrameVariableTypeName()
	{
		return OneFrameVariableDefinitions< T >::FullTypeName;
	}

	template< typename T >
	inline void OneFrameVariable< T >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
	{
		Castor::string::parse( p_value, this->m_values[p_index] );
	}

	template< typename T >
	inline Castor::String OneFrameVariable< T >::DoGetStrValue( uint32_t p_index )const
	{
		Castor::StringStream l_stream;
		l_stream << this->m_values[p_index];
		return l_stream.str();
	}
}
