namespace Castor3D
{
	template<> struct MtxFrameVariableDefinitions< bool, 2, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 2, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 2, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 2, 2 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 2, 3 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 2, 4 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 2, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 2, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 2, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 2, 2 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 2, 3 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 2, 4 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 2, 2 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 2 );
		static constexpr VariableType Count = VariableType::eMat2x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 2, 3 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 3 );
		static constexpr VariableType Count = VariableType::eMat2x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 2, 4 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 2 * 4 );
		static constexpr VariableType Count = VariableType::eMat2x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat2x4d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 3, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 3, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 3, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 3, 2 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 3, 3 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 3, 4 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 3, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 3, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 3, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 3, 2 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 3, 3 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 3, 4 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 3, 2 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 2 );
		static constexpr VariableType Count = VariableType::eMat3x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 3, 3 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 3 );
		static constexpr VariableType Count = VariableType::eMat3x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 3, 4 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 3 * 4 );
		static constexpr VariableType Count = VariableType::eMat3x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat3x4d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 4, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 4, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 4, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( bool ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 4, 2 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 4, 3 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 4, 4 >
		: public FrameVariableDataTyper< int >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( int ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 4, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 4, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 4, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 4, 2 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 4, 3 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 4, 4 >
		: public FrameVariableDataTyper< float >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( float ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 4, 2 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 2 );
		static constexpr VariableType Count = VariableType::eMat4x2;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 4, 3 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 3 );
		static constexpr VariableType Count = VariableType::eMat4x3;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 4, 4 >
		: public FrameVariableDataTyper< double >
	{
		static constexpr uint32_t Size = uint32_t( sizeof( double ) * 4 * 4 );
		static constexpr VariableType Count = VariableType::eMat4x4;
		static constexpr FrameVariableType Full = FrameVariableType::eMat4x4d;
		C3D_API static const xchar * FullTypeName;
	};

	//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( ShaderProgram & p_program )
		: MatrixFrameVariable< T, Rows, Columns >( p_program, 1u )
	{
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( ShaderProgram & p_program, uint32_t p_occurences )
		: TFrameVariable< T >( p_program, p_occurences )
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
	MatrixFrameVariable< T, Rows, Columns >::~MatrixFrameVariable()
	{
		m_mtxValues.clear();
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_value )
	{
		MatrixFrameVariable< T, Rows, Columns >::SetValue( p_value, 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_value, uint32_t p_index )
	{
		m_mtxValues[p_index] = p_value;
		TFrameVariable< T >::m_changed = true;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns >::GetValue()
	{
		return MatrixFrameVariable< T, Rows, Columns >::GetValue( 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > const & MatrixFrameVariable< T, Rows, Columns >::GetValue()const
	{
		return MatrixFrameVariable< T, Rows, Columns >::GetValue( 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > & MatrixFrameVariable< T, Rows, Columns >::GetValue( uint32_t p_index )
	{
		if ( p_index < TFrameVariable< T >::m_occurences )
		{
			return m_mtxValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixFrameVariable< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::Matrix< T, Rows, Columns > const & MatrixFrameVariable< T, Rows, Columns >::GetValue( uint32_t p_index )const
	{
		if ( p_index < TFrameVariable< T >::m_occurences )
		{
			return m_mtxValues[p_index];
		}
		else
		{
			throw ( std::out_of_range )( "MatrixFrameVariable< T, Rows, Columns > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	uint32_t MatrixFrameVariable< T, Rows, Columns >::size()const
	{
		return MtxFrameVariableDefinitions< T, Rows, Columns >::Size * this->m_occurences;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	void MatrixFrameVariable< T, Rows, Columns >::link( uint8_t * p_buffer, uint32_t p_stride )
	{
		uint8_t * l_buffer = reinterpret_cast< uint8_t * >( p_buffer );
		constexpr uint32_t l_size = MtxFrameVariableDefinitions< T, Rows, Columns >::Size;

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
	inline VariableType MatrixFrameVariable< T, Rows, Columns >::GetVariableType()
	{
		return VariableType( MtxFrameVariableDefinitions< T, Rows, Columns >::Count );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline FrameVariableType MatrixFrameVariable< T, Rows, Columns >::GetFrameVariableType()
	{
		return FrameVariableType( MtxFrameVariableDefinitions< T, Rows, Columns >::Full );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline Castor::String MatrixFrameVariable< T, Rows, Columns >::GetFrameVariableTypeName()
	{
		return MtxFrameVariableDefinitions< T, Rows, Columns >::FullTypeName;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
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
	inline Castor::String MatrixFrameVariable< T, Rows, Columns >::DoGetStrValue( uint32_t p_index )const
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
