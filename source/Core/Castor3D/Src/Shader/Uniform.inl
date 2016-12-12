namespace Castor3D
{
	template< UniformType Type, typename Enable = void >
	struct Linker;
	template< VariableType Type, typename Enable = void >
	struct Unlinker;
	template< UniformType Type, typename Enable = void >
	struct StringValueSetter;
	template< UniformType Type, typename Enable = void >
	struct StringValueGetter;

	template< VariableType Type >
	struct Unlinker< Type, typename std::enable_if< Type == VariableType::eMat2x2 || Type == VariableType::eMat3x3 || Type == VariableType::eMat4x4 >::type >
	{
		static void UnLink( typename UniformTyper< Type >::value_type * p_dst, uint32_t p_occurences, uint8_t * p_src, uint32_t & p_srcStride )
		{
			uint8_t * l_buffer = p_src;
			constexpr uint32_t l_size = UniformTyper< Type >::size;

			if ( !p_srcStride )
			{
				p_srcStride = l_size;
			}

			REQUIRE( p_stride >= l_size );

			for ( uint32_t i = 0; i < p_occurences; i++ )
			{
				std::memcpy( l_buffer, p_dst[i].const_ptr(), l_size );
				l_buffer += p_stride;
			}
		}
	};
	
	template< VariableType Type >
	struct Linker< Type, typename std::enable_if< Type == VariableType::eMat2x2 || Type == VariableType::eMat3x3 || Type == VariableType::eMat4x4 >::type >
	{
		static void Link( typename UniformTyper< Type >::value_type * p_dst, uint32_t p_occurences, uint8_t * p_src, uint32_t p_stride )
		{
			uint8_t * l_buffer = p_src;

			for ( uint32_t i = 0; i < p_occurences; i++ )
			{
				p_dst[i].link( reinterpret_cast< typename UniformTyper< Type >::value_sub_type * >( l_buffer ) );
				l_buffer += p_stride;
			}
		}
	};

	template< VariableType Type >
	struct StringValueSetter< Type, typename std::enable_if< Type == VariableType::eMat2x2 || Type == VariableType::eMat3x3 || Type == VariableType::eMat4x4 >::type >
	{
		static inline void SetStrValue( typename UniformTyper< Type >::value_type * p_dst, Castor::String const & p_value, uint32_t p_index )
		{
			Castor::StringArray l_arrayLines = Castor::string::split( p_value, cuT( ";" ) );

			if ( l_arrayLines.size() == Rows )
			{
				bool l_continue = true;

				for ( uint32_t i = 0; i < Rows && l_continue; i++ )
				{
					l_continue = false;
					Castor::StringArray l_arraySplitted = Castor::string::split( l_arrayLines[i], cuT( ", \t" ) );

					if ( l_arraySplitted.size() == Columns )
					{
						l_continue = true;

						for ( uint32_t j = 0; j < Columns; j++ )
						{
							Castor::string::parse( l_arraySplitted[j], p_dst[0][j][i] );
						}
					}
				}
			}
		}
	};

	template< VariableType Type >
	struct StringValueGetter< Type, typename std::enable_if< Type == VariableType::eMat2x2 || Type == VariableType::eMat3x3 || Type == VariableType::eMat4x4 >::type >
	{
		static inline Castor::String GetStrValue( typename UniformTyper< Type >::value_type const * p_src, uint32_t p_index )
		{
			Castor::StringStream l_stream;

			for ( uint32_t j = 0; j < Columns; j++ )
			{
				Castor::String l_sep;

				for ( uint32_t i = 0; i < Rows; i++ )
				{
					l_stream << l_sep << p_src[0][j][i];
					l_sep = ", ";
				}

				l_stream << ";";
			}

			return l_stream.str();
		}
	};

	//*************************************************************************************************

	template< UniformType Type >
	TUniform< Type >::TUniform( uint32_t p_occurences )
		: Uniform{ p_occurences }
	{
		m_values = new value_sub_type[p_occurences * UniformTyper< Type >::size];
		memset( m_values, 0, p_occurences * UniformTyper< Type >::size );
		m_typedValues.resize( p_occurences );

		for ( uint32_t i = 0; i < p_occurences; i++ )
		{
			m_typedValues[i].link( &m_values[i * UniformTyper< Type >::count] );
		}
	}

	template< UniformType Type >
	TUniform< Type >::~TUniform()
	{
		DoCleanupBuffer();
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValue( value_type const & p_value )
	{
		SetValue( p_value, 0u );
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValue( value_type const & p_value, uint32_t p_index )
	{
		m_typedValues[p_index] = p_value;
		this->m_changed = true;
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_type & TUniform< Type >::GetValue()
	{
		return GetValue( 0u );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_type const & TUniform< Type >::GetValue()const
	{
		return GetValue( 0u );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_type & TUniform< Type >::GetValue( uint32_t p_index )
	{
		if ( p_index < m_occurences )
		{
			return m_typedValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_type const & TUniform< Type >::GetValue( uint32_t p_index )const
	{
		if ( p_index < m_occurences )
		{
			return m_typedValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< UniformType Type >
	template< size_t N >
	inline void TUniform< Type >::SetValues( value_type const( & p_values )[N] )
	{
		SetValues( p_values, N );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TUniform< Type >::SetValues( std::array< value_type, N > const & p_values )
	{
		SetValues( p_values.data(), N );
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValues( std::vector< value_type > const & p_values )
	{
		SetValues( p_values.data(), uint32_t( p_values.size() ) );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_type & TUniform< Type >::operator[]( uint32_t p_index )
	{
		return this->m_typedValues[p_index];
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_type const & TUniform< Type >::operator[]( uint32_t p_index )const
	{
		return this->m_typedValues[p_index];
	}

	template< UniformType Type >
	inline constexpr VariableType TUniform< Type >::GetVariableType()
	{
		return UniformTyper< Type >::variable_type;
	}

	template< UniformType Type >
	inline constexpr UniformType TUniform< Type >::GetUniformType()
	{
		return Type;
	}

	template< UniformType Type >
	inline Castor::String const & TUniform< Type >::GetUniformTypeName()
	{
		return UniformTyper< Type >::full_type_name;
	}

	template< UniformType Type >
	Castor::String const & TUniform< Type >::GetDataTypeName()const
	{
		return UniformTyper< Type >::data_type_name;
	}

	template< UniformType Type >
	uint8_t const * const TUniform< Type >::const_ptr()const
	{
		return reinterpret_cast< uint8_t * >( m_values );
	}

	template< UniformType Type >
	inline VariableType TUniform< Type >::GetType()const
	{
		return TUniform< Type >::GetVariableType();
	}

	template< UniformType Type >
	inline UniformType TUniform< Type >::GetFullType()const
	{
		return TUniform< Type >::GetUniformType();
	}

	template< UniformType Type >
	inline Castor::String const & TUniform< Type >::GetFullTypeName()const
	{
		return TUniform< Type >::GetUniformTypeName();
	}

	template< UniformType Type >
	uint32_t TUniform< Type >::size()const
	{
		return UniformTyper< Type >::size * this->m_occurences;
	}

	template< UniformType Type >
	void TUniform< Type >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		Unlinker< UniformTyper< Type >::variable_type >::Unlink( m_typedValues, m_occurences, p_buffer, p_stride );
		DoCleanupBuffer();
		Linker< UniformTyper< Type >::variable_type >::Link( m_typedValues, m_occurences, p_buffer, p_stride );
		m_stride = p_stride;
		m_values = reinterpret_cast< value_sub_type * >( p_buffer );
		m_ownBuffer = false;
	}

	template< UniformType Type >
	inline void TUniform< Type >::DoCleanupBuffer()
	{
		if ( m_ownBuffer )
		{
			delete [] m_values;
			m_stride = 0u;
		}
	}

	template< UniformType Type >
	inline void TUniform< Type >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
	{
		StringValueSetter< UniformTyper< Type >::variable_type >::SetStrValue( m_typedValues, p_values, p_index );
	}

	template< UniformType Type >
	inline Castor::String TUniform< Type >::DoGetStrValue( uint32_t p_index )const
	{
		return StringValueGetter< UniformTyper< Type >::variable_type >::GetStrValue( m_typedValues, p_index );
	}
}
