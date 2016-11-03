namespace Castor3D
{
	template<> struct MtxFrameVariableDefinitions< bool, 2, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 2 * 2 );
		static const VariableType Count = VariableType::Mat2x2;
		static const FrameVariableType Full = FrameVariableType::Mat2x2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 2, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 2 * 3 );
		static const VariableType Count = VariableType::Mat2x3;
		static const FrameVariableType Full = FrameVariableType::Mat2x3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 2, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 2 * 4 );
		static const VariableType Count = VariableType::Mat2x4;
		static const FrameVariableType Full = FrameVariableType::Mat2x4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 2, 2 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 2 * 2 );
		static const VariableType Count = VariableType::Mat2x2;
		static const FrameVariableType Full = FrameVariableType::Mat2x2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 2, 3 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 2 * 3 );
		static const VariableType Count = VariableType::Mat2x3;
		static const FrameVariableType Full = FrameVariableType::Mat2x3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 2, 4 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 2 * 4 );
		static const VariableType Count = VariableType::Mat2x4;
		static const FrameVariableType Full = FrameVariableType::Mat2x4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 2, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 2 );
		static const VariableType Count = VariableType::Mat2x2;
		static const FrameVariableType Full = FrameVariableType::Mat2x2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 2, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 3 );
		static const VariableType Count = VariableType::Mat2x3;
		static const FrameVariableType Full = FrameVariableType::Mat2x3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 2, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 4 );
		static const VariableType Count = VariableType::Mat2x4;
		static const FrameVariableType Full = FrameVariableType::Mat2x4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 2, 2 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 2 * 2 );
		static const VariableType Count = VariableType::Mat2x2;
		static const FrameVariableType Full = FrameVariableType::Mat2x2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 2, 3 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 2 * 3 );
		static const VariableType Count = VariableType::Mat2x3;
		static const FrameVariableType Full = FrameVariableType::Mat2x3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 2, 4 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 2 * 4 );
		static const VariableType Count = VariableType::Mat2x4;
		static const FrameVariableType Full = FrameVariableType::Mat2x4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 2, 2 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 2 * 2 );
		static const VariableType Count = VariableType::Mat2x2;
		static const FrameVariableType Full = FrameVariableType::Mat2x2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 2, 3 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 2 * 3 );
		static const VariableType Count = VariableType::Mat2x3;
		static const FrameVariableType Full = FrameVariableType::Mat2x3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 2, 4 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 2 * 4 );
		static const VariableType Count = VariableType::Mat2x4;
		static const FrameVariableType Full = FrameVariableType::Mat2x4d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 3, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 3 * 2 );
		static const VariableType Count = VariableType::Mat3x2;
		static const FrameVariableType Full = FrameVariableType::Mat3x2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 3, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 3 * 3 );
		static const VariableType Count = VariableType::Mat3x3;
		static const FrameVariableType Full = FrameVariableType::Mat3x3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 3, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 3 * 4 );
		static const VariableType Count = VariableType::Mat3x4;
		static const FrameVariableType Full = FrameVariableType::Mat3x4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 3, 2 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 3 * 2 );
		static const VariableType Count = VariableType::Mat3x2;
		static const FrameVariableType Full = FrameVariableType::Mat3x2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 3, 3 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 3 * 3 );
		static const VariableType Count = VariableType::Mat3x3;
		static const FrameVariableType Full = FrameVariableType::Mat3x3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 3, 4 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 3 * 4 );
		static const VariableType Count = VariableType::Mat3x4;
		static const FrameVariableType Full = FrameVariableType::Mat3x4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 3, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 2 );
		static const VariableType Count = VariableType::Mat3x2;
		static const FrameVariableType Full = FrameVariableType::Mat3x2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 3, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 3 );
		static const VariableType Count = VariableType::Mat3x3;
		static const FrameVariableType Full = FrameVariableType::Mat3x3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 3, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 4 );
		static const VariableType Count = VariableType::Mat3x4;
		static const FrameVariableType Full = FrameVariableType::Mat3x4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 3, 2 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 3 * 2 );
		static const VariableType Count = VariableType::Mat3x2;
		static const FrameVariableType Full = FrameVariableType::Mat3x2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 3, 3 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 3 * 3 );
		static const VariableType Count = VariableType::Mat3x3;
		static const FrameVariableType Full = FrameVariableType::Mat3x3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 3, 4 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 3 * 4 );
		static const VariableType Count = VariableType::Mat3x4;
		static const FrameVariableType Full = FrameVariableType::Mat3x4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 3, 2 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 3 * 2 );
		static const VariableType Count = VariableType::Mat3x2;
		static const FrameVariableType Full = FrameVariableType::Mat3x2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 3, 3 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 3 * 3 );
		static const VariableType Count = VariableType::Mat3x3;
		static const FrameVariableType Full = FrameVariableType::Mat3x3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 3, 4 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 3 * 4 );
		static const VariableType Count = VariableType::Mat3x4;
		static const FrameVariableType Full = FrameVariableType::Mat3x4d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 4, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 4 * 2 );
		static const VariableType Count = VariableType::Mat4x2;
		static const FrameVariableType Full = FrameVariableType::Mat4x2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 4, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 4 * 3 );
		static const VariableType Count = VariableType::Mat4x3;
		static const FrameVariableType Full = FrameVariableType::Mat4x3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< bool, 4, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 4 * 4 );
		static const VariableType Count = VariableType::Mat4x4;
		static const FrameVariableType Full = FrameVariableType::Mat4x4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 4, 2 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 4 * 2 );
		static const VariableType Count = VariableType::Mat4x2;
		static const FrameVariableType Full = FrameVariableType::Mat4x2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 4, 3 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 4 * 3 );
		static const VariableType Count = VariableType::Mat4x3;
		static const FrameVariableType Full = FrameVariableType::Mat4x3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< int, 4, 4 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 4 * 4 );
		static const VariableType Count = VariableType::Mat4x4;
		static const FrameVariableType Full = FrameVariableType::Mat4x4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 4, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 2 );
		static const VariableType Count = VariableType::Mat4x2;
		static const FrameVariableType Full = FrameVariableType::Mat4x2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 4, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 3 );
		static const VariableType Count = VariableType::Mat4x3;
		static const FrameVariableType Full = FrameVariableType::Mat4x3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< uint32_t, 4, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 4 );
		static const VariableType Count = VariableType::Mat4x4;
		static const FrameVariableType Full = FrameVariableType::Mat4x4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 4, 2 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 4 * 2 );
		static const VariableType Count = VariableType::Mat4x2;
		static const FrameVariableType Full = FrameVariableType::Mat4x2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 4, 3 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 4 * 3 );
		static const VariableType Count = VariableType::Mat4x3;
		static const FrameVariableType Full = FrameVariableType::Mat4x3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< float, 4, 4 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 4 * 4 );
		static const VariableType Count = VariableType::Mat4x4;
		static const FrameVariableType Full = FrameVariableType::Mat4x4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 4, 2 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 4 * 2 );
		static const VariableType Count = VariableType::Mat4x2;
		static const FrameVariableType Full = FrameVariableType::Mat4x2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 4, 3 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 4 * 3 );
		static const VariableType Count = VariableType::Mat4x3;
		static const FrameVariableType Full = FrameVariableType::Mat4x3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct MtxFrameVariableDefinitions< double, 4, 4 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 4 * 4 );
		static const VariableType Count = VariableType::Mat4x4;
		static const FrameVariableType Full = FrameVariableType::Mat4x4d;
		C3D_API static const xchar * FullTypeName;
	};

	//*************************************************************************************************

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( ShaderProgram & p_program )
		:	TFrameVariable< T >( p_program )
	{
		this->m_values = new T[Rows * Columns];
		memset( this->m_values, 0, sizeof( T ) * Rows * Columns );
		m_mtxValue = new Castor::Matrix< T, Rows, Columns >[1];
		m_mtxValue[0].link( &this->m_values[0] );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::MatrixFrameVariable( ShaderProgram & p_program, uint32_t p_occurences )
		:	TFrameVariable< T >( p_program, p_occurences )
	{
		this->m_values = new T[Rows * Columns * p_occurences];
		memset( this->m_values, 0, sizeof( T ) * p_occurences * Rows * Columns );
		m_mtxValue = new Castor::Matrix < T, Rows, Columns >[p_occurences];

		for ( uint32_t i = 0; i < p_occurences; i++ )
		{
			m_mtxValue[i].link( &this->m_values[i * Rows * Columns] );
		}
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	MatrixFrameVariable< T, Rows, Columns >::~MatrixFrameVariable()
	{
		delete [] m_mtxValue;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_value )
	{
		MatrixFrameVariable< T, Rows, Columns >::SetValue( p_value, 0 );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline void MatrixFrameVariable< T, Rows, Columns >::SetValue( Castor::Matrix< T, Rows, Columns > const & p_value, uint32_t p_index )
	{
		m_mtxValue[p_index] = p_value;
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
			return m_mtxValue[p_index];
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
			return m_mtxValue[p_index];
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
	void MatrixFrameVariable< T, Rows, Columns >::link( uint8_t * p_buffer )
	{
		memcpy( p_buffer, this->m_values, size() );
		T * l_pBuffer = reinterpret_cast< T * >( p_buffer );

		for ( uint32_t i = 0; i < this->m_occurences; i++ )
		{
			m_mtxValue[i].link( &l_pBuffer[i * Rows * Columns] );
		}

		TFrameVariable< T >::link( p_buffer );
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	inline VariableType MatrixFrameVariable< T, Rows, Columns >::GetVariableType()
	{
		return VariableType( MtxFrameVariableDefinitions< T, Rows, Columns >::eCount );
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
	inline void MatrixFrameVariable< T, Rows, Columns >::DoSetValueStr( Castor::String const & p_value, uint32_t p_index )
	{
		Castor::StringArray l_arrayLines = Castor::string::split( p_value, cuT( ";" ) );

		if ( l_arrayLines.size() == Rows )
		{
			bool l_bOK = true;

			for ( uint32_t i = 0; i < Rows && l_bOK; i++ )
			{
				l_bOK = false;
				Castor::StringArray l_arraySplitted = Castor::string::split( l_arrayLines[i], cuT( ", \t" ) );

				if ( l_arraySplitted.size() == Columns )
				{
					l_bOK = true;

					for ( uint32_t j = 0; j < Columns; j++ )
					{
						policy::assign( m_mtxValue[p_index][j][i], policy::parse( l_arraySplitted[j] ) );
					}
				}
			}
		}
	}
}
