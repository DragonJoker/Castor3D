namespace GLSL
{
	template< typename T >
	inline void Struct::DeclMember( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterName( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << type().m_type << p_name << cuT( ";" ) << Endl();
	}

	template< typename T >
	inline void Struct::DeclMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		using type = typename type_of< T >::type;
		RegisterName( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << type().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
	}

	template< typename T >
	inline void Struct::DeclMemberArray( Castor::String const & p_name )
	{
		using type = typename type_of< T >::type;
		RegisterName( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << type().m_type << p_name << cuT( "[];" ) << Endl();
	}

	template< typename T >
	inline T Struct::GetMember( Castor::String const & p_name )
	{
		CheckNameExists( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return T( &m_writer, m_instName + cuT( "." ) + p_name );
	}

	template< typename T >
	inline Array< T > Struct::GetMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		CheckNameExists( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return Array< T >( &m_writer, m_instName + cuT( "." ) + p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Struct::GetMemberArray( Castor::String const & p_name )
	{
		CheckNameExists( m_writer, m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return Array< T >( &m_writer, m_instName + cuT( "." ) + p_name, 0xFFFFFFFF );
	}
}
