namespace GLSL
{
	inline Boolean::Boolean()
		: Type( cuT( "bool " ) )
	{
	}

	inline Boolean::Boolean( bool p_value )
		: Type( cuT( "bool " ) )
	{
		m_value << p_value;
	}

	inline Boolean::Boolean( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "bool " ), p_writer, p_name )
	{
	}

	template< typename T >
	inline Boolean & Boolean::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		writeAssign( m_writer, *this, p_rhs );
		return *this;
	}

	template< typename T >
	inline Boolean & Boolean::operator=( int p_rhs )
	{
		writeAssign( m_writer, *this, p_rhs );
		return *this;
	}

	inline Boolean::operator uint32_t()
	{
		return 0u;
	}
}
