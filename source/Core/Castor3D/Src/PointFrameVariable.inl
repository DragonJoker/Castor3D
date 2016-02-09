namespace Castor3D
{
	template<> struct PntFrameVariableDefinitions< bool, 2 >
			: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2B;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< int, 2 >
			: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2I;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< uint32_t, 2 >
			: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2UI;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< float, 2 >
			: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2F;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< double, 2 >
			: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 2 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC2;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC2D;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< bool, 3 >
			: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3B;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< int, 3 >
			: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3I;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< uint32_t, 3 >
			: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3UI;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< float, 3 >
			: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3F;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< double, 3 >
			: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 3 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC3;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC3D;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< bool, 4 >
			: public FrameVariableDataTyper< bool >
	{
		static const uint32_t Size = uint32_t( sizeof( bool ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4B;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< int, 4 >
			: public FrameVariableDataTyper< int >
	{
		static const uint32_t Size = uint32_t( sizeof( int ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4I;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< uint32_t, 4 >
			: public FrameVariableDataTyper< uint32_t >
	{
		static const uint32_t Size = uint32_t( sizeof( uint32_t ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4UI;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< float, 4 >
			: public FrameVariableDataTyper< float >
	{
		static const uint32_t Size = uint32_t( sizeof( float ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4F;
		C3D_API static const xchar * FullTypeName;
	};
	template<> struct PntFrameVariableDefinitions< double, 4 >
			: public FrameVariableDataTyper< double >
	{
		static const uint32_t Size = uint32_t( sizeof( double ) * 4 * 1 );
		static const eVARIABLE_TYPE Count = eVARIABLE_TYPE_VEC4;
		static const eFRAME_VARIABLE_TYPE Full = eFRAME_VARIABLE_TYPE_VEC4D;
		C3D_API static const xchar * FullTypeName;
	};

	//*************************************************************************************************

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( ShaderProgram * p_program )
		:	TFrameVariable< T >( p_program )
	{
		this->m_values = new T[Count];
		memset( this->m_values, 0, size() );
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( ShaderProgram * p_program, uint32_t p_occurences )
		:	TFrameVariable< T >( p_program, p_occurences )
	{
		this->m_values = new T[p_occurences * Count];
		memset( this->m_values, 0, size() );
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( PointFrameVariable< T, Count> const & p_rVariable )
		:	TFrameVariable< T >( p_rVariable )
	{
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count >::PointFrameVariable( PointFrameVariable< T, Count > && p_rVariable )
		:	TFrameVariable< T >( std::move( p_rVariable ) )
	{
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count > & PointFrameVariable< T, Count >::operator=( PointFrameVariable< T, Count> const & p_rVariable )
	{
		PointFrameVariable< T, Count > l_tmp( p_rVariable );
		std::swap( *this, l_tmp );
		return *this;
	}

	template< typename T, uint32_t Count >
	PointFrameVariable< T, Count > & PointFrameVariable< T, Count >::operator=( PointFrameVariable< T, Count > && p_rVariable )
	{
		TFrameVariable< T >::operator =( std::move( p_rVariable ) );

		if ( this != &p_rVariable )
		{
		}

		return *this;
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
	inline eVARIABLE_TYPE PointFrameVariable< T, Count >::GetVariableType()
	{
		return eVARIABLE_TYPE( PntFrameVariableDefinitions< T, Count >::Count );
	}

	template< typename T, uint32_t Count >
	inline eFRAME_VARIABLE_TYPE PointFrameVariable< T, Count >::GetFrameVariableType()
	{
		return eFRAME_VARIABLE_TYPE( PntFrameVariableDefinitions< T, Count >::Full );
	}

	template< typename T, uint32_t Count >
	inline Castor::String PointFrameVariable< T, Count >::GetFrameVariableTypeName()
	{
		return PntFrameVariableDefinitions< T, Count >::FullTypeName;
	}

	template< typename T, uint32_t Count >
	inline void PointFrameVariable< T, Count >::DoSetValueStr( Castor::String const & p_strValue, uint32_t p_index )
	{
		Castor::StringArray l_arraySplitted = Castor::string::split( p_strValue, cuT( ", \t" ) );
		Castor::Coords< T, Count > l_ptValue( GetValue( p_index ) );

		if ( l_arraySplitted.size() == Count )
		{
			for ( uint32_t i = 0; i < Count; i++ )
			{
				policy::assign( l_ptValue[i], policy::parse( l_arraySplitted[i] ) );
			}
		}
	}
}
