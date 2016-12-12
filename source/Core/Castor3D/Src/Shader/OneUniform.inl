namespace Castor3D
{
	template<> struct OneUniformDefinitions< bool >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 1 * 1 );
		static constexpr VariableType Count = VariableType::eOne;
		static constexpr UniformType Full = UniformType::eBool;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct OneUniformDefinitions< int >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 1 * 1 );
		static constexpr VariableType Count = VariableType::eOne;
		static constexpr UniformType Full = UniformType::eInt;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct OneUniformDefinitions< uint32_t >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 1 * 1 );
		static constexpr VariableType Count = VariableType::eOne;
		static constexpr UniformType Full = UniformType::eUInt;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct OneUniformDefinitions< float >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 1 * 1 );
		static constexpr VariableType Count = VariableType::eOne;
		static constexpr UniformType Full = UniformType::eFloat;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct OneUniformDefinitions< double >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 1 * 1 );
		static constexpr VariableType Count = VariableType::eOne;
		static constexpr UniformType Full = UniformType::eDouble;
		C3D_API static const Castor::String FullTypeName;
	};

	//*************************************************************************************************

	template< typename T >
	OneUniform< T >::OneUniform()
		: OneUniform< T >( 1u )
	{
	}

	template< typename T >
	OneUniform< T >::OneUniform( uint32_t p_occurences )
		: TUniform< T >( p_occurences )
	{
		this->m_values = new T[p_occurences];
		memset( this->m_values, 0, size() );
	}

	template< typename T >
	OneUniform< T >::~OneUniform()
	{
	}

	template< typename T >
	inline void OneUniform< T >::SetValue( T const & p_value )
	{
		OneUniform< T >::SetValue( p_value, 0 );
	}

	template< typename T >
	inline void OneUniform< T >::SetValue( T const & p_value, uint32_t p_index )
	{
		REQUIRE( p_index < this->m_occurences );
		policy::assign( this->m_values[p_index + p_index * this->m_stride], p_value );
		TUniform< T >::m_changed = true;
	}

	template< typename T >
	inline void OneUniform< T >::SetValues( T const * p_values, size_t p_size )
	{
		REQUIRE( p_size <= this->m_occurences );

		for ( uint32_t i = 0; i < p_size; ++i )
		{
			SetValue( p_values[i], i );
		}
	}

	template< typename T >
	inline T & OneUniform< T >::GetValue()
	{
		return OneUniform< T >::GetValue( 0 );
	}

	template< typename T >
	inline T const & OneUniform< T >::GetValue()const
	{
		return OneUniform< T >::GetValue( 0 );
	}

	template< typename T >
	inline T & OneUniform< T >::GetValue( uint32_t p_index )
	{
		if ( p_index < this->m_occurences )
		{
			return this->m_values[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "OneUniform< T > array subscript out of range" );
		}
	}

	template< typename T >
	inline T const & OneUniform< T >::GetValue( uint32_t p_index )const
	{
		if ( p_index < this->m_occurences )
		{
			return this->m_values[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "OneUniform< T > array subscript out of range" );
		}
	}

	template< typename T >
	uint32_t OneUniform< T >::size()const
	{
		return OneUniformDefinitions< T >::Size * this->m_occurences;
	}

	template< typename T >
	void OneUniform< T >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		uint8_t * l_buffer = reinterpret_cast< uint8_t * >( p_buffer );
		constexpr uint32_t l_size = OneUniformDefinitions< T >::Size;

		if ( p_stride == 0 )
		{
			p_stride = l_size;
		}

		REQUIRE( p_stride >= l_size );

		for ( uint32_t i = 0; i < this->m_occurences; i++ )
		{
			std::memcpy( l_buffer, &this->m_values[i], l_size );
			l_buffer += p_stride;
		}

		this->DoCleanupBuffer();
		this->m_stride = p_stride > l_size ? p_stride : 0u;
		this->m_values = reinterpret_cast< T * >( p_buffer );
		this->m_bOwnBuffer = false;
	}

	template< typename T >
	inline constexpr VariableType OneUniform< T >::GetVariableType()
	{
		return VariableType::eOne;
	}

	template< typename T >
	inline constexpr UniformType OneUniform< T >::GetUniformType()
	{
		return UniformType( OneUniformDefinitions< T >::Full );
	}

	template< typename T >
	inline Castor::String const & OneUniform< T >::GetUniformTypeName()
	{
		return OneUniformDefinitions< T >::FullTypeName;
	}

	template< typename T >
	inline void OneUniform< T >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
	{
		Castor::string::parse( p_value, this->m_values[p_index] );
	}

	template< typename T >
	inline Castor::String OneUniform< T >::DoGetStrValue( uint32_t p_index )const
	{
		Castor::StringStream l_stream;
		l_stream << this->m_values[p_index];
		return l_stream.str();
	}
}
