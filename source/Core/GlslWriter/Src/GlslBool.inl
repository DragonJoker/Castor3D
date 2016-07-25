namespace GLSL
{
	inline GlslBool::GlslBool()
		: Type( cuT( "bool " ) )
	{
	}

	inline GlslBool::GlslBool( bool p_value )
		: Type( cuT( "bool " ) )
	{
		m_value << p_value;
	}

	inline GlslBool::GlslBool( GlslWriter * p_writer, Castor::String const & p_name )
		: Type( cuT( "bool " ), p_writer, p_name )
	{
	}

	template< typename T >
	inline GlslBool & GlslBool::operator=( T const & p_rhs )
	{
		UpdateWriter( p_rhs );
		WriteAssign( m_writer, *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline GlslBool & GlslBool::operator=( int p_rhs )
	{
		WriteAssign( m_writer, *this, p_rhs );
		return *this;
	}

	inline GlslBool::operator uint32_t()
	{
		return 0u;
	}
}
