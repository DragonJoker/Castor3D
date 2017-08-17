namespace castor3d
{
	namespace details
	{
		template< UniformType Type, typename Enable = void >
		struct Helper;

		template< UniformType Type >
		struct UniformDataAllocator
		{
			using value_type = typename UniformTypeTraits< Type >::value_sub_type;
			static constexpr auto count = VariableTypeTraits< Type >::count;

			static value_type * allocate( uint32_t p_count )
			{
				return new value_type[count * p_count];
			}

			static void deallocate( value_type * p_data )
			{
				delete[] p_data;
			}
		};

		template<>
		struct UniformDataAllocator< UniformType::eMat4x4f >
		{
			using value_type = typename UniformTypeTraits< UniformType::eMat4x4f >::value_sub_type;
			static constexpr auto count = VariableTypeTraits< UniformType::eMat4x4f >::count;

			static value_type * allocate( uint32_t p_count )
			{
				return castor::MatrixDataAllocator< float, 4, 4 >::allocate( p_count );
			}

			static void deallocate( value_type * p_data )
			{
				castor::MatrixDataAllocator< float, 4, 4 >::deallocate( p_data );
			}
		};

		template< UniformType Type >
		struct Helper< Type, typename std::enable_if< IsMatrixType< Type >::value >::type >
		{
			using typed_type = typename UniformTypeTraits< Type >::typed_value_type;
			using sub_type = typename UniformTypeTraits< Type >::value_sub_type;
			using param_type = typename UniformTypeTraits< Type >::param_type;
			using return_type = typename UniformTypeTraits< Type >::return_type;
			using return_const_type = typename UniformTypeTraits< Type >::return_const_type;
			static constexpr auto count = VariableTypeTraits< Type >::count;
			static constexpr auto rows = VariableTypeTraits< Type >::rows;
			static constexpr auto cols = VariableTypeTraits< Type >::cols;

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
					std::memcpy( buffer, dst.constPtr(), size );
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

			static inline void setValue( std::vector< typed_type > & p_dst, uint32_t p_index, param_type const & p_value )
			{
				p_dst[p_index] = p_value;
			}

			static inline return_type & getValue( std::vector< typed_type > & p_src, uint32_t p_index )
			{
				return p_src[p_index];
			}

			static inline return_const_type const & getValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				return p_src[p_index];
			}

			static inline void setStrValue( std::vector< typed_type > & p_dst, castor::String const & p_value, uint32_t p_index )
			{
				castor::StringArray arrayLines = castor::string::split( p_value, cuT( ";" ) );

				if ( arrayLines.size() == rows )
				{
					bool cont = true;

					for ( uint32_t i = 0; i < rows && cont; i++ )
					{
						cont = false;
						castor::StringArray arraySplitted = castor::string::split( arrayLines[i], cuT( ", \t" ) );

						if ( arraySplitted.size() == cols )
						{
							cont = true;

							for ( uint32_t j = 0; j < cols; j++ )
							{
								castor::string::parse( arraySplitted[j], p_dst[0][j][i] );
							}
						}
					}
				}
			}

			static inline castor::String getStrValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				castor::StringStream stream;

				for ( uint32_t j = 0; j < cols; j++ )
				{
					castor::String sep;

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
		struct Helper< Type, typename std::enable_if< IsPointType< Type >::value >::type >
		{
			using typed_type = typename UniformTypeTraits< Type >::typed_value_type;
			using sub_type = typename UniformTypeTraits< Type >::value_sub_type;
			using param_type = typename UniformTypeTraits< Type >::param_type;
			using return_type = typename UniformTypeTraits< Type >::return_type;
			using return_const_type = typename UniformTypeTraits< Type >::return_const_type;
			static constexpr auto count = VariableTypeTraits< Type >::count;

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
					std::memcpy( buffer, dst.constPtr(), size );
					buffer += p_stride;
				}
			}

			static void Link( std::vector< typed_type > & p_dst, uint8_t * p_src, uint32_t p_stride )
			{
				uint8_t * buffer = p_src;

				for ( auto & dst : p_dst )
				{
					dst = castor::Coords< sub_type, count >( reinterpret_cast< sub_type * >( buffer ) );
					buffer += p_stride;
				}
			}

			static inline void setValue( std::vector< typed_type > & p_dst, uint32_t p_index, param_type const & p_value )
			{
				auto & value = p_dst[p_index];
				std::memcpy( value.ptr(), p_value.constPtr(), value.size() );
			}

			static inline return_type & getValue( std::vector< typed_type > & p_src, uint32_t p_index )
			{
				return p_src[p_index];
			}

			static inline return_const_type const & getValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				return return_const_type{ p_src[p_index] };
			}

			static inline void setStrValue( std::vector< typed_type > & p_dst, castor::String const & p_value, uint32_t p_index )
			{
				castor::StringArray arraySplitted = castor::string::split( p_value, cuT( ", \t" ) );

				if ( arraySplitted.size() == count )
				{
					for ( uint32_t i = 0; i < count; i++ )
					{
						castor::string::parse( arraySplitted[i], p_dst[p_index][i] );
					}
				}
			}

			static inline castor::String getStrValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				castor::StringStream stream;
				castor::String sep;

				for ( uint32_t i = 0; i < count; i++ )
				{
					stream << sep << p_src[p_index][i];
					sep = ", ";
				}

				return stream.str();
			}
		};

		template< UniformType Type >
		struct Helper< Type, typename std::enable_if< IsOneType< Type >::value >::type >
		{
			using typed_type = typename UniformTypeTraits< Type >::typed_value_type;
			using sub_type = typename UniformTypeTraits< Type >::value_sub_type;
			using param_type = typename UniformTypeTraits< Type >::param_type;
			using return_type = typename UniformTypeTraits< Type >::return_type;
			using return_const_type = typename UniformTypeTraits< Type >::return_const_type;

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

			static inline void setValue( std::vector< typed_type > & p_dst, uint32_t p_index, param_type const & p_value )
			{
				*p_dst[p_index] = p_value;
			}

			static inline return_type & getValue( std::vector< typed_type > & p_src, uint32_t p_index )
			{
				return *p_src[p_index];
			}

			static inline return_const_type const & getValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				return *p_src[p_index];
			}

			static inline void setStrValue( std::vector< typed_type > & p_dst, castor::String const & p_value, uint32_t p_index )
			{
				castor::string::parse( p_value, *p_dst[p_index] );
			}

			static inline castor::String getStrValue( std::vector< typed_type > const & p_src, uint32_t p_index )
			{
				castor::StringStream stream;
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
		, m_values{ details::UniformDataAllocator< Type >::allocate( p_occurences * stride ) }
	{
		memset( m_values, 0, p_occurences * stride );
		details::Helper< Type >::Link( m_typedValues, reinterpret_cast< uint8_t * >( m_values ), stride );
	}

	template< UniformType Type >
	TUniform< Type >::~TUniform()
	{
		doCleanupBuffer();
	}

	template< UniformType Type >
	inline typename TUniform< Type >::value_sub_type const * TUniform< Type >::getValues()const
	{
		return m_values;
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_type & TUniform< Type >::getValue()
	{
		return getValue( 0u );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_const_type const & TUniform< Type >::getValue()const
	{
		return getValue( 0u );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_type & TUniform< Type >::getValue( uint32_t p_index )
	{
		if ( p_index < m_occurences )
		{
			return details::Helper< Type >::getValue( m_typedValues, p_index );
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_const_type const & TUniform< Type >::getValue( uint32_t p_index )const
	{
		if ( p_index < m_occurences )
		{
			return details::Helper< Type >::getValue( m_typedValues, p_index );
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< UniformType Type >
	inline void TUniform< Type >::setValue( param_type const & p_value )
	{
		setValue( p_value, 0u );
	}

	template< UniformType Type >
	inline void TUniform< Type >::setValue( param_type const & p_value, uint32_t p_index )
	{
		details::Helper< Type >::setValue( m_typedValues, p_index, p_value );
		this->m_changed = true;
	}

	template< UniformType Type >
	template< size_t N >
	inline void TUniform< Type >::setValues( param_type const( & p_values )[N] )
	{
		setValues( p_values, N );
	}

	template< UniformType Type >
	template< size_t N >
	inline void TUniform< Type >::setValues( std::array< param_type, N > const & p_values )
	{
		setValues( p_values.data(), N );
	}

	template< UniformType Type >
	inline void TUniform< Type >::setValues( std::vector< param_type > const & p_values )
	{
		setValues( p_values.data(), uint32_t( p_values.size() ) );
	}

	template< UniformType Type >
	inline void TUniform< Type >::setValues( param_type const * p_values, size_t p_count )
	{
		for ( auto i = 0u; i < std::min( uint32_t( p_count ), m_occurences ); ++i )
		{
			setValue( p_values[i], i );
		}
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_type & TUniform< Type >::operator[]( uint32_t p_index )
	{
		return details::Helper< Type >::getValue( m_typedValues, p_index );
	}

	template< UniformType Type >
	inline typename TUniform< Type >::return_const_type const & TUniform< Type >::operator[]( uint32_t p_index )const
	{
		return details::Helper< Type >::getValue( m_typedValues, p_index );
	}

	template< UniformType Type >
	inline constexpr VariableType TUniform< Type >::getVariableType()
	{
		return VariableTypeTraits< Type >::value;
	}

	template< UniformType Type >
	inline constexpr UniformType TUniform< Type >::getUniformType()
	{
		return Type;
	}

	template< UniformType Type >
	inline castor::String const & TUniform< Type >::getUniformTypeName()
	{
		return UniformTypeName< Type >::full_type_name;
	}

	template< UniformType Type >
	uint32_t TUniform< Type >::size()const
	{
		return uint32_t( m_occurences * stride );
	}

	template< UniformType Type >
	uint8_t const * const TUniform< Type >::constPtr()const
	{
		return reinterpret_cast< uint8_t const * >( m_values );
	}

	template< UniformType Type >
	void TUniform< Type >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		details::Helper< Type >::Unlink( m_typedValues, p_buffer, p_stride );
		doCleanupBuffer();
		details::Helper< Type >::Link( m_typedValues, p_buffer, p_stride );
		m_stride = p_stride;
		m_values = reinterpret_cast< value_sub_type * >( p_buffer );
		m_ownBuffer = false;
	}

	template< UniformType Type >
	castor::String const & TUniform< Type >::getDataTypeName()const
	{
		return UniformTypeName< Type >::data_type_name;
	}

	template< UniformType Type >
	inline VariableType TUniform< Type >::getType()const
	{
		return TUniform< Type >::getVariableType();
	}

	template< UniformType Type >
	inline UniformType TUniform< Type >::getFullType()const
	{
		return TUniform< Type >::getUniformType();
	}

	template< UniformType Type >
	inline castor::String const & TUniform< Type >::getFullTypeName()const
	{
		return TUniform< Type >::getUniformTypeName();
	}

	template< UniformType Type >
	inline void TUniform< Type >::doCleanupBuffer()
	{
		if ( m_ownBuffer )
		{
			details::UniformDataAllocator< Type >::deallocate( m_values );
			m_stride = 0u;
		}
	}

	template< UniformType Type >
	inline void TUniform< Type >::doSetStrValue( castor::String const & p_value, uint32_t p_index )
	{
		details::Helper< Type >::setStrValue( m_typedValues, p_value, p_index );
	}

	template< UniformType Type >
	inline castor::String TUniform< Type >::doGetStrValue( uint32_t p_index )const
	{
		return details::Helper< Type >::getStrValue( m_typedValues, p_index );
	}
}
