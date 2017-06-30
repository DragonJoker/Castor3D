namespace GLSL
{
	template< typename T >
	inline void Struct::DeclMember( Castor::String const & p_name )
	{
		m_writer.RegisterName( m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << T().m_type << p_name << cuT( ";" ) << Endl();
	}

	template< typename T >
	inline void Struct::DeclMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		m_writer.RegisterName( m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << T().m_type << p_name << cuT( "[" ) << p_dimension << cuT( "];" ) << Endl();
	}

	template< typename T >
	inline void Struct::DeclMemberArray( Castor::String const & p_name )
	{
		m_writer.RegisterName( m_name + cuT( "::" ) + p_name, name_of< T >::value );
		m_writer << T().m_type << p_name << cuT( "[];" ) << Endl();
	}

	template< typename T >
	inline T Struct::GetMember( Castor::String const & p_name )
	{
		m_writer.CheckNameExists( m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return T( &m_writer, m_instName + cuT( "." ) + p_name );
	}

	template< typename T >
	inline Array< T > Struct::GetMember( Castor::String const & p_name, uint32_t p_dimension )
	{
		m_writer.CheckNameExists( m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return Array< T >( &m_writer, m_instName + cuT( "." ) + p_name, p_dimension );
	}

	template< typename T >
	inline Array< T > Struct::GetMemberArray( Castor::String const & p_name )
	{
		m_writer.CheckNameExists( m_name + cuT( "::" ) + p_name, name_of< T >::value );
		return Array< T >( &m_writer, m_instName + cuT( "." ) + p_name, 0xFFFFFFFF );
	}
}
