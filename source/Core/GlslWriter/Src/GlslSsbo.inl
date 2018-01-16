namespace glsl
{
	template< typename T >
	inline T Ssbo::declMember( castor::String const & name )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( name, TypeTraits< T >::TypeEnum );
		m_stream << type().m_type << name << cuT( ";" ) << std::endl;
		m_count++;
		return T( &m_writer, name );
	}

	template< typename T >
	inline Array< T > Ssbo::declMember( castor::String const & name
		, uint32_t dimension )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( name, TypeTraits< T >::TypeEnum );
		m_stream << type().m_type << name << cuT( "[" ) << dimension << cuT( "];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, name, dimension );
	}

	template< typename T >
	inline Array< T > Ssbo::declMemberArray( castor::String const & name )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( name, TypeTraits< T >::TypeEnum );
		m_stream << type().m_type << name << cuT( "[];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ssbo::declMember( castor::String const & name
		, bool enabled )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( name, TypeTraits< T >::TypeEnum );

		if ( enabled )
		{
			m_stream << type().m_type << name << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< T >( &m_writer, name, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::declMember( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( name, TypeTraits< T >::TypeEnum );

		if ( enabled )
		{
			m_stream << type().m_type << name << cuT( "[" ) << dimension << cuT( "]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::declMemberArray( castor::String const & name
		, bool enabled )
	{
		using type = typename TypeOf< T >::Type;
		registerName( m_writer, name, TypeTraits< T >::TypeEnum );
		m_info.registerMember( name, TypeTraits< T >::TypeEnum );

		if ( enabled )
		{
			m_stream << type().m_type << name << cuT( "[]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, name, 0xFFFFFFFF, enabled );
	}

	template< typename T >
	inline T Ssbo::getMember( castor::String const & name )
	{
		checkNameExists( m_writer, name, TypeTraits< T >::TypeEnum );
		return T( &m_writer, name );
	}

	template< typename T >
	inline Array< T > Ssbo::getMember( castor::String const & name
		, uint32_t dimension )
	{
		checkNameExists( m_writer, name, TypeTraits< T >::TypeEnum );
		return Array< T >( &m_writer, name, dimension );
	}

	template< typename T >
	inline Array< T > Ssbo::getMemberArray( castor::String const & name )
	{
		checkNameExists( m_writer, name, TypeTraits< T >::TypeEnum );
		return Array< T >( &m_writer, name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ssbo::getMember( castor::String const & name
		, bool enabled )
	{
		checkNameExists( m_writer, name, TypeTraits< T >::TypeEnum );
		return Optional< T >( &m_writer, name, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::getMember( castor::String const & name
		, uint32_t dimension
		, bool enabled )
	{
		checkNameExists( m_writer, name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( &m_writer, name, dimension, enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::getMemberArray( castor::String const & name
		, bool enabled )
	{
		checkNameExists( m_writer, name, TypeTraits< T >::TypeEnum );
		return Optional< Array< T > >( &m_writer, name, 0xFFFFFFFF, enabled );
	}
}
