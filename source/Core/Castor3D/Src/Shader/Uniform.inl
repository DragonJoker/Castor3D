namespace Castor3D
{
	namespace details
	{
		template< UniformType Type, typename Enable = void >
		struct Helper;

		template< UniformType Type >
		struct UniformDataAllocator
		{
			using value_type = typename uniform_type< Type >::value_sub_type;
			static constexpr auto count = variable_type< Type >::count;

			static value_type * Alloc( uint32_t p_count )
			{
				return new value_type[count * p_count];
			}

			static void Free( value_type * p_data )
			{
				delete[] p_data;
			}
		};

		template<>
		struct UniformDataAllocator< UniformType::eMat4x4f >
		{
			using value_type = typename uniform_type< UniformType::eMat4x4f >::value_sub_type;
			static constexpr auto count = variable_type< UniformType::eMat4x4f >::count;

			static value_type * Alloc( uint32_t p_count )
			{
				return Castor::MatrixDataAllocator< float, 4, 4 >::Alloc( p_count );
			}

			static void Free( value_type * p_data )
			{
				Castor::MatrixDataAllocator< float, 4, 4 >::Free( p_data );
			}
		};

		template< UniformType Type >
		struct Helper< Type, typename std::enable_if< is_matrix_type< Type >::value >::type >
		{
			using typed_type = typename uniform_type< Type >::typed_value_type;
			using sub_type = typename uniform_type< Type >::value_sub_type;
			using param_type = typename uniform_type< Type >::param_type;
			using return_type = typename uniform_type< Type >::return_type;
			using return_const_type = typename uniform_type< Type >::return_const_type;
			static constexpr auto count = variable_type< Type >::count;
			static constexpr auto rows = variable_type< Type >::rows;
			static constexpr auto cols = variable_type< Type >::cols;

			static void Unlink( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t & p_stride )
			{
				constexpr uint32_t size = sizeof( sub_type ) * count;
				uint8_t * buffer = p_src;

				if ( !p_stride )
				{
					p_stride = size;
				}

				REQUIRE( p_stride >= size );

				for ( auto & dst : p_dst )
				{
					std::memcpy( buffer, dst.const_ptr(), size );
					buffer += p_stride;
				}
			}

			static void Link( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t p_stride )
			{
				uint8_t * buffer = p_src;

				for ( auto & dst : p_dst )
				{
					dst.link( reinterpret_cast< sub_type * >( buffer ) );
					buffer += p_stride;
				}
			}

			static inline void SetValue( std::vector< typed_type > & p_dst, uint32_t p_index, param_type const & p_value )
			{
				p_dst[p_index] = p_value;
			}

			static inline return_type & GetValue( std::vector< typed_type > & p_src, uint32_t p_index )
			{
				return p_src[p_index];
			}

			static inline return_const_type const & GetValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				return p_src[p_index];
			}

			static inline void SetStrValue( std::vector< typed_type > & p_dst, Castor::String const & p_value, uint32_t p_index )
			{
				Castor::StringArray arrayLines = Castor::string::split( p_value, cuT( ";" ) );

				if ( arrayLines.size() == rows )
				{
					bool cont = true;

					for ( uint32_t i = 0; i < rows && cont; i++ )
					{
						cont = false;
						Castor::StringArray arraySplitted = Castor::string::split( arrayLines[i], cuT( ", \t" ) );

						if ( arraySplitted.size() == cols )
						{
							cont = true;

							for ( uint32_t j = 0; j < cols; j++ )
							{
								Castor::string::parse( arraySplitted[j], p_dst[0][j][i] );
							}
						}
					}
				}
			}

			static inline Castor::String GetStrValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				Castor::StringStream stream;

				for ( uint32_t j = 0; j < cols; j++ )
				{
					Castor::String sep;

					for ( uint32_t i = 0; i < rows; i++ )
					{
						stream << sep << p_src[p_index][j][i];
						sep = ", ";
					}

					stream << ";";
				}

				return stream.str();
			}
		};

		template< UniformType Type >
		struct Helper< Type, typename std::enable_if< is_point_type< Type >::value >::type >
		{
			using typed_type = typename uniform_type< Type >::typed_value_type;
			using sub_type = typename uniform_type< Type >::value_sub_type;
			using param_type = typename uniform_type< Type >::param_type;
			using return_type = typename uniform_type< Type >::return_type;
			using return_const_type = typename uniform_type< Type >::return_const_type;
			static constexpr auto count = variable_type< Type >::count;

			static void Unlink( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t & p_stride )
			{
				uint8_t * buffer = p_src;
				constexpr uint32_t size = sizeof( sub_type ) * count;

				if ( !p_stride )
				{
					p_stride = size;
				}

				REQUIRE( p_stride >= size );

				for ( auto & dst : p_dst )
				{
					std::memcpy( buffer, dst.const_ptr(), size );
					buffer += p_stride;
				}
			}

			static void Link( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t p_stride )
			{
				uint8_t * buffer = p_src;

				for ( auto & dst : p_dst )
				{
					dst = Castor::Coords< sub_type, count >( reinterpret_cast< sub_type * >( buffer ) );
					buffer += p_stride;
				}
			}

			static inline void SetValue( std::vector< typed_type > & p_dst, uint32_t p_index, param_type const & p_value )
			{
				auto & value = p_dst[p_index];
				std::memcpy( value.ptr(), p_value.const_ptr(), value.size() );
			}

			static inline return_type & GetValue( std::vector< typed_type > & p_src, uint32_t p_index )
			{
				return p_src[p_index];
			}

			static inline return_const_type const & GetValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				return return_const_type{ p_src[p_index] };
			}

			static inline void SetStrValue( std::vector< typed_type > & p_dst, Castor::String const & p_value, uint32_t p_index )
			{
				Castor::StringArray arraySplitted = Castor::string::split( p_value, cuT( ", \t" ) );

				if ( arraySplitted.size() == count )
				{
					for ( uint32_t i = 0; i < count; i++ )
					{
						Castor::string::parse( arraySplitted[i], p_dst[p_index][i] );
					}
				}
			}

			static inline Castor::String GetStrValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				Castor::StringStream stream;
				Castor::String sep;

				for ( uint32_t i = 0; i < count; i++ )
				{
					stream << sep << p_src[p_index][i];
					sep = ", ";
				}

				return stream.str();
			}
		};

		template< UniformType Type >
		struct Helper< Type, typename std::enable_if< is_one_type< Type >::value >::type >
		{
			using typed_type = typename uniform_type< Type >::typed_value_type;
			using sub_type = typename uniform_type< Type >::value_sub_type;
			using param_type = typename uniform_type< Type >::param_type;
			using return_type = typename uniform_type< Type >::return_type;
			using return_const_type = typename uniform_type< Type >::return_const_type;

			static void Unlink( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t & p_stride )
			{
				uint8_t * buffer = p_src;
				constexpr uint32_t size = sizeof( sub_type );

				if ( !p_stride )
				{
					p_stride = size;
				}

				REQUIRE( p_stride >= size );

				for ( auto & dst : p_dst )
				{
					std::memcpy( buffer, dst, size );
					buffer += p_stride;
				}
			}

			static void Link( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t p_stride )
			{
				uint8_t * buffer = p_src;
				constexpr uint32_t size = sizeof( sub_type );

				for ( auto & dst : p_dst )
				{
					dst = reinterpret_cast< sub_type * >( buffer );
					buffer += p_stride;
				}
			}

			static inline void SetValue( std::vector< typed_type > & p_dst, uint32_t p_index, param_type const & p_value )
			{
				*p_dst[p_index] = p_value;
			}

			static inline return_type & GetValue( std::vector< typed_type > & p_src, uint32_t p_index )
			{
				return *p_src[p_index];
			}

			static inline return_const_type const & GetValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				return *p_src[p_index];
			}

			static inline void SetStrValue( std::vector< typed_type > & p_dst, Castor::String const & p_value, uint32_t p_index )
			{
				Castor::string::parse( p_value, *p_dst[p_index] );
			}

			static inline Castor::String GetStrValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				Castor::StringStream stream;
				stream << *p_src[p_index];
				return stream.str();
			}
		};
	}

	//*************************************************************************************************

	template< UniformType Type >
	TUniform< Type >::TUniform( uint32_t p_occurences )
		: Uniform{ p_occurences }
		, m_typedValues( p_occurences, typed_value{} )
		, m_values{ details::UniformDataAllocator< Type >::Alloc( p_occurences * stride ) }
	{
		memset( m_values, 0, p_occurences * stride );
		details::Helper< Type >::Link( m_typedValues, reinterpret_cast< uint8_t * >( m_values ), stride );
	}

	template< UniformType Type >
	TUniform< Type >::~TUniform()
	{
		DoCleanupBuffer();
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_sub_type const * TUniform< Type >::GetValues()const
	{
		return m_values;
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_type & TUniform< Type >::GetValue()
	{
		return GetValue( 0u );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_const_type const & TUniform< Type >::GetValue()const
	{
		return GetValue( 0u );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_type & TUniform< Type >::GetValue( uint32_t p_index )
	{
		if ( p_index < m_occurences )
		{
			return details::Helper< Type >::GetValue( m_typedValues, p_index );
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_const_type const & TUniform< Type >::GetValue( uint32_t p_index )const
	{
		if ( p_index < m_occurences )
		{
			return details::Helper< Type >::GetValue( m_typedValues, p_index );
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValue( param_type const & p_value )
	{
		SetValue( p_value, 0u );
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValue( param_type const & p_value, uint32_t p_index )
	{
		details::Helper< Type >::SetValue( m_typedValues, p_index, p_value );
		this->m_changed = true;
	}

	template< UniformType Type >
	template< size_t N >
	inline void TUniform< Type >::SetValues( param_type const( & p_values )[N] )
	{
		SetValues( p_values, N );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TUniform< Type >::SetValues( std::array< param_type, N > const & p_values )
	{
		SetValues( p_values.data(), N );
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValues( std::vector< param_type > const & p_values )
	{
		SetValues( p_values.data(), uint32_t( p_values.size() ) );
	}

	template< UniformType Type >
	inline void TUniform< Type >::SetValues( param_type const * p_values, size_t p_count )
	{
		for ( auto i = 0u; i < std::min( uint32_t( p_count ), m_occurences ); ++i )
		{
			SetValue( p_values[i], i );
		}
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_type & TUniform< Type >::operator[]( uint32_t p_index )
	{
		return details::Helper< Type >::GetValue( m_typedValues, p_index );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_const_type const & TUniform< Type >::operator[]( uint32_t p_index )const
	{
		return details::Helper< Type >::GetValue( m_typedValues, p_index );
	}

	template< UniformType Type >
	inline constexpr VariableType TUniform< Type >::GetVariableType()
	{
		return variable_type< Type >::value;
	}

	template< UniformType Type >
	inline constexpr UniformType TUniform< Type >::GetUniformType()
	{
		return Type;
	}

	template< UniformType Type >
	inline Castor::String const & TUniform< Type >::GetUniformTypeName()
	{
		return uniform_type_name< Type >::full_type_name;
	}

	template< UniformType Type >
	uint32_t TUniform< Type >::size()const
	{
		return uint32_t( m_occurences * stride );
	}

	template< UniformType Type >
	uint8_t const * const TUniform< Type >::const_ptr()const
	{
		return reinterpret_cast< uint8_t const * >( m_values );
	}

	template< UniformType Type >
	void TUniform< Type >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		details::Helper< Type >::Unlink( m_typedValues, p_buffer, p_stride );
		DoCleanupBuffer();
		details::Helper< Type >::Link( m_typedValues, p_buffer, p_stride );
		m_stride = p_stride;
		m_values = reinterpret_cast< value_sub_type * >( p_buffer );
		m_ownBuffer = false;
	}

	template< UniformType Type >
	Castor::String const & TUniform< Type >::GetDataTypeName()const
	{
		return uniform_type_name< Type >::data_type_name;
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
	inline void TUniform< Type >::DoCleanupBuffer()
	{
		if ( m_ownBuffer )
		{
			details::UniformDataAllocator< Type >::Free( m_values );
			m_stride = 0u;
		}
	}

	template< UniformType Type >
	inline void TUniform< Type >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
	{
		details::Helper< Type >::SetStrValue( m_typedValues, p_value, p_index );
	}

	template< UniformType Type >
	inline Castor::String TUniform< Type >::DoGetStrValue( uint32_t p_index )const
	{
		return details::Helper< Type >::GetStrValue( m_typedValues, p_index );
	}
}
