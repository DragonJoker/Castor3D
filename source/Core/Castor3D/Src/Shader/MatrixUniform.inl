namespace Castor3D
{
	template<> struct MtxUniformDefinitions< bool, 2, 2 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr UniformType Full = UniformType::eMat2x2b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 2, 3 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr UniformType Full = UniformType::eMat2x3b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 2, 4 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr UniformType Full = UniformType::eMat2x4b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 2, 2 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr UniformType Full = UniformType::eMat2x2i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 2, 3 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr UniformType Full = UniformType::eMat2x3i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 2, 4 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr UniformType Full = UniformType::eMat2x4i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 2, 2 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr UniformType Full = UniformType::eMat2x2ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 2, 3 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr UniformType Full = UniformType::eMat2x3ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 2, 4 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr UniformType Full = UniformType::eMat2x4ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 2, 2 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr UniformType Full = UniformType::eMat2x2f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 2, 3 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr UniformType Full = UniformType::eMat2x3f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 2, 4 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr UniformType Full = UniformType::eMat2x4f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 2, 2 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr UniformType Full = UniformType::eMat2x2d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 2, 3 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr UniformType Full = UniformType::eMat2x3d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 2, 4 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr UniformType Full = UniformType::eMat2x4d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 3, 2 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr UniformType Full = UniformType::eMat3x2b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 3, 3 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr UniformType Full = UniformType::eMat3x3b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 3, 4 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr UniformType Full = UniformType::eMat3x4b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 3, 2 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr UniformType Full = UniformType::eMat3x2i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 3, 3 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr UniformType Full = UniformType::eMat3x3i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 3, 4 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr UniformType Full = UniformType::eMat3x4i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 3, 2 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr UniformType Full = UniformType::eMat3x2ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 3, 3 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr UniformType Full = UniformType::eMat3x3ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 3, 4 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr UniformType Full = UniformType::eMat3x4ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 3, 2 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr UniformType Full = UniformType::eMat3x2f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 3, 3 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr UniformType Full = UniformType::eMat3x3f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 3, 4 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr UniformType Full = UniformType::eMat3x4f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 3, 2 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr UniformType Full = UniformType::eMat3x2d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 3, 3 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr UniformType Full = UniformType::eMat3x3d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 3, 4 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr UniformType Full = UniformType::eMat3x4d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 4, 2 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr UniformType Full = UniformType::eMat4x2b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 4, 3 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr UniformType Full = UniformType::eMat4x3b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< bool, 4, 4 >
		: public UniformDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr UniformType Full = UniformType::eMat4x4b;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 4, 2 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr UniformType Full = UniformType::eMat4x2i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 4, 3 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr UniformType Full = UniformType::eMat4x3i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< int, 4, 4 >
		: public UniformDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr UniformType Full = UniformType::eMat4x4i;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 4, 2 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr UniformType Full = UniformType::eMat4x2ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 4, 3 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr UniformType Full = UniformType::eMat4x3ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< uint32_t, 4, 4 >
		: public UniformDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr UniformType Full = UniformType::eMat4x4ui;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 4, 2 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr UniformType Full = UniformType::eMat4x2f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 4, 3 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr UniformType Full = UniformType::eMat4x3f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< float, 4, 4 >
		: public UniformDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr UniformType Full = UniformType::eMat4x4f;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 4, 2 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr UniformType Full = UniformType::eMat4x2d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 4, 3 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr UniformType Full = UniformType::eMat4x3d;
		C3D_API static const Castor::String FullTypeName;
	};
	template<> struct MtxUniformDefinitions< double, 4, 4 >
		: public UniformDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr UniformType Full = UniformType::eMat4x4d;
		C3D_API static const Castor::String FullTypeName;
	};

	//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixUniform< T, Rows, Columns >::MatrixUniform( ShaderProgram & p_program )
		: MatrixUniform< T, Rows, Columns >( p_program, 1u )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixUniform< T, Rows, Columns >::MatrixUniform( ShaderProgram & p_program, uint32_t p_occurences )
		: TUniform< T >( p_program, p_occurences )
	{
		this->m_values = new T[Rows * Columns * p_occurences];
		memset( this->m_values, 0, sizeof( T ) * p_occurences * Rows * Columns );
		m_mtxValues.resize( p_occurences );

		for ( uint32_t i = 0; i < p_occurences; i++ )
		{
			m_mtxValues[i].link( &this->m_values[i * Rows * Columns] );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixUniform< T, Rows, Columns >::~MatrixUniform()
	{
		m_mtxValues.clear();
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixUniform< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_value )
	{
		MatrixUniform< T, Rows, Columns >::SetValue( p_value, 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixUniform< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_value, uint32_t p_index )
	{
		m_mtxValues[p_index] = p_value;
		this->m_changed = true;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > & MatrixUniform< T, Rows, Columns >::GetValue()
	{
		return MatrixUniform< T, Rows, Columns >::GetValue( 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > const & MatrixUniform< T, Rows, Columns >::GetValue()const
	{
		return MatrixUniform< T, Rows, Columns >::GetValue( 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > & MatrixUniform< T, Rows, Columns >::GetValue( uint32_t p_index )
	{
		if ( p_index < this->m_occurences )
		{
			return m_mtxValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > const & MatrixUniform< T, Rows, Columns >::GetValue( uint32_t p_index )const
	{
		if ( p_index < this->m_occurences )
		{
			return m_mtxValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixUniform< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	uint32_t MatrixUniform< T, Rows, Columns >::size()const
	{
		return MtxUniformDefinitions< T, Rows, Columns >::Size * this->m_occurences;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void MatrixUniform< T, Rows, Columns >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		uint8_t * l_buffer = reinterpret_cast< uint8_t * >( p_buffer );
		constexpr uint32_t l_size = MtxUniformDefinitions< T, Rows, Columns >::Size;

		if ( p_stride == 0 )
		{
			p_stride = l_size;
		}

		REQUIRE( p_stride >= l_size );

		for ( uint32_t i = 0; i < this->m_occurences; i++ )
		{
			std::memcpy( l_buffer, m_mtxValues[i].const_ptr(), l_size );
			l_buffer += p_stride;
		}

		this->DoCleanupBuffer();
		l_buffer = reinterpret_cast< uint8_t * >( p_buffer );

		for ( uint32_t i = 0; i < this->m_occurences; i++ )
		{
			m_mtxValues[i].link( reinterpret_cast< T * >( l_buffer ) );
			l_buffer += p_stride;
		}

		this->m_stride = p_stride;
		this->m_values = reinterpret_cast< T * >( p_buffer );
		this->m_bOwnBuffer = false;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline constexpr VariableType MatrixUniform< T, Rows, Columns >::GetVariableType()
	{
		return VariableType( MtxUniformDefinitions< T, Rows, Columns >::Count );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline constexpr UniformType MatrixUniform< T, Rows, Columns >::GetUniformType()
	{
		return UniformType( MtxUniformDefinitions< T, Rows, Columns >::Full );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::String const & MatrixUniform< T, Rows, Columns >::GetUniformTypeName()
	{
		return MtxUniformDefinitions< T, Rows, Columns >::FullTypeName;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixUniform< T, Rows, Columns >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
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
						policy::assign( m_mtxValues[0][j][i], policy::parse( l_arraySplitted[j] ) );
					}
				}
			}
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::String MatrixUniform< T, Rows, Columns >::DoGetStrValue( uint32_t p_index )const
	{
		Castor::StringStream l_stream;

		for ( uint32_t j = 0; j < Columns; j++ )
		{
			Castor::String l_sep;

			for ( uint32_t i = 0; i < Rows; i++ )
			{
				l_stream << l_sep << m_mtxValues[0][j][i];
				l_sep = ", ";
			}

			l_stream << ";";
		}

		return l_stream.str();
	}
}
