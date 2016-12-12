namespace Castor3D
{
	template<> struct PntUniformDefinitions< bool, 2 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 1 );
		static constexpr VariableType Count = VariableType::eVec2;
		static constexpr UniformType Full = UniformType::eVec2b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< int, 2 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 1 );
		static constexpr VariableType Count = VariableType::eVec2;
		static constexpr UniformType Full = UniformType::eVec2i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< uint32_t, 2 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 1 );
		static constexpr VariableType Count = VariableType::eVec2;
		static constexpr UniformType Full = UniformType::eVec2ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< float, 2 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 1 );
		static constexpr VariableType Count = VariableType::eVec2;
		static constexpr UniformType Full = UniformType::eVec2f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< double, 2 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 1 );
		static constexpr VariableType Count = VariableType::eVec2;
		static constexpr UniformType Full = UniformType::eVec2d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< bool, 3 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 1 );
		static constexpr VariableType Count = VariableType::eVec3;
		static constexpr UniformType Full = UniformType::eVec3b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< int, 3 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 1 );
		static constexpr VariableType Count = VariableType::eVec3;
		static constexpr UniformType Full = UniformType::eVec3i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< uint32_t, 3 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 1 );
		static constexpr VariableType Count = VariableType::eVec3;
		static constexpr UniformType Full = UniformType::eVec3ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< float, 3 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 1 );
		static constexpr VariableType Count = VariableType::eVec3;
		static constexpr UniformType Full = UniformType::eVec3f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< double, 3 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 1 );
		static constexpr VariableType Count = VariableType::eVec3;
		static constexpr UniformType Full = UniformType::eVec3d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< bool, 4 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 1 );
		static constexpr VariableType Count = VariableType::eVec4;
		static constexpr UniformType Full = UniformType::eVec4b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< int, 4 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 1 );
		static constexpr VariableType Count = VariableType::eVec4;
		static constexpr UniformType Full = UniformType::eVec4i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< uint32_t, 4 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 1 );
		static constexpr VariableType Count = VariableType::eVec4;
		static constexpr UniformType Full = UniformType::eVec4ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< float, 4 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 1 );
		static constexpr VariableType Count = VariableType::eVec4;
		static constexpr UniformType Full = UniformType::eVec4f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct PntUniformDefinitions< double, 4 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 1 );
		static constexpr VariableType Count = VariableType::eVec4;
		static constexpr UniformType Full = UniformType::eVec4d;
		C3D_API static const Castor::String FullTypeName;
	};

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	PointUniform< T, Count >::PointUniform( ShaderProgram & p_program )
		: PointUniform< T, Count >( p_program, 1u )
	{
	}

	template< typename T, uint32_t Count >
	PointUniform< T, Count >::PointUniform( ShaderProgram & p_program, uint32_t p_occurences )
		: TUniform< T >( p_program, p_occurences )
	{
		this->m_values = new T[p_occurences * Count];
		memset( this->m_values, 0, size() );
		m_ptValues.reserve( p_occurences );
		auto l_buffer = this->m_values;

		for ( uint32_t i = 0; i < p_occurences; ++i )
		{
			m_ptValues.emplace_back( l_buffer );
			l_buffer += Count;
		}
	}

	template< typename T, uint32_t Count >
	PointUniform< T, Count >::~PointUniform()
	{
	}

	template< typename T, uint32_t Count >
	inline void PointUniform< T, Count >::SetValue( Castor::Point< T, Count > const & p_ptValue )
	{
		PointUniform< T, Count >::SetValue( p_ptValue, 0 );
	}

	template< typename T, uint32_t Count >
	inline void PointUniform< T, Count >::SetValue( Castor::Point< T, Count > const & p_ptValue, uint32_t p_index )
	{
		auto & l_value = m_ptValues[p_index];
		std::memcpy( l_value.ptr(), p_ptValue.const_ptr(), l_value.size() );
		this->m_changed = true;
	}

	template< typename T, uint32_t Count >
	inline Castor::Coords< T, Count > PointUniform< T, Count >::GetValue()
	{
		return PointUniform< T, Count >::GetValue( 0 );
	}

	template< typename T, uint32_t Count >
	inline Castor::Point< T, Count > PointUniform< T, Count >::GetValue()const
	{
		return PointUniform< T, Count >::GetValue( 0 );
	}

	template< typename T, uint32_t Count >
	inline Castor::Coords< T, Count > PointUniform< T, Count >::GetValue( uint32_t p_index )
	{
		if ( p_index < this->m_occurences )
		{
			return m_ptValues[p_index];
		}
		else
		{
			throw std::out_of_range( "PointUniform< T, Count > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Count >
	inline Castor::Point< T, Count > PointUniform< T, Count >::GetValue( uint32_t p_index )const
	{
		if ( p_index < this->m_occurences )
		{
			return Castor::Point< T, Count >( m_ptValues[p_index].const_ptr() );
		}
		else
		{
			throw std::out_of_range( "PointUniform< T, Count > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Count >
	uint32_t PointUniform< T, Count >::size()const
	{
		return PntUniformDefinitions< T, Count >::Size * this->m_occurences;
	}

	template< typename T, uint32_t Count >
	void PointUniform< T, Count >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		uint8_t * l_buffer = reinterpret_cast< uint8_t * >( p_buffer );
		constexpr uint32_t l_size = PntUniformDefinitions< T, Count >::Size;

		if ( p_stride == 0 )
		{
			p_stride = l_size;
		}

		REQUIRE( p_stride >= l_size );

		for ( uint32_t i = 0u; i < this->m_occurences; i++ )
		{
			std::memcpy( l_buffer, m_ptValues[i].const_ptr(), l_size );
			l_buffer += p_stride;
		}

		this->DoCleanupBuffer();
		l_buffer = reinterpret_cast< uint8_t * >( p_buffer );

		for ( uint32_t i = 0u; i < this->m_occurences; ++i )
		{
			m_ptValues[i] = Castor::Coords< T, Count >( reinterpret_cast< T * >( l_buffer ) );
			l_buffer += p_stride;
		}

		this->m_stride = p_stride;
		this->m_values = reinterpret_cast< T * >( p_buffer );
		this->m_bOwnBuffer = false;
	}

	template< typename T, uint32_t Count >
	inline constexpr VariableType PointUniform< T, Count >::GetVariableType()
	{
		return PntUniformDefinitions< T, Count >::Count;
	}

	template< typename T, uint32_t Count >
	inline constexpr UniformType PointUniform< T, Count >::GetUniformType()
	{
		return PntUniformDefinitions< T, Count >::Full;
	}

	template< typename T, uint32_t Count >
	inline Castor::String const & PointUniform< T, Count >::GetUniformTypeName()
	{
		return PntUniformDefinitions< T, Count >::FullTypeName;
	}

	template< typename T, uint32_t Count >
	inline void PointUniform< T, Count >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
	{
		Castor::StringArray l_arraySplitted = Castor::string::split( p_value, cuT( ", \t" ) );
		Castor::Coords< T, Count > l_ptValue( GetValue( p_index ) );

		if ( l_arraySplitted.size() == Count )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				policy::assign( l_ptValue[i], policy::parse( l_arraySplitted[i] ) );
			}
		}
	}

	template< typename T, uint32_t Count >
	inline Castor::String PointUniform< T, Count >::DoGetStrValue( uint32_t p_index )const
	{
		Castor::StringStream l_stream;
		Castor::String l_sep;
		Castor::Point< T, Count > l_ptValue( GetValue( p_index ) );

		for ( uint32_t i = 0; i < Count; i++ )
		{
			l_stream << l_sep << l_ptValue[i];
			l_sep = ", ";
		}

		return l_stream.str();
	}
}
