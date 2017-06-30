namespace GLSL
{
	template< typename T >
	inline T Ssbo::DeclMember( Castor::String const & p_name )
	{
		m_writer.RegisterName( p_name, name_of< T >::value );
		m_info.RegisterMember( p_name, name_of< T >::value );
		m_stream << T().m_type << p_name << cuT( ";" ) << std::endl;
		m_count++;
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ssbo::DeclMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		m_writer.RegisterName( p_name, name_of< T >::value );
		m_info.RegisterMember( p_name, name_of< T >::value );
		m_stream << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Ssbo::DeclMemberArray( Castor::String const & p_name )
	{
		m_writer.RegisterName( p_name, name_of< T >::value );
		m_info.RegisterMember( p_name, name_of< T >::value );
		m_stream << T().m_type << p_name << cuT( "[];" ) << std::endl;
		m_count++;
		return Array< T >( &m_writer, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ssbo::DeclMember( Castor::String const & p_name, bool p_enabled )
	{
		m_writer.RegisterName( p_name, name_of< T >::value );
		m_info.RegisterMember( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::DeclMember( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		m_writer.RegisterName( p_name, name_of< T >::value );
		m_info.RegisterMember( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::DeclMemberArray( Castor::String const & p_name, bool p_enabled )
	{
		m_writer.RegisterName( p_name, name_of< T >::value );
		m_info.RegisterMember( p_name, name_of< T >::value );

		if ( p_enabled )
		{
			m_stream << T().m_type << p_name << cuT( "[]" ) << cuT( ";" ) << std::endl;
			m_count++;
		}

		return Optional< Array< T > >( &m_writer, p_name, 0xFFFFFFFF, p_enabled );
	}

	template< typename T >
	inline T Ssbo::GetMember( Castor::String const & p_name )
	{
		m_writer.CheckNameExists( p_name, name_of< T >::value );
		return T( &m_writer, p_name );
	}

	template< typename T >
	inline Array< T > Ssbo::GetMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		m_writer.CheckNameExists( p_name, name_of< T >::value );
		return Array< T >( &m_writer, p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Ssbo::GetMemberArray( Castor::String const & p_name )
	{
		m_writer.CheckNameExists( p_name, name_of< T >::value );
		return Array< T >( &m_writer, p_name, 0xFFFFFFFF );
	}

	template< typename T >
	inline Optional< T > Ssbo::GetMember( Castor::String const & p_name, bool p_enabled )
	{
		m_writer.CheckNameExists( p_name, name_of< T >::value );
		return Optional< T >( &m_writer, p_name, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::GetMember( Castor::String const & p_name, uint32_t p_dimension, bool p_enabled )
	{
		m_writer.CheckNameExists( p_name, name_of< T >::value );
		return Optional< Array< T > >( &m_writer, p_name, p_dimension, p_enabled );
	}

	template< typename T >
	inline Optional< Array< T > > Ssbo::GetMemberArray( Castor::String const & p_name, bool p_enabled )
	{
		m_writer.CheckNameExists( p_name, name_of< T >::value );
		return Optional< Array< T > >( &m_writer, p_name, 0xFFFFFFFF, p_enabled );
	}
}
