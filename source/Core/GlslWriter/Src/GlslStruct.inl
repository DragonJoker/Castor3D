namespace GLSL
{
	template< typename T >
	inline void Struct::declMember( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << type().m_type << p_name << cuT( ";" ) << Endl();
	}

	template< typename T >
	inline void Struct::declMember( castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
	}

	template< typename T >
	inline void Struct::declMemberArray( castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		registerName( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << type().m_type << p_name << cuT( "[];" ) << Endl();
	}

	template< typename T >
	inline T Struct::getMember( castor::String const & p_name )
	{
		checkNameExists( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return T( &m_writer, m_instName + cuT( "." ) + p_name );
	}

	template< typename T >
	inline Array< T > Struct::getMember( castor::String const & p_name, uint32_t p_dimension )
	{
		checkNameExists( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return Array< T >( &m_writer, m_instName + cuT( "." ) + p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Struct::getMemberArray( castor::String const & p_name )
	{
		checkNameExists( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return Array< T >( &m_writer, m_instName + cuT( "." ) + p_name, 0xFFFFFFFF );
	}
}
