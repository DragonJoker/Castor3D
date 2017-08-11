namespace glsl
{
	template< typename T >
	inline T Ubo::declMember( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, p_name, name_of< T >::value );
		m_info.registerMember( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( ";" ) << std::endl;
		m_count++;
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ubo::declMember( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, p_name, name_of< T >::value );
		m_info.registerMember( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Ubo::declMemberArray( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, p_name, name_of< T >::value );
		m_info.registerMember( p_name, name_of< T >::value );
		m_stream << type().m_type << p_name << cuT( "[];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ubo::declMember( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, p_name, name_of< T >::value );
		m_info.registerMember( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ubo::declMember( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, p_name, name_of< T >::value );
		m_info.registerMember( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ubo::declMemberArray( castor::String const & p_name, bool p_enabled )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, p_name, name_of< T >::value );
		m_info.registerMember( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline T Ubo::getMember( castor::String const & p_name )
	{
		checkNameExists( m_writer, p_name, name_of< T >::value );
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ubo::getMember( castor::String const & p_name, uint32_t p_dimension )
	{
		checkNameExists( m_writer, p_name, name_of< T >::value );
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Ubo::getMemberArray( castor::String const & p_name )
	{
		checkNameExists( m_writer, p_name, name_of< T >::value );
		return Array< T >( &m_writer, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ubo::getMember( castor::String const & p_name, bool p_enabled )
	{
		checkNameExists( m_writer, p_name, name_of< T >::value );
		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ubo::getMember( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		checkNameExists( m_writer, p_name, name_of< T >::value );
		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ubo::getMemberArray( castor::String const & p_name, bool p_enabled )
	{
		checkNameExists( m_writer, p_name, name_of< T >::value );
		return Optional< Array< T > >( &m_writer, p_name, 0xFFFFFFFF, p_enabled );
	}
}
