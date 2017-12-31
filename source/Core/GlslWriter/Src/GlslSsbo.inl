namespace glsl
{
	template< typename T >
	inline T Ssbo::declMember( castor::String const & p_name )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, p_name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( p_name, TypeTraits< T >::TypeEnum );
		m_stream << type().m_type << p_name << cuT( ";" ) << std::endl;
		m_count++;
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ssbo::declMember( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, p_name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( p_name, TypeTraits< T >::TypeEnum );
		m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Ssbo::declMemberArray( castor::String const & p_name )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, p_name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( p_name, TypeTraits< T >::TypeEnum );
		m_stream << type().m_type << p_name << cuT( "[];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ssbo::declMember( castor::String const & p_name, bool p_enabled )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, p_name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( p_name, TypeTraits< T >::TypeEnum );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::declMember( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, p_name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( p_name, TypeTraits< T >::TypeEnum );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::declMemberArray( castor::String const & p_name, bool p_enabled )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, p_name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( p_name, TypeTraits< T >::TypeEnum );

		if ( p_enabled )
		{
			m_stream << type().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline T Ssbo::getMember( castor::String const & p_name )
	{
		checkNameExists( m_writer, p_name, TypeTraits< T >::TypeEnum );
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ssbo::getMember( castor::String const & p_name, uint32_t p_dimension )
	{
		checkNameExists( m_writer, p_name, TypeTraits< T >::TypeEnum );
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Ssbo::getMemberArray( castor::String const & p_name )
	{
		checkNameExists( m_writer, p_name, TypeTraits< T >::TypeEnum );
		return Array< T >( &m_writer, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ssbo::getMember( castor::String const & p_name, bool p_enabled )
	{
		checkNameExists( m_writer, p_name, TypeTraits< T >::TypeEnum );
		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::getMember( castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		checkNameExists( m_writer, p_name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::getMemberArray( castor::String const & p_name, bool p_enabled )
	{
		checkNameExists( m_writer, p_name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( &m_writer, p_name, 0xFFFFFFFF, p_enabled );
	}
}
