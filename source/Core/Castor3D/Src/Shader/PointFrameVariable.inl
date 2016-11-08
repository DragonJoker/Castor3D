namespace Castor3D
{
	template<> struct PntFrameVariableDefinitions< bool, 2 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 2 * 1 );
		static const VariableType Count = VariableType::eVec2;
		static const FrameVariableType Full = FrameVariableType::eVec2b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< int, 2 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 2 * 1 );
		static const VariableType Count = VariableType::eVec2;
		static const FrameVariableType Full = FrameVariableType::eVec2i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< uint32_t, 2 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 1 );
		static const VariableType Count = VariableType::eVec2;
		static const FrameVariableType Full = FrameVariableType::eVec2ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< float, 2 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 2 * 1 );
		static const VariableType Count = VariableType::eVec2;
		static const FrameVariableType Full = FrameVariableType::eVec2f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< double, 2 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 2 * 1 );
		static const VariableType Count = VariableType::eVec2;
		static const FrameVariableType Full = FrameVariableType::eVec2d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< bool, 3 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 3 * 1 );
		static const VariableType Count = VariableType::eVec3;
		static const FrameVariableType Full = FrameVariableType::eVec3b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< int, 3 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 3 * 1 );
		static const VariableType Count = VariableType::eVec3;
		static const FrameVariableType Full = FrameVariableType::eVec3i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< uint32_t, 3 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 1 );
		static const VariableType Count = VariableType::eVec3;
		static const FrameVariableType Full = FrameVariableType::eVec3ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< float, 3 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 3 * 1 );
		static const VariableType Count = VariableType::eVec3;
		static const FrameVariableType Full = FrameVariableType::eVec3f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< double, 3 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 3 * 1 );
		static const VariableType Count = VariableType::eVec3;
		static const FrameVariableType Full = FrameVariableType::eVec3d;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< bool, 4 >
		: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 4 * 1 );
		static const VariableType Count = VariableType::eVec4;
		static const FrameVariableType Full = FrameVariableType::eVec4b;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< int, 4 >
		: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 4 * 1 );
		static const VariableType Count = VariableType::eVec4;
		static const FrameVariableType Full = FrameVariableType::eVec4i;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< uint32_t, 4 >
		: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 1 );
		static const VariableType Count = VariableType::eVec4;
		static const FrameVariableType Full = FrameVariableType::eVec4ui;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< float, 4 >
		: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 4 * 1 );
		static const VariableType Count = VariableType::eVec4;
		static const FrameVariableType Full = FrameVariableType::eVec4f;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< double, 4 >
		: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 4 * 1 );
		static const VariableType Count = VariableType::eVec4;
		static const FrameVariableType Full = FrameVariableType::eVec4d;
		C3D_API static const xchar * FullTypeName;
	};

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( ShaderProgram & p_program )
		: PointFrameVariable< T, Count >( p_program, 1u )
	{
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( ShaderProgram & p_program, uint32_t p_occurences )
		: TFrameVariable< T >( p_program, p_occurences )
	{
		this->m_values = new T[p_occurences * Count];
		memset( this->m_values, 0, size() );
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::~PointFrameVariable()
	{
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::SetValue( Castor::Point< T, Count > const & p_ptValue )
	{
		PointFrameVariable< T, Count >::SetValue( p_ptValue, 0 );
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::SetValue( Castor::Point< T, Count > const & p_ptValue, uint32_t p_index )
	{
		Castor::Coords< T, Count > l_ptValue( GetValue( p_index ) );
		memcpy( l_ptValue.ptr(), p_ptValue.const_ptr(), l_ptValue.size() );
		TFrameVariable< T >::m_changed = true;
	}

	template< typename T, uint32_t Count >
	inline Castor::Coords< T, Count > PointFrameVariable< T, Count >::GetValue()
	{
		return PointFrameVariable< T, Count >::GetValue( 0 );
	}

	template< typename T, uint32_t Count >
	inline Castor::Point< T, Count > PointFrameVariable< T, Count >::GetValue()const
	{
		return PointFrameVariable< T, Count >::GetValue( 0 );
	}

	template< typename T, uint32_t Count >
	inline Castor::Coords< T, Count > PointFrameVariable< T, Count >::GetValue( uint32_t p_index )
	{
		if ( p_index < this->m_occurences )
		{
			return Castor::Coords< T, Count >( &this->m_values[p_index * Count] );
		}
		else
		{
			throw ( std::out_of_range )( "PointFrameVariable< T, Count > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Count >
	inline Castor::Point< T, Count > PointFrameVariable< T, Count >::GetValue( uint32_t p_index )const
	{
		if ( p_index < this->m_occurences )
		{
			return Castor::Point< T, Count >( &this->m_values[p_index * Count] );
		}
		else
		{
			throw ( std::out_of_range )( "PointFrameVariable< T, Count > array subscript out of range" );
		}
	}

	template< typename T, uint32_t Count >
	uint32_t PointFrameVariable< T, Count >::size()const
	{
		return PntFrameVariableDefinitions< T, Count >::Size * this->m_occurences;
	}

	template< typename T, uint32_t Count >
	inline VariableType PointFrameVariable< T, Count >::GetVariableType()
	{
		return VariableType( PntFrameVariableDefinitions< T, Count >::Count );
	}

	template< typename T, uint32_t Count >
	inline FrameVariableType PointFrameVariable< T, Count >::GetFrameVariableType()
	{
		return FrameVariableType( PntFrameVariableDefinitions< T, Count >::Full );
	}

	template< typename T, uint32_t Count >
	inline Castor::String PointFrameVariable< T, Count >::GetFrameVariableTypeName()
	{
		return PntFrameVariableDefinitions< T, Count >::FullTypeName;
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::DoSetStrValue( Castor::String const & p_value, uint32_t p_index )
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
	inline Castor::String PointFrameVariable< T, Count >::DoGetStrValue( uint32_t p_index )const
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
